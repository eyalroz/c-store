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
// Sparse value block

#ifndef _SPARSEBLOCK_H_
#define _SPARSEBLOCK_H_

#include "MultiBlock.h"
#include "Type2Block.h"
#include "../common/PosBlock.h"
#include "CodingException.h"
#include <iostream>

class SparseBlock : public MultiBlock
{
public:
	SparseBlock(bool valSorted_, bool posContig_, bool posSorted_);
	SparseBlock(bool valSorted_, bool posContig_, bool posSorted_, short type_);
	SparseBlock(bool valSorted_, bool posContig_, bool posSorted_, ValPos* vp_);
	SparseBlock(const SparseBlock&);
	virtual ~SparseBlock();
	Block* clone(Block&);
	virtual void setBuffer(int startpos_, int endpos_, int numValues_, byte* buffer_);
	void setBufferDirect(int startpos_, int endpos_, int numValues_, byte* buffer_);
	void setPosBlock(PosBlock* p);
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
	SparseBlock* copySubset(int fromPos, int untilPos);

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
	virtual bool applyPredicate(Type2Block* t2b, Predicate* p);

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
		return new SparseBlockIter(const_cast<SparseBlock*>(this));
	}
	
	virtual BlockIter* getIterator(unsigned int startpos, unsigned int endpos) const {
		return new RangedSparseBlockIter(const_cast<SparseBlock*>(this), startpos, endpos);
	}
	
	class SparseBlockIter : public MultiBlockIter {
		public:
			SparseBlockIter(SparseBlock* sb_) : MultiBlockIter(sb_) {
			  //pair = new Pair();
			  //utilityPair = new Pair();
			  //pair->value = *currPosPtr;
			  //pair->position = sb->startPos;
				arrayBuffer=NULL;
				pbi=sb->pb->getIterator();
			}
			virtual ~SparseBlockIter() {}
			virtual bool hasNext() ;
			virtual bool hasNext(int value_) ;
			virtual ValPos* getNext();
			virtual ValPos* peekNext() ;
			virtual void resetIterator();
			virtual int getCurrLoc() ;
			virtual bool skipToLoc(unsigned int loc_);
			virtual bool skipToBLoc(unsigned int loc_);
			virtual int getSize()  { return sb->getSize(); }
  			virtual ValPos* getStartPair()  { return sb->getStartPair(); }
	 		virtual unsigned int getEndPosition()  { return sb->getEndPosition(); }
	 		virtual bool isPosSorted() { return sb->isPosSorted(); }
 	 		virtual bool isPosContiguous() { return sb->isPosContiguous(); }
  			virtual bool isBlockPosSorted() { return sb->isBlockPosSorted(); }
  			virtual Block* getBlock() { return sb; }
  			virtual Block* clone(Block& b) { return new SparseBlockIter((SparseBlockIter&)b); }
  			virtual bool isValueSorted() { return sb->isValueSorted(); }
  			virtual bool isOneValue() { return sb->isOneValue(); }
  			virtual bool isBlockValueSorted() { return sb->isBlockValueSorted(); }
  			virtual ValPos* getPairAtLocNotUtility(unsigned int loc_);
  			virtual ValPos* getPairAtLoc(unsigned int loc_);
  			virtual void setValAtLoc(unsigned int loc_, int val);
  			virtual const byte* asArray();
		protected:
			SparseBlock* sb;
			PosBlockIter* pbi;
			int nullCount;
			byte* arrayBuffer;
			//Pair pair;
			//mutable Pair utilityPair;
	};
	
	class RangedSparseBlockIter : public SparseBlockIter 
	{
		public:
			RangedSparseBlockIter(SparseBlock* sb, unsigned int startPos_, unsigned int endPos_);
			virtual bool hasNext();
			virtual bool hasNext(int value_);
			virtual void resetIterator();
			//virtual bool skipToLoc(unsigned int loc_);
			virtual int getSize() { return endpos-startpos+ 1; }
			virtual int getCurrLoc() ;
			virtual ValPos* getStartPair() { return getPairAtLoc(0); }
			virtual unsigned int getEndPosition() { return endpos; }			
  			//virtual Pair* getPairAtLocNotUtility(unsigned int loc_);
  			//virtual Pair* getPairAtLoc(unsigned int loc_);	
  			//virtual void setValAtLoc(unsigned int loc_, int val);
  			virtual const byte* asArray();
		protected:
			int numPositions;
			unsigned int startLoc;
			//unsigned int endLoc;
			unsigned int startpos;	
			unsigned int endpos;
	};
		
protected:
	int numPositions;
	int endPos;
	PosBlock* pb;
	PosBlockIter* pbi;
	int nullCount;
	byte* nullval;
	byte* startPtr;
};

#endif //_SPARSEBLOCK_H_
