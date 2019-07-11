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
#include "PosRLEBlock.h"
#include <stdio.h>

// Position RLE Blocks

PosRLEBlock::PosRLEBlock()
{
	currPair=NULL;
	numOccCounter=0;
	block=new RLEBlock(false);
	trip = new RLETriple(0,0,1);
	setTriple(trip);	
}
PosRLEBlock::PosRLEBlock(RLETriple* triple_)
{
	currPair=NULL;
	block=new RLEBlock(false);
	numOccCounter=0;
	trip = new RLETriple( *triple_ );
	setTriple(trip);
}

PosRLEBlock::PosRLEBlock(const PosRLEBlock& block_) {
  block=new RLEBlock(*block_.block);
  numOccCounter=0;
  
  // that was not proper clonig, just doing a setTriple without new!
  trip = new RLETriple( *block_.block->getTriple() );
  setTriple( block_.block->getTriple() == NULL ? NULL : trip );
	    //block_.block->getTriple());
}

PosBlock* PosRLEBlock::clone(PosBlock& block_)
{
  PosBlock* newBlock = new PosRLEBlock((PosRLEBlock&)block_);
  return newBlock;
}

PosRLEBlock::~PosRLEBlock() {
  delete block;
  delete trip;

}
void PosRLEBlock::setTriple(RLETriple* triple_) {
  block->setTriple(triple_);
}
RLETriple* PosRLEBlock::getTriple() {
  return block->getTriple();
}
bool PosRLEBlock::hasNext() {
	if (numOccCounter==0)
		return block->hasNext();
	else 
		return true;
}

unsigned int PosRLEBlock::getNext() {
	if (numOccCounter==0) {
		currPair=block->getNext();
	}
	if (currPair==NULL) throw new CodingException("PosRLEBlock: error no next position");
	numOccCounter=(numOccCounter+1)%numOccurences;
	return currPair->position;
}

unsigned int PosRLEBlock::peekNext() {
	if (numOccCounter==0) {
		return block->peekNext()->position;
	}
	if (currPair==NULL) throw new CodingException("PosRLEBlock: error no next position");
	return currPair->position;
}

// Zero indexed, gets the pair at this loc
unsigned int PosRLEBlock::getPosAtLoc(unsigned int loc_) {
  currPair=block->getPairAtLoc(loc_/numOccurences);
  //numOccCounter=loc_%numOccurences;
  if (currPair==NULL) throw new CodingException("PosRLEBlock: error no next position");
  return currPair->position;	
}
unsigned int PosRLEBlock::getCurrLoc() {
	return block->getCurrLoc()*numOccurences+numOccCounter;
}
// return size of block		
int PosRLEBlock::getSize() {
	return block->getSize()*numOccurences;
}
int PosRLEBlock::getSizeInBits() {
	return block->getSizeInBits();
}
unsigned int PosRLEBlock::getStartPos() {
	return block->getStartPair()->position;	
}

unsigned int PosRLEBlock::getEndPos() {
  return getPosAtLoc(getSize() - 1);
}

void PosRLEBlock::resetBlock() {	
	block->resetBlock();
	numOccCounter=0;
	currPair=NULL;
}

bool PosRLEBlock::skipToPos(unsigned int pos) {
  if (pos >= getStartPos() && pos <= getEndPos())
    return block->skipToLoc(pos-getStartPos());
  else
    return false;
}

PosBlock* PosRLEBlock::copySubset(int fromPos, int untilPos) {
  int startPos = getStartPos();
  int endPos = getEndPos();
  assert(startPos <= fromPos);
  assert(startPos <= untilPos);
  assert(fromPos <= endPos);
  assert(untilPos <= endPos);
  assert(fromPos <= untilPos);
  RLETriple* t = new RLETriple(NULL,fromPos, untilPos-fromPos+1);
  return new PosRLEBlock(t);
  delete t;
}

// Stream properties
bool PosRLEBlock::isPosSorted() {
  return true;	
}

// Stream properties
bool PosRLEBlock::isSparse() {
  return false;	
}
	
// Block properties
bool PosRLEBlock::isPosContiguous()  {
  return true;	
}

bool PosRLEBlock::isBlockPosSorted()  {
  return true;	
}

unsigned int PosRLEBlock::initBitStringIterator(unsigned int pos) {
  unsigned int startPos = getStartPos();
  unsigned int endPos = getEndPos();
  assert(pos>=startPos);
  assert(pos<=endPos);
  
  int adjustedStartPos;
  if (pos%32)
    adjustedStartPos = pos - ((pos % 32) - 1);
  else 
    adjustedStartPos = pos - 31;

  int adjustedEndPos;
  if (endPos%32)
    adjustedEndPos = endPos - ((endPos % 32) - 1);
  else 
    adjustedEndPos = endPos - 31;

  bitstringsize = (adjustedEndPos - adjustedStartPos) / 32;


  int index = (pos % 32) - 1;
  if (index == -1)
    index = 31;
  bitStringStart=0;
  for (int i = index; i<=31; i++)
    bitStringStart |= 1 << (31-i);

  int index2 = (endPos % 32) - 1;
  if (index2 == -1)
    index2 = 31;
  bitStringEnd=0;
  for (int i = 0; i <=index2; i++)
    bitStringEnd |= 1 << (31-i);

  if (bitstringsize == 1)
    bitStringStart &= bitStringEnd;

  bitStringCounter=0;

  return adjustedStartPos;

}

int PosRLEBlock::getNextBitString() {
  assert(bitStringCounter < bitstringsize);
  if (bitStringCounter == 0) {
    bitStringCounter++;
    return bitStringStart;
  }
  else if (bitStringCounter == bitstringsize-1) {
    bitStringCounter++;
    return bitStringEnd;
  }
  else {
    bitStringCounter++;
    return 0xff;
  }
}

//bool PosRLEBlock::PosRLEBlockIter::hasNext() const {
//	return numOccCounter == 0 ? blockIter->hasNext() : true; 
//}
/*
unsigned int PosRLEBlock::PosRLEBlockIter::getNext() {
	
	if (numOccCounter == 0) {
		currPair = blockIter->getNext();	
	} 
	assert(currPair != NULL);
	numOccCounter=(numOccCounter+1)%pRLEBlock->numOccurences;
	return currPair->position;
}*/

PosRLEBlock::PosRLEBlockIter::~PosRLEBlockIter() {
  delete andTrip;
  delete blockIter;
}

unsigned int PosRLEBlock::PosRLEBlockIter::peekNext() {
	if (numOccCounter==0) {
		return blockIter->peekNext()->position;
	}
	assert(currPair != NULL);
	return currPair->position;
}
unsigned int PosRLEBlock::PosRLEBlockIter::getCurrLoc() {
	return blockIter->getCurrLoc()*pRLEBlock->numOccurences+numOccCounter; 
}
void PosRLEBlock::PosRLEBlockIter::resetIterator() {
	
	numOccCounter = 0;
	currPair = NULL;
	blockIter->resetIterator();
}
bool PosRLEBlock::PosRLEBlockIter::skipToPos(unsigned int pos) {
  //cerr << "PosRLEBlock skipping to: " << pos << " with start pos = " << pRLEBlock->getStartPos() << endl;
	assert (pos-pRLEBlock->getStartPos() <= (getEndPos() -getStartPos()));
	if (pos >= pRLEBlock->getStartPos() && pos <= pRLEBlock->getEndPos()) {
		return blockIter->skipToLoc(pos-pRLEBlock->getStartPos()); 
	} else {
		return false;
	}
}

int PosRLEBlock::PosRLEBlockIter::findLocAtPosAndSkip(unsigned int pos) {
  unsigned int loc = pos-pRLEBlock->getStartPos();
  assert (loc <= (getEndPos() - getStartPos()));
  blockIter->skipToLoc(loc);
  return (int)loc;
}


int PosRLEBlock::PosRLEBlockIter::getSize() {
  return (endpos-startpos)+1;
}

unsigned int PosRLEBlock::PosRLEBlockIter::getStartPos() {
  return startpos;
}

unsigned int PosRLEBlock::PosRLEBlockIter::getEndPos() {
  return endpos;
}

unsigned int PosRLEBlock::PosRLEBlockIter::initBitStringIterator(unsigned int pos) {
  return pRLEBlock->initBitStringIterator(pos);
}

int PosRLEBlock::PosRLEBlockIter::getNextBitString() {
  return pRLEBlock->getNextBitString();
}

void PosRLEBlock::PosRLEBlockIter::posAnd(PosBlockIter* other, PosBlock* toWrite) {
  assert (other->isPosSorted());
  if ((!other->isPosContiguous()) || (other->isSparse()))
    throw new UnexpectedException("It isn't supposed to work this way. PosRLEBlock can only and with something if it can produce a PosRLEBlock as a result.");
	
//	cerr << "RLE posAnd of " << other->getStartPos() << " to " << other->getEndPos() << 
//			" and " << getStartPos() << " to " << getEndPos() << endl;
	PosRLEBlock* tw = (PosRLEBlock*) toWrite;
  //RLETriple* t1 = (RLEBlock::RLEBlockIter*)blockIter->getTriple();
  int sp1 = blockIter->getStartPair()->position;
  int sp2 = other->getStartPos();
  int rl1 = blockIter->getSize();
  int rl2 = other->getSize();
  int ep1 = sp1+rl1-1;
  int ep2 = sp2+rl2-1;
  int sp3;
  int ep3;
  
  if (sp1 < sp2) 
    sp3 = sp2;
  else
    sp3 = sp1;
  if (ep1 < ep2) 
    ep3 = ep1;
  else
    ep3 = ep2;
  andTrip = new RLETriple(blockIter->getStartPair(), sp3, ep3-sp3+1);
  tw->setTriple(andTrip);
 // cerr << "toWrite now from " << tw->getStartPos() << " to " << tw->getEndPos() << endl;
}

bool PosRLEBlock::PosRLEBlockIter::isPosSorted() {
  return pRLEBlock->isPosSorted();
}

bool PosRLEBlock::PosRLEBlockIter::isPosContiguous() {
  return pRLEBlock->isPosContiguous();
}

bool PosRLEBlock::PosRLEBlockIter::isBlockPosSorted() {
  return pRLEBlock->isBlockPosSorted();
}

bool PosRLEBlock::PosRLEBlockIter::isSparse() {
  return pRLEBlock->isSparse();
}
