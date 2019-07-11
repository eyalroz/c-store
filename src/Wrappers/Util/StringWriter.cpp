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
// Write Strings

#include "StringWriter.h"

StringWriter::StringWriter(int stringSize_, int bfrSizeInBits_)
{
	buffer=new byte[bfrSizeInBits_/8];
	currBytePtr=buffer;
	memset(buffer, 0, (bfrSizeInBits_/8)); // to prevent rounding errors
	numBytesWritten=0;
	bufferSizeInBytes=bfrSizeInBits_/8;
	oldBufferSizeInBytes=bfrSizeInBits_/8;
	stringSize= stringSize_;
}

StringWriter::~StringWriter()
{
	
}

bool StringWriter::writeString(char* val_) {
	if ((numBytesWritten)>(bufferSizeInBytes-stringSize)) return false;
	memcpy(currBytePtr,val_,stringSize);
	currBytePtr+=stringSize;
	numBytesWritten+=stringSize;
	return true;
}

bool StringWriter::writeString(char* val_, int numBytes) {
	if ((numBytesWritten)>(bufferSizeInBytes-numBytes)) return false;
	memcpy(currBytePtr,val_,numBytes);
	currBytePtr+=numBytes;
	numBytesWritten+=numBytes;
	return true;
}

bool StringWriter::skipToBytePos(unsigned int pos_) {
  currBytePtr=buffer+(pos_);
  numBytesWritten=(pos_);
  return true;
}

bool StringWriter::skipToIntPos(unsigned int pos_) {
  //if (pos_>=3) return false; //NOT great code clean later
  assert(pos_<=5);
  currBytePtr=buffer+(pos_*sizeof(int));
  numBytesWritten=(pos_*sizeof(int));
  return true;
}

int StringWriter::getNumBytesWritten() {
  return numBytesWritten;
}

int StringWriter::getBufferSize() {
	return 8*bufferSizeInBytes;
}

byte* StringWriter::getBuffer() {
	return ((byte*) buffer);	
}

void StringWriter::resetBuffer() {
  currBytePtr=buffer;
  numBytesWritten=0;
}

bool StringWriter::reserveBufferSpace(int numInts) {
  assert(numInts>=0);
  bufferSizeInBytes -= (numInts*sizeof(int));
  if (numBytesWritten>=bufferSizeInBytes) {
    bufferSizeInBytes += (numInts*sizeof(int));
    return false;
  }
  return true;
}

void StringWriter::unReserveSpace() {
  bufferSizeInBytes=oldBufferSizeInBytes;
}

void StringWriter::unWriteString() {
  assert(numBytesWritten >= stringSize);
  currBytePtr-=stringSize;
  numBytesWritten-=stringSize;
}
