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
#include "DeltaEncoder.h"

DeltaEncoder::DeltaEncoder(Operator* dataSrc_, int colIndex_, unsigned int deltaSize_, unsigned int bfrSize_, int headerValue_) : Encoder(dataSrc_, colIndex_)
{
	// PULL initialization
	isValLeftOver=false;
	bbWriter=NULL;
	
	// PUSH initialization
	init=true;
	full=false;
	done=false;
	headerValue=headerValue_;
	size=1;
	currVal=0;
	writer=new BitWriter(bfrSize_);
	deltaSize=deltaSize_;
	escapeInt=0xFFFFFFFF>>(32-deltaSize);
	if (deltaSize+8*sizeof(int)>=(bfrSize_-sizeof(DeltaHeader)*8-5))
		throw new CodingException("DeltaEncoder: buffer is too small");
	if ((!writer->skipBits(sizeof(DeltaHeader)*8)) ||
		(!writer->writeBits(deltaSize_, 5)))
		throw new CodingException("DeltaEncoder: Could not write delta header in constructor");
}

DeltaEncoder::~DeltaEncoder()
{
	delete writer;
}

byte* DeltaEncoder::getPage() {
	if (mode==Encoder::UNINIT) {
		bbWriter=new BasicBlockWriter(dataSrc, colIndex);	
	}
	Encoder::getPage();
	
	// now depending on mode, I either return the buffer, or I fill a buffer from dataSource
	if (mode==Encoder::PUSH) {
		return getBuffer();
	}
	else if (mode==Encoder::PULL) {
		while(true) {
			if (!isValLeftOver) 
				block=(BasicBlock*) bbWriter->getNextValBlock(0);
			else {			
				if (!writeVal(valLeftOver, 0)) // position does not actually matter
					throw new CodingException("DeltaEncoder: error can never encode first value");				
				isValLeftOver=false;
			}
			
			if (block==NULL) {
				if (done)
					return NULL;
				else {
					done=true;
					return getBuffer();	
				}
			}
			else {
				unsigned int value=block->getStartPair()->position;
				if (!writeVal(value, 0)) { // position does not actually matter
					valLeftOver=value;
					isValLeftOver=true;
					return getBuffer();
				}	
			}
		}
	}
	return NULL;
} 
void DeltaEncoder::setHeaderValue(int headerValue_) {
	headerValue=headerValue_;
}
	
// Push logic code
bool DeltaEncoder::writeVal(int val_, unsigned int pos_) {
	Encoder::writeVal(val_, pos_);
	if (val_<0) throw new CodingException("DeltaEncoder: Coding negative values unsupported");
	
	Log::writeToLog("DeltaEncoder", 0, "Writing value=", val_);
	if (init) {
		startVal=val_;
		writer->clearBuffer(); // non delayed memset
		if ((!writer->skipBits(sizeof(DeltaHeader)*8)) ||
			(!writer->writeBits(deltaSize, 5)))
			throw new CodingException("DeltaEncoder: Could not write delta header in constructor");

		init=false;
		currVal=val_;
		return true;
	}
	else {
		if ((currVal>=(unsigned int) val_) && currVal!=0 && val_!=0)
			throw new CodingException("DeltaEncoder: failed to encode position as it is smaller than current position");
	
		bool success=true;
		unsigned int delta=val_-currVal;
		Log::writeToLog("DeltaEncoder", 0, "Writing delta=", delta);
		if (!full) {	
			if (escapeInt<=delta) {
				Log::writeToLog("DeltaEncoder", 0, "Delta does not fit in deltaSize, escapeInt=", escapeInt);
				success&=writer->writeBits(escapeInt, deltaSize);
				Log::writeToLog("DeltaEncoder", 0, "Wrote escapeInt=", escapeInt);
				if (success) {
					success&=writer->writeBits(delta, 32);
					Log::writeToLog("DeltaEncoder", 0, "Wrote delta in 32bits, delta=", delta);
				}
			}
			else {
				success&=writer->writeBits(delta, deltaSize);
				Log::writeToLog("DeltaEncoder", 0, "Delta fits in deltaSize");
			}
		}
		if (success) {
			size++;
			Log::writeToLog("DeltaEncoder", 0, "Successful in writing Delta, size=",size);
			currVal=val_;
		}
		return success;
	}
}
unsigned int DeltaEncoder::getBufferSize() {
	return writer->getNumBitsWritten();	
}
byte* DeltaEncoder::getBuffer() {
	Log::writeToLog("DeltaEncoder", 1, "Returning buffer, headerValue=", headerValue);
	Log::writeToLog("DeltaEncoder", 1, "Returning buffer, startVal=", startVal);
	Log::writeToLog("DeltaEncoder", 1, "Returning buffer, numValues=", size);
	byte* bfr=writer->getBuffer();
	DeltaHeader header(headerValue, startVal, size);
	*((DeltaHeader*) bfr)=header;
	init=true;
	size=1;
	return writer->getBuffer();
}



