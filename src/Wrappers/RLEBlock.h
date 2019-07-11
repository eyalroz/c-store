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
// RLE Block 

#ifndef RLEBLOCK_H
#define RLEBLOCK_H

#include "../common/Block.h"
//#include "../common/Constants.h"
//#include "../common/Interfaces.h"
//#include "../common/Util.h"
#include "RLETriple.h"
#include "CodingException.h"


class RLEBlock : public Block {
public:
	RLEBlock(bool valSorted_); // must be immediately followed by call to setTriple
	RLEBlock(RLETriple* trip_, bool valSorted_);
	RLEBlock(const RLEBlock&);
	virtual ~RLEBlock();
	Block* clone(Block&);
	
	virtual void resetBlock();
	// Iterator access to block
	virtual bool hasNext();
	virtual bool hasNext(int value_);
	virtual ValPos* getNext();
	virtual ValPos* peekNext();

	// Zero indexed, gets the pair at this pos_
	virtual ValPos* getPairAtLoc(unsigned int loc_);
	
	//Like getPairAtLoc except set the regular pair variable rather than 
	//the utilityPair variable so can be called by getNext(). 
	virtual ValPos* getPairAtLocNotUtility(unsigned int loc_);
	virtual int getCurrLoc();
	virtual bool skipToLoc(unsigned int loc_);

	// return size of block		
	virtual int getSize();
	virtual int getSizeInBits();
	virtual ValPos* getStartPair();
	virtual unsigned int getEndPosition();
	
	// Stream properties
	virtual bool isValueSorted();	
	virtual bool isPosSorted();
	
	// Block properties
	virtual bool isOneValue();
	virtual bool isPosContiguous();
	virtual bool isBlockValueSorted();	
	virtual bool isBlockPosSorted();
	
	bool isSplittable();
	Block* split(int npositions);
	
	virtual RLETriple* getTriple();
	virtual void setTriple(RLETriple* trip_);
	
	virtual BlockIter* getIterator() const {
		return new RLEBlockIter((RLEBlock*)this);
	}
	
	virtual BlockIter* getIterator(unsigned int startPos, unsigned int endPos) const {
		return new RangedRLEBlockIter((RLEBlock*) this, startPos, endPos);
	}
	
	class RLEBlockIter : public BlockIter {
		public:
			RLEBlockIter(RLEBlock* rleBlock_) : rleBlock(rleBlock_), adjustedCurPos(0), decomp_array(NULL) {
			  //pair = new Pair();
			  //utilityPair = new Pair();
			  vp = rleBlock_->vp->clone();
			  utilityVP = rleBlock_->utilityVP->clone();
			}
			virtual ~RLEBlockIter() {delete vp; delete utilityVP; delete[] decomp_array;}
			virtual bool hasNext() ;
			virtual bool hasNext(int value_) ;
			virtual ValPos* getNext();
			virtual ValPos* peekNext() ;
			virtual void resetIterator();
			virtual int getCurrLoc() ;
			virtual bool skipToLoc(unsigned int loc_);
			virtual int getSize() { return rleBlock->getSize(); }
  			virtual ValPos* getStartPair() { return rleBlock->getStartPair(); }
	 		virtual unsigned int getEndPosition() { return rleBlock->getEndPosition(); }
	 		virtual bool isPosSorted() { return true; }
 	 		virtual bool isPosContiguous() { return true; }
  			virtual bool isBlockPosSorted() { return true; }
  			virtual bool isBlockValueSorted() { return true; }
  			virtual bool isValueSorted() { return true; }
  			virtual bool isOneValue() { return rleBlock->isOneValue(); }
  			virtual Block* getBlock() { return rleBlock; }
  			virtual Block* clone(Block& b) { return new RLEBlockIter((RLEBlockIter&)b); }
  			virtual ValPos* getPairAtLoc(unsigned int loc_);
  			virtual ValPos* getPairAtLocNotUtility(unsigned int loc_);
  			virtual const byte* asArray();
  			BlockIter* getIterator() const { throw UnexpectedException("unsupported"); }
		protected:
			RLEBlock* const rleBlock;
			unsigned int adjustedCurPos;
			byte* decomp_array;
			//mutable Pair pair; // mutable needed because modified by peekNext
	};
	class RangedRLEBlockIter : public RLEBlockIter {
		public:
			RangedRLEBlockIter(RLEBlock* rleBlock_, unsigned int startPos_, unsigned int endPos_);
			virtual bool hasNext() {
				return adjustedCurPos<=endLoc;
			}
			virtual bool hasNext(int value_) ;
			virtual void resetIterator();
			virtual int getCurrLoc() ;
			virtual bool skipToLoc(unsigned int loc_);
			virtual int getSize() { return getEndPosition() - getStartPair()->position + 1; }
  			virtual ValPos* getStartPair() { return rleBlock->getPairAtLoc(startLoc); }
	 		virtual unsigned int getEndPosition() { return rleBlock->getPairAtLoc(endLoc)->position; }
	 		virtual ValPos* getPairAtLoc(unsigned int loc_);
	 		virtual ValPos* getPairAtLocNotUtility(unsigned int loc_);
	 		BlockIter* getIterator() const { throw UnexpectedException("unsupported"); }
	 		// Can safely inherit asArray from RLEBlockIter due to virtual getSize() function
		protected:
			unsigned int startLoc;
			unsigned int endLoc;
			unsigned int startpos;
	};
	
protected:
	RLETriple* triple;
	unsigned int adjustedCurPos;
	bool valSorted;
	bool createdTriple;
};

#endif // RLEBLOCK_H
