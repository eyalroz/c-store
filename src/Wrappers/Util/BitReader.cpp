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
#include "BitReader.h"

BitReader::BitReader(byte* buffer_, int numBitsInBuffer_)
{
	Log::writeToLog("BitReader", 0, "Constructing buffer");
	buffer=buffer_;
	numBitsInBuffer=numBitsInBuffer_;
	initReader();	
}

BitReader::~BitReader()
{
}

bool BitReader::readBits(unsigned int& value, int numBits_) {
	if (numBits_==0) {
		value=0;
		return true;	
	}
	if (numBits_+numBitsRead>numBitsInBuffer)
		return false;

	byte numBitsAvail=32-currentBit;
	if (numBitsAvail==numBits_) {
		value=(0xFFFFFFFF>>currentBit)&(*currentInt);
		currentInt++;
		currentBit=0;	
	}
	else if (numBitsAvail>=numBits_) {
		value=(0xFFFFFFFF>>currentBit)&(*currentInt);
		value=value>>(numBitsAvail-numBits_);			
		currentBit+=numBits_;
	}
	else {
		// first part
		value=(0xFFFFFFFF>>currentBit)&(*currentInt);
		byte numBitsRemain=numBits_-numBitsAvail;
		value=value<<numBitsRemain;

		// second part
		currentInt++;
		value|=(*currentInt)>>(32-numBitsRemain);		
		currentBit=numBitsRemain;
	}
	numBitsRead+=numBits_;
	return true;
}

// Starts reading the buffer from the start
void BitReader::resetPos() {
	currentInt=(unsigned int*) buffer;
	currentBit=0;
	numBitsRead=0;	
}
	
// Start reading from a specified bit postion
bool BitReader::skipToBitPos(int bitPos_) {
	if (bitPos_<numBitsInBuffer) {
		currentInt=(unsigned int*) buffer;
		currentInt+=(bitPos_/32);
		currentBit=bitPos_%32;
		numBitsRead=bitPos_;
		return true;
	}
	else {
		return false;
	}	
}
	
// Get current bit position
int BitReader::getCurrentBitPosition() {
	return (8*((int)((byte*) currentInt-buffer))+currentBit);
}
	
// Get max bit
int BitReader::getNumBitsInBuffer() {
	return numBitsInBuffer;
}
	
void BitReader::setBuffer(byte* buffer_, int numBitsInBuffer_) {
	Log::writeToLog("BitReader", 0, "Constructing buffer");
	buffer=buffer_;
	numBitsInBuffer=numBitsInBuffer_;
	initReader();	
}

void BitReader::initReader() {
	if (buffer==NULL) {
		Log::writeToLog("BitReader", 2, "Error, bitreader cannot be initialized with a NULL buffer");
		throw new CodingException("BitReader: Passed a NULL buffer in initializer");	
	}
	resetPos();
}
