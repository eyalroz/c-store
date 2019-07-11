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
// Multi value block using PosBlockArray

#include "MultiBlockWithPos.h"

MultiBlockWithPos::MultiBlockWithPos(bool valSorted_, bool posContig_, bool posSorted_) : MultiBlock(valSorted_, posContig_, posSorted_)
{
  posBlockArray=NULL;
}

MultiBlockWithPos::MultiBlockWithPos(const MultiBlockWithPos& block_) : MultiBlock(block_)
{
  posBlockArray=block_.posBlockArray;
}

Block* MultiBlockWithPos::clone(Block& block_)
{
  Block* newBlock = new MultiBlockWithPos((MultiBlockWithPos&)block_);
  return newBlock;
}

MultiBlockWithPos::~MultiBlockWithPos()
{
}

void MultiBlockWithPos::resetBlock() {
  MultiBlock::resetBlock();
  posBlockArray->reset();
}

void MultiBlockWithPos::setMB(MultiBlock* block) {
  int startpos_=block->getStartPair()->position;
  int numValues_=block->getSize();
  byte* buffer_=block->getBuffer();
  setBuffer(startpos_, numValues_, buffer_);
}


/*void Block::setPosBlockArray(PosBlockArray* pba) {
  posBlockArray=pba;
  }*/

/*
// Assume:
// 1. posBlockArray has been set (or else will throw an exception)
// 2. position range covered by posBlockArray matches exactly position
//    range covered by this block
bool MultiBlockWithPos::hasNext() {
  return posBlockArray->hasNext();
}
bool MultiBlockWithPos::hasNext(int value_) {
  return ((posBlockArray->hasNext()) && ((getPairAtLoc(posBlockArray->peekNext() - startPos)->value) == value_));
}

Pair* MultiBlockWithPos::getNext() {
  //call getPairAtLocPair rather than regular getPairAtLoc because we want this method to set the regular pair variable rather than the utilityPair variable (so we can call peekNext without changing the pointer returned by this method.
    return getPairAtLocPair(posBlockArray->getNext() - startPos);
}

Pair* MultiBlockWithPos::peekNext() {
    return getPairAtLoc(posBlockArray->peekNext() - startPos);
}
*/

/*
int MultiBlockWithPos::getValue() {
  return getPairAtLoc(posBlockArray->getPosAtLoc(posBlockArray->getCurrLoc()) - startPos)->value;
}

unsigned int MultiBlockWithPos::getPosition() {
  return posBlockArray->getPosAtLoc(posBlockArray->getCurrLoc());
}
*/

unsigned int MultiBlockWithPos::getLastPosition() {
  return posBlockArray->getEndPos();
}
unsigned int MultiBlockWithPos::getEndPosition() {
  return posBlockArray->getEndPos();
}

// Zero indexed, gets the pair at this pos_ and points that 
Pair* MultiBlockWithPos::getPairAtLoc(unsigned int loc_) {
  //if (loc_<(unsigned int)numValues) {
  utilityPair->value=*(((int*)(buffer))+loc_);
  utilityPair->position=loc_+startPos;
  return utilityPair;
    //}
    //else 
    //return NULL;
}

  //This method to set the regular pair variable rather than the utilityPair variable so can be called by getNext(). See comment in getNext() for reasoning.
// Zero indexed, gets the pair at this pos_ 
Pair* MultiBlockWithPos::getPairAtLocPair(unsigned int loc_) {
  pair->value=*(((int*)(buffer))+loc_);
  pair->position=loc_+startPos;
  return pair;
}

//Get pair at the derived loc ... i.e. if loc_ = 4 then get the pair not
// at the fourth position in this buffer (like above) but at the position
// of the fourth position in the posBlockArray
Pair* MultiBlockWithPos::getPairAtDerivedLoc(unsigned int loc_) {
  int realpos = posBlockArray->getPosAtLoc(loc_);
  utilityPair->value=*(((int*)(buffer))+(realpos-startPos));
  utilityPair->position=realpos;
  return pair;
}

/*void MultiBlockWithPos::cutOffEnd(int endPos) {
  if (endPos < startPos+numValues)
    numValues=(endPos-startPos)+1;
    }*/

int MultiBlockWithPos::getCurrLoc() {
  return posBlockArray->getCurrLoc();
}

// return size of block		
int MultiBlockWithPos::getSize() {
  return posBlockArray->getSize();
}

//return buffer size in bits
int MultiBlockWithPos::getSizeInBits() {
  return (numValues)*sizeof(int)*8;	
}

Pair* MultiBlockWithPos::getStartPair() {
  if (buffer==NULL) return NULL;
  else return getPairAtLoc(0);
}
	
bool MultiBlockWithPos::isOneValue() {
    return false;
}

bool MultiBlockWithPos::isPosContiguous() {
  return false;
}

/////////////////////////////////////////////////////////////////////

	

