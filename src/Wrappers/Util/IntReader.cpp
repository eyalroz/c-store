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
// Read Int by Int
// Problems: migfer@mit.edu


#include "IntReader.h"
IntReader::IntReader() {
	buffer=NULL;
	currPosPtr=NULL;
	numIntsInBuffer=0;
	currPos=0;
}
IntReader::IntReader(byte* buffer_, int numBitsInBuffer_)
{
	buffer=(int*) buffer_;
	currPosPtr=buffer;
	numIntsInBuffer=numBitsInBuffer_/(8*sizeof(int));
	currPos=0;
}

IntReader::~IntReader()
{

}

byte* IntReader::getBuffer(int& numInts) {
  if (currPos>=numIntsInBuffer) return NULL;
  numInts=numIntsInBuffer-currPos;
  currPos=numIntsInBuffer;
  return ((byte*)currPosPtr);
}

bool IntReader::readInt(int& value) {
	if (currPos>=numIntsInBuffer) return false;
	value=*currPosPtr;
	currPosPtr++;
	currPos++;
	return true;
}
void IntReader::resetPos() {
	currPosPtr=buffer;
	currPos=0;			
}
bool IntReader::skipToIntPos(unsigned int intPos_) {
	if (intPos_>=numIntsInBuffer) return false;
	currPos=intPos_;
	currPosPtr=buffer+intPos_;	
	return true;
}
int IntReader::readFirst() {
	return buffer[0];
}
int IntReader::readLast() {
	return buffer[numIntsInBuffer-1];
}

int IntReader::getCurrentIntPos() {
	return currPos;
}
int IntReader::getMaxNumInts() {
	return numIntsInBuffer;
}
void IntReader::setBuffer(byte* buffer_, int numBitsInBuffer_) {
	buffer=(int*) buffer_;
	currPosPtr=buffer;
	numIntsInBuffer=numBitsInBuffer_/(8*sizeof(int));
	currPos=0;		
}
