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

#include "RLEEncoder.h"

RLEEncoder::RLEEncoder(Operator* dataSrc_, int colIndex_, int bfrSizeInBits_, byte valueType_, short valueSize_, unsigned int startPosSize_, unsigned int repsSize_) : Encoder(dataSrc_, colIndex_) {
	writer=new BitWriter(bfrSizeInBits_);
	rleWriter=new RLEWriter(dataSrc_, colIndex_);
	bfrSizeInBits=bfrSizeInBits_;
	valueType=valueType_;
	valueSize=valueSize_;
	startPosSize=startPosSize_;
	repsSize=repsSize_;
	tripleSize=valueSize+startPosSize+repsSize;
	Log::writeToLog("RLEEncoder", 0, "Created buffer with size[bits]=",bfrSizeInBits_);
	dataSrc=rleWriter;
	initPage();
	block=(RLEBlock*) dataSrc->getNextValBlock(0);
}

RLEEncoder::~RLEEncoder() {	
	delete writer;
	delete rleWriter;
}
	
int RLEEncoder::getNumBitsWritten() {
	return *pageLengthPtr;
}

int RLEEncoder::getBufferSizeBytes() {
	int bufferSizeBytes=bfrSizeInBits/8;
	if ((bfrSizeInBits%8)!=0) bufferSizeBytes++;
	return bufferSizeBytes;
}
void RLEEncoder::initPage() {
	writer->clearBuffer();
	bool headerSuccess=true;
	buffer=writer->getBuffer();

	*((short *) buffer)=valueSize;
	pageLengthPtr=(int*) (buffer+sizeof(short));

	byte* data=((byte*) pageLengthPtr)+sizeof(int);
	*data=startPosSize;
	*(data+1)=repsSize;
	headerSuccess&=writer->skipBits(8*(sizeof(short)+sizeof(int)+2*sizeof(byte)));		

	*pageLengthPtr=0;	
	
	if (!headerSuccess) {
		Log::writeToLog("RLEEncoder", 3, "RLEEncoder: failed in initializing header, in initPage");
		throw new CodingException("RLEEncoder: failed to write header in initPage()");
	}
}
byte* RLEEncoder::getPage() {
	Log::writeToLog("RLEEncoder", 0, "Called getPage()");
	initPage();
	if (block==NULL) {
		Log::writeToLog("RLEEncoder", 2, "RLEEncoder: got a NULL block, returning NULL");
		return NULL;	
	}
	while (block!=NULL && ((writer->getNumBitsWritten()+tripleSize)<=bfrSizeInBits)) {
		RLETriple* triple=block->getTriple();
		if (triple==NULL) {
			Log::writeToLog("RLEEncoder", 3, "Error, got a block with a NULL triple");
			throw CodingException("RLEEncode: Error, block has a NULL triple");
		}
		bool success=true;
		//assume value is an integer
		assert(triple->value->type == ValPos::INTTYPE);
		Log::writeToLog("RLEEncoder", 0, "Trying to write block value=",*(int*)triple->value->value);
		success&=writer->writeBits(*(int*)triple->value->value, valueSize);
		Log::writeToLog("RLEEncoder", 0, "Trying to write start=",triple->startPos);
		success&=writer->writeBits(triple->startPos, startPosSize);
		Log::writeToLog("RLEEncoder", 0, "Trying to write reps=",triple->reps);
		success&=writer->writeBits(triple->reps, repsSize);
		if (success) {
			*pageLengthPtr+=tripleSize;
			Log::writeToLog("RLEEncoder", 0, "Wrote Block");
			block=(RLEBlock*) dataSrc->getNextValBlock(colIndex);			
		}
		else {
			Log::writeToLog("RLEEncoder", 0, "Failed in writing triple");
			Log::writeToLog("RLEEncoder", 0, "failed when numbitswritten", writer->getNumBitsWritten());			
			throw new CodingException("RLEEncoder: failed to write triple in RLEEncoder");
		}
	}
	//if ((writer->getNumBitsWritten()+tripleSize)>bfrSizeInBits)
	//  success&=writer->writeBits(0,bfrSizeInBits - writer->getNumBitsWritten());
	Log::writeToLog("RLEEncoder", 1, "Returning page");
	return buffer;
}

