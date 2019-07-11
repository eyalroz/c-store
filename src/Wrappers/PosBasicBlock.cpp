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
#include "PosBasicBlock.h"
#include <cstdio>

PosBasicBlock::PosBasicBlock(bool posSorted_)
{
	bBlock=new BasicBlock(true, true, posSorted_);
	posSorted = posSorted_;
	numOccCounter=0;
	currPair=NULL;
}

PosBasicBlock::~PosBasicBlock()
{
	delete bBlock;
}

PosBasicBlock::PosBasicBlock(const PosBasicBlock& block_)
{
  bBlock=new BasicBlock(*block_.bBlock); 
  numOccCounter=0;
  currPair=NULL;
 
}

PosBlock* PosBasicBlock::clone(PosBlock& block_)
{
  //cout << " Look at me, I ama PosBasicBlock call! " << endl;
  PosBlock* newBlock = new PosBasicBlock((PosBasicBlock&)block_);
  return newBlock;
}

unsigned int PosBasicBlock::getPosition() {
	return bBlock->getPosition();
}
void PosBasicBlock::setPosition(unsigned int pos_) {
	bBlock->setValue(1, pos_);
}
// Iterator access to block
bool PosBasicBlock::hasNext() {  
  if (numOccCounter==0)
    return bBlock->hasNext();
  else return true;
}

unsigned int PosBasicBlock::getNext() {  
  if (numOccCounter==0) {
    currPair = bBlock->getNext();
  }
  assert(currPair!=NULL);
  numOccCounter=(numOccCounter+1)%numOccurences;
  return currPair->position;
}
unsigned int PosBasicBlock::peekNext() {  
  if (numOccCounter==0) 
    return bBlock->peekNext()->position;
  assert(currPair!=NULL);
  return currPair->position;
}
// Zero indexed, gets the pair at this loc
unsigned int PosBasicBlock::getPosAtLoc(unsigned int loc_) {  
  //currpair=block->getPairAtLoc(loc_%(block->getSize()));
  //numOccCounter=loc_%numOccurences;
  currPair=bBlock->getPairAtLoc(loc_/numOccurences);
  assert(currPair!=NULL);
  return currPair->position;	
}
unsigned int PosBasicBlock::getCurrLoc() {  
  return bBlock->getCurrLoc()*numOccurences+numOccCounter;
}
	
int PosBasicBlock::getSize() {  
	return numOccurences;
}
int PosBasicBlock::getSizeInBits() {  
  throw new UnexpectedException("Shouldn't be used");
  return 64;//probably not right but I don't think used anymore
}
/*
unsigned int PosBasicBlock::getStartPos() const {  
	return bBlock->getStartPair()->position;
}
*/
unsigned int PosBasicBlock::getStartPos() {  
	return bBlock->getStartPair()->position;
}
/*
unsigned int PosBasicBlock::getEndPos() const {
  return bBlock->getStartPair()->position;
}*/

unsigned int PosBasicBlock::getEndPos() {
  return bBlock->getStartPair()->position;
}

void PosBasicBlock::resetBlock() {  
  bBlock->resetBlock();
  numOccCounter=0;
  currPair=NULL;
}

PosBlock* PosBasicBlock::copySubset(int fromPos, int untilPos) {
  int startPos = getStartPos();
  int endPos = getEndPos();
  assert(startPos <= fromPos);
  assert(startPos <= untilPos);
  assert(fromPos <= endPos);
  assert(untilPos <= endPos);
  assert(fromPos <= untilPos);
  PosBasicBlock* temp = new PosBasicBlock(posSorted);
  temp->setPosition(fromPos);
  return temp;
}	
	
// Stream properties
bool PosBasicBlock::isPosSorted() {  
	return posSorted;
}
bool PosBasicBlock::isSparse() {  
	return true;
}
	
// Block properties
bool PosBasicBlock::isPosContiguous() {  
	return true;
}
bool PosBasicBlock::isBlockPosSorted() {  
	return true;
}

unsigned int PosBasicBlock::initBitStringIterator(unsigned int pos) {
  assert (pos==getPosition());
  int index = (pos % 32) - 1;
  if (index == -1)
    index = 31;
  bitString = 1 << index;
  bitStringPtr = &bitString;
  if (pos % 32)
    return pos - ((pos % 32) - 1);
  else {
    return pos - 31;
  }
}

int PosBasicBlock::getNextBitString() {
  if (bitStringPtr)
    bitStringPtr=NULL;
  return bitString;
}
