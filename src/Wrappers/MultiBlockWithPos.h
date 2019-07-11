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
// Multi value block using PosBlockArray


#ifndef _MULTIBLOCKWITHPOS_H_
#define _MULTIBLOCKWITHPOS_H_

#include "MultiBlock.h"
#include "CodingException.h"
#include <iostream>
#include "../common/PosBlockArray.h"

class MultiBlockWithPos : public MultiBlock
{
public:
	MultiBlockWithPos(bool valSorted_, bool posContig_, bool posSorted_);
	MultiBlockWithPos(const MultiBlockWithPos&);
	virtual ~MultiBlockWithPos();
	Block* clone(Block&);
	void setMB(MultiBlock* block);
	//virtual void setBuffer(int startpos_, int numValues_, byte* buffer_);
	virtual void resetBlock();
	// Iterator access to block
	virtual bool hasNext() { return ourIter.hasNext(); }
	virtual bool hasNext(int value_) { return ourIter.hasNext(value_); }
	virtual Pair* getNext() { return ourIter.getNext(); }
	virtual Pair* peekNext() { return ourIter.peekNext(); }
	//DSM virtual int getValue();
	//DSM virtual unsigned int getPosition();
	virtual unsigned int getLastPosition();
	virtual unsigned int getEndPosition();
	//virtual void cutOffEnd(int endPos);

	// Zero indexed, gets the pair at this pos_ and points that 
	// Cursor to the following pair
	virtual Pair* getPairAtLoc(unsigned int loc_);
	virtual Pair* getPairAtLocPair(unsigned int loc_); 
	virtual Pair* getPairAtDerivedLoc(unsigned int loc_);
	virtual int getCurrLoc();

	// return size of block (number of values)
	virtual int getSize();
	//return buffer size in block
	virtual int getSizeInBits(); 
	virtual Pair* getStartPair();

	/*
	// Stream properties
	virtual bool isValueSorted();	
	virtual bool isPosSorted();*/
	
	// Block properties
	virtual bool isOneValue();
	
	virtual bool isPosContiguous();
	/*virtual bool isBlockValueSorted();	
	virtual bool isBlockPosSorted();*/
	
	//virtual bool isSplittable();
	//Block* split(int npositions);
	
	
	virtual BlockIter* getIterator() {
		return new MultiBlockWithPosIter(this);
	}

	class MultiBlockWithPosIter : public BlockIter {
	public:
		MultiBlockWithPosIter(MultiBlockWithPos* mbwp) : pbIter(mbwp->getPosBlockArray()->getIterator()), mblock(mbwp) {
			cerr << "new MultiBlocckWithPos iter" << endl;
		}
		~MultiBlockWithPosIter() { delete pbIter; }
		
		virtual bool hasNext() const {
			return pbIter->hasNext();
		}
		// returns true if has next value and next value is equal to value_ 
		virtual bool hasNext(int value_) const {
			return return ((pbIter->hasNext()) && 
					((mblock->getPairAtLoc(pbIter->peekNext() - mblock->startPos)->value) == value_));
		}
		virtual Pair* getNext() {
			return mblock->getPairAtLocPair(pbIter->getNext() - mblock->startPos);
		}
		virtual Pair* peekNext() const {
			return mblock->getPairAtLoc(pbIter->peekNext() - mblock->startPos);
		}
		virtual void getCurrLoc() const {
			return pbIter->getCurrLoc();
		}
		virtual void resetIterator() {
			pbIter->resetIterator();
		}
		virtual Block* getBlock() { return mblock; }
	protected:
		PosBlockArray::PosBlockArrayIter* pbIter;
		const MultiBlockWithPos* mblock;
		
	};	
	
	MultiBlockWithPosIter ourIter;
};

#endif //_MULTIBLOCKWITHPOS_H_
