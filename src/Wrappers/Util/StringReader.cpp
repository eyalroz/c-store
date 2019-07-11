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
// Read Strings

#include "StringReader.h"
StringReader::StringReader() {
  buffer=NULL;
  currPosPtr=NULL;
  numBytesInBuffer=0;
  currPos=0;
}
/*StringReader::StringReader(byte* buffer_, int numBitsInBuffer_)
{
	buffer=(int*) buffer_;
	currPosPtr=buffer;
	numStringsInBuffer=numBitsInBuffer_/(8*sizeof(int));
	currPos=0;
}*/

StringReader::~StringReader()
{

}

void StringReader::setStringSize(int stringSize_) {
  stringSize=stringSize_;
}

byte* StringReader::getBuffer(int& numVals) {
  if ((currPos*stringSize)>(numBytesInBuffer-stringSize))return NULL;
  numVals=(numBytesInBuffer/stringSize)-currPos;
  currPos=(numBytesInBuffer/stringSize);
  return (currPosPtr);
}

bool StringReader::readString(char* value) {
  if ((currPos*stringSize)>(numBytesInBuffer-stringSize)) return false;
  memcpy(value,currPosPtr,stringSize);
  currPosPtr+=stringSize;
  currPos++;
  return true;
}

void StringReader::unReadString() {
  assert(currPos>0);
  currPosPtr-=stringSize;
  currPos--;  
}

bool StringReader::readString(char* value, int ssize) {
  if ((currPos*stringSize)>(numBytesInBuffer-ssize)) return false;
  memcpy(value,currPosPtr,ssize);
  currPosPtr+=ssize;
  //ick - can't really update currPos anymore
  //currPos++;
  return true;
}

bool StringReader::hasNextString() {
  return ((currPos*stringSize)<=(numBytesInBuffer-stringSize));
}
void StringReader::resetPos() {
  currPosPtr=buffer;
  currPos=0;			
}
bool StringReader::skipToStringPos(unsigned int intPos_) {
  if ((intPos_*stringSize)>(numBytesInBuffer-stringSize)) return false;
  currPos=intPos_;
  currPosPtr=buffer+(intPos_*stringSize);	
  return true;
  }
/*int StringReader::readFirst() {
	return buffer[0];
}
int StringReader::readLast() {
	return buffer[numIntsInBuffer-1];
	}*/

int StringReader::getCurrentPos() {
  return currPos;
}
/*
int StringReader::getMaxNumInts() {
return numIntsInBuffer;
}
*/
void StringReader::setBuffer(byte* buffer_, int numBitsInBuffer_) {
	buffer=buffer_;
	currPosPtr=buffer;
	numBytesInBuffer=numBitsInBuffer_/8;
	currPos=0;		
}
