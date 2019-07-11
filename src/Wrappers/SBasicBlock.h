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


#ifndef _SBASICBLOCK_H_
#define _SBASICBLOCK_H_

#include "../common/SBlock.h"
#include "CodingException.h"

class SBasicBlock : public SBlock
{
public:
	SBasicBlock(bool valSorted_, bool posContig_, bool posSorted_);
	SBasicBlock(const SBasicBlock&);
	virtual ~SBasicBlock();
	//Block* clone(Block&);
	virtual void setValue(char* val_, int pos_);
	virtual void setValue(SPair* val_);
	//virtual void resetBlock();
	// Iterator access to block
	virtual bool hasNext();
	//virtual bool hasNext(int value_);
	virtual SPair* getNext();
	/*virtual Pair* peekNext();
	virtual int getValue();
	virtual unsigned int getPosition();

	// Zero indexed, gets the pair at this pos_
	virtual Pair* getPairAtLoc(unsigned int loc_);
	virtual int getCurrLoc();

	// return size of block		
	virtual int getSize();
	virtual int getSizeInBits();
	virtual Pair* getStartPair();

	// Stream properties
	virtual bool isValueSorted();	
	virtual bool isPosSorted();
	
	// Block properties
	virtual bool isOneValue();
	virtual bool isPosContiguous();
	virtual bool isBlockValueSorted();	
	virtual bool isBlockPosSorted();*/
	
	SPair* pair;
	int currPos;
protected:
	bool valSorted;
	bool posContig;
	bool posSorted;

	bool pairNewed;
};

#endif //_SBASICBLOCK_H_
