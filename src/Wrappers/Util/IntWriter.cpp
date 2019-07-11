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
// Write Int by Int

#include "IntWriter.h"

IntWriter::IntWriter(int bfrSizeInBits_)
{
	buffer=new int[bfrSizeInBits_/(8*sizeof(int))];
	currIntPtr=buffer;
	memset(buffer, 0, (bfrSizeInBits_/(8*sizeof(int)))); // to prevent rounding errors
	numIntsWritten=0;
	bufferSizeInInts=bfrSizeInBits_/(8*sizeof(int));
	oldBufferSizeInInts=bfrSizeInBits_/(8*sizeof(int));
}

IntWriter::~IntWriter()
{
	
}

bool IntWriter::writeInt(int val_) {
	if (numIntsWritten>=bufferSizeInInts) 
	  return false;
	*currIntPtr=val_;
	currIntPtr++;
	numIntsWritten++;
	return true;
}

void IntWriter::unWriteInt() {
  assert(numIntsWritten > 0);
  currIntPtr--;
  numIntsWritten--;
}

bool IntWriter::skipToIntPos(unsigned int pos_) {
  //I shouldn't really cast, but the buffer size shouldn't get that big
  if ((int)pos_>=bufferSizeInInts) return false;
  currIntPtr=buffer+pos_;
  numIntsWritten=pos_;
  return true;
}
int IntWriter::getNumIntsWritten() {
	return numIntsWritten;
}

int IntWriter::getBufferSize() {
	return 8*sizeof(int)*bufferSizeInInts;
}

byte* IntWriter::getBuffer() {
	return ((byte*) buffer);	
}

void IntWriter::resetBuffer() {
	currIntPtr=buffer;
	numIntsWritten=0;
}

bool IntWriter::reserveBufferSpace(int numInts) {
  assert(numInts>=0);
  bufferSizeInInts -= numInts;
  if (numIntsWritten>=bufferSizeInInts) {
    bufferSizeInInts += numInts;
    return false;
  }
  return true;
}

void IntWriter::unReserveSpace() {
  bufferSizeInInts=oldBufferSizeInInts;
}
