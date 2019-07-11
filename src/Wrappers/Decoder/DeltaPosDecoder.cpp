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
// Decodes DeltaPos pages
// Problems: migfer@mit.edu

#include "DeltaPosDecoder.h"

DeltaPosDecoder::DeltaPosDecoder(bool valSorted_) : Decoder() {
	block=new DeltaPosBlock(valSorted_);
	count=-1;
}

DeltaPosDecoder::DeltaPosDecoder(byte* bfr_, bool valSorted_) : Decoder(bfr_)
{
	if (bfr_==NULL) throw new CodingException("DeltaPosDecoder: Error, buffer passed in constructor is NULL");
	block=new DeltaPosBlock(bfr_, 8*PAGE_SIZE, valSorted_);
	count=-1;
}

DeltaPosDecoder::~DeltaPosDecoder()
{
	delete block;	
}

void DeltaPosDecoder::setBuffer(byte* bfr_) {
	Decoder::setBuffer(bfr_);
	block->setDeltaBuffer(bfr_, 8*PAGE_SIZE);
	count=-1;
}
bool DeltaPosDecoder::hasNextBlock() {
	if (!initialized) return false;
	return (count<0);
}
Block* DeltaPosDecoder::getNextBlock() {
	if (!initialized) throw new CodingException("DeltaPosDecoder: Error, decoder not initialized");
	if (count<0) {
		count++;
		return block;
	}
	else 
		return NULL;
}
Block* DeltaPosDecoder::skipToBlock(unsigned int blockPos_) {
	if (!initialized) throw new CodingException("DeltaPosDecoder: Error, decoder not initialized");
	count=blockPos_;
	if (blockPos_==0)	
		return block;
	else 
		return NULL;
}
bool DeltaPosDecoder::skipToPos(unsigned int blockPos_) {
	if (!initialized) throw new CodingException("DeltaPosDecoder: Error, decoder not initialized");
	count=blockPos_;
	if (blockPos_==0)	
	  return true;
	else 
	  return false; //DNA: this code seems odd - is this a potential bug?
}
	
ValPos* DeltaPosDecoder::getStartVal() {
	if (!initialized) throw new CodingException("DeltaPosDecoder: Error, decoder not initialized");
	throw new UnimplementedException("DeltaPosDecoder: do later");
	//return block->getValue();
	
}

unsigned int DeltaPosDecoder::getStartPos() {
	if (!initialized) throw new CodingException("DeltaPosDecoder: Error, decoder not initialized");
	int oldLoc=block->getCurrLoc();
	unsigned int start=block->getPairAtLoc(0)->position;
	block->getPairAtLoc(oldLoc);
	return start;
}

ValPos* DeltaPosDecoder::getEndVal() {
	if (!initialized) throw new CodingException("DeltaPosDecoder: Error, decoder not initialized");
	throw new UnimplementedException("DeltaPosDecoder: do later");
	//return block->getValue();	
}

unsigned int DeltaPosDecoder::getEndPos() {
	if (!initialized) throw new CodingException("DeltaPosDecoder: Error, decoder not initialized");
	throw new UnimplementedException("DeltaPosDecoder: do later");
	int oldLoc=block->getCurrLoc();
	unsigned int end=block->getPairAtLoc(block->getSize()-1)->position;
	block->getPairAtLoc(oldLoc);	
	return end;
}
unsigned int DeltaPosDecoder::getSize() {
	return 	getEndPos()-getStartPos()+1;
}
