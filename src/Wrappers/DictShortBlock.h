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
// Coded by DNA


#ifndef _DICTSHORTBLOCK_H_
#define _DICTSHORTBLOCK_H_

#include "MultiBlock.h"
#include "CodingException.h"
#include "../common/UnexpectedException.h"
#include <iostream>

using namespace std;
class DictShortBlock : public MultiBlock
{
public:
	DictShortBlock(bool valSorted_, bool posContig_, bool posSorted_);
	DictShortBlock(const DictShortBlock&);
	virtual ~DictShortBlock();
	Block* clone(Block&);
	virtual void setBuffer(int startpos_, int numValues_, byte* buffer_);
	//virtual void setValue(Pair* val_);
	virtual void resetBlock();
	// Iterator access to block
	virtual bool hasNext();
	virtual bool hasNext(int value_);
	virtual ValPos* getNext();
	virtual ValPos* peekNext();
	//virtual int getValue();
	//virtual unsigned int getPosition();
	//virtual unsigned int getLastPosition();
	//virtual void cutOffEnd(int endPos);

	// Zero indexed, gets the pair at this pos_
	virtual ValPos* getPairAtLoc(unsigned int loc_);
	
	//Like getPairAtLoc except set the regular pair variable rather than 
	//the utilityPair variable so can be called by getNext(). 
	virtual ValPos* getPairAtLocNotUtility(unsigned int loc_);
	//virtual int getCurrLoc();

	// return size of block		
	//virtual int getSizeInBits();
	virtual ValPos* getStartPair();
	virtual void setDictSpecs(int fs, unsigned int fb/*,unsigned int** evm*/);

protected:
	int fieldSize;
	unsigned int fieldBytes;
	int numvalsperentry;
	//unsigned int** entryValueMap;
	int startVal;
	short* realCurrPosPtr;
	int currIndex;
	short* masks;
};

#endif //_DICTSHORTBLOCK_H_
