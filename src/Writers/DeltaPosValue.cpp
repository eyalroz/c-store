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
// Writes Delta Pos Block for a Particular value
// Problems: migfer@mit.edu

#include "DeltaPosValue.h"

DeltaPosValue::DeltaPosValue(int bfrSize_,int value_, int startPos_, int deltaSize_)
{
	full=false;
	value=value_;
	size=1;
	writer=new BitWriter(bfrSize_);
	deltaSize=deltaSize_;
	escapeInt=0xFFFFFFFF>>(32-deltaSize);
	startPos=startPos_;
	currPos=startPos_;
	if ((!writer->skipBits(sizeof(DeltaPosHeader)*8)) ||
		(!writer->writeBits(deltaSize_, 5)))
		throw new CodingException("DeltaPosValue: Could not write deltaPos header in constructor");
}

DeltaPosValue::~DeltaPosValue()
{
  delete writer;
}

void DeltaPosValue::clearWriterSpace() {
  writer->clearSpace();
}

void DeltaPosValue::reset(int val, unsigned int pos) {
	writer->resetBuffer();

	startPos=pos;
	size=1;
	value=val;
	full=false;

	if (!writer->skipBits(5+sizeof(DeltaPosHeader)*8))
		throw new CodingException("DeltaPosValue: Could not reset deltaPosValue");
}
byte* DeltaPosValue::getBuffer() {
	Log::writeToLog("DeltaPosValue", 1, "Returning buffer, value=", value);
	Log::writeToLog("DeltaPosValue", 1, "Returning buffer, startPos=", startPos);
	Log::writeToLog("DeltaPosValue", 1, "Returning buffer, numValues=", size);
	byte* bfr=writer->getBuffer();
	DeltaPosHeader header(value,startPos, size);
	*((DeltaPosHeader*) bfr)=header;
	return writer->getBuffer();
}

bool DeltaPosValue::isFull() {
	return full;
}

int DeltaPosValue::getNumValues() {
	return size;
}

bool DeltaPosValue::writePos(unsigned int pos_) {
	Log::writeToLog("DeltaPosValue", 0, "Writing position=", pos_);
	if ((currPos>=pos_) && currPos!=0 && pos_!=0)
		throw new CodingException("DeltaPosValue: failed to encode position as it is smaller than current position");

	bool success=true;
	unsigned int delta=pos_-currPos;
	Log::writeToLog("DeltaPosValue", 0, "Writing delta=", delta);
	if (!full) {	
		if (escapeInt<=delta) {
			Log::writeToLog("DeltaPosValue", 0, "Delta does not fit in deltaSize, escapeInt=", escapeInt);
			success&=writer->writeBits(escapeInt, deltaSize);
			success&=writer->writeBits(delta, 32);
		}
		else {
			success&=writer->writeBits(delta, deltaSize);
			Log::writeToLog("DeltaPosValue", 0, "Delta fits in deltaSize");
		}
	}
	if (success) {
		size++;
		Log::writeToLog("DeltaPosValue", 0, "Successful in writing Delta, size=",size);
		currPos=pos_;
	}
	return success;
}
