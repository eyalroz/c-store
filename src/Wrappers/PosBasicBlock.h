/* Copyright (c) 2005, Regents of Massachusetts Institute of Technology, 
 * Brandeis University, Brown University, and University of Massachusetts 
 * Boston. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, 
 *     this list of conditions and the following disclaimer.
 *   - Redistributions in binary form must reproduce the above copyright 
 *     notice, this list of conditions and the following disclaimer in the 
 *     documentation and/or other materials provided with the distribution.
 *   - Neither the name of Massachusetts Institute of Technology, 
 *     Brandeis University, Brown University, or University of 
 *     Massachusetts Boston nor the names of its contributors may be used 
 *     to endorse or promote products derived from this software without 
 *     specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED 
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef _POSBASICBLOCK_H_
#define _POSBASICBLOCK_H_

#include "../common/PosBlock.h"
#include "BasicBlock.h"
#include <iostream>
#include "../common/UnexpectedException.h"
class PosBasicBlock : public PosBlock
{
public:
	PosBasicBlock(bool posSorted_);
	virtual ~PosBasicBlock();
	PosBasicBlock(const PosBasicBlock& block_);

	PosBlock* clone(PosBlock&);
	
	unsigned int getPosition();
	
	void setPosition(unsigned int pos_);

	// Iterator access to block
	virtual bool hasNext();
	virtual unsigned int getNext();
	virtual unsigned int peekNext();
	// Zero indexed, gets the pair at this loc
	virtual unsigned int getPosAtLoc(unsigned int loc_);
	virtual unsigned int getCurrLoc();
	
	virtual int getSize();
	virtual int getSizeInBits();
	//virtual unsigned int getStartPos() const;
	//virtual unsigned int getEndPos() const;
	virtual unsigned int getStartPos();
	virtual unsigned int getEndPos();
	virtual void resetBlock();	

	virtual PosBlock* copySubset(int fromPos, int untilPos);
	virtual unsigned int initBitStringIterator(unsigned int pos);
	virtual int getNextBitString();
	
	// Stream properties
	virtual bool isPosSorted();
	
	// Block properties
	virtual bool isPosContiguous();
	virtual bool isBlockPosSorted();
	virtual bool isSparse();
	
	PosBlockIter* getIterator() const {
//		cerr << "new PosBasicBlock iterator" << endl;
		return new PosBasicBlockIter(*((PosBasicBlock*) this));
	}
	PosBlockIter* getIterator(unsigned int startpos, unsigned int endpos) const {
	//	cerr << "New PosBasicBlock s/e iter" << endl;
		return new PosBasicBlockIter(const_cast<PosBasicBlock&>(*this), startpos, endpos);
	}
	
	class PosBasicBlockIter : public PosBlockIter {
	public:
	  PosBasicBlockIter(PosBasicBlock& b) 
	    : pb(&b), bblockIter(pb->bBlock->getIterator()), numOccCounter(0) {
	  }
	  PosBasicBlockIter(PosBasicBlock& b, unsigned int spos, unsigned int epos) : pb(&b), numOccCounter(0) {
	    if (spos < pb->getStartPos()) {
	      spos = pb->getStartPos();
	    }
	    if (epos > pb->getEndPos()) {
	      epos = pb->getEndPos();
	    }
	    
	    assert(spos <= epos);
	    
	    if (spos > pb->getEndPos() || epos < pb->getStartPos()) {
	      bblockIter = new EmptyBlockIter();
	    } else {				
	      bblockIter = pb->bBlock->getIterator();
	    }
	    startpos = spos;
	    endpos = epos;
	  };
	  
	  PosBasicBlockIter(const PosBasicBlockIter& i) : 
	    pb(i.pb), bblockIter(i.bblockIter), numOccCounter(i.numOccCounter) {
	    currPair = i.currPair->clone();
	    startpos = i.startpos;
	    endpos = i.endpos;
	  }
	  virtual ~PosBasicBlockIter(){;}
	  
	  virtual bool hasNext() {
	    return numOccCounter == 0 ? bblockIter->hasNext() : true;
	  }
	  
	  virtual unsigned int getNext() {
	    if (numOccCounter == 0) {
	      currPair = bblockIter->getNext();
	    }
	    assert(currPair != NULL);
	    numOccCounter=(numOccCounter+1)%pb->numOccurences;
	    return currPair->position;
	  }
	  
	  virtual unsigned int peekNext() {
	    if (numOccCounter==0) {
	      return bblockIter->peekNext()->position;
	    }
	    assert(currPair!=NULL);
	    return currPair->position;
	  }
	  virtual unsigned int getCurrLoc() {
	    return bblockIter->getCurrLoc()*pb->numOccurences+numOccCounter;
	  }
	  virtual void resetIterator() {
	    throw UnexpectedException("not supported");
	  }
	  virtual bool skipToPos(unsigned int pos) {
	    if (pos >= pb->getStartPos() && 
		pos <= pb->getEndPos())
	      return true;
	    //pRLEBlock->block->skipToLoc(pos-pRLEBlock->getStartPos()); // FIXME iterator
	    else {
	      std::cerr << "invalid skip to: " << pos << std::endl;
	      throw UnexpectedException("invalid skip");
	    }
	  }
	  virtual PosBlockIter* clone() const {
	    return new PosBasicBlockIter(*this);
	  }
	  virtual int getSize() {
	    if (startpos == pb->getStartPos())
	      return pb->getSize();
	    else
	      return 0;
	  }
	  virtual unsigned int getStartPos() {
	    return startpos;
	  }
	  virtual unsigned int getEndPos() {
	    return endpos;
	  }
	  virtual unsigned int initBitStringIterator(unsigned int pos) {
	    return pb->initBitStringIterator(pos);
	  }
	  virtual int getNextBitString() {
	    return pb->getNextBitString();
	  }
	  virtual bool isPosSorted() {
	    return pb->isPosSorted();
	  }
	  virtual bool isPosContiguous() {
	    return pb->isPosContiguous();
	  }
	  virtual bool isBlockPosSorted() {
	    return pb->isBlockPosSorted();
	  }
	  virtual bool isSparse() {
	    return pb->isSparse();
	  }
	  virtual void posAnd(PosBlockIter* other, PosBlock* toWrite) {
	    throw new UnexpectedException("For now I don't see this being usedfor PosBasicBlocks. If that changes, will implement later.");
	  }
	  virtual PosBlock* clone(PosBlock& pb) {
	  	return new PosBasicBlockIter((PosBasicBlockIter&) pb);
	  }
	  virtual unsigned int getPosAtLoc(unsigned int loc_) {
	  	currPair=pb->bBlock->getPairAtLoc(loc_/numOccurences);
  		assert(currPair!=NULL);
  		return currPair->position;	
	  }
	  virtual PosBlock* getPosBlock() {
	  	return pb;
	  }
	protected:
	  PosBasicBlock* pb;
	  BlockIter* bblockIter;
	  int numOccCounter;
	  ValPos* currPair;
	  unsigned int startpos;
	  unsigned int endpos;
	};
	
protected:
	BasicBlock* bBlock;
	bool posSorted;
	int numOccCounter;
	ValPos* currPair;
	int bitString;

};

#endif //_POSBASICBLOCK_H_
