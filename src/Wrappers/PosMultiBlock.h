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
#ifndef _POSMULTIBLOCK_H_
#define _POSMULTIBLOCK_H_

#include "../common/PosBlock.h"
#include "MultiBlock.h"
#include "../common/UnimplementedException.h"
class PosMultiBlock : public PosBlock
{
public:
	PosMultiBlock(bool posSorted_);
	PosMultiBlock(MultiBlock* mb);
	virtual ~PosMultiBlock();
	PosMultiBlock(const PosMultiBlock& block_);

	PosBlock* clone(PosBlock&);
	
	unsigned int getPosition();
	
	void setMB(MultiBlock* mb);

	// Iterator access to block
	virtual bool hasNext();
	virtual unsigned int getNext();
	virtual unsigned int peekNext();
	// Zero indexed, gets the pair at this loc
	virtual unsigned int getPosAtLoc(unsigned int loc_);
	virtual unsigned int getCurrLoc();
	
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
	  //cerr << "new PosMultiBlock iterator" << endl;
		return new PosMultiBlockIter(const_cast<PosMultiBlock*>(this));
	}
	PosBlockIter* getIterator(unsigned int startpos, unsigned int endpos) const {
	  //cerr << "New PosMultiBlock s/e iter" << endl;
		return new PosMultiBlockIter(const_cast<PosMultiBlock*>(this), startpos, endpos);
	}

	class PosMultiBlockIter : public PosBlockIter {
	public:
	  PosMultiBlockIter(PosMultiBlock* pmb_);
	  PosMultiBlockIter(PosMultiBlock* pmb_, unsigned int spos, unsigned int epos);
	  virtual ~PosMultiBlockIter(){;}
	  virtual bool hasNext();
	  virtual unsigned int getNext();
	  virtual unsigned int peekNext();
	  virtual unsigned int getCurrLoc();
	  virtual void resetIterator();
	  virtual bool skipToPos(unsigned int pos);
	  virtual PosBlockIter* clone()
	   {
	    return new PosMultiBlockIter(*this);
	  }
	  
	  virtual int getSize();
	  virtual unsigned int getStartPos();
	  virtual unsigned int getEndPos();
	  virtual unsigned int initBitStringIterator(unsigned int pos);
	  virtual int getNextBitString();
	  virtual void posAnd(PosBlockIter* other, PosBlock* toWrite) { 
	    throw new UnexpectedException("For now I don't see this being usedfor PosMultiBlocks. If that changes, will implement later.");
	  }
	  virtual bool isPosSorted();
	  virtual bool isPosContiguous();
	  virtual bool isBlockPosSorted();
	  virtual bool isSparse();
	  virtual PosBlock* clone(PosBlock& pb) {
	    return new PosMultiBlockIter((PosMultiBlockIter&) pb);
	  }
	  virtual unsigned int getPosAtLoc(unsigned int loc_) {
	    return pmb->bBlock->getPairAtLoc(loc_+startLoc)->position;
	  }
	  virtual PosBlock* getPosBlock() { return pmb; }
	protected:
	  PosMultiBlock* pmb;
	  BlockIter* blockIter;
	  unsigned int startpos;
	  unsigned int endpos;
	  unsigned int startLoc;
	  unsigned int endLoc;
	};
	
protected:
	MultiBlock* bBlock;
	bool posSorted;
};

#endif //_POSBASICBLOCK_H_
