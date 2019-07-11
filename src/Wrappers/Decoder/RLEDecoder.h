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

// Decode RLE Pages with RLETriples
// Problems: migfer@mit.edu


#ifndef RLEDECODER_H
#define RLEDECODER_H

#include "RLESuperDecoder.h"
#include "../Util/BitReader.h"
#include "../RLETriple.h"
#include "../CodingException.h"
#include "../RLEBlock.h"


class RLEDecoder : public RLESuperDecoder {
public:
	RLEDecoder(bool valSorted_);
	RLEDecoder(byte* buffer, bool valSorted_);
	virtual ~RLEDecoder();	
	
	virtual void setBuffer(byte* buffer_);
	
	virtual bool hasNextBlock();
	virtual Block* getNextBlock();
	virtual Block* skipToBlock(unsigned int blockPos_);
	virtual bool skipToPos(unsigned int blockPos_);
	virtual Block* getBlock(unsigned int blockPos_);

	virtual bool skipToBlockOnValue(int value_);
	virtual bool skipToBlockOnPos(unsigned int pos);
	
	
	virtual ValPos* getStartVal();
	virtual unsigned int getStartPos();
	virtual ValPos* getEndVal();
	virtual unsigned int getEndPos();
	virtual int getPageLengthBits();	
	virtual unsigned int getSize();

protected:
	virtual bool hasNextTriple();
	virtual RLETriple* peekNextTriple();
	virtual RLETriple* getNextTriple();
	
	virtual bool skipToTriple(unsigned int triplePos_);
	virtual RLETriple* getTriple(unsigned int triplePos_);
	
	bool valSorted;
	BitReader* reader;
	RLETriple* result;
	RLEBlock* block;
	byte* data;
	int valueSize;
	int startPosSize;
	int repsSize;
	int tripleSize;
	ValPos* vp;
};

#endif // RLEDECODER_H
