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
// Encodes Null Suppression
// Problems: migfer@mit.edu

#include "NSEncoder.h"

NSEncoder::NSEncoder(Operator* dataSrc_, int colIndex_) : Encoder(dataSrc_, colIndex_)
{
	currBlock=NULL;
	init=true;
	valsRemaining=false;
	srcExhausted=false;

	blockWriter=new BasicBlockWriter(dataSrc_, colIndex_);
	writer=new ByteWriter(8*PAGE_SIZE);
	buffer=writer->getBuffer();

	numBytesPtr=(int*) buffer;
	startPosPtr=(int*) (buffer+sizeof(int));
	resetWriter();
	generateNSTable();		
	Log::writeToLog("NSEncoder", 0, "Constructor: done initializing");
}

NSEncoder::~NSEncoder()
{
	delete blockWriter;
	delete writer;
}
void NSEncoder::resetWriter() {
	writer->resetBuffer();
	writer->skipToBytePos(8);
	*numBytesPtr=0;
	*startPosPtr=0;
	Log::writeToLog("NSEncoder", 0, "resetWriter: writer reset");	
}

byte* NSEncoder::getPage() {
	int numValues=0;
	if (srcExhausted) {
		Log::writeToLog("NSEncoder", 0, "getPage(): source exhausted, returning NULL");		
		return NULL;
	}
	while (true) {
		// we write in blocks of four values (i.e. bitvector, val1, val2, val3, val4) 	
				
		if (!valsRemaining) {
			numValsRead=getFourValues();
			Log::writeToLog("NSEncoder", 0, "getPage(): no values in buffer, call to get vlaues, numValuesRead", numValsRead);		
		}
		else { 
			valsRemaining=false;
			numValues=numValsRead;
		}
		
		if (init) {
			Log::writeToLog("NSEncoder", 0, "getPage(): init true");		
			resetWriter();
			*startPosPtr=currStartPos;
			init=false;
		}
		
		byte size[4];
		for (int i=0; i<4; i++) {
			size[i]=getSize(values[i]);
			Log::writeToLog("NSEncoder", 0, "getPage(): values have size: ", size[i]);		
		}
		
		byte bitVector=table[size[0]][size[1]][size[2]][size[3]];
		Log::writeToLog("NSEncoder", 0, "getPage(): bitVector is: ", bitVector);		
		int oldBytePos=writer->getNumBytesWritten();
		bool success=true;
		int sizeIncrement=0;
		if (numValsRead!=0) {
			success&=writer->writeByte(bitVector);
			sizeIncrement=+1;
		}

		for (int i=0; i<numValsRead && success; i++) {
			Log::writeToLog("NSEncoder", 0, "getPage(): writing val: ", values[i]);		
			if (size[i]==0) {
				success&=writer->writeByte((byte) (0xFF&values[i]));
				sizeIncrement+=1;
			}
			else if (size[i]==1) {
				success&=writer->writeShort((short) (0xFFFF&values[i]));
				sizeIncrement+=2;
			}
			else if (size[i]==2) {
				success&=writer->writeThree((int) (0xFFFFFF&values[i]));
				sizeIncrement+=3;
			}
			else if (size[i]==3) {
				success&=writer->writeInt((int) (0xFFFFFFFF&values[i]));
				sizeIncrement+=4;
			}
		}
		if (!success) {
			// we know we filled the page
			Log::writeToLog("NSEncoder", 1, "getPage(): page filled, returning, numBytes",*numBytesPtr);		
			Log::writeToLog("NSEncoder", 1, "getPage(): page filled, returning, numValues",numValues);		
			writer->skipToBytePos(oldBytePos);
			valsRemaining=true; // values buffer full for next page
			init=true;
			return buffer;
		}
		else {
			// we are still filling page	
			*numBytesPtr+=sizeIncrement;
			Log::writeToLog("NSEncoder", 0, "getPage(): page notFilled, numBytes=",*numBytesPtr);		
		}

		// we know we have exhausted source	
		if (numValsRead<4) {	
			valsRemaining=false;
			srcExhausted=true;
			Log::writeToLog("NSEncoder", 1, "getPage(): srcExhaused, returning page, numValues", numValues);		
			return buffer;
		}
	}
}

byte NSEncoder::getSize(unsigned int val_) {
	if (val_<=0xFF) return 0;
	else if (val_<=0xFFFF) return 1;
	else if (val_<=0xFFFFFF) return 2;
	else return 3;
}
// Gets the next four values, if init true, it also sets the startPos
int NSEncoder::getFourValues() {
	int count=0;
	for (int i=0; i<4; i++) {
		currBlock=(BasicBlock*) blockWriter->getNextValBlock(0);
		if (currBlock!=NULL) {
		  //asume integer types ... extend later if need be
		  assert(currBlock->getStartPair()->type == ValPos::INTTYPE);
			values[count]=*(int*)currBlock->getStartPair()->value;
			if (i==0) {		
				currStartPos=currBlock->getStartPair()->position;
			}
			Log::writeToLog("NSEncoder", 0, "getFourValues(): setStartPos=", *startPosPtr);		
			count++;
		}
	}
	return count;
}

void NSEncoder::generateNSTable() {
	for (byte i=0; i<4; i++) {
		for (byte j=0; j<4; j++) {
			for (byte k=0; k<4; k++) {
				for (byte l=0; l<4; l++) {
					table[i][j][k][l]=(i<<6)+((j)<<4)+((k)<<2)+((l));
				}	
			}	
		}	
	}
}
