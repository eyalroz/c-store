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
// Decodes NullSuprression pages
// Problems: migfer@mit.edu
#ifndef _NSDECODER_H_
#define _NSDECODER_H_

#include "Decoder.h"
#include "../Util/ByteReader.h"
#include "../MultiBlock.h"
#include "../../common/Constants.h"

class NSDecoder : public Decoder
{
public:
	NSDecoder(bool valSorted_);
	NSDecoder(NSDecoder& decoder_); 
	NSDecoder(byte* buffer_, bool valSorted_);
	virtual ~NSDecoder();
	
	virtual void setBuffer(byte* buffer_);
	virtual int getValueSize();
	virtual int getPageLengthBits();
	
	virtual bool hasNextBlock();
	virtual Block* getNextBlock();
	virtual Block* getNextBlockSingle();
	virtual bool getNextValue(int* value);
	virtual Block* skipToBlock(unsigned int blockPos_);
	virtual bool skipToPos(unsigned int blockPos_);
	
	virtual ValPos* getStartVal();
	virtual unsigned int getStartPos();
	virtual ValPos* getEndVal();
	virtual unsigned int getEndPos();
	
	virtual unsigned int getSize();
	
protected:
	void initDecoder();
	void generateNSTable();
	bool skipADelta();
	bool readValue(unsigned int& value_, byte numBits_);
	void readValues();

	
	unsigned int* numBytesPtr;
	unsigned int* startPosPtr;
	ByteReader* reader;
	MultiBlock* outBlock;
	unsigned int currLoc;
	unsigned int numValues;
	
	unsigned int values[4];
	int valuesCurrIndex;
	int maxValsInBuffer;
	bool valsInBuffer;
	unsigned int startInt;
	unsigned int endInt;
	
	byte table[256][5];
	int* outBuffer;
	int bufferSize;

};

#endif //_NSDECODER_H_
