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
// Write Byte by Byte
// Problems: migfer@mit.edu


#include "ByteWriter.h"

ByteWriter::ByteWriter(int bfrSizeInBits_)
{
	buffer=new byte[bfrSizeInBits_/8];
	currBytePtr=buffer;
	memset(buffer, 0, (bfrSizeInBits_/8)); 
	numBytesWritten=0;
	bufferSizeInBytes=bfrSizeInBits_/8;
}

ByteWriter::~ByteWriter()
{
}
bool ByteWriter::writeByte(byte val_) {
	if (numBytesWritten+1>bufferSizeInBytes) return false;
	*((short*) currBytePtr)=val_;
	currBytePtr+=1;
	numBytesWritten+=1;
	return true;	
}
bool ByteWriter::writeShort(unsigned short val_) {
	if (numBytesWritten+2>bufferSizeInBytes) return false;
	*((unsigned short*) currBytePtr)=val_;
	currBytePtr+=2;
	numBytesWritten+=2;
	return true;
}
bool ByteWriter::writeThree(unsigned int val_) {
	bool success=true;
	
	unsigned short valUp=(0xFFFF&(val_>>8));
	byte valLow=0xFF&(val_);
	
	byte* oldBytePtr=currBytePtr;
	int oldNumBytesWritten=numBytesWritten;
	
	
	success&=writeShort(valUp);
	success&=writeByte(valLow);
	
	if (!success) {
		currBytePtr=oldBytePtr;
		numBytesWritten=oldNumBytesWritten;
		return false;
	}
	return true;
}
bool ByteWriter::writeInt(unsigned int val_) {
	if (numBytesWritten+4>bufferSizeInBytes) return false;
	*((unsigned int*) currBytePtr)=val_;
	currBytePtr+=4;
	numBytesWritten+=4;
	return true;
}

bool ByteWriter::skipToBytePos(unsigned int pos_) {
	if (pos_>=bufferSizeInBytes) return false;
	currBytePtr=buffer+pos_;
	numBytesWritten=pos_;
	return true;
}

int ByteWriter::getNumBytesWritten() {
	return numBytesWritten;
}

int ByteWriter::getBufferSize() {
	return 8*bufferSizeInBytes;
}

byte* ByteWriter::getBuffer() {
	return buffer;	
}

void ByteWriter::resetBuffer() {
	currBytePtr=buffer;
	numBytesWritten=0;
}
