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
// Decodes Type2 pages

#include "Type2Decoder.h"

Type2Decoder::Type2Decoder(bool valSorted_, ValPos** valindex_) : Decoder() {
  valindex = valindex_;
  block=new Type2Block(valSorted_);
  count=-1;
}

Type2Decoder::Type2Decoder(byte* bfr_, bool valSorted_, ValPos** valindex_) : Decoder(bfr_)
{
	if (bfr_==NULL) throw new CodingException("Type2Decoder: Error, buffer passed in constructor is NULL");
	valindex = valindex_;
	block=new Type2Block(valSorted_);
	block->setBufferWithHeader(bfr_);
	count=-1;
}

Type2Decoder::~Type2Decoder()
{
	delete block;	
}

void Type2Decoder::setBuffer(byte* bfr_) {
	Decoder::setBuffer(bfr_);
	block->setBufferWithHeader(bfr_);
	if (valindex && valindex[0])
	  block->setValue(valindex);
	count=-1;
}
bool Type2Decoder::hasNextBlock() {
	if (!initialized) return false;
	return (count<0);
}
Block* Type2Decoder::getNextBlock() {
	if (!initialized) throw new CodingException("Type2Decoder: Error, decoder not initialized");
	if (count<0) {
		count++;
		return block;
	}
	else 
		return NULL;
}
Block* Type2Decoder::skipToBlock(unsigned int blockPos_) {
	if (!initialized) throw new CodingException("Type2Decoder: Error, decoder not initialized");
	count=blockPos_;
	if (blockPos_==0)	
		return block;
	else 
		return NULL;
}
bool Type2Decoder::skipToPos(unsigned int blockPos_) {
	if (!initialized) throw new CodingException("Type2Decoder: Error, decoder not initialized");
	count=blockPos_;
	if (blockPos_==0)	
	  return true;
	else 
	  return false;
}
	
ValPos* Type2Decoder::getStartVal() {
	if (!initialized) throw new CodingException("Type2Decoder: Error, decoder not initialized");
	//return valindex[block->getValue()];
	return block->getValue();
}

unsigned int Type2Decoder::getStartPos() {
	if (!initialized) throw new CodingException("Type2Decoder: Error, decoder not initialized");
	return block->getStartPosition();
}

ValPos* Type2Decoder::getEndVal() {
	if (!initialized) throw new CodingException("Type2Decoder: Error, decoder not initialized");
	//return valindex[block->getValue()];	
	return block->getValue();
}

unsigned int Type2Decoder::getEndPos() {
	if (!initialized) throw new CodingException("Type2Decoder: Error, decoder not initialized");
	return block->getEndPosition();
}
unsigned int Type2Decoder::getSize() {
	return 	getEndPos()-getStartPos()+1;
}
