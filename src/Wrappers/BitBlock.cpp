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
// Bit block
// Problems: migfer@mit.edu

#include "BitBlock.h"

BitBlock::BitBlock(bool valSorted_) : Block()
{
	bfr=NULL;
	bfrSize=0;
	valSorted=valSorted_;
	reader=NULL;
	//outPair=new Pair();
	outPair=new IntValPos();
	fieldSize=0;
	startPos=0;
	currLoc=0;
}

BitBlock::BitBlock(BitBlock& block_) : Block (block_) {
	valSorted=block_.valSorted;
	//outPair=new Pair();
	outPair=new IntValPos();
	bfrSize=block_.bfrSize;
	if (block_.bfr!=NULL) {
		memcpy(bfr, block_.bfr, bfrSize);
	}
	else {
		bfr=NULL;
	}

	init=block_.init;
	fieldSize=block_.fieldSize;
	startPos=block_.startPos;
	setBuffer(bfr, bfrSize);
}

Block* BitBlock::clone(Block& block_)
{
  Block* newBlock = new BitBlock((BitBlock&)block_);
  return newBlock;
}

BitBlock::~BitBlock()
{
	if (reader!=NULL) delete reader;
	delete outPair;
}
byte* BitBlock::getBuffer() {
	return bfr;	
}
void BitBlock::setBuffer(byte* bfr_, int bfrSize_) {
	if (reader==NULL) {
		reader=new BitReader(bfr_+3*sizeof(unsigned int), bfrSize_-8*3*sizeof(unsigned int));	
	}
	else {
		reader->setBuffer(bfr_+3*sizeof(unsigned int), bfrSize_-8*3*sizeof(unsigned int));
	}
	currLoc=0;
	init=true;
	bfr=bfr_;
	bfrSize=bfrSize_;
	fieldSize=*((unsigned int*) bfr_);
	startPos=*((unsigned int*) (bfr_+sizeof(int)));
	numValues=*((unsigned int*) (bfr_+2*sizeof(int)));
	Log::writeToLog("BitBlock", 1, "Set buffer, fieldSize", fieldSize);
	Log::writeToLog("BitBlock", 1, "Set buffer, startPos", startPos);
	Log::writeToLog("BitBlock", 1, "Set buffer, numValues", numValues);
}
bool BitBlock::hasNext() {
	if (!init) throw new UnexpectedException("BitBlock: error, not initialized.");
	return (currLoc<numValues);
}
bool BitBlock::hasNext(int value_) {
  if (!hasNext()) return false;
  unsigned int val;
  if (reader->readBits(val, fieldSize)) {
    return (int) val == value_;
  } else {
    return false;
  }
}
ValPos* BitBlock::getNext() {
	if (!init) throw new UnexpectedException("BitBlock: error, not initialized.");
	unsigned int val;
	if (!hasNext()) return NULL;
	if (reader->readBits(val, fieldSize)) {
		outPair->set(startPos+currLoc, (byte*)&val);
		currLoc++;	
		return outPair;
	}
	throw new CodingException("BitBlock: Error, could not read");
}
ValPos* BitBlock::peekNext() {
	if (!init) throw new UnexpectedException("BitBlock: error, not initialized.");
	unsigned int val;
	if (!hasNext()) return NULL;
	
	int oldPos=reader->getCurrentBitPosition();
	if (reader->readBits(val, fieldSize)) {
		outPair->set(startPos+currLoc, (byte*)&val);
		reader->skipToBitPos(oldPos);
		return outPair;
	}
	reader->skipToBitPos(oldPos);
	throw new CodingException("BitBlock: Error, could not read");
}
	
// Zero indexed, gets the pair at this loc
ValPos* BitBlock::getPairAtLoc(unsigned int loc_) {
  //Note, this actually changes the iterator ... this should not happen. fix if DeltaPosBlock ever used ...
  if (!init) throw new UnexpectedException("BitBlock: error, not initialized.");	
  if (loc_>=numValues) return NULL;
  if (reader->skipToBitPos(loc_*fieldSize)) {
    currLoc=loc_;
    return getNext();
  }
  else
    return NULL;
}


//Like getPairAtLoc except set the regular pair variable rather than 
//the utilityPair variable so can be called by getNext().
ValPos* BitBlock::getPairAtLocNotUtility(unsigned int loc_) {
  return getPairAtLoc(loc_);
}

int BitBlock::getCurrLoc() {
	if (!init) throw new UnexpectedException("BitBlock: error, not initialized.");
	return currLoc;
}

unsigned int BitBlock::getFieldSize() {
	return fieldSize;	
}
// return size of block		
int BitBlock::getSize() {
	return numValues;	
}
int BitBlock::getSizeInBits() {
	return 8*(((numValues*fieldSize)/8)+1);
}
ValPos* BitBlock::getStartPair() {
	if (numValues==0) return NULL;
	unsigned int oldLoc=currLoc;
	int val=*(int*)getPairAtLoc(0)->value;
	if (oldLoc!=0)
		getPairAtLoc(oldLoc-1);
	else 
		resetBlock();
		
	outPair->set(startPos, (byte*)&val);
	if (oldLoc!=currLoc) throw new UnexpectedException("BitBlock: Miguel you screwed up");
	return outPair;
}
ValPos* BitBlock::getEndPair() {
	if (numValues==0) return NULL;
	unsigned int oldLoc=currLoc;
	int val=*(int*)getPairAtLoc(getSize()-1)->value;

	if (oldLoc!=0)
		getPairAtLoc(oldLoc-1);
	else 
		resetBlock();

	outPair->set(startPos+getSize()-1, (byte*)&val);
	if (oldLoc!=currLoc) {
		cout << "Old Loc: " << oldLoc << " CurrLoc: " << currLoc << endl;
		throw new UnexpectedException("BitBlock: Miguel you screwed up");
	}
	return outPair;
}

unsigned int BitBlock::getEndPosition() {
  return getEndPair()->position;
}
	
void BitBlock::resetBlock() {
	if (!init) return;
	currLoc=0;
	reader->skipToBitPos(0);
}

bool BitBlock::isOneValue() {
	return false;
}	
bool BitBlock::isValueSorted() {
	return valSorted;	
}
		
bool BitBlock::isPosContiguous() {
	return true;	
}
bool BitBlock::isPosSorted() {
	return true;	
}
bool BitBlock::isBlockValueSorted() {
	return valSorted;
}
bool BitBlock::isBlockPosSorted() {
	return true;
}

