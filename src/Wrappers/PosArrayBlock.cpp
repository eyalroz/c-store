#include "PosArrayBlock.h"
#include <iostream.h>

PosArrayBlock::PosArrayBlock(int capacity)
{
  this->capacity = capacity;
  nelements = 0;
  nextpos = 0;
  positions = new unsigned int[capacity];
  currloc = 0;
  //repeats = 1;
}

PosArrayBlock::~PosArrayBlock()
{
  delete[] positions;
}

PosArrayBlock::PosArrayBlock(const PosArrayBlock& block_) {
  capacity = block_.capacity;
  nelements = block_.nelements;
  nextpos = 0;
  currloc = block_.nelements;
  //repeats = block_.repeats;
  positions = new unsigned int[block_.capacity];
  memcpy(positions, block_.positions, sizeof(int)*nelements);
}

PosBlock* PosArrayBlock::clone(PosBlock& block_) {
	return new PosArrayBlock((PosArrayBlock&)block_);
}

bool PosArrayBlock::hasNext() {
	//cerr << "hasNext called--returning " << (nextpos < nelements) << endl;
  return nextpos < (nelements/**repeats*/);
}

unsigned int PosArrayBlock::getNext() {
	//cerr << "Return position: " << positions[nextpos] << endl;
  return positions[nextpos++/*%nelements*/];
}

unsigned int PosArrayBlock::peekNext() {
	//cerr << "peeknext called" << endl;
  return positions[nextpos/*%nelements*/];
}

unsigned int PosArrayBlock::getCurrLoc() {
	//cerr << "getcurrloc called" << endl;
	return nextpos - 1;
}
	
unsigned int PosArrayBlock::getPosAtLoc(unsigned int loc_) {
  assert (loc_ < (unsigned int)getSize());
  return positions[loc_/*%nelements*/];
}

/*
void PosArrayBlock::setNumOcurrences(int numOccurences_) {
	throw new UnexpectedException("Set of numOccurences not supported");
}

int PosArrayBlock::getNumOcurrences() {
	cerr << "getNO called" << endl;
	//throw new UnexpectedException("Get of numOccurences not supported");
	return nelements;
}
*/

int PosArrayBlock::getSize() {
//	throw new UnexpectedException();
	//cerr << "getSize called; returns " << nelements << endl;
  return nelements/**repeats*/;
}

int PosArrayBlock::getSizeInBits() {
	throw new UnexpectedException("getSizeInBits not supported");
}

unsigned int PosArrayBlock::getStartPos() {
	//cerr << "called getstartpos; returns " << positions[0] << endl;
	return positions[0];
}

unsigned int PosArrayBlock::getEndPos() {
  return getPosAtLoc(getSize() - 1);
}

void PosArrayBlock::resetBlock() {
  //nelements = 0;
  nextpos = 0;
}

PosBlock* PosArrayBlock::copySubset(int fromPos, int untilPos) {
  //copy from fromPos to untilPos inclusive
  assert(isBlockPosSorted());
  int startPos = getStartPos();
  int endPos = getEndPos();
  assert(startPos <= fromPos);
  assert(startPos <= untilPos);
  assert(fromPos <= endPos);
  assert(untilPos <= endPos);
  assert(fromPos <= untilPos);
  PosArrayBlock* temp = new PosArrayBlock(capacity);
  int i = 0;
  while (positions[i] < (unsigned int)fromPos && i < nelements)
    i++;
  assert(i != nelements);
  if (positions[i] > (unsigned int)fromPos)
    i--;
  assert(i>=0);
  int j = 0;
  while (positions[i] <= (unsigned int)untilPos && i < nelements) {
    temp->setPosAtLoc(j, positions[i]);
    j++;
    i++;
  }
  assert(i < nelements || positions[i-1]==(unsigned int)untilPos);
  temp->setNumElements(j);
  return temp;

}

bool PosArrayBlock::isPosContiguous() {
	return false;
}

bool PosArrayBlock::isBlockPosSorted() {
	return true;
}

bool PosArrayBlock::isSparse() {
	return true;
}

bool PosArrayBlock::isPosSorted() {
	return true;
}

void PosArrayBlock::setPosVarCap(unsigned int pos) {
  if (currloc == capacity) {
    capacity*=2;
    unsigned int* tempbuf = new unsigned int[capacity];
    for (int i = 0; i < currloc; i++)
      tempbuf[i] = positions[i];
    delete[] positions;
    positions = tempbuf;
  }
  positions[currloc]=pos;
  currloc++;
  nelements = currloc;
}

void PosArrayBlock::setPosAtLoc(int loc, int pos) {
  if (loc >= capacity)
  positions[loc] = pos;
}

void PosArrayBlock::setBuffer(int* inBuffer, int numValues) {
  memcpy(positions, inBuffer, numValues*sizeof(int));
  nelements=numValues;
}

unsigned int PosArrayBlock::getThisOrNextPos(unsigned int pos, int* loc) {
  /*if (repeats != 1)
    throw new UnimplementedException("Not sure what to do in this case yet");*/
  int i = 0;
  while (positions[i] < pos) {
    i++;
    if (i >= nelements)
      throw new UnexpectedException("Illegal input to method");
  }
  *loc = i;
  return positions[i];
}

unsigned int PosArrayBlock::getThisOrPrevPos(unsigned int pos, int* loc) {
  /*if (repeats != 1)
    throw new UnimplementedException("Not sure what to do in this case yet");*/
  int i = (nelements-1);
  while (positions[i] > pos) {
    i--;
    if (i <= 0)
      throw new UnexpectedException("Illegal input to method");
  }
  *loc = i;
  return positions[i]; 
}

void PosArrayBlock::setNumElements(int n) {
	this->nelements = n;
}

unsigned int PosArrayBlock::initBitStringIterator(unsigned int pos) {
  throw new UnexpectedException("Not implemented yet.");
}

int PosArrayBlock::getNextBitString() {
  throw new UnexpectedException("Not implemented yet.");
}

PosArrayBlock::PosArrayBlockIter::PosArrayBlockIter(PosArrayBlock* pArrayBlock_, unsigned int spos, unsigned int epos) : pArrayBlock(pArrayBlock_) {
  if (spos < pArrayBlock->getStartPos()) {
    spos = pArrayBlock->getStartPos();
  }
  else if (spos == pArrayBlock->getStartPos())
    startLoc = 0;
  else
    spos = pArrayBlock->getThisOrNextPos(spos, &startLoc);
  
  if (epos > pArrayBlock->getEndPos()) {
    epos = pArrayBlock->getEndPos();
  }
  else if (epos == pArrayBlock->getEndPos())
    endLoc = pArrayBlock->getSize()-1;
  else
    epos = pArrayBlock->getThisOrPrevPos(epos, &endLoc);
  
  assert(spos <= epos);
  assert(spos <= pArrayBlock->getEndPos());
  assert(epos >= pArrayBlock->getStartPos());
  /*if ((pArrayBlock->repeats != 1) && ((spos != pArrayBlock->getStartPos()) || (epos != pArrayBlock->getEndPos())))
    throw new UnimplementedException("Not sure what to do in this case yet");*/
  
  /*if (spos > pArrayBlock->getEndPos() || epos < pArrayBlock->getStartPos()) {
    blockIter = new EmptyBlockIter();
    } else {				
    blockIter = pArrayBlock->block->getIterator(spos,epos);
    }*/
  startpos = spos;
  endpos = epos;
  resetIterator();
}



/*************************
for now ignore numOccCounter in all methods
*************************/

bool PosArrayBlock::PosArrayBlockIter::hasNext() {
	return nextPos <= endLoc;
}

unsigned int PosArrayBlock::PosArrayBlockIter::getNext() {
  return pArrayBlock->positions[nextPos++/*%pArrayBlock->nelements*/];
}
		
unsigned int PosArrayBlock::PosArrayBlockIter::peekNext() {
  return pArrayBlock->positions[nextPos/*%pArrayBlock->nelements*/];
}

unsigned int PosArrayBlock::PosArrayBlockIter::getCurrLoc() {
  throw new UnimplementedException("Think below code is wrong. So I will throw an execption in case it is ever used.");
  return pArrayBlock->positions[nextPos-1];
}

void PosArrayBlock::PosArrayBlockIter::resetIterator() {
  nextPos = startLoc;
}

bool PosArrayBlock::PosArrayBlockIter::skipToPos(unsigned int pos) {
  throw new UnimplementedException("This will be really expensive to calculate ... do I really need to ever do this?");
}

int PosArrayBlock::PosArrayBlockIter::findLocAtPosAndSkip(unsigned int pos) {
  /*if (pArrayBlock->repeats != 1)
    throw new UnimplementedException("Not sure what to do in this case yet");*/
  unsigned int* bufi = pArrayBlock->positions + startLoc;
  int nv = endLoc-startLoc+1;
  unsigned int leftend = 0;
  unsigned int rightend = nv-1;
  unsigned int midPoint = ((rightend-leftend) / 2);
  while (!((bufi[midPoint] >= pos) && (bufi[midPoint-1] < pos))) {
    if (pos < bufi[midPoint]) {
      rightend=midPoint;
      midPoint = (leftend + midPoint) / 2;
      if (midPoint == rightend)
	midPoint--;
    }
    else {
      leftend = midPoint;
      midPoint = (rightend + midPoint) / 2;
      if (midPoint == leftend)
	midPoint++;
    }
  }
  nextPos = midPoint+startLoc;
  return midPoint;
}

unsigned int PosArrayBlock::PosArrayBlockIter::getPosAtLoc(unsigned int loc_) {
  assert((int)loc_ < (endLoc-startLoc+1));
  assert(loc_ >= 0);
  return pArrayBlock->positions[startLoc+loc_];
}

int PosArrayBlock::PosArrayBlockIter::getSize() {
  return (endLoc-startLoc+1)/**pArrayBlock->repeats*/;
}

unsigned int PosArrayBlock::PosArrayBlockIter::getStartPos() {
  return startpos;
}

unsigned int PosArrayBlock::PosArrayBlockIter::getEndPos() {
  return endpos;
}

unsigned int PosArrayBlock::PosArrayBlockIter::initBitStringIterator(unsigned int pos) {
  throw new UnexpectedException("Unimplemented for now ...");
}

int PosArrayBlock::PosArrayBlockIter::getNextBitString() {
  throw new UnexpectedException("Unimplemented for now ...");
}

void PosArrayBlock::PosArrayBlockIter::posAnd(PosBlockIter* other, PosBlock* toWrite) {
    throw new UnexpectedException("Unimplemented for now ...");
}

void PosArrayBlock::PosArrayBlockIter::posOr(PosBlockIter* other, PosBlock* toWrite) {
    throw new UnexpectedException("Unimplemented for now ...");
}

bool PosArrayBlock::PosArrayBlockIter::isPosSorted() {
  return pArrayBlock->isPosSorted();
}

bool PosArrayBlock::PosArrayBlockIter::isPosContiguous() {
  return pArrayBlock->isPosContiguous();
}

bool PosArrayBlock::PosArrayBlockIter::isBlockPosSorted() {
  return pArrayBlock->isBlockPosSorted();
}

bool PosArrayBlock::PosArrayBlockIter::isSparse() {
  return pArrayBlock->isSparse();
}

PosBlock* PosArrayBlock::PosArrayBlockIter::clone(PosBlock& pb) {
	return new PosArrayBlockIter((PosArrayBlockIter&)pb);
}
