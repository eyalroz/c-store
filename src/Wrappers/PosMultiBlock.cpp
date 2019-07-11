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
#include "PosMultiBlock.h"
#include <cstdio>

PosMultiBlock::PosMultiBlock(bool posSorted_)
{
	bBlock=new MultiBlock(true, true, posSorted_);
	posSorted = posSorted_;
}
PosMultiBlock::PosMultiBlock(MultiBlock* mb)
{
	bBlock=mb;
	posSorted = mb->isPosSorted();
}

PosMultiBlock::~PosMultiBlock()
{
	delete bBlock;
}

PosMultiBlock::PosMultiBlock(const PosMultiBlock& block_)
{
  bBlock=new MultiBlock(*block_.bBlock);  
}

PosBlock* PosMultiBlock::clone(PosBlock& block_)
{
  //cout << " Look at me, I am a PosMultiBlock clone call " << endl;
  PosBlock* newBlock = new PosMultiBlock((PosMultiBlock&)block_);
  return newBlock;
}

void PosMultiBlock::setMB(MultiBlock* mb) {
	bBlock=mb;
}

unsigned int PosMultiBlock::getPosition() {
	return bBlock->getPosition();
}

// Iterator access to block
bool PosMultiBlock::hasNext() {  
	return bBlock->hasNext();
}

unsigned int PosMultiBlock::getNext() {  
	return bBlock->getNext()->position;
}
unsigned int PosMultiBlock::peekNext() {  
	return bBlock->peekNext()->position;
}
// Zero indexed, gets the position at this loc
unsigned int PosMultiBlock::getPosAtLoc(unsigned int loc_) {  
	return bBlock->getPairAtLoc(loc_)->position;
}
unsigned int PosMultiBlock::getCurrLoc() {  
	return bBlock->getCurrLoc();
}

unsigned int PosMultiBlock::getEndPos() {
  return getPosAtLoc(getSize() - 1);
}
	
int PosMultiBlock::getSize() {  
	return bBlock->getSize();
}
int PosMultiBlock::getSizeInBits() {  
  throw new UnimplementedException("Not implemented yet");
}
unsigned int PosMultiBlock::getStartPos() {  
	return bBlock->getStartPair()->position;
}
void PosMultiBlock::resetBlock() {  
	bBlock->resetBlock();
}	

bool PosMultiBlock::skipToPos(unsigned int pos) {
  assert(bBlock->isPosContiguous());
  if (pos >= getStartPos() && pos <= getEndPos())
    return bBlock->skipToLoc(pos-getStartPos());
  else
    return false;
}

PosBlock* PosMultiBlock::copySubset(int fromPos, int untilPos) {
  int startPos = getStartPos();
  int endPos = getEndPos();
  assert(startPos <= fromPos);
  assert(startPos <= untilPos);
  assert(fromPos <= endPos);
  assert(untilPos <= endPos);
  assert(fromPos <= untilPos);
  //Type2Block* temp = new Type2Block(block->isValueSorted());
  //temp->setBufferWithHeader(block->makeNewBuffer(block->getValue(),fromPos,untilPos));
  //return temp;
  return new PosMultiBlock(bBlock->copySubset(fromPos, untilPos));
}
	
// Stream properties
bool PosMultiBlock::isPosSorted() {  
	return posSorted;
}
bool PosMultiBlock::isSparse() {  
	return true;
}
	
// Block properties
bool PosMultiBlock::isPosContiguous() {  
	return true;
}
bool PosMultiBlock::isBlockPosSorted() {  
	return true;
}

unsigned int PosMultiBlock::initBitStringIterator(unsigned int pos) {
  throw new UnexpectedException("Not implemented yet.");
}

int PosMultiBlock::getNextBitString() {
  throw new UnexpectedException("Not implemented yet.");
}

bool PosMultiBlock::PosMultiBlockIter::hasNext() {
	return blockIter->hasNext();
}
unsigned int PosMultiBlock::PosMultiBlockIter::getNext() {
	return blockIter->getNext()->position;
}

unsigned int PosMultiBlock::PosMultiBlockIter::peekNext() {
	return blockIter->peekNext()->position;
}

unsigned int PosMultiBlock::PosMultiBlockIter::getCurrLoc() {
	return blockIter->getCurrLoc();
}

void PosMultiBlock::PosMultiBlockIter::resetIterator() {
	blockIter->resetIterator();
}

bool PosMultiBlock::PosMultiBlockIter::skipToPos(unsigned int pos) {
	assert(pmb->bBlock->isPosContiguous());
 	if (pos >= pmb->getStartPos() && pos <= pmb->getEndPos())
   		return ((MultiBlock::MultiBlockIter*)blockIter)->skipToLoc(pos-pmb->getStartPos());
  	else
    	return false;
}

PosMultiBlock::PosMultiBlockIter::PosMultiBlockIter(PosMultiBlock* pmb_) : pmb(pmb_), blockIter(pmb->bBlock->getIterator()) {
  startpos = pmb->getStartPos();
  endpos = pmb->getEndPos();
  startLoc = 0;
  endLoc = pmb->getSize() - 1;
}

PosMultiBlock::PosMultiBlockIter::PosMultiBlockIter(PosMultiBlock* pmb_, unsigned int spos, unsigned int epos) : pmb(pmb_)
{
	if (spos < pmb->getStartPos()) {
		spos = pmb->getStartPos();
	}
	if (epos > pmb->getEndPos()) {
		epos = pmb->getEndPos();
	}
	
	assert(spos <= epos);

	if (spos > pmb->getEndPos() || epos < pmb->getStartPos()) {
	  blockIter = new EmptyBlockIter();
	} else {				
	  blockIter = pmb->bBlock->getIterator(spos, epos);
	}
	startpos = spos;
	endpos = epos;
	startLoc = spos - pmb->getStartPos();
	endLoc = epos - pmb->getStartPos();
}
	



int PosMultiBlock::PosMultiBlockIter::getSize() {
  //Is this right???? Right now unchecked ...
  return (endpos-startpos)+1;
}

unsigned int PosMultiBlock::PosMultiBlockIter::getStartPos() {
  return startpos;
}

unsigned int PosMultiBlock::PosMultiBlockIter::getEndPos() {
  return endpos;
}

unsigned int PosMultiBlock::PosMultiBlockIter::initBitStringIterator(unsigned int pos) {
  return pmb->initBitStringIterator(pos);
}

int PosMultiBlock::PosMultiBlockIter::getNextBitString() {
  return pmb->getNextBitString();
}

bool PosMultiBlock::PosMultiBlockIter::isPosSorted() {
  return pmb->isPosSorted();
}

bool PosMultiBlock::PosMultiBlockIter::isPosContiguous() {
  return pmb->isPosContiguous();
}

bool PosMultiBlock::PosMultiBlockIter::isBlockPosSorted() {
  return pmb->isBlockPosSorted();
}

bool PosMultiBlock::PosMultiBlockIter::isSparse() {
  return pmb->isSparse();
}

