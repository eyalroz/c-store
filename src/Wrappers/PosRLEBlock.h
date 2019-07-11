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
#ifndef _POSRLEBLOCK_H_
#define _POSRLEBLOCK_H_

#include "../common/PosBlock.h"
#include "RLEBlock.h"

// Position RLE Blocks

class PosRLEBlock : public PosBlock
{
public:  
	PosRLEBlock();
	PosRLEBlock(RLETriple* triple_);
	PosRLEBlock(const PosRLEBlock& block_);
	virtual ~PosRLEBlock();

	PosBlock* clone(PosBlock&);

	virtual void setTriple(RLETriple* triple);
	virtual RLETriple* getTriple();

	virtual bool hasNext();
	virtual unsigned int getNext();
	virtual unsigned int peekNext();

	// Zero indexed, gets the pair at this loc
	virtual unsigned int getPosAtLoc(unsigned int loc_);
	virtual unsigned int getCurrLoc();

	// return size of block		
	virtual int getSize();
	virtual int getSizeInBits();
	virtual unsigned int getStartPos();
	virtual unsigned int getEndPos();
	virtual void resetBlock();

	virtual bool skipToPos(unsigned int pos);
	virtual PosBlock* copySubset(int fromPos, int untilPos);
	virtual unsigned int initBitStringIterator(unsigned int pos);
	virtual int getNextBitString();
	
	// Stream properties
	virtual bool isPosSorted();
	
	// Block properties
	virtual bool isPosContiguous();
	virtual bool isBlockPosSorted();
	virtual bool isSparse();
	
	virtual PosBlockIter* getIterator() const {
		//cerr << "new PosRLEBlock iter" << endl;
		return new PosRLEBlockIter(const_cast<PosRLEBlock*>(this));
	}
	PosBlockIter* getIterator(unsigned int startpos, unsigned int endpos) const {
		//cerr << "New PosRLEBlock s/e iter" << endl;
		return new PosRLEBlockIter(const_cast<PosRLEBlock*>(this), startpos, endpos);
	}
	
	class PosRLEBlockIter : public PosBlockIter {
	public:
	  PosRLEBlockIter(PosRLEBlock* pRLEBlock_) 
	    : pRLEBlock(pRLEBlock_), blockIter(pRLEBlock->block->getIterator()), numOccCounter(0), currPair(NULL) {
	    andTrip=NULL;
	    startpos = pRLEBlock->getStartPos();
	    endpos = pRLEBlock->getEndPos();
	  }
	  PosRLEBlockIter(PosRLEBlock* pRLEBlock_, unsigned int spos, unsigned int epos) 
	    : pRLEBlock(pRLEBlock_), blockIter(NULL), numOccCounter(0), currPair(NULL) {
	    
	    andTrip=NULL;
	    if (spos < pRLEBlock->getStartPos()) {
	      spos = pRLEBlock->getStartPos();
	    }
	    if (epos > pRLEBlock->getEndPos()) {
	      epos = pRLEBlock->getEndPos();
	    }
	    
	    assert(spos <= epos);
	    
	    if (spos > pRLEBlock->getEndPos() || epos < pRLEBlock->getStartPos()) {
	      blockIter = new EmptyBlockIter();
	    } else {				
//	    	if (spos - pRLEBlock->getStartPos() == 0) {
//		      blockIter = pRLEBlock->block->getIterator(0, 
//							epos - pRLEBlock->getStartPos());
//	    	} else {
	    		blockIter = pRLEBlock->block->getIterator(spos,epos);
//	    	}
	    }
	    startpos = spos;
	    endpos = epos;
	    assert(blockIter->peekNext()->position == startpos);
	  //  cerr << "New PosRLEBlockIter on " << startpos << " to " <<  endpos << endl;
	  }
	  virtual ~PosRLEBlockIter();
	  
	  virtual bool hasNext() {
	  	return numOccCounter == 0 ? blockIter->hasNext() : true;
	  }
	  virtual unsigned int getNext() {
	  	if (numOccCounter == 0) {
			currPair = blockIter->getNext();	
		} 
		assert(currPair != NULL);
		numOccCounter=(numOccCounter+1)%pRLEBlock->numOccurences;
		return currPair->position;
	  }
	  
	  virtual unsigned int peekNext();
	  virtual unsigned int getCurrLoc();
	  virtual void resetIterator();
	  virtual bool skipToPos(unsigned int pos);
	  virtual int findLocAtPosAndSkip(unsigned int pos);
	  
	  virtual PosBlockIter* clone() const { return new PosRLEBlockIter(*this); }
	  virtual PosBlock* clone(PosBlock& pb) { 
	    return new PosRLEBlockIter((PosRLEBlockIter&)pb);
	  }
	  unsigned int getPosAtLoc(unsigned int loc_) {
	    //currPair=pRLEBlock->block->getPairAtLoc((loc_+(getStartPos()-pRLEBlock->getStartPos()))/numOccurences);
	    
	    unsigned int sp = getStartPos();
//	    unsigned int ep = getEndPos();
	    assert (loc_ <= (getEndPos() -sp));
	      return sp + (loc_/numOccurences);
	    //numOccCounter=loc_%numOccurences;
	    //if (currPair==NULL) throw new CodingException("PosRLEBlock: error no next position");
	    //return currPair->position;	
	  }
	  virtual int getSize();
	  virtual unsigned int getStartPos();
	  virtual unsigned int getEndPos();
	  virtual unsigned int initBitStringIterator(unsigned int pos);
	  virtual int getNextBitString();
	  virtual void posAnd(PosBlockIter* other, PosBlock* toWrite);
	  virtual bool isPosSorted();
	  virtual bool isPosContiguous();
	  virtual bool isBlockPosSorted();
	  virtual bool isSparse();
	  virtual PosBlock* getPosBlock() { return pRLEBlock; }
	protected:
	  PosRLEBlock* pRLEBlock;
	  RLETriple* andTrip;
	  BlockIter* blockIter;
	  int numOccCounter;
	  ValPos* currPair;
	  unsigned int startpos;
	  unsigned int endpos;
	  //	MultiBlock::MultiBlockIter* mbIter;
	};
protected:
	RLEBlock* block;
	RLETriple* trip;
	int numOccCounter;
	ValPos* currPair;
	int bitStringCounter;
	int bitstringsize;
	int bitStringStart;
	int bitStringEnd;
};

#endif //_POSRLEBLOCK_H_
