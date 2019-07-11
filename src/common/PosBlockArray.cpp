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
#include "PosBlockArray.h"

PosBlockArray::PosBlockArray(PosBlock** pba, int numEntries) 
{
  posBlockArray=pba;
  size = numEntries;
  currindex = 0;
  numPositions=0;
  sizes=new int[size];
  posSorted = true;
  posContig = true;
  blockPosSorted = true;
  sparse = false;
  startEndPosValid = false;
 // startPos = endPos = 0;
  int sparseCount = 0;
  for (int i=0; i < size; i++) {
    sizes[i] = pba[i]->getSize();
    numPositions += sizes[i];
    if (!pba[i]->isPosSorted())
      posSorted=false;
    if (!pba[i]->isPosContiguous())
      posContig=false;
    if (!pba[i]->isBlockPosSorted())
      blockPosSorted=false;
    if (pba[i]->isSparse())
      sparseCount++;
  }
  if (sparseCount > (size / 2))
    sparse = true;
  first = -1;
  last = -1;
  
  // Initializer iterator
 // ourIter = PosBlockArrayIter(this);
}

PosBlockArray::PosBlockArray(PosBlock** pba, int numEntries, unsigned int startPos, unsigned int endPos) 								
{
  int iter = 0;
  PosBlock* temp1;
  PosBlock* temp2;
//  cerr << "PBA: start pos is: " << startPos << " and endpos is " << endPos << endl;
  while ((!pba[iter]->skipToPos(startPos)) && (iter < numEntries)) {
  // 	cerr << "pba[" << iter << "] had start pos of: " << pba[iter]->getStartPos() << endl;
    iter++;
  }
  assert(iter < numEntries);

  posBlockArray=pba+iter;
  if (pba[iter]->skipToPos(endPos)) { // Entire position range lies within this block
    temp1 = pba[iter]->copySubset(startPos, endPos);	// So copy the part we need
    size = 1;
    posBlockArray[0] = temp1; 
    first = 0; 
    last = -1;
  }
  else {
  	// Desired range spans multiple blocks
    temp1 = pba[iter]->copySubset(startPos, pba[iter]->getEndPos()); // Copy [startPos, block_end] from first block
    posBlockArray[0] = temp1; 
    int iter2 = iter;
    // Skip to appropriate block to end on
    while ((!pba[iter2]->skipToPos(endPos)) && (iter2 < numEntries))
      iter2++;
    assert(iter2 < numEntries);
    // Copy the range from the ending block
    temp2 = pba[iter2]->copySubset(pba[iter]->getStartPos(), endPos);
    size=iter2-iter+1;
    posBlockArray[size-1] = temp2; 
    first = 0; 
    last = size-1;
  }

  currindex = 0;
  numPositions=0;
  posSorted = true;
  posContig = true;
  blockPosSorted = true;
  sparse = false;
  int sparseCount = 0;
  sizes=new int[size];
  startEndPosValid = true;
  this->startPos = startPos;
  this->endPos = endPos;
  for (int i=0; i < size; i++) {
    sizes[i] = posBlockArray[i]->getSize();
    numPositions += sizes[i];
    PosBlock* pb = posBlockArray[i];
    bool posS = pb->isPosSorted();
	if (!posS) {
//    if (!posBlockArray[i]->isPosSorted())
      posSorted=false;
	}
    if (!posBlockArray[i]->isPosContiguous())
      posContig=false;
    if (!posBlockArray[i]->isBlockPosSorted())
      blockPosSorted=false;
    if (posBlockArray[i]->isSparse())
      sparseCount++;
  }
  if (sparseCount > (size / 2))
    sparse = true;
    
   // Initialize iterator
   //ourIter = PosBlockArrayIter(this, true, startPos, endPos);
}

PosBlockArray::PosBlockArray(const PosBlockArray& block_) {
  posBlockArray=block_.posBlockArray;
  size=block_.size;
  currindex=block_.currindex;
  posSorted = block_.posSorted;
  blockPosSorted = block_.posSorted;
  sparse = block_.sparse;
  posContig = block_.posContig;
  sizes = block_.sizes;
  //ourIter = block_.ourIter; // object copy
}

PosBlockArray::~PosBlockArray()
{
  //delete [] posBlockArray;
  if (first != -1) {
    delete posBlockArray[first];
  }
  if (last != -1) {
    delete posBlockArray[last];
  }
  delete [] sizes;
}

PosBlock* PosBlockArray::clone(PosBlock& block_) {
  //note: this does not cause each component PosBlock to be copied.
  //thus, it might beed to be changed in the future.
  PosBlockArray* newBlock = new PosBlockArray((PosBlockArray&)block_);
  return newBlock;
}

PosBlock* PosBlockArray::copySubset(int fromPos, int untilPos) {
  return new PosBlockArray(posBlockArray, size, fromPos, untilPos);
}
	
// Iterator access to block array

bool PosBlockArray::hasNext() {
//	throw UnexpectedException();
  return ((currindex < size) && (posBlockArray[currindex]->hasNext()));
}

unsigned int PosBlockArray::getNext() {
//		throw UnexpectedException();
  if (posBlockArray[currindex]->hasNext())
    return posBlockArray[currindex]->getNext();
  else {
    currindex++;
    assert (currindex!=size);
    return posBlockArray[currindex]->getNext();
  }
}

unsigned int PosBlockArray::peekNext() {
//		throw UnexpectedException();
  if (posBlockArray[currindex]->hasNext())
    return posBlockArray[currindex]->peekNext();
  else {
    assert ((currindex+1)!=size);
    return posBlockArray[currindex+1]->peekNext();
  }
}


// Zero indexed, gets the position at specified loc
unsigned int PosBlockArray::getPosAtLoc(unsigned int loc_) {
  assert (loc_ >=0 && loc_ < (unsigned int)numPositions);
  int tempindex =0;
  while (loc_ >= (unsigned int)sizes[tempindex]) {
    loc_-=sizes[tempindex];
    tempindex++;
  }
  return posBlockArray[tempindex]->getPosAtLoc(loc_);
}


unsigned int PosBlockArray::getCurrLoc() {
		throw UnexpectedException();
  return posBlockArray[currindex]->getCurrLoc();
}


int PosBlockArray::getSize() {
  return numPositions;
}

int PosBlockArray::getSizeInBits() {
  int sum;
  for (int i=0; i < size; i++)
    sum+=posBlockArray[i]->getSizeInBits();
  return sum;
}

unsigned int PosBlockArray::getStartPos() {
  return posBlockArray[0]->getStartPos();
}

unsigned int PosBlockArray::getEndPos() {
  return posBlockArray[size-1]->getEndPos();
}

void PosBlockArray::resetBlock() {
	throw UnexpectedException();
  for (int i=0; i < size; i++)
    posBlockArray[i]->resetBlock();
}

unsigned int PosBlockArray::initBitStringIterator(unsigned int pos) {
  assert(posSorted);
  assert(pos >= getStartPos());
  assert(pos <= getEndPos());
  startPoses = new unsigned int[size];
  adjStartPoses = new unsigned int[size];
  endPoses = new unsigned int[size];
  int index;
  for (int i = 0; i < size; i++) {
    startPoses[i] = posBlockArray[i]->getStartPos();
    adjStartPoses[i] = startPoses[i];
    if (adjStartPoses[i] % 32)
      adjStartPoses[i] -= ((adjStartPoses[i] % 32) - 1);
    else {
      adjStartPoses[i] -= 31;
    }

    endPoses[i] = posBlockArray[i]->getEndPos();
    if (startPoses[i] > pos)
      index = i-1;
  }
  currBSPos = posBlockArray[index]->initBitStringIterator(pos);
  currBSIndex = index;
  //for (int i = index+1; i < size; i++)
  //  posBlockArray[i]->initBitStringIterator(startPoses[i]);
  return currBSPos;
}

int PosBlockArray::getNextBitString() {
  int retval;
  assert(currBSPos < endPoses[currBSIndex]);
  if (currBSPos < adjStartPoses[currBSIndex]) {
    currBSPos+=(sizeof(int)*8);
    return 0;
  }
  retval = posBlockArray[currBSIndex]->getNextBitString();
  currBSPos+=(sizeof(int)*8);
  if (currBSPos > endPoses[currBSIndex]) {
    currBSIndex++;
    unsigned int temppos = posBlockArray[currBSIndex]->initBitStringIterator(startPoses[currBSIndex]);
    if (temppos == (currBSPos-(sizeof(int)*8)))
      retval |= posBlockArray[currBSIndex]->getNextBitString();
  }
  return retval;
}
	
// Stream properties
bool PosBlockArray::isPosSorted() {  
	return posSorted;
}
bool PosBlockArray::isSparse() {  
	return sparse;
}
	
// Block properties
bool PosBlockArray::isPosContiguous() {  
	return posContig;
}
bool PosBlockArray::isBlockPosSorted() {  
	return blockPosSorted;
}

PosBlockIter* PosBlockArray::getIterator() const {
//	throw UnexpectedException();
	if (startEndPosValid) {
//		cerr << "new PosBlockArrayIterator; valid start/end positions of " << startPos << "/" << endPos << endl;
//		return new PosBlockArrayIter(this, true, startPos, endPos);
		throw UnexpectedException();
	} else {
	//	cerr << "new PosBlockArrayIterator; start/end positions invalid" << endl;
		throw UnexpectedException();
	}
}

PosBlockIter* PosBlockArray::getIterator(unsigned int startPos_, unsigned int endPos_) const {
//	throw UnexpectedException();
//	assert (startPos_ >= startPos && startPos_ <= endPos);
//	assert (endPos_ >= startPos && endPos <= endPos);
//	return new PosBlockArrayIter(this, true, startPos_, endPos_);
	throw UnexpectedException();
}


PosBlockArray::PosBlockArrayIter::PosBlockArrayIter(const PosBlockArray* pba_, bool startEndPosValid_, unsigned int startPos_, unsigned int endPos_) 
				: pba((PosBlockArray*)pba_), curBlockIter(NULL), currindex(0), startPos(startPos_), endPos(endPos_) , startEndPosValid(startEndPosValid_)
{
	resetIterator();
}

PosBlockArray::PosBlockArrayIter::PosBlockArrayIter(const PosBlockArrayIter& pbai) {
/*	pba = pbai.pba;
	currindex = pbai.currindex;
	curBlockIter = pbai.curBlockIter->clone();
	startPos = pbai.startPos;
	endPos = pbai.endPos;*/
	throw UnexpectedException();
}

PosBlockArray::PosBlockArrayIter& PosBlockArray::PosBlockArrayIter::operator=(const PosBlockArray::PosBlockArrayIter& pbai) {
/*	pba = pbai.pba;
	currindex = pbai.currindex;
	curBlockIter = pbai.curBlockIter->clone();
	startPos = pbai.startPos;
	endPos = pbai.endPos;
	return *this;*/
	throw UnexpectedException();
}

void PosBlockArray::PosBlockArrayIter::resetIterator()
{
	currindex = 0;
	delete curBlockIter;
	if (startEndPosValid) {
//		cerr << "PBAI asking for iter from " << startPos << " to " << endPos << endl;
		curBlockIter = pba->posBlockArray[0]->getIterator(startPos, endPos);
//		cerr << "set curblockiter to: " << curBlockIter << " has next = " << curBlockIter->hasNext() << endl;
	} else {
		curBlockIter = pba->posBlockArray[0]->getIterator();
	}		
	/*curBlockIter = pba->posBlockArray[0]->getIterator();
	if (startEndPosValid && startPos != pba->posBlockArray[0]->getStartPos()) {
		if (!curBlockIter->skipToPos(startPos - 1)) { // Need -1 so that getNext will give us startPos
			throw UnexpectedException("could not skip to pos");
		}
	}*/
}

PosBlockArray::PosBlockArrayIter::~PosBlockArrayIter() {
	//cerr << "Delete: " << curBlockIter << endl;
	delete curBlockIter;
}

/*
bool PosBlockArray::PosBlockArrayIter::hasNext() const {
	//	cerr << "hasNext: lt -> " << (currindex < pba->size - 1) << "  b->hn --> " << curBlockIter->hasNext() << endl;
	// Another block is available or this one still has stuff--check currindex first since cheaper
	return (currindex < pba->size - 1 || curBlockIter->hasNext());
	//return pba->hasNext();
		
}*/
	
	/*	
unsigned int PosBlockArray::PosBlockArrayIter::getNext() {
	assert(hasNext());
	if (!curBlockIter->hasNext()) {
		delete curBlockIter;
		assert (currindex < pba->size - 1);
		assert (curBlockIter->hasNext());
//		curBlockIter = pba->posBlockArray[++currindex]->getIterator();
		if (startEndPosValid) { // && currindex == pba->size) {
//			++currindex;
		//	unsigned int s = pba->posBlockArray[currindex]->getStartPos();
			curBlockIter = pba->posBlockArray[++currindex]->getIterator(startPos, endPos);
		} else {
			curBlockIter = pba->posBlockArray[++currindex]->getIterator();
		}
  	}
  	return curBlockIter->getNext();
//      	return pba->getNext();
}
*/
  
unsigned int PosBlockArray::PosBlockArrayIter::peekNext() const {
	if (curBlockIter->hasNext()) {
		return curBlockIter->peekNext();
	} else {
		assert (currindex < pba->size - 1);
  		// DSM should we cache this iterator?
  		PosBlockIter* pbi = NULL;
		if (startEndPosValid) {
			pbi = pba->posBlockArray[currindex+1]->getIterator(startPos, endPos);
		} else {
			pbi = pba->posBlockArray[currindex+1]->getIterator();
		}
		unsigned int pos = pbi->peekNext();
		delete pbi;
		return pos;
	}
	//return pba->peekNext();
}

int PosBlockArray::PosBlockArrayIter::getSize() {
  if (isPosContiguous())
    return (endPos-startPos)+1;
  else
    throw UnexpectedException("This is going to be annoying to write. Do I really need to?");
}

unsigned int PosBlockArray::PosBlockArrayIter::getStartPos() {
  return startPos;
}

unsigned int PosBlockArray::PosBlockArrayIter::getEndPos() {
  return endPos;
}

unsigned int PosBlockArray::PosBlockArrayIter::initBitStringIterator(unsigned int pos) {
  return pba->initBitStringIterator(pos);
}

int PosBlockArray::PosBlockArrayIter::getNextBitString() {
  return pba->getNextBitString();
}

void PosBlockArray::PosBlockArrayIter::posAnd(PosBlockIter* other, PosBlock* toWrite) {
  throw new UnexpectedException("Not sure this is necessary. Will wait until have to implement before doing so.");
}

bool PosBlockArray::PosBlockArrayIter::isPosSorted() {
  return pba->isPosSorted();
}

bool PosBlockArray::PosBlockArrayIter::isPosContiguous() {
  return pba->isPosContiguous();
}

bool PosBlockArray::PosBlockArrayIter::isBlockPosSorted() {
  return pba->isBlockPosSorted();
}

bool PosBlockArray::PosBlockArrayIter::isSparse() {
  return pba->isSparse();
}
