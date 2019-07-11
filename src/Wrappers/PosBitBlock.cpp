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
#include "PosBitBlock.h"

// Position Bit Blocks
// Problems: migfer@mit.edu


PosBitBlock::PosBitBlock()
{
	block=new BitBlock(true);
	isPosAvail=false;
}

PosBitBlock::PosBitBlock(const PosBitBlock& block_) {
	block=new BitBlock(block_.block);
	isPosAvail=false;
}

PosBlock* PosBitBlock::clone(PosBlock& block_)
{
  PosBlock* newBlock = new PosBitBlock((PosBitBlock&)block_);
  return newBlock;
}

PosBitBlock::~PosBitBlock()
{
	delete block;
}

void PosBitBlock::setBuffer(byte* bfr_, int bfrSize_) {
	block->setBuffer(bfr_, bfrSize_);
	isPosAvail=false;
}
byte* PosBitBlock::getBuffer() {
	return block->getBuffer();	
}
// Iterator access to block
bool PosBitBlock::hasNext() {
	int value=0;
	if (isPosAvail==true) return true;
	while (value==0) {
		Pair* pair=block->getNext();
		if (pair==NULL) return false;
		value=pair->value;
		isPosAvail=true;
		numOccurences=pair->value;
		posAvail=pair->position;
	}
	return true;
}
unsigned int PosBitBlock::getNext() {
	if (isPosAvail) {
		isPosAvail=false;
		return posAvail;
	}
	else {
		if (hasNext()) {
			isPosAvail=false;
			return posAvail;
		}	
		else {
			throw new CodingException("PosBitBlock: error, no next pair");
		}
	}	
}

// Zero indexed, gets the pair at this loc
unsigned int PosBitBlock::getPosAtLoc(unsigned int loc_) {
	Pair* pair=block->getPairAtLoc(loc_);
	if (pair==NULL) throw new CodingException("PosBitBlock: error, no pair at that location");
	return pair->value;	
}
unsigned int PosBitBlock::getCurrLoc() {
	return block->getCurrLoc();
}

// return size of block		
int PosBitBlock::getSize() {
	return block->getSize();	
}
int PosBitBlock::getSizeInBits() {
	return block->getSizeInBits();
}

unsigned int PosBitBlock::getStartPos() {
	Pair* pair=block->getStartPair();
	if (pair==NULL) throw new CodingException("PosBitBlock: Error: start pair is null");
	return pair->position;
}

unsigned int PosBitBlock::getEndPos() {
  return getPosAtLoc(getSize() - 1);
}

void PosBitBlock::resetBlock() {
	isPosAvail=false;
	return block->resetBlock();
}	
bool PosBitBlock::isPosContiguous() {
	return true;	
}
