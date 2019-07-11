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
// Delta on Position Block
// Problems: migfer@mit.edu


#ifndef _DELTAPOSBLOCK_H_
#define _DELTAPOSBLOCK_H_

#include "../common/Block.h"
#include "Util/BitReader.h"
#include "CompDataStructs/DeltaPosHeader.h"
#include "../common/UnexpectedException.h"
#include "../common/UnimplementedException.h"

class DeltaPosBlock : public Block
{
public:
	DeltaPosBlock(bool valueSorted_);
	DeltaPosBlock(DeltaPosBlock& block_);
	DeltaPosBlock(byte* bfr_, int bfrSize_, bool valueSorted_);
	virtual ~DeltaPosBlock();
	Block* clone(Block&);

	virtual void setDeltaBuffer(byte* bfr_, int bfrSize_);
	virtual void resetBlock();	
	// Iterator access to block
	virtual bool hasNext();
	virtual bool hasNext(int value_);
	virtual ValPos* getNext();
	virtual ValPos* peekNext();

	// Zero indexed, gets the pair at this loc
	virtual ValPos* getPairAtLoc(unsigned int loc_);
	
	//Like getPairAtLoc except set the regular pair variable rather than 
	//the utilityPair variable so can be called by getNext(). 
	virtual ValPos* getPairAtLocNotUtility(unsigned int loc_);
	virtual int getCurrLoc();

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

	int getValue();
	byte* getBuffer();
protected:
	void initDeltaPosBlock(byte* bfr_);
	BitReader* reader;
	bool valueSorted;
	byte* bfr;
	int bfrSize;
	int value;
	bool init;
	unsigned int numValues;
	unsigned int deltaSize;
	unsigned int escapeInt;
	unsigned int currLoc;
	unsigned int startPos;
	unsigned int currPos;
};

#endif //_DELTAPOSBLOCK_H_
