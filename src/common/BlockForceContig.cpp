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
#include "BlockForceContig.h"

BlockForceContig::BlockForceContig() : Block() {
  vp = NULL;
}

BlockForceContig::BlockForceContig(const BlockForceContig& block_) : Block(block_) {
}

BlockForceContig::~BlockForceContig() {
  delete vp;
  delete utilityVP;
}

void BlockForceContig::resetBlock() {
  return baseBlock->resetBlock();
  currPos=0;
}

Block* BlockForceContig::clone(Block& block_)
{
  Block* newBlock = new BlockForceContig((BlockForceContig&)block_);
  return newBlock;
}

void BlockForceContig::setBaseBlock(BlockPosNonContig* block) {
  baseBlock = block;
  vp = block->vp->clone();
  utilityVP = block->vp->clone();
  resetBlock();
}
	
// Iterator access to block
bool BlockForceContig::hasNext() {
  return baseBlock->hasNext();
}

bool BlockForceContig::hasNext(int value_) {
  return baseBlock->hasNext(value_);
}

ValPos* BlockForceContig::getNext() {
  vp->set(currPos, baseBlock->getNext()->value);
  currPos++;
  return vp;
}

ValPos* BlockForceContig::peekNext() {
  utilityVP->set(currPos, baseBlock->peekNext()->value);
  //utilityPair->value = baseBlock->peekNext()->value;
  //utilityPair->position = currPos;
  return utilityVP;
}

// Zero indexed, gets the pair at this pos_
ValPos* BlockForceContig::getPairAtLoc(unsigned int loc_) {
  return baseBlock->getPairAtLoc(loc_);
}

//Like getPairAtLoc except set the regular pair variable rather than 
//the utilityPair variable so can be called by getNext().
ValPos* BlockForceContig::getPairAtLocNotUtility(unsigned int loc_) {
  return baseBlock->getPairAtLocNotUtility(loc_);
}

int BlockForceContig::getCurrLoc() {
  return currPos-1;
}

// return size of block		
int BlockForceContig::getSize() {
  return baseBlock->getSize();	
}

// return buffer size of block in bits
int BlockForceContig::getSizeInBits() {
  return baseBlock->getSizeInBits();	
}

ValPos* BlockForceContig::getStartPair() {
  return baseBlock->getStartPair();	
}
	
bool BlockForceContig::isOneValue() {
  return baseBlock->isOneValue();	
}

bool BlockForceContig::isValueSorted() {
  return baseBlock->isValueSorted();	
}
		
bool BlockForceContig::isPosSorted() {
  return baseBlock->isPosSorted();	
}

bool BlockForceContig::isBlockValueSorted() {
  return baseBlock->isBlockValueSorted();	
}

bool BlockForceContig::isPosContiguous() {
  return true;
}

bool BlockForceContig::isBlockPosSorted() {
  return true;
}

unsigned int BlockForceContig::getEndPosition() {
  return getStartPair()->position + getSize() - 1;
}

