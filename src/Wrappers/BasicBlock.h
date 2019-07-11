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
// Single value block
// NOTE: BASIC BLOCK IS OLD CODE SO IT STILL ASSUMES INTEGERS!!!!
// Problems: migfer@mit.edu


#ifndef _BASICBLOCK_H_
#define _BASICBLOCK_H_

#include "../common/Block.h"
#include "CodingException.h"

class BasicBlock : public Block
{
public:
	BasicBlock(bool valSorted_, bool posContig_, bool posSorted_);
	BasicBlock(bool valSorted_, bool posContig_, bool posSorted_, short type_);
	BasicBlock(bool valSorted_, bool posContig_, bool posSorted_, ValPos* vp_);
	BasicBlock(const BasicBlock&);
	virtual ~BasicBlock();
	Block* clone(Block&);
	virtual void setValue(int val_, int pos_);
	virtual void setValue(ValPos* val_);
	virtual void resetBlock();
	// Iterator access to block
	virtual bool hasNext();
	virtual bool hasNext(int value_);
	virtual ValPos* getNext();
	virtual ValPos* peekNext();
	virtual int getValue();
	virtual unsigned int getPosition();
	virtual unsigned int getEndPosition();

	// Zero indexed, gets the pair at this pos_
	virtual ValPos* getPairAtLoc(unsigned int loc_);
	
	//Like getPairAtLoc except set the regular pair variable rather than 
	//the utilityPair variable so can be called by getNext(). 
	virtual ValPos* getPairAtLocNotUtility(unsigned int loc_);
	virtual int getCurrLoc();

	// return size of block		
	virtual int getSize();
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
	
	int currPos;
	
	BlockIter* getIterator() const {
	//	cerr << "new basic block iterator" << endl;
	//	throw UnexpectedException("not implemented");
		return new BasicBlockIter((BasicBlock&)*this);
	}
	
	class BasicBlockIter : public BlockIter {
		public:
			BasicBlockIter(BasicBlock& bb_) : bb(bb_), currPos(bb_.currPos)  {;}
			virtual ~BasicBlockIter() {;}
			virtual bool hasNext() { return currPos < 0; }
			// returns true if has next value and next value is equal to value_ 
			virtual bool hasNext(int value_) { return currPos < 0 && *(bb.vp) == value_; }
			virtual ValPos* getNext() { 
				if (currPos < 0) {
					currPos = 0;
					return bb.vp;
				} else {
					return NULL;
				}
			}
			virtual ValPos* peekNext() { return currPos < 0 ? bb.vp : NULL; }
			virtual void resetIterator() { throw UnexpectedException("not supported"); }
			virtual int getCurrLoc() { return currPos + 1; }
			virtual bool skipToLoc(unsigned int loc_) {
				throw UnexpectedException("not implemented");
			}
			virtual int getSize() { return const_cast<BasicBlock&>(bb).getSize(); }
  			virtual ValPos* getStartPair() { return const_cast<BasicBlock&>(bb).getStartPair(); }
	 		virtual unsigned int getEndPosition() { return const_cast<BasicBlock&>(bb).getEndPosition(); }
	 		virtual bool isPosSorted() { return const_cast<BasicBlock&>(bb).isPosSorted(); }
 	 		virtual bool isPosContiguous() { return const_cast<BasicBlock&>(bb).isPosSorted(); }
  			virtual bool isBlockPosSorted() { return const_cast<BasicBlock&>(bb).isBlockPosSorted(); }
  			virtual bool isSparse() { return false; }
  			virtual Block* getBlock() { return (Block*) &bb; }
  			virtual Block* clone(Block& b) { return new BasicBlock( (BasicBlock&)b); }
  			virtual ValPos* getPairAtLoc(unsigned int loc) { throw UnexpectedException(); }
  			virtual ValPos* getPairAtLocNotUtility(unsigned int loc) { throw UnexpectedException(); }
  			virtual bool isValueSorted() { return bb.isValueSorted(); }
  			virtual bool isOneValue() { return bb.isOneValue(); }
  			virtual bool isBlockValueSorted() { return bb.isBlockValueSorted(); }
		protected:
			BasicBlock& bb;
			int currPos;
	};
protected:
	bool valSorted;
	bool posContig;
	bool posSorted;

	bool pairNewed;
};

#endif //_BASICBLOCK_H_
