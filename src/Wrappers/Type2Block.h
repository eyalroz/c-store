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

#ifndef _TYPE2BLOCK_H_
#define _TYPE2BLOCK_H_

#include "../common/Block.h"
#include "CodingException.h"
#include "../common/UnimplementedException.h"
#include "DeltaPosBlock.h"
#include "CompDataStructs/Type2Header.h"

class Type2Block : public Block
{
public:
	Type2Block(bool valSorted_);
	//Type2Block(bool valSorted_, short type_);
	Type2Block(const Type2Block&);
	virtual ~Type2Block();
	Block* clone(Block&);
	virtual void init(bool valSorted_);
	//virtual void setBuffer(int startpos_, int numValues_, byte* buffer_);
	virtual void setBufferWithHeader(byte* buffer_);
	virtual void setBuffer(DeltaPosBlock* dpb);
	virtual void initEmptyBuffer(unsigned int startpos, ValPos* startVal);
	//returns the new value of numValues if the position is added
	virtual int testAddPos(unsigned int pos);
	virtual void setCurrEndPos(unsigned int pos);
	virtual void addPosition(unsigned int pos);
	virtual void finishBufferEditing(unsigned int endpos);
	//virtual void setValue(ValPos* val_);
	virtual void resetBlock();
	// Iterator access to block
	virtual bool hasNext();
	virtual bool hasNext(int value_);
	virtual ValPos* getNext();
	virtual ValPos* peekNext();
	virtual ValPos* getValue();
	virtual unsigned int getStartPosition();
	virtual unsigned int getEndPosition();
	virtual unsigned int getPosition();
	virtual unsigned int getLastPosition();
	virtual void cutOffEnd(int endPos_);
	virtual void cutOffStart(int begPos_);
	void printBits(int bits);
	void setPosIndex(int v, int* pidx, int& currIndexInVal);
	byte* getBuffer();
	virtual int initBitStringIterator(unsigned int pos);
	virtual int getNextBitString();
	virtual void setValue(ValPos** valindex);


	// Zero indexed, gets the pair at this pos_
	virtual ValPos* getPairAtLoc(unsigned int loc_);
	
	//Like getPairAtLoc except set the regular pair variable rather than 
	//the utilityPair variable so can be called by getNext(). 
	virtual ValPos* getPairAtLocNotUtility(unsigned int loc_);
	virtual int getCurrLoc();

	// return size of block		
	virtual int getSize();	
	virtual int getNumValues();
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

	int precomputed16_bitcount (unsigned int n);
	void compute_bits_in_16bits ();
	int iterated_bitcount (unsigned int n);
	int howManyOnBits(byte* buf, int nv, int startind, int endind);
	void setKeepSame();
	byte* makeNewBuffer(int value_, int startPos_, int endPos_);
	
	virtual BlockIter* getIterator() const {
	  return new Type2BlockIter((Type2Block*)this, startPos, endPos);
	}
	
	virtual BlockIter* getIterator(unsigned int startPos, unsigned int endPos) const {
		return new Type2BlockIter((Type2Block*) this, startPos, endPos);
	}
	
	class Type2BlockIter : public BlockIter {
	public:
	  Type2BlockIter(Type2Block* t2Block_, unsigned int startPos_, unsigned int endPos_);
	  Type2BlockIter(const Type2BlockIter&);
	  virtual ~Type2BlockIter();
	  virtual bool hasNext();
	  virtual bool hasNext(int value_);
	  virtual ValPos* getNext();
	  virtual ValPos* peekNext();
	  virtual void resetIterator();
	  virtual int getCurrLoc();
	  virtual bool skipToLoc(unsigned int loc_);
	  virtual bool skipToPos(unsigned int loc_);
	  virtual Block* clone(Block&);
	  // Zero indexed, gets the pair at this loc
	  virtual ValPos* getPairAtLoc(unsigned int loc_);
	  
	  //Like getPairAtLoc except set the regular pair variable rather than 
	  //the utilityPair variable so can be called by getNext(). 
	  virtual ValPos* getPairAtLocNotUtility(unsigned int loc_);
	  // return size of block	(number of values)
	  virtual int getSize();
	  virtual ValPos* getStartPair();
	  virtual unsigned int getEndPosition();
	  virtual Block* getBlock(); 
	  virtual int initBitStringIterator(unsigned int pos);
	  virtual int getNextBitString();

	  // Stream properties
	  virtual bool isValueSorted();	
	  virtual bool isPosSorted();
	  
	  // Block properties
	  virtual bool isOneValue();
	  virtual bool isPosContiguous();
	  virtual bool isBlockValueSorted();	
	  virtual bool isBlockPosSorted();
	protected:
	  Type2Block* const t2Block;
	  unsigned int startpos;
	  unsigned int endpos;
	  unsigned int adjStartPos;
	  int* currPosPtr;
	  bool peeked;
	  int currPos;
	  ValPos* startPair;
	  int numvalues;
	  int* posIndex;
	  int currIndexInVal;
	  bool calculatedSize;
	  int numOnBits;
	  bool bitStringInitialized;
	  int* bitStringPtr;
	  byte* newBufferStart;
	  int GPALlastLoc;
	  int GPALlastIndex;
	  //mutable Pair pair; // mutable needed because modified by peekNext
	};
	
	ValPos* startPair;
	int currPos;

protected:
	bool valSorted;
	bool posContig;
	bool posSorted;

	bool pairNewed;
	byte* bfrWithHeader;
	byte* buffer;
	int* bufferPtrAsIntArr;
	byte* playBuffer;
	int numValues;
	int startVal;
	int startPos;
	int endPos;
	unsigned int currEndPos;
	int* currPosPtr;
	int* posIndex;
	int currIndexInVal;
	char* bits_in_16bits;
	bool keepSame;
	bool peeked;
	bool calculatedSize;
	int numOnBits;
	int realStartPos;
	int lastPos;
	
	int GPALlastLoc;
	int GPALlastIndex;

	bool bitStringInitialized;
	int* bitStringPtr;

};

#endif //_TYPE2BLOCK_H_
