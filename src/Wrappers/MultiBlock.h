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
// Multi value block

#ifndef _MULTIBLOCK_H_
#define _MULTIBLOCK_H_

#include "../common/Block.h"
#include "Type2Block.h"
#include "CodingException.h"
#include <iostream.h>

class MultiBlock : public Block
{
public:
	MultiBlock(bool valSorted_, bool posContig_, bool posSorted_);
	MultiBlock(bool valSorted_, bool posContig_, bool posSorted_, short type_);
	MultiBlock(bool valSorted_, bool posContig_, bool posSorted_, ValPos* vp_);
	MultiBlock(const MultiBlock&);
	virtual ~MultiBlock();
	Block* clone(Block&);
	virtual void init(bool valSorted_, bool posContig_, bool posSorted_);
	virtual void setBuffer(int startpos_, int numValues_, byte* buffer_);
	void setBufferDirect(int startpos_, int numValues_, byte* buffer_);
	//virtual void setValue(Pair* val_);
	virtual void resetBlock();
	// Iterator access to block
	virtual bool hasNext();
	virtual bool hasNext(int value_);
	virtual ValPos* getNext();
	virtual ValPos* peekNext();
	virtual int getValue();
	virtual unsigned int getPosition();
	virtual unsigned int getLastPosition();
	virtual unsigned int getEndPosition();
	virtual void cutOffEnd(int endPos);
	virtual byte* getBuffer();
	MultiBlock* copySubset(int fromPos, int untilPos);
	bool applyPredicate(Type2Block* t2b, Predicate* p);

	// Zero indexed, gets the pair at this pos_
	virtual ValPos* getPairAtLoc(unsigned int loc_);
	
	//Like getPairAtLoc except set the regular pair variable rather than 
	//the utilityPair variable so can be called by getNext(). 
	virtual ValPos* getPairAtLocNotUtility(unsigned int loc_);
	virtual int getCurrLoc();
	virtual void setValAtLoc(unsigned int loc_, int val);
	virtual bool skipToLoc(unsigned int loc_);

	// return size of block (number of values)
	virtual int getSize();
	//return buffer size in block
	virtual int getSizeInBits();
	virtual ValPos* getStartPair();

	// Stream properties
	virtual bool isValueSorted();	
	virtual bool isPosSorted();
	
	// Block properties
	virtual bool isOneValue();
	virtual bool isPosContiguous();
	virtual bool isBlockValueSorted();	
	virtual bool isBlockPosSorted();
	
	virtual bool isSplittable();
	Block* split(int npositions);
	
	int currPos;
	
	virtual BlockIter* getIterator() const {
		return new MultiBlockIter(const_cast<MultiBlock*>(this));
	}
	
	virtual BlockIter* getIterator(unsigned int startpos, unsigned int endpos) const {
		return new RangedMultiBlockIter(const_cast<MultiBlock*>(this), startpos, endpos);
	}
	
	class MultiBlockIter : public BlockIter {
		public:
			MultiBlockIter(MultiBlock* mb_) : mb(mb_), currPos(-1), 
											currPosPtr(mb->buffer) {
			  valsize = mb->valsize;
			  vp = mb_->vp->clone();
			  resetIterator(); //set vp to first value
			  utilityVP = vp->clone();
			  //utilityPair = new Pair();
			  //pair->value = *currPosPtr;
			  //pair->position = mb->startPos;
			}
			virtual ~MultiBlockIter() {delete vp; delete utilityVP; }
			virtual bool hasNext() ;
			virtual bool hasNext(int value_) ;
			virtual ValPos* getNext();
			virtual ValPos* peekNext() ;
			virtual void resetIterator();
			virtual int getCurrLoc() ;
			virtual bool skipToLoc(unsigned int loc_);
			virtual int getSize()  { return mb->getSize(); }
  			virtual ValPos* getStartPair()  { return mb->getStartPair(); }
	 		virtual unsigned int getEndPosition()  { return mb->getEndPosition(); }
	 		virtual bool isPosSorted() { return mb->isPosSorted(); }
 	 		virtual bool isPosContiguous() { return mb->isPosContiguous(); }
  			virtual bool isBlockPosSorted() { return mb->isBlockPosSorted(); }
  			virtual Block* getBlock() { return mb; }
  			virtual Block* clone(Block& b) { return new MultiBlockIter((MultiBlockIter&)b); }
  			virtual bool isValueSorted() { return mb->isValueSorted(); }
  			virtual bool isOneValue() { return mb->isOneValue(); }
  			virtual bool isBlockValueSorted() { return mb->isBlockValueSorted(); }
  			virtual ValPos* getPairAtLocNotUtility(unsigned int loc_);
  			virtual ValPos* getPairAtLoc(unsigned int loc_);
  			virtual void setValAtLoc(unsigned int loc_, int val);
  			virtual const byte* asArray();
		protected:
			MultiBlock* mb;
			int currPos;
			byte* currPosPtr;
			//Pair pair;
			//mutable Pair utilityPair;
	};
	
	class RangedMultiBlockIter : public MultiBlockIter 
	{
		public:
			RangedMultiBlockIter(MultiBlock* mb, unsigned int startPos_, unsigned int endPos_);
			virtual bool hasNext();
			virtual bool hasNext(int value_);
			virtual void resetIterator();
			virtual bool skipToLoc(unsigned int loc_);
			virtual int getSize() { return endpos-startpos+ 1; }
			virtual int getCurrLoc() ;
			virtual ValPos* getStartPair() { return getPairAtLoc(0); }
			virtual unsigned int getEndPosition() { return endpos; }			
  			virtual ValPos* getPairAtLocNotUtility(unsigned int loc_);
  			virtual ValPos* getPairAtLoc(unsigned int loc_);	
  			virtual void setValAtLoc(unsigned int loc_, int val);
  			virtual const byte* asArray();
		protected:
			unsigned int startLoc;
			unsigned int endLoc;
			unsigned int startpos;	
			unsigned int endpos;		
	};
		
protected:
	bool valSorted;
	bool posContig;
	bool posSorted;

	bool bufferdirect;
	byte* buffer;
	int buffer_size;
	int numValues;
	//int startVal;
	int startPos;
	byte* currPosPtr;
	
};

#endif //_MULTIBLOCK_H_
