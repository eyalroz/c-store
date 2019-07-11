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
// Encodes RLE Pages
// Problems: migfer@mit.edu

#include "RLEEncoderII.h"

RLEEncoderII::RLEEncoderII(Operator* dataSrc_, int colIndex_, int bfrSizeInBits_, byte valueType_, short valueSize_, unsigned int repsSize_)
				: RLEEncoder(dataSrc_, colIndex_, bfrSizeInBits_, valueType_, valueSize_, 0, repsSize_)
{
	init=true;
}

RLEEncoderII::~RLEEncoderII()
{
	
}
void RLEEncoderII::initPage() {
	writer->clearBuffer();
	bool headerSuccess=true;
	buffer=writer->getBuffer();
	
	pageLengthPtr=(int*) (buffer);
	byte* data=((byte*) pageLengthPtr)+sizeof(int);
	*data=valueSize;
	*(data+1)=repsSize;
	startPosPtr=(unsigned int*) (data+2*sizeof(byte));
	endPosPtr=(unsigned int*) (data+2*sizeof(byte)+sizeof(unsigned int));
	
	headerSuccess&=writer->skipBits(8*(sizeof(int)+2*sizeof(byte)+2*sizeof(int)));		
	*pageLengthPtr=0;	
	
	if (!headerSuccess) {
		Log::writeToLog("RLEEncoder", 3, "RLEEncoder: failed in initializing header, in initPage");
		throw new CodingException("RLEEncoder: failed to write header in initPage()");
	}
}

byte* RLEEncoderII::getPage() {
	Log::writeToLog("RLEEncoderII", 0, "Called getPage()");
	initPage();
	if (block==NULL) {
		Log::writeToLog("RLEEncoderII", 2, "RLEEncoderII: got a NULL block, returning NULL");
		return NULL;	
	}
	while (block!=NULL && ((writer->getNumBitsWritten()+tripleSize)<=bfrSizeInBits)) {
		RLETriple* triple=block->getTriple();
		if (triple==NULL) {
			Log::writeToLog("RLEEncoderII", 3, "Error, got a block with a NULL triple");
			throw CodingException("RLEEncode: Error, block has a NULL triple");
		}
		bool success=true;
		//for now assume value is an integer ... fix later
		Log::writeToLog("RLEEncoderII", 0, "Trying to write block value=",*(int*)triple->value->value);
		success&=writer->writeBits(*(int*)triple->value->value, valueSize);
		if (init) {
			currPos=triple->startPos;
			*startPosPtr=triple->startPos;
			init=false;	
		}
		currPos+=triple->reps;
		Log::writeToLog("RLEEncoderII", 0, "Trying to write reps=",triple->reps);
		success&=writer->writeBits(triple->reps, repsSize);
		if (success) {
			*pageLengthPtr+=tripleSize;
			Log::writeToLog("RLEEncoderII", 0, "Wrote Block");
			block=(RLEBlock*) dataSrc->getNextValBlock(colIndex);			
		}
		else {
			Log::writeToLog("RLEEncoderII", 0, "Failed in writing triple");
			Log::writeToLog("RLEEncoderII", 0, "Failed when numbitswritten", writer->getNumBitsWritten());			
			throw new CodingException("RLEEncoderII: failed to write triple in RLEEncoder");
		}
	}
	Log::writeToLog("RLEEncoderII", 1, "Returning page");
	
	byte* buffer=writer->getBuffer();
	*endPosPtr=currPos-1;	
	Log::writeToLog("RLEEncoderII", 0, "Returning buffer with buffer+=", *((int *) (buffer+HEADER_SIZE+2+4)));
	Log::writeToLog("RLEEncoderII", 0, "Returning buffer with currPos=", currPos);
	init=true;
	return buffer;
}
