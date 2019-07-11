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
#include "PosDeltaBlock.h"

// Position Delta Blocks
// Problems: migfer@mit.edu


PosDeltaBlock::PosDeltaBlock()
{
	block=new DeltaPosBlock(false);
	isBlockExternal=false;
}

PosDeltaBlock::PosDeltaBlock(const PosDeltaBlock& block_) {
	block=new DeltaPosBlock(block_.block);
	isBlockExternal=false;
}

PosBlock* PosDeltaBlock::clone(PosBlock& block_)
{
  PosBlock* newBlock = new PosDeltaBlock((PosDeltaBlock&)block_);
  return newBlock;
}

PosDeltaBlock::~PosDeltaBlock()
{
	if (!isBlockExternal) delete block;

}
	
void PosDeltaBlock::setBuffer(byte* bfr_, unsigned int bfrSize_) {
	block->setDeltaBuffer(bfr_, bfrSize_);
}
void PosDeltaBlock::setDeltaBlock(DeltaPosBlock* blk_) {
	if (!isBlockExternal) delete block;
	block=blk_;
	isBlockExternal=true;
}
byte* PosDeltaBlock::getBuffer() {
	return block->getBuffer();
}	
bool PosDeltaBlock::hasNext() {
	return block->hasNext();
}
unsigned int PosDeltaBlock::getNext() {
	ValPos* pair=block->getNext();
	if (pair==NULL) throw new CodingException("PosRLEBlock: error no next position");
	return pair->position;
}
unsigned int PosDeltaBlock::peekNext() {
	ValPos* pair=block->getNext();
	if (pair==NULL) throw new CodingException("PosRLEBlock: error no next position");
	return pair->position;
}

// Zero indexed, gets the pair at this loc
unsigned int PosDeltaBlock::getPosAtLoc(unsigned int loc_) {
	ValPos* pair=block->getPairAtLoc(loc_);
	if (pair==NULL) throw new CodingException("PosRLEBlock: error no next position");
	return pair->position;	
}

unsigned int PosDeltaBlock::getCurrLoc() {
	return block->getCurrLoc();
}

// return size of block		
int PosDeltaBlock::getSize() {
	return block->getSize();	
}

int PosDeltaBlock::getSizeInBits() {
	return block->getSizeInBits();	
}

unsigned int PosDeltaBlock::getStartPos() {
	return block->getStartPair()->position;
}

unsigned int PosDeltaBlock::getEndPos() {
  return getPosAtLoc(getSize() - 1);
}

void PosDeltaBlock::resetBlock() {	
	block->resetBlock();
}

PosBlock* PosDeltaBlock::copySubset(int fromPos, int untilPos) {
  throw new UnimplementedException("Not done yet");
}

// Stream properties
bool PosDeltaBlock::isPosSorted() {
	return false;
}
bool PosDeltaBlock::isSparse() {
	return true;	
}	
	// Block properties
bool PosDeltaBlock::isPosContiguous() {
	return false;	
}
bool PosDeltaBlock::isBlockPosSorted() {
	return true;	
}

unsigned int PosDeltaBlock::initBitStringIterator(unsigned int pos) {
  throw new UnexpectedException("Not implemented yet.");
}

int PosDeltaBlock::getNextBitString() {
  throw new UnexpectedException("Not implemented yet.");
}
