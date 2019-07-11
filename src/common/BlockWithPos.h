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
// Generalized block using PosBlockArray


#ifndef _BLOCKWITHPOS_H_
#define _BLOCKWITHPOS_H_

#include "BlockPosNonContig.h"
#include "PosBlock.h"
#include <iostream>

class BlockWithPos : public BlockPosNonContig
{
public:
	BlockWithPos();
	BlockWithPos(const BlockWithPos&);
	virtual ~BlockWithPos();
	Block* clone(Block&);
	void setBlock(Block* block);
	//virtual void setBuffer(int startpos_, int numValues_, byte* buffer_);
	virtual void resetBlock();
	// Iterator access to block
	virtual bool hasNext();
	virtual bool hasNext(int value_);
	virtual ValPos* getNext();
	virtual ValPos* peekNext();
	//virtual int getValue();
	//virtual unsigned int getPosition();
	virtual unsigned int getLastPosition();
	virtual unsigned int getEndPosition();
	//virtual void cutOffEnd(int endPos);

	virtual ValPos* getPairAtLoc(unsigned int loc_);
	
	//Like getPairAtLoc except set the regular pair variable rather than 
	//the utilityPair variable so can be called by getNext(). 
	virtual ValPos* getPairAtLocNotUtility(unsigned int loc_);
	//virtual Pair* getPairAtLocPair(unsigned int loc_);
	//virtual Pair* getPairAtDerivedLoc(unsigned int loc_);
	virtual int getCurrLoc();

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
	
	//virtual bool isPosContiguous();
	virtual bool isBlockValueSorted();	
	virtual bool isBlockPosSorted();
	
	virtual void setPosBlockIter(PosBlockIter* pb); //, bool save_old_bounds = false);
	virtual PosBlockIter* getPosBlockIter();

	//virtual bool isSplittable();
	//Block* split(int npositions);
	//int currPos;
	
	virtual BlockIter* getIterator() const {
		return new BlockWithPosIter((BlockWithPos*)this);
	}
	
	virtual BlockIter* getIterator(unsigned int startPos, unsigned int endPos) const {
		return new BlockWithPosIter((BlockWithPos*)this, startPos, endPos);
	}
	
	virtual BlockIter* getIterator(PosBlockIter* pbi) {
		return new BlockWithPosIter((BlockWithPos*) this, pbi);
	}
	
	class BlockWithPosIter : public BlockIter {
	public:
	  BlockWithPosIter(BlockWithPos* bwp_);
	  BlockWithPosIter(BlockWithPos* bwp_, int startPos, int endPos);
	  BlockWithPosIter(BlockWithPos* bwp_, PosBlockIter* pbi);
			
			~BlockWithPosIter() {
				delete [] decomp_array;
			}
										
			virtual bool hasNext() {
				return pbIter->hasNext();
			}
			virtual ValPos* peekNext() {
				return bwp->baseBlock->getPairAtLoc(pbIter->peekNext() - bwp->startPos);
			}
			// returns true if has next value and next value is equal to value_ 
			virtual bool hasNext(int value_) {
				return pbIter->hasNext() && ((*(bwp->baseBlock->getPairAtLoc(pbIter->peekNext() - bwp->startPos))->value) == value_);
			}
			virtual ValPos* getNext();
			virtual void resetIterator() {
				pbIter->resetIterator();
			}
			virtual int getCurrLoc() {
				return pbIter->getCurrLoc();
			}
			virtual bool skipToLoc(unsigned int loc) {
//			return pbIter->skipToLoc(loc);
				throw UnexpectedException();
			}
			virtual int getSize() {
				return pbIter->getSize();
			}
	  		virtual ValPos* getStartPair() {
	  			return bwp->getPairAtLoc(0); // DANGER
	  		}
		 	virtual unsigned int getEndPosition() {
		 		return pbIter->getEndPos();
		 	}
		 	virtual bool isPosSorted() {
		 		return pbIter->isPosSorted();
		 	}
	 	 	virtual bool isPosContiguous() {
	 	 		return pbIter->isPosContiguous();
	 	 	}
	  		virtual bool isBlockPosSorted() {
	  			return pbIter->isPosContiguous();
	  		}
	  		virtual Block* getBlock();
	  		virtual Block* clone(Block& b) { return new BlockWithPosIter((BlockWithPosIter&)b); }
	  		virtual ValPos* getPairAtLoc(unsigned int loc_) {
	  			return bwp->getPairAtLoc(loc_);
	  		}
	  		virtual ValPos* getPairAtLocNotUtility(unsigned int loc_) { throw UnexpectedException(); }
	  		virtual bool isValueSorted() { return bwp->isValueSorted(); }
	  		virtual bool isOneValue() { return bwp->isOneValue(); }
	  		virtual bool isBlockValueSorted() { return bwp->isBlockValueSorted(); }
	  		const byte* asArray();
		protected:
			BlockWithPos* bwp;
			PosBlockIter* pbIter;
			byte* decomp_array;
	};
protected:

	//PosBlockArray* posBlockArray;
	//PosBlockIter* pbIter;
	//PosBlock* posBlock;
	PosBlockIter* pbIter;
	Block* baseBlock;
	int startPos;

};

#endif //_BLOCKWITHPOS_H_
