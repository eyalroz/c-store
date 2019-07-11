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

#include "SparseBlock.h"

SparseBlock::SparseBlock(bool valSorted_, bool posContig_, bool posSorted_) : MultiBlock(valSorted_, posContig_, posSorted_)
{
  pb=NULL;
  pbi=NULL;
  nullval = new byte[valsize];
  memset(nullval, 0, valsize);
  nullval[valsize-1] = NULLBYTE;
}

SparseBlock::SparseBlock(bool valSorted_, bool posContig_, bool posSorted_, short type_) : MultiBlock(valSorted_, posContig_, posSorted_, type_)
{
  pb=NULL;
  pbi=NULL;
  nullval = new byte[valsize];
  memset(nullval, 0, valsize);
  nullval[valsize-1] = NULLBYTE;
}

SparseBlock::SparseBlock(bool valSorted_, bool posContig_, bool posSorted_, ValPos* vp_) : MultiBlock(valSorted_, posContig_, posSorted_, vp_)
{
  pb=NULL;
  pbi=NULL;
  nullval = new byte[valsize];
  memset(nullval, 0, valsize);
  nullval[valsize-1] = NULLBYTE;
}

SparseBlock::SparseBlock(const SparseBlock& block_) : MultiBlock(block_.valSorted, block_.posContig, block_.posSorted, block_.vp)
{
  //pair=new Pair();
  //pair->set(*block_.pair);
  //utilityPair=new Pair();
  
  delete vp;
  delete utilityVP;
  vp=block_.vp->clone();
  utilityVP=block_.utilityVP->clone();
  //valSorted=block_.valSorted;
  //posContig=block_.posContig;
  //posSorted=block_.posSorted;
  buffer=NULL;
  buffer_size=0;
  setBuffer(block_.startPos, block_.endPos, block_.numValues, block_.buffer);
  pb=NULL;
  pbi=NULL;
}

Block* SparseBlock::clone(Block& block_)
{
  Block* newBlock = new SparseBlock((SparseBlock&)block_);
  return newBlock;
}

SparseBlock::~SparseBlock()
{
  //No need to delete the following (done in Multiblock)
  /*delete pair;
  delete utilityPair;
  if (buffer)
  delete[] buffer;*/
  delete pbi;
}

void SparseBlock::resetBlock() {
  currPos=0;
  currPosPtr=buffer;
  vp->set(startPos, startPtr);
  //pair->value=startVal;
  //pair->position=startPos;
  pbi->resetIterator();
  nullCount = pbi->getNext() - startPos;
}

void SparseBlock::setBuffer(int startpos_, int endpos_, int numValues_, byte* buffer_) {
  startPos=startpos_;
  endPos=endpos_;
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
  numPositions = endPos - startPos + 1;
  //assume setPosBlock will be called which takes care of the rest of the init
}

// Assumes buffer is being provided to us externally--we just set a pointer to it
void SparseBlock::setBufferDirect(int startpos_, int endpos_, int numValues_, byte* buffer_) {
  startPos = startpos_;
  endPos=endpos_;
  numPositions = endPos - startPos + 1;
  buffer = buffer_;
  buffer_size = numValues_ * valsize;
  currPosPtr = buffer;
  numValues = numValues_;
  //assume setPosBlock will be called which takes care of the rest of the init
}

void SparseBlock::setPosBlock(PosBlock* p) {
  pb = p;
  delete pbi;
  pbi = p->getIterator();
  //assume buffer already set for code below
  assert(pbi->getSize() == numValues);
  if (pbi->getStartPos() == (unsigned int)startPos)
    startPtr = buffer;
  else
    startPtr = nullval;
  resetBlock();
}

byte* SparseBlock::getBuffer() {
  return buffer;
}

//void SparseBlock::setValue(Pair* val_) {
//	currPos=-1;
//	pair->set(*val_);
//}

	
// Iterator access to block
bool SparseBlock::hasNext() {
	return (currPos<(numPositions));
}
bool SparseBlock::hasNext(int value_) {
  //assume values are integers for now ... fix later??
  return ((currPos<(numPositions)) && ((value_== NULLINT && nullCount) || ((*((int*)currPosPtr)) == value_)));
}

ValPos* SparseBlock::getNext() {
  if (nullCount) {
    vp->set(currPos+startPos, nullval);
    //pair->value=NULLINT;
    //pair->position=currPos+startPos;
    currPos++;
    nullCount--;
    return vp;
  }
  else if (currPos<(numPositions)) {
    vp->set(currPos+startPos, currPosPtr);
    //pair->value=*currPosPtr;
    currPosPtr+=valsize;
    //pair->position=currPos+startPos;
    //assert(currPos+startPos==pair->position);
    if (pbi->hasNext())
      nullCount = pbi->getNext() - startPos-currPos-1;
    else
      nullCount = numPositions-currPos-1;
    currPos++;
    return vp;
  }
  else
    return NULL;
}

ValPos* SparseBlock::peekNext() {
  if (nullCount) {
    utilityVP->set(currPos+startPos, nullval);
    //utilityPair->value=NULLINT;
    //utilityPair->position=currPos+startPos;
  }
  else {
    assert(hasNext());
    utilityVP->set(currPos+startPos, currPosPtr);
    ////assert(currPos+startPos == currPosPtr[1]);
    //utilityPair->value=*currPosPtr;
    //utilityPair->position=currPos+startPos;
  }
  return utilityVP;
  //else
  //  return NULL;
}

int SparseBlock::getValue() {
	if (vp->position==0) throw new CodingException("SparseBlock: cannot get value of empty multi block");
	if (vp->type != ValPos::INTTYPE) throw new CodingException("MultiBlock: this function shouldn't be used anymore, and definitely not if the ValPos is not of type integer");
	return *(int*)(vp->value);
}
unsigned int SparseBlock::getPosition() {
	if (vp->position==0) throw new CodingException("SparseBlock: cannot get position of empty multi block");	
	return vp->position;
}
unsigned int SparseBlock::getLastPosition() {
  return endPos;
}
unsigned int SparseBlock::getEndPosition() {
  return endPos;
}

// Zero indexed, gets the pair at this pos_
ValPos* SparseBlock::getPairAtLoc(unsigned int loc_) {
  assert (loc_<(unsigned int)numPositions);
  unsigned int postofind = loc_+startPos;
  int loc = pbi->findLocAtPosAndSkip(postofind);
  utilityVP->set(pbi->getNext(), buffer + (loc*valsize));
  return utilityVP;
}

//Like getPairAtLoc except set the regular pair variable rather than 
//the utilityPair variable so can be called by getNext().
ValPos* SparseBlock::getPairAtLocNotUtility(unsigned int loc_) {
  //throw UnexpectedException("this is going to be slow, do I have to do this?");
  assert (loc_<(unsigned int)numPositions);
  unsigned int postofind = loc_+startPos;
  int loc = pbi->findLocAtPosAndSkip(postofind);
  vp->set(pbi->getNext(), buffer + (loc*valsize));
  //pair->value=*(((int*)buffer)+loc);
  //pair->position=pbi->getNext();
  return vp;
}

void SparseBlock::setValAtLoc(unsigned int loc_, int val) {
  throw UnexpectedException("this is going to be slow, do I have to do this?");
  //*(((int*) buffer) + loc_) = val;
}	

bool SparseBlock::skipToLoc(unsigned int loc_) {
  throw UnexpectedException("this is going to be slow, do I have to do this?");
  /*  if ((loc_<(unsigned int)numPositions) && (loc_>=0)) {
    currPosPtr=((int*)buffer)+loc_;
    currPos=loc_;
    return true;
  }    
  else
  return false;*/
}

void SparseBlock::cutOffEnd(int endPos_) {
  if (endPos_ < endPos)
    numPositions=(endPos_-startPos)+1;
}

int SparseBlock::getCurrLoc() {
  throw UnexpectedException("This code seems wrong, does it ever get called?");
  if (currPos == -1)
    return 0;
  else
    return currPos-1;
}

// return size of block		
int SparseBlock::getSize() {
  return numPositions;	
}
//return buffer size in bits
int SparseBlock::getSizeInBits() {
  return (numValues)*valsize*8;	
}
ValPos* SparseBlock::getStartPair() {
  if (buffer==NULL) return NULL;
  utilityVP->set(startPos, startPtr);
  //utilityPair->value=startVal;
  //utilityPair->position=startPos;
  return utilityVP;
}
	
bool SparseBlock::isOneValue() {
  if (numPositions == 1)
    return true;
  else
    return false;
}
bool SparseBlock::isValueSorted() {
  return valSorted;	
}

bool SparseBlock::isPosContiguous() {
  return posContig;	
}
bool SparseBlock::isPosSorted() {
  return posSorted;	
}
bool SparseBlock::isBlockValueSorted() {
  return false;	
}
bool SparseBlock::isBlockPosSorted() {
  return true;
}

bool SparseBlock::isSplittable() {
  return numPositions > 1;
}

// FIXME make the two blocks share memory
Block* SparseBlock::split(int npositions) {
  throw UnexpectedException("I don't think split gets used anymore ... though it will eventually in which case this will have to be implemented");
  /*
  assert(npositions < numValues && npositions > 0); // sanity-check the split
  cerr << "want to retain " << npositions << "/" << numValues << endl;
  
  // Create the leftover block
  byte* leftover_buf = new byte[sizeof(int)*(numValues - npositions)];
  memcpy(leftover_buf, &(buffer[npositions*sizeof(int)]), sizeof(int)*(numValues - npositions));
  SparseBlock* leftover = new SparseBlock(true, true, true);
  leftover->setBuffer(startPos + npositions, 
		      (numValues - npositions), leftover_buf);
  delete [] leftover_buf;
  
  // Modify this block to reflect the split
  byte* newbuf = new byte[sizeof(int)*npositions];
  memcpy(newbuf, buffer, npositions);
  setBuffer(startPos, npositions, newbuf);
  delete [] newbuf;
  
  return leftover;
  */
}

SparseBlock* SparseBlock::copySubset(int fromPos, int untilPos) {
  throw UnexpectedException("this is going to be slow, do I have to do this?");
  /*
  //int endPos = getEndPosition();
  assert(startPos <= fromPos);
  assert(startPos <= untilPos);
  assert(fromPos <= endPos);
  assert(untilPos <= endPos);
  assert(fromPos <= untilPos);
  SparseBlock* temp = new SparseBlock(valSorted, posContig, posSorted);
  temp->setBuffer(startPos, untilPos-fromPos+1, buffer+(fromPos-startPos));
  return temp;
  */
}

bool SparseBlock::applyPredicate(Type2Block* t2b, Predicate* p) {
  ValPos* rhs = p->getRHS();
  if (rhs->type == ValPos::INTTYPE) {
    //t2b = new Type2Block(false);
    //int lastVal = -1;
    //int startLoc = pbi->getCurrLoc();
    unsigned int pos;
    int rhsint = *(int*)rhs->value;
    ValPos* x = vp->clone();
    p->setLHS(x);
    x->set(currPosPtr);
    byte* newPtr = currPosPtr;
    //for (; currPos < numValues; currPos++)
    //p->setLHS(currPosPtr[currPos]);  
    while ((!p->evalPredicate(rhsint)) && ((++currPos) < numValues)) {
      newPtr+=valsize;
      x->set(newPtr);
      //p->setLHS(currPosPtr[currPos]); 
      ////pos = pbi->getNext();
    }
    if (currPos < numValues) { //we stopped while loop via 1st end condition
      //assume there is 1 position for every value in this block
      pos = pbi->getPosAtLoc(currPos);
      x->position = pos;
      t2b->initEmptyBuffer(pos, x);
      t2b->addPosition(pos);
    }
    else return false;
    int currEndPos = pos;
    currPos++; 
    //pos = pbi->getNext();
    while (currPos < numValues) {
      newPtr+=valsize;
      x->set(newPtr);
      //p->setLHS(currPosPtr[currPos]);
      if (p->evalPredicate(rhsint)) {
	pos = pbi->getPosAtLoc(currPos);
	t2b->addPosition(pos);
	currEndPos = pos;
      }
      currPos++; 
      //pos = pbi->getNext();
    }
    t2b->finishBufferEditing(currEndPos);
    return true;
  }
  else {
    //t2b = new Type2Block(false);
    //int lastVal = -1;
    //int startLoc = pbi->getCurrLoc();
    unsigned int pos;
    ValPos* x = vp->clone();
    p->setLHS(x);
    x->set(currPosPtr);
    byte* newPtr = currPosPtr;
    //for (; currPos < numValues; currPos++)
    //p->setLHS(currPosPtr[currPos]);  
    while ((!p->evalPredicate()) && ((++currPos) < numValues)) {
      newPtr+=valsize;
      x->set(newPtr);
      //p->setLHS(currPosPtr[currPos]); 
      ////pos = pbi->getNext();
    }
    if (currPos < numValues) { //we stopped while loop via 1st end condition
      //assume there is 1 position for every value in this block
      pos = pbi->getPosAtLoc(currPos);
      x->position = pos;
      t2b->initEmptyBuffer(pos, x);
      t2b->addPosition(pos);
    }
    else return false;
    int currEndPos = pos;
    currPos++; 
    //pos = pbi->getNext();
    while (currPos < numValues) {
      newPtr+=valsize;
      x->set(newPtr);
      //p->setLHS(currPosPtr[currPos]);
      if (p->evalPredicate()) {
	pos = pbi->getPosAtLoc(currPos);
	t2b->addPosition(pos);
	currEndPos = pos;
      }
      currPos++; 
      //pos = pbi->getNext();
    }
    t2b->finishBufferEditing(currEndPos);
    return true;
  }
}
bool SparseBlock::SparseBlockIter::hasNext() {
	return currPos < sb->numPositions;
}

bool SparseBlock::SparseBlockIter::hasNext(int value_) {
  //assume values are integers for now ... fix later??
  return ((currPos<(sb->numPositions)) && ((value_== NULLINT && nullCount) || ((*((int*)currPosPtr)) == value_)));
}

ValPos* SparseBlock::SparseBlockIter::getNext() {
  if (nullCount) {
    vp->set(currPos+sb->startPos, sb->nullval);
    //pair->value=NULLINT;
    //pair->position=currPos+sb->startPos;
    currPos++;
    nullCount--;
    return vp;
  }
  else if (currPos<(sb->numPositions)) {
    //assert(currPos+sb->startPos==currPosPtr[1]);
    vp->set(currPos+sb->startPos, currPosPtr);
    //pair->value=*currPosPtr;
    currPosPtr+=valsize;
    //pair->position=currPos+sb->startPos;
    if (pbi->hasNext())
      nullCount = pbi->getNext() - sb->startPos-currPos-1;
    else
      nullCount = sb->numPositions-currPos-1;
    currPos++;
    return vp;
  }
  else
    return NULL;
}

ValPos* SparseBlock::SparseBlockIter::peekNext() {
  if (nullCount) {
    utilityVP->set(currPos+sb->startPos, sb->nullval);
    //utilityPair->value=NULLINT;
    //utilityPair->position=currPos+sb->startPos;
  }
  else {
    assert(hasNext());
    //assert(currPos+sb->startPos == currPosPtr[1]);
    utilityVP->set(currPos+sb->startPos, currPosPtr);
    //utilityPair->value=*currPosPtr;
    //utilityPair->position=currPos+sb->startPos;
  }
  return utilityVP;

}
void SparseBlock::SparseBlockIter::resetIterator() {
  currPos=0;
  currPosPtr=sb->buffer;
  vp->set(sb->startPos, sb->startPtr);
  //pair->value=sb->startVal;
  //pair->position=sb->startPos;
  pbi->resetIterator();
  nullCount = pbi->getNext() - sb->startPos;
}
int SparseBlock::SparseBlockIter::getCurrLoc() {
  throw UnexpectedException("This code seems wrong, does it ever get called?");
  if (currPos == -1)
    return 0;
  else
    return currPos-1;
}

bool SparseBlock::SparseBlockIter::skipToLoc(unsigned int loc_) {
  throw UnexpectedException("this is going to be slow, do I have to do this?");
  /*if ((loc_<(unsigned int)sb->numValues) && (loc_>=0)) {
    currPosPtr=((int*)sb->buffer)+loc_;
    currPos=loc_;
    return true;
  } else {
    return false;
    }*/
}

//faster than above because we don't need to worry about NULLs ... just skip to loc in current block  ... used by RangedSparseBlockIter
bool SparseBlock::SparseBlockIter::skipToBLoc(unsigned int loc_) {
  //note: this method resets currPos so be careful
  if ((loc_<(unsigned int)sb->numValues) && (loc_>=0)) {
    currPosPtr=(sb->buffer)+(loc_*valsize);
    currPos=0;
    return true;
  } else {
    throw UnexpectedException("Illegal use of skipToBLoc");
  }
}

ValPos* SparseBlock::SparseBlockIter::getPairAtLoc(unsigned int loc_) {
  throw UnexpectedException("this is going to be slow, do I have to do this?");
  /*utilityPair->value=*(((int*)sb->buffer)+loc_);
  utilityPair->position=loc_+sb->startPos;
  return utilityPair;*/
}

ValPos* SparseBlock::SparseBlockIter::getPairAtLocNotUtility(unsigned int loc_) {
  throw UnexpectedException("this is going to be slow, do I have to do this?");
  /*pair->value=*(((int*)sb->buffer)+loc_);
  pair->position=loc_+sb->startPos;
  return pair;*/
}

void SparseBlock::SparseBlockIter::setValAtLoc(unsigned int loc_, int val) {
  throw UnexpectedException("this is going to be slow, do I have to do this?");
	*(((int*) sb->buffer) + loc_) = val;
}
	
const byte* SparseBlock::SparseBlockIter::asArray() {
  if (arrayBuffer)
    delete[] arrayBuffer;
  arrayBuffer = new byte[sb->numPositions*valsize];
  int count = 0;
  int i;
  while (count < sb->numPositions) {
    for (i = 0; i < nullCount && count < sb->numPositions; i++) {
      memcpy(arrayBuffer+((count)*valsize), sb->nullval, valsize);
      //arrayBuffer[count+i] = NULLINT;
      count++;
    }
    if (count < sb->numPositions) {
      memcpy(arrayBuffer+((count)*valsize), currPosPtr, valsize);
      //arrayBuffer[count] = *currPosPtr;
      currPosPtr+=valsize;
      if (pbi->hasNext())
	nullCount = pbi->getNext() - sb->startPos-count-1;
      else
	nullCount = sb->numPositions-count-1;
    }
  }
  return arrayBuffer;
}

SparseBlock::RangedSparseBlockIter::RangedSparseBlockIter(SparseBlock* sb, unsigned int startPos_, unsigned int endPos_) : SparseBlockIter(sb)
{
  unsigned int sbsp = sb->getStartPair()->position;
  startPos_ = startPos_ > sbsp ? startPos_ : sbsp; // max
//  assert(startPos_ >= sbsp);
  //startLoc =  startPos_ - sbsp;
  //endLoc = endPos_-sbsp;
  startpos = startPos_;
  endpos = endPos_;
  numPositions = endPos_ - startPos_ + 1;
  pbi = sb->pb->getIterator();
  int count = 0;
  while (pbi->getNext() < startPos_)
    count++;
  startLoc = count;
  delete pbi;
  pbi = sb->pb->getIterator(startPos_, endPos_);
  resetIterator();
  //if (startLoc > 0 /*&& startLoc - 1 > 0 */&& !skipToLoc(startLoc/* - 1*/)) {
  //  throw UnexpectedException("cannot skip to loc");
  //}
}

bool SparseBlock::RangedSparseBlockIter::hasNext() {
  return currPos <= numPositions;
}

bool SparseBlock::RangedSparseBlockIter::hasNext(int value_) {
  //assume values are integers for now ... fix later??
  return ((currPos<(numPositions)) && ((value_== NULLINT && nullCount) || ((*((int*)currPosPtr)) == value_)));
}

void SparseBlock::RangedSparseBlockIter::resetIterator() {
  if (startLoc > 0) {
    pbi->resetIterator();
    nullCount = pbi->getNext() - startpos;
    skipToBLoc(startLoc);
    currPos=startpos-sb->startPos;
  } else {
    SparseBlockIter::resetIterator();
  }
  pbi->resetIterator();
}

/*bool SparseBlock::RangedSparseBlockIter::skipToLoc(unsigned int loc_) {
	assert(loc_ >= startLoc - 1 && loc_ <= endLoc);
	return SparseBlockIter::skipToLoc(loc_);
}

Pair* SparseBlock::RangedSparseBlockIter::getPairAtLoc(unsigned int loc_) {
  utilityPair->value=*(((int*)sb->buffer)+loc_+startLoc);
  utilityPair->position=loc_+startpos;
  return utilityPair;
}

Pair* SparseBlock::RangedSparseBlockIter::getPairAtLocNotUtility(unsigned int loc_) {
  pair->value=*(((int*)sb->buffer)+loc_+startLoc);
  pair->position=loc_+startpos;
  return pair;
}

void SparseBlock::RangedSparseBlockIter::setValAtLoc(unsigned int loc_, int val) {
	*(((int*)sb->buffer)+loc_+startLoc) = val;
	}*/

int SparseBlock::RangedSparseBlockIter::getCurrLoc() {
    throw UnexpectedException("Not currently implemented"); 
}

const byte* SparseBlock::RangedSparseBlockIter::asArray() {
  if (arrayBuffer)
    delete[] arrayBuffer;
  arrayBuffer = new byte[numPositions*valsize];
  int count = 0;
  int i;
  while (count < numPositions) {
    for (i = 0; i < nullCount && count < numPositions; i++) {
      memcpy(arrayBuffer+((count)*valsize), sb->nullval, valsize);
      //arrayBuffer[count+i] = NULLINT;
      count++;
    }
    if (count < numPositions) {
      memcpy(arrayBuffer+((count)*valsize), currPosPtr, valsize);
      //arrayBuffer[count] = *currPosPtr;
      currPosPtr+=valsize;
      if (pbi->hasNext())
	nullCount = pbi->getNext() - sb->startPos-count-1;
      else
	nullCount = numPositions-count-1;
    }
  }
  return arrayBuffer;
}
