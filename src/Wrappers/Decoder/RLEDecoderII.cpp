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
// Decoders RLE pages
// Problems: migfer@mit.edu


#include "RLEDecoderII.h"


RLEDecoderII::RLEDecoderII(bool valSorted_) : RLEDecoder(valSorted_) {
	
}

RLEDecoderII::RLEDecoderII(byte* buffer_, bool valSorted_) : RLEDecoder(buffer_, valSorted_)
{	
	setBuffer(buffer_);
}
RLEDecoderII::~RLEDecoderII() 
{
	
}

void RLEDecoderII::setBuffer(byte* buffer_) {
	int pageLength=*((int*) buffer);

	valueSize=*((byte*) buffer+sizeof(int));
	startPosSize=0;
	repsSize=*((byte*) (buffer+sizeof(byte)+sizeof(int)));
	startPos=*((unsigned int*) (buffer+2*sizeof(byte)+sizeof(int)));
	endPos=*((unsigned int*) (buffer+2*sizeof(byte)+2*sizeof(int)));
	currPos=startPos;
	data=(buffer+2*sizeof(byte)+3*sizeof(int));
	
	Log::writeToLog("RLEDecoderII", 0, "Buffer valueSize=", valueSize);
	Log::writeToLog("RLEDecoderII", 0, "Buffer startPosSize=", startPosSize);
	Log::writeToLog("RLEDecoderII", 0, "Buffer repsSize=", repsSize);
	
	tripleSize=valueSize+startPosSize+repsSize;
	
	if (reader==NULL)
		reader=new BitReader(data, pageLength);
	else
		reader->setBuffer(data, pageLength);
	Log::writeToLog("RLEDecoderII", 0, "Initializing reader with page length", pageLength);
	
	
}

unsigned int RLEDecoderII::getStartPos() {
	if (!initialized) throw new CodingException("RLEDecoder: Error, decoder not initialized with buffer");
	return startPos;
}

unsigned int RLEDecoderII::getEndPos() {
	if (!initialized) throw new CodingException("RLEDecoder: Error, decoder not initialized with buffer");
	return endPos;
}

RLETriple* RLEDecoderII::getNextTriple() {
	// to guarantee atomicity in reading if there is a failure
	int oldBitPos=reader->getCurrentBitPosition();
	unsigned int value;
	if (!reader->readBits(value, valueSize)) {
		reader->skipToBitPos(oldBitPos);	
		return NULL;
	}	
	result->startPos=currPos;
	if (!reader->readBits(result->reps, repsSize)) {
		reader->skipToBitPos(oldBitPos);	
		return NULL;
	}
	currPos+=result->reps;
	//result->value=(unsigned int) value; // assuming encoding for all positive values
	vp->set(result->startPos, (byte*)&value);
	result->value = vp;
	return result;
}
bool RLEDecoderII::skipToTriple(unsigned int triplePos_) {
	if (triplePos_>=getSize()) return false;
	int oldBitPos=reader->getCurrentBitPosition();
	int oldCurrPos=currPos;
	reader->skipToBitPos(0);
	for (unsigned int i=0; i<triplePos_-1; i++) {
		RLETriple* triple=getNextTriple();
		if (triple==NULL) {
			// return to old position for sequential read
			reader->skipToBitPos(oldBitPos);	
			currPos=oldCurrPos;
			return false;
		}
	}
	return true;
}

RLETriple* RLEDecoderII::getTriple(unsigned int triplePos_) {
	if (triplePos_>=getSize()) return NULL;
	// to guarantee atomicity in reading if there is a failure
	int oldBitPos=reader->getCurrentBitPosition();
	int oldCurrPos=currPos;
	
	reader->skipToBitPos(0);
	RLETriple* triple = NULL;
	
	for (unsigned int i=0; i<triplePos_-1; i++) {
		triple=getNextTriple();
		if (triple==NULL) {
			// return to old position for sequential read
			reader->skipToBitPos(oldBitPos);	
			currPos=oldCurrPos;
			return NULL;
		}
	}
	
	// return to old position for sequential read
	reader->skipToBitPos(oldBitPos);	
	currPos=oldCurrPos;
	return triple;		
}	
