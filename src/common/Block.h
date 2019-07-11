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
#ifndef BLOCK_H
#define BLOCK_H

#pragma once

#include "Constants.h"
#include "Predicate.h"
#include "IntValPos.h"
#include "NullValPos.h"
#include "LongValPos.h"
#include "FloatValPos.h"
#include "DoubleValPos.h"
#include "StringValPos.h"
#include "UnexpectedException.h"
//#include "PosBlockArray.h"

class Cursor;
class Operator;
class BlockIter;
class Type2Block;

// Interface for data object passed between operators
class Block {
public:	
	Block();
	Block(const Block&);
	virtual ~Block();	

	virtual Block* clone(Block&) = 0;

	// Iterator access to block
	virtual bool hasNext()=0;
	// returns true if has next value and next value is equal to value_ 
	virtual bool hasNext(int value_)=0;
	virtual ValPos* getNext()=0;
	virtual ValPos* peekNext()=0;
	virtual void setType(short type_);
	virtual void setType(ValPos* vp_);
	
	// Split the block such that it retains the first npositions positions and
	// the remaining positions are in the returned Block. 
//	virtual Block* split(int npositions);

	// Method to determine whether or not a Block can be split
//	virtual bool isSplittable();

	// Zero indexed, gets the pair at this loc
	virtual ValPos* getPairAtLoc(unsigned int loc_)=0;
	
	//Like getPairAtLoc except set the regular pair variable rather than 
	//the utilityPair variable so can be called by getNext(). 
	virtual ValPos* getPairAtLocNotUtility(unsigned int loc_)=0;
	virtual int getCurrLoc()=0;

	// return size of block	(number of values)
	virtual int getSize()=0;
	//return buffer size in block
	virtual int getSizeInBits()=0;
	virtual ValPos* getStartPair()=0;
	virtual unsigned int getEndPosition()=0;

	virtual void resetBlock()=0;
	virtual bool applyPredicate(Type2Block* t2b, Predicate* p);

	// Stream properties
	virtual bool isValueSorted()=0;	
	virtual bool isPosSorted()=0;
	
	// Block properties
	virtual bool isOneValue()=0;
	virtual bool isPosContiguous()=0;
	virtual bool isBlockValueSorted()=0;	
	virtual bool isBlockPosSorted()=0;
	
	virtual BlockIter* getIterator() const {
	  throw UnexpectedException("unsupported");
	}
	
	virtual BlockIter* getIterator(unsigned int startPos, unsigned int endPos) const {
	  throw UnexpectedException("unsupported");
	}

	virtual Block* getBlock() { return this; }
	
	ValPos* vp;
	short valsize;

protected:
	ValPos* utilityVP;
};

class BlockIter : public Block {
	public:
//		BlockIter() : Block(), decomp_array(NULL) {;}
		virtual ~BlockIter() {;}
		virtual void resetIterator() = 0;
  		virtual int getSizeInBits() { throw UnexpectedException("unimplemented"); }
  		virtual void resetBlock() { resetIterator(); } 
		virtual Block* getBlock() = 0; 
		virtual bool skipToLoc(unsigned int loc_) { throw UnexpectedException("unimplemented"); };
		// Caller should not delete [] returned memory
		virtual const byte* asArray() { throw UnexpectedException("unimplemented"); };
	protected:
};

class EmptyBlockIter : public BlockIter {
	virtual bool hasNext()  { return false; }
	virtual bool hasNext(int value_)  { return false; }
	virtual ValPos* getNext() { throw UnexpectedException("not implemented"); }
	virtual ValPos* peekNext()  { throw UnexpectedException("not implemented"); }
	virtual void resetIterator() {throw UnexpectedException("not implemented"); }
	virtual int getCurrLoc()  { throw UnexpectedException("not implemented"); }
	//virtual bool skipToLoc(unsigned int loc_) { throw UnexpectedException("not implemented"); }
	virtual int getSize()  { return 0; }
	virtual ValPos* getStartPair() { return NULL; }
	virtual unsigned int getEndPosition() { return 0; }
	virtual bool isPosSorted() {return true; }
	virtual bool isPosContiguous() { return true; }
	virtual bool isBlockPosSorted() { return true; }
	virtual Block* getBlock() { throw UnexpectedException("not implemented"); }
	virtual Block* clone(Block& b) { return new EmptyBlockIter((EmptyBlockIter&)b); }
	virtual ValPos* getPairAtLoc(unsigned int loc_) { throw new UnexpectedException("not implemented"); }
	virtual ValPos* getPairAtLocNotUtility(unsigned int loc_) { throw new UnexpectedException("not implemented"); }
	virtual bool isValueSorted() { return true; }
	virtual bool isOneValue() { return true; }
	virtual bool isBlockValueSorted() { return true; }
	const byte* asArray() {return NULL; }
};

#endif // BLOCK_H
