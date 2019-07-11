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
// Read Byte by Byte
// Problems: migfer@mit.edu

#include "ByteReader.h"

ByteReader::ByteReader(byte* buffer_, int numBitsInBuffer_)
{
	buffer=buffer_;
	currPosPtr=buffer;
	numBytesInBuffer=numBitsInBuffer_/8;
	currPos=0;
}

ByteReader::~ByteReader()
{
}

bool ByteReader::readByte(byte& value) {
	if (currPos+1>numBytesInBuffer) return false;
	value=(byte) (*currPosPtr);
	currPosPtr++;
	currPos++;
	return true;
}
bool ByteReader::readShort(unsigned short& value) {
	if (currPos+2>numBytesInBuffer) return false;
	value=(unsigned short) (*((unsigned short*) currPosPtr));
	currPosPtr+=2;
	currPos+=2;
	return true;
}

bool ByteReader::readThree(unsigned int& value) {
	if (currPos+3>numBytesInBuffer) return false;
	unsigned short shrt;
	byte byt;
	readShort(shrt);
	readByte(byt);
	
	value=(unsigned int) ((shrt*0x100)+byt);
	return true;
}
bool ByteReader::readInt(unsigned int& value) {
	if (currPos+4>numBytesInBuffer) return false;
	value=(unsigned int) (*((unsigned int*) currPosPtr));
	currPosPtr+=4;
	currPos+=4;
	return true;
}

void ByteReader::resetPos() {
	currPosPtr=buffer;
	currPos=0;			
}
bool ByteReader::skipToIntPos(unsigned int intPos_) {
	return skipToBytePos(intPos_*sizeof(int));
}
bool ByteReader::skipToBytePos(unsigned int bytePos_) {
	if (bytePos_>=numBytesInBuffer) return false;
	currPos=bytePos_;
	currPosPtr=buffer+bytePos_;	
	return true;
}
int ByteReader::readFirst() {
	return buffer[0];
}
int ByteReader::readLast() {
	return buffer[numBytesInBuffer-1];
}

int ByteReader::getCurrentBytePos() {
	return currPos;
} 
void ByteReader::setCurrentBytePos(int currPos_) {
        currPos = currPos_;
}
byte* ByteReader::getCurrentPosPtr() {
        return currPosPtr;
}
void ByteReader::setCurrentPosPtr(byte* currPosPtr_) {
        currPosPtr = currPosPtr_;
}
int ByteReader::getMaxNumBytes() {
	return numBytesInBuffer;
}
void ByteReader::setBuffer(byte* buffer_, int numBitsInBuffer_) {
	buffer=buffer_;
	currPosPtr=buffer;
	numBytesInBuffer=numBitsInBuffer_/8;
	currPos=0;		
}

