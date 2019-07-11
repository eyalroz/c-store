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
#include "BitWriter.h"

BitWriter::BitWriter(unsigned int maxNumBits_)
{
	bufferSizeBytes=maxNumBits_/8;
	if ((maxNumBits_%32)!=0) bufferSizeBytes+=4;
	buffer=new byte[bufferSizeBytes];
	memset(buffer, 0, bufferSizeBytes);
	delayedMemset=false;

	currentInt=(unsigned int*) buffer;
	bitPosition=0;
	
	numBitsWritten=0;
	maxNumBits=maxNumBits_;
	
}

BitWriter::~BitWriter()
{
  delete[] buffer;
}

void BitWriter::clearSpace() {
  delete[] buffer;
}


bool BitWriter::writeBits(unsigned int value_, unsigned int numBits_) {
	if (delayedMemset) {
		memset(buffer, 0, bufferSizeBytes);
		delayedMemset=false;
	}
	assert(value_<=(0xFFFFFFFF>>(32-numBits_))); // otherwise can't encode value_ in this number of bits
	
	// can we write this in this buffer?
	if (numBitsWritten+numBits_>=maxNumBits) { // =as we start at bit position 0
		return false;
	}
	
	
	// two cases: 
	// 	1. It will fit in current int
	//	2. It won't fit in current int
	
	byte numBitsAvail=32-bitPosition;	
	if (numBitsAvail==numBits_) {
		unsigned int valToOr=value_<<(numBitsAvail-numBits_);
		*currentInt|=valToOr;
		bitPosition=0;
		currentInt++;
	}
	else if (numBitsAvail>numBits_) {
		unsigned int valToOr=value_<<(numBitsAvail-numBits_);
		bitPosition+=numBits_;
		bitPosition=bitPosition%32;
		*currentInt|=valToOr;
	}
	else {
		unsigned int valToOr=value_>>(numBits_-numBitsAvail);
		*currentInt|=valToOr;
		currentInt++;
		valToOr=value_<<(32-(numBits_-numBitsAvail));
		*currentInt|=valToOr;
		bitPosition=(numBits_-numBitsAvail);
	}
	numBitsWritten+=numBits_;
	return true;
}

bool BitWriter::skipBits(unsigned int numBits_) {
	if (numBitsWritten+numBits_<maxNumBits) {
		Log::writeToLog("BitWriter", 0, "Skipping numBits_:", numBits_);
		currentInt+=((int) numBits_)/(sizeof(unsigned int)*8);
		bitPosition+=(numBits_%(8*sizeof(unsigned int)));
		currentInt+=bitPosition/(8*sizeof(unsigned int));
		bitPosition=bitPosition%(8*sizeof(unsigned int));
		numBitsWritten+=numBits_;
		return true;
	}
	else {
		return false;
	}		
}
int BitWriter::getNumBitsWritten() {
	return numBitsWritten;
}
byte* BitWriter::getBuffer() {
	return buffer;		
}

bool BitWriter::clearBuffer() {
	bool success=resetBuffer();
	int bufferSizeBytes=maxNumBits/8;
	if ((maxNumBits%8)!=0) bufferSizeBytes+=4;
	memset(buffer, 0, bufferSizeBytes);
	delayedMemset=false;
	return success;
}

bool BitWriter::resetBuffer() {
	numBitsWritten=0;
	currentInt=(unsigned int*) buffer;
	bitPosition=0;
	delayedMemset=true;
	return true;
}
