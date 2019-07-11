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
// Decodes Bit Pages
// Problems: migfer@mit.edu


#include "BitDecoder.h"

BitDecoder::BitDecoder(bool valSorted_) : Decoder() {
	valSorted=valSorted_;
	block=new BitBlock(valSorted_);
	outBlock=new BasicBlock(valSorted_, true, true);
} 
BitDecoder::BitDecoder(byte* buffer_, bool valSorted_) : Decoder(buffer_) {
	valSorted=valSorted_;	
}
BitDecoder::BitDecoder(BitDecoder& decoder_) : Decoder(decoder_) {
	valSorted=decoder_.valSorted;
}
BitDecoder::~BitDecoder() {
	delete block;
	delete outBlock;
}
		
void BitDecoder::setBuffer(byte* buffer_) {
	Decoder::setBuffer(buffer_);
	if (buffer_==NULL) throw new UnexpectedException("BitDecoder: setBuffer with NULL");
	block->setBuffer(buffer_, 8*PAGE_SIZE);
}
int BitDecoder::getPageLengthBits() {
	return block->getSize()*block->getFieldSize();	
}

bool BitDecoder::hasNextBlock() {
	return block->hasNext();
}
Block* BitDecoder::getNextBlock()  {
	if (!block->hasNext()) return NULL;
	ValPos* pair=block->getNext();
	if (pair==NULL) return NULL;
	outBlock->setValue(pair);
	//outBlock->setValue(pair->value, pair->position);
	return outBlock;
}
Block* BitDecoder::skipToBlock(unsigned int blockPos_) {
	ValPos* pair=block->getPairAtLoc(blockPos_);
	if (pair==NULL) return NULL;
	outBlock->setValue(pair);
	//outBlock->setValue(pair->value, pair->position);
	return outBlock;
}
bool BitDecoder::skipToPos(unsigned int blockPos_) {
	ValPos* pair=block->getPairAtLoc(blockPos_);
	if (pair==NULL) return false;
	return true;
}
	
ValPos* BitDecoder::getStartVal() {
  ValPos* vp = block->getStartPair();
  //assert(vp->type == ValPos::INTTYPE);
  return vp;
}
unsigned int BitDecoder::getStartPos() {
	return block->getStartPair()->position;	
}
ValPos* BitDecoder::getEndVal() {
  ValPos* vp = block->getEndPair();
  //assert(vp->type == ValPos::INTTYPE);
  return vp;	
}
unsigned int BitDecoder::getEndPos() {
	return block->getEndPair()->position;	
}
	
unsigned int BitDecoder::getSize() {
	return block->getSize();
}
