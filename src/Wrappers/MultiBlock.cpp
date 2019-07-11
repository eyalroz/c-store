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
// Single value block
// Coded by DNA

#include "MultiBlock.h"

MultiBlock::MultiBlock(bool valSorted_, bool posContig_, bool posSorted_) : Block()
{
  init(valSorted_, posContig_, posSorted_);
  setType(ValPos::INTTYPE);
}

MultiBlock::MultiBlock(bool valSorted_, bool posContig_, bool posSorted_, short type_) : Block()
{
  init(valSorted_, posContig_, posSorted_);
  setType(type_);
}

MultiBlock::MultiBlock(bool valSorted_, bool posContig_, bool posSorted_, ValPos* vp_) : Block()
{
  init(valSorted_, posContig_, posSorted_);
  setType(vp_);
}

void MultiBlock::init(bool valSorted_, bool posContig_, bool posSorted_) {
  valSorted=valSorted_ ;
  posContig=posContig_;
  posSorted=posSorted_;
  currPos=-1;
  buffer = NULL;
  buffer_size = 0;
  startPos=-1;
  //startVal=-1;
  numValues=0;
  bufferdirect = false;
  vp = NULL;
  utilityVP = NULL;
}

MultiBlock::MultiBlock(const MultiBlock& block_) : Block(block_)
{
  vp=block_.vp->clone();
  utilityVP=block_.utilityVP->clone();
  valSorted=block_.valSorted;
  posContig=block_.posContig;
  posSorted=block_.posSorted;
  buffer=NULL;
  buffer_size=0;
  setBuffer(block_.startPos, block_.numValues, block_.buffer);
}

Block* MultiBlock::clone(Block& block_)
{
  Block* newBlock = new MultiBlock((MultiBlock&)block_);
  return newBlock;
}

MultiBlock::~MultiBlock()
{
  delete vp;
  delete utilityVP;
  if (buffer && !bufferdirect)
    delete[] buffer;
}

void MultiBlock::resetBlock() {
  currPos=-1;
  currPosPtr=buffer;
  vp->set(startPos, currPosPtr);
  //pair->value=(*currPosPtr);
  //pair->position=startPos;
}
void MultiBlock::setBuffer(int startpos_, int numValues_, byte* buffer_) {

  currPos=-1;
  startPos=startpos_;
  int sz = numValues_*valsize;
  if (buffer_size != sz) {
    if (buffer)
      delete[] buffer;
    buffer_size=sz;
    buffer=new byte[sz];
  }
  memcpy(buffer, buffer_, numValues_*valsize);
  currPosPtr=buffer;
  numValues=numValues_;
  //startVal=(*currPosPtr);
  vp->set(startpos_, currPosPtr);
  //pair->value=(*currPosPtr);
  //pair->position=startpos_;
  bufferdirect = false;
}

// Assumes buffer is being provided to us externally--we just set a pointer to it
void MultiBlock::setBufferDirect(int startpos_, int numValues_, byte* buffer_) {
	currPos = -1;
	startPos = startpos_;
	bufferdirect = true;
	buffer = buffer_;
	buffer_size = numValues_ * valsize;
	currPosPtr = buffer;
	numValues = numValues_;
	//startVal = *currPosPtr;
	vp->set(startPos, currPosPtr);
	//pair->value = startVal;
	//pair->position = startPos;
}

byte* MultiBlock::getBuffer() {
  return buffer;
}

//void MultiBlock::setValue(Pair* val_) {
//	currPos=-1;
//	pair->set(*val_);
//}

	
// Iterator access to block
bool MultiBlock::hasNext() {
	return (currPos<(numValues));
}
bool MultiBlock::hasNext(int value_) {
  //assume for now that MultiBlock stores integers only
  return ((currPos<(numValues)) && ((*((int*)currPosPtr)) == value_));
}

ValPos* MultiBlock::getNext() {
  if (currPos<0) {
    currPos=1;	
    currPosPtr+=valsize;
    return vp;
  }
  else if (currPos<(numValues)) {
    vp->set(currPos+startPos, currPosPtr);
    //pair->value=*currPosPtr;
    currPosPtr+=valsize;
    //pair->position=currPos+startPos;
    currPos++;
    return vp;
  }
  else
    return NULL;
}

ValPos* MultiBlock::peekNext() {
  if (currPos<0) {
    utilityVP->set(startPos, buffer);
    //utilityPair->value=pair->value;
    //utilityPair->position=pair->position;
    return utilityVP;
  }
  else if (hasNext()) {
    utilityVP->set(currPos+startPos, currPosPtr);
    //utilityPair->value=*currPosPtr;
    //utilityPair->position=currPos+startPos;
    return utilityVP;
  }
  else
    return NULL;
}

int MultiBlock::getValue() {
	if (vp->position==0) throw new CodingException("MultiBlock: cannot get value of empty multi block");
	if (vp->type != ValPos::INTTYPE) throw new CodingException("MultiBlock: this function shouldn't be used anymore, and definitely not if the ValPos is not of type integer");
	return *(int*)(vp->value);
}
unsigned int MultiBlock::getPosition() {
	if (vp->position==0) throw new CodingException("MultiBlock: cannot get position of empty multi block");	
	return vp->position;
}
unsigned int MultiBlock::getLastPosition() {
  return getPairAtLoc(getSize()-1)->position;
}
unsigned int MultiBlock::getEndPosition() {
  return getPairAtLoc(getSize()-1)->position;
}

// Zero indexed, gets the pair at this pos_
ValPos* MultiBlock::getPairAtLoc(unsigned int loc_) {
  assert (loc_<(unsigned int)numValues);
  utilityVP->set(loc_+startPos, buffer + (loc_*valsize));
  //utilityPair->value=*(((int*)buffer)+loc_);
  //utilityPair->position=loc_+startPos;
  return utilityVP;
}

//Like getPairAtLoc except set the regular pair variable rather than 
//the utilityPair variable so can be called by getNext().
ValPos* MultiBlock::getPairAtLocNotUtility(unsigned int loc_) {
  assert (loc_<(unsigned int)numValues);
  vp->set(loc_+startPos, buffer + (loc_*valsize));
  //pair->value=*(((int*)buffer)+loc_);
  //pair->position=loc_+startPos;
  return vp;
}

void MultiBlock::setValAtLoc(unsigned int loc_, int val) {
  //this method should really not be used anymore
  if (vp->type != ValPos::INTTYPE) throw new CodingException("MultiBlock: this function shouldn't be used anymore, and definitely not if the ValPos is not of type integer");
  *(((int*) buffer) + loc_) = val;
}	

bool MultiBlock::skipToLoc(unsigned int loc_) {
  if ((loc_<(unsigned int)numValues) && (loc_>=0)) {
    currPosPtr=buffer+(valsize*loc_);
    currPos=loc_;
    return true;
  }    
  else
    return false;
}

void MultiBlock::cutOffEnd(int endPos) {
  if (endPos < startPos+numValues)
    numValues=(endPos-startPos)+1;
}

int MultiBlock::getCurrLoc() {
  if (currPos == -1)
    return 0;
  else
    return currPos-1;
}

// return size of block		
int MultiBlock::getSize() {
  return numValues;	
}
//return buffer size in bits
int MultiBlock::getSizeInBits() {
  return (numValues)*valsize*8;	
}
ValPos* MultiBlock::getStartPair() {
  if (buffer==NULL) return NULL;
  else return getPairAtLoc(0);
}
	
bool MultiBlock::isOneValue() {
  if (numValues == 1)
    return true;
  else
    return false;
}
bool MultiBlock::isValueSorted() {
	return valSorted;	
}
		
bool MultiBlock::isPosContiguous() {
	return posContig;	
}
bool MultiBlock::isPosSorted() {
	return posSorted;	
}
bool MultiBlock::isBlockValueSorted() {
	return false;	
}
bool MultiBlock::isBlockPosSorted() {
	return true;
}

bool MultiBlock::isSplittable() {
	return numValues > 1;
}

// FIXME make the two blocks share memory
Block* MultiBlock::split(int npositions) {
	assert(npositions < numValues && npositions > 0); // sanity-check the split
	cerr << "want to retain " << npositions << "/" << numValues << endl;
	
	// Create the leftover block
	byte* leftover_buf = new byte[valsize*(numValues - npositions)];
	memcpy(leftover_buf, &(buffer[npositions*valsize]), valsize*(numValues - npositions));
	MultiBlock* leftover = new MultiBlock(true, true, true);
	leftover->setBuffer(startPos + npositions, 
							(numValues - npositions), leftover_buf);
	delete [] leftover_buf;
	
	// Modify this block to reflect the split
	byte* newbuf = new byte[valsize*npositions];
	memcpy(newbuf, buffer, npositions);
	setBuffer(startPos, npositions, newbuf);
	delete [] newbuf;
	
	return leftover;
}

MultiBlock* MultiBlock::copySubset(int fromPos, int untilPos) {
  int endPos = getEndPosition();
  assert(startPos <= fromPos);
  assert(startPos <= untilPos);
  assert(fromPos <= endPos);
  assert(untilPos <= endPos);
  assert(fromPos <= untilPos);
  MultiBlock* temp = new MultiBlock(valSorted, posContig, posSorted);
  temp->setBuffer(startPos, untilPos-fromPos+1, buffer+(valsize*(fromPos-startPos)));
  return temp;
}

bool MultiBlock::applyPredicate(Type2Block* t2b, Predicate* p) {
  ValPos* rhs = p->getRHS();
  if (rhs->type == ValPos::INTTYPE) {
    int rhsint = *(int*)rhs->value;
    //unsigned int pos = currPos+startPos;
    if (currPos == -1)
      currPos = 0;
    //for (; currPos < numValues; currPos++)
    ValPos* x = vp->clone();
    p->setLHS(x);
    x->set(currPosPtr);
    int nv = numValues-1;
    byte* newPtr = currPosPtr;
    while ((!p->evalPredicate(rhsint)) && (currPos < nv)) {
      currPos++;
      newPtr+=valsize;
      x->set(newPtr);
      //p->setLHS(currPosPtr[currPos]); 
      //pos = pbi->getNext();
    }
    if (p->evalPredicate(rhsint)) { //we stopped while loop via 1st end condition
      x->position = currPos+startPos;
      t2b->initEmptyBuffer(currPos+startPos, x);
      t2b->addPosition(currPos+startPos);
    }
    else {
      delete x;
      return false;
    }
    int currEndPos = currPos+startPos;
    currPos++; 
    //pos = pbi->getNext();
    while (currPos < numValues) {
      newPtr+=valsize;
      x->set(newPtr);
      //p->setLHS(currPosPtr[currPos]);
      if (p->evalPredicate(rhsint)) {
	t2b->addPosition(currPos+startPos);
	currEndPos = currPos+startPos;
      }
      currPos++; 
      //pos = pbi->getNext();
    }
    t2b->finishBufferEditing(currEndPos);
    delete x;
    return true;
  }
  else {
    //unsigned int pos = currPos+startPos;
    if (currPos == -1)
      currPos = 0;
    //for (; currPos < numValues; currPos++)
    ValPos* x = vp->clone();
    p->setLHS(x);
    x->set(currPosPtr);
    int nv = numValues-1;
    byte* newPtr = currPosPtr;
    while ((!p->evalPredicate()) && (currPos < nv)) {
      currPos++;
      newPtr+=valsize;
      x->set(newPtr);
      //p->setLHS(currPosPtr[currPos]); 
      //pos = pbi->getNext();
    }
    if (p->evalPredicate()) { //we stopped while loop via 1st end condition
      x->position = currPos+startPos;
      t2b->initEmptyBuffer(currPos+startPos, x);
      t2b->addPosition(currPos+startPos);
    }
    else {
      delete x;
      return false;
    }
    int currEndPos = currPos+startPos;
    currPos++; 
    //pos = pbi->getNext();
    while (currPos < numValues) {
      newPtr+=valsize;
      x->set(newPtr);
      //p->setLHS(currPosPtr[currPos]);
      if (p->evalPredicate()) {
	t2b->addPosition(currPos+startPos);
	currEndPos = currPos+startPos;
      }
      currPos++; 
      //pos = pbi->getNext();
    }
    t2b->finishBufferEditing(currEndPos);
    delete x;
    return true;
  }
}

bool MultiBlock::MultiBlockIter::hasNext() {
	return currPos < mb->numValues;
}
bool MultiBlock::MultiBlockIter::hasNext(int value_) {
  //assume for now that MultiBlock stores integers only
  return currPos < mb->numValues && ((*(int*)currPosPtr) == value_);
}
ValPos* MultiBlock::MultiBlockIter::getNext() {
	assert(hasNext());
	if (currPos<0) {
	    currPos=1;	
	    currPosPtr+=valsize;
	    return vp;
	} else if (currPos<(mb->numValues)) {
	  vp->set(currPos+mb->startPos, currPosPtr);
	  //pair->value=*currPosPtr;
	  currPosPtr+=valsize;
	  //pair->position=currPos+mb->startPos;
	  ++currPos;
	  return vp;
  	}
  	else
    	return NULL;
}
ValPos* MultiBlock::MultiBlockIter::peekNext() {
	if (currPos<0) {
	  //utilityPair->value=pair->value;
	  //utilityPair->position=pair->position;
	  utilityVP->set(mb->startPos, mb->buffer);
	  return utilityVP;
  	}
  	else if (hasNext()) {
	  utilityVP->set(currPos+mb->startPos, currPosPtr);
	  //utilityPair->value=*currPosPtr;
	  //utilityPair->position=currPos+mb->startPos;
	  return utilityVP;
  	}
  	else
    	return NULL;
}
void MultiBlock::MultiBlockIter::resetIterator() {
	currPos=-1;
  	currPosPtr=mb->buffer;
	vp->set(mb->startPos, currPosPtr);
  	//pair->value=(*currPosPtr);
  	//pair->position=mb->startPos;
}
int MultiBlock::MultiBlockIter::getCurrLoc() {
	if (currPos == -1)
    	return 0;
  	else
    	return currPos-1;
}

bool MultiBlock::MultiBlockIter::skipToLoc(unsigned int loc_) {
	if ((loc_<(unsigned int)mb->numValues) && (loc_>=0)) {
    	currPosPtr=(mb->buffer)+(loc_*valsize);
    	currPos=loc_;
    	return true;
  	} else {
    	return false;
	}
}

ValPos* MultiBlock::MultiBlockIter::getPairAtLoc(unsigned int loc_) {
  //utilityPair->value=*(((int*)mb->buffer)+loc_);
  //utilityPair->position=loc_+mb->startPos;
  utilityVP->set(loc_+mb->startPos, mb->buffer+(valsize*loc_));
  return utilityVP;
}

ValPos* MultiBlock::MultiBlockIter::getPairAtLocNotUtility(unsigned int loc_) {
  //pair->value=*(((int*)mb->buffer)+loc_);
  //pair->position=loc_+mb->startPos;
  vp->set(loc_+mb->startPos, mb->buffer+(valsize*loc_));
  return vp;
}

void MultiBlock::MultiBlockIter::setValAtLoc(unsigned int loc_, int val) {
  //this method should really not be used anymore
  if (vp->type != ValPos::INTTYPE) throw new CodingException("MultiBlock: this function shouldn't be used anymore, and definitely not if the ValPos is not of type integer");
  *(((int*) mb->buffer) + loc_) = val;
}
	
const byte* MultiBlock::MultiBlockIter::asArray() {
	return mb->buffer;
}

MultiBlock::RangedMultiBlockIter::RangedMultiBlockIter(MultiBlock* mb, unsigned int startPos_, unsigned int endPos_) 
								: MultiBlockIter(mb)
{
  unsigned int mbsp = mb->getStartPair()->position;
  startPos_ = startPos_ > mbsp ? startPos_ : mbsp; // max
//  assert(startPos_ >= mbsp);
  startLoc =  startPos_ - mbsp;
  endLoc = endPos_-mbsp;
  startpos = startPos_;
  endpos = endPos_;

  if (startLoc > 0 /*&& startLoc - 1 > 0 */&& !skipToLoc(startLoc/* - 1*/)) {
    throw UnexpectedException("cannot skip to loc");
  }
}

bool MultiBlock::RangedMultiBlockIter::hasNext() {
	return currPos <= (int) endLoc; // DSM warning: type conversion
}

bool MultiBlock::RangedMultiBlockIter::hasNext(int value_) {
  //assume for now that MultiBlock stores integers only
  return hasNext() && (*(int*)currPosPtr) == value_;
}

void MultiBlock::RangedMultiBlockIter::resetIterator() {
	if (startLoc > 0) {
		skipToLoc(startLoc);
	} else {
		MultiBlockIter::resetIterator();
	}
}

bool MultiBlock::RangedMultiBlockIter::skipToLoc(unsigned int loc_) {
	assert(loc_ >= startLoc - 1 && loc_ <= endLoc);
	return MultiBlockIter::skipToLoc(loc_);
}

ValPos* MultiBlock::RangedMultiBlockIter::getPairAtLoc(unsigned int loc_) {
  //utilityPair->value=*(((int*)mb->buffer)+loc_+startLoc);
  //utilityPair->position=loc_+startpos;
  utilityVP->set(loc_+startpos, mb->buffer+(valsize*(loc_+startLoc)));
  return utilityVP;
}

ValPos* MultiBlock::RangedMultiBlockIter::getPairAtLocNotUtility(unsigned int loc_) {
  //pair->value=*(((int*)mb->buffer)+loc_+startLoc);
  //pair->position=loc_+startpos;
  vp->set(loc_+startpos, mb->buffer+(valsize*(loc_+startLoc)));
  return vp;
}

void MultiBlock::RangedMultiBlockIter::setValAtLoc(unsigned int loc_, int val) {
  //this method should really not be used anymore
  if (vp->type != ValPos::INTTYPE) throw new CodingException("MultiBlock: this function shouldn't be used anymore, and definitely not if the ValPos is not of type integer");
	*(((int*)mb->buffer)+loc_+startLoc) = val;
}

int MultiBlock::RangedMultiBlockIter::getCurrLoc() {
    throw UnexpectedException("Not currently implemented"); 
}

const byte* MultiBlock::RangedMultiBlockIter::asArray() {
	return mb->buffer + (startLoc * valsize);
}
