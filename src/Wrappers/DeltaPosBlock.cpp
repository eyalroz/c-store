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
// Delta on Position Block
// Problems: migfer@mit.edu


#include "DeltaPosBlock.h"

DeltaPosBlock::DeltaPosBlock(bool valueSorted_) {
	valueSorted=valueSorted_;	
	//for now assume value is an integer ... fix later
	vp=new IntValPos();
	bfr=NULL;
	bfrSize=0;
	reader=NULL;
	value=0;
	
	numValues=0;
	deltaSize=0;
	escapeInt=0;
	currLoc=0;
	startPos=0;
	currPos=0;
	
	init=false;
	
}

DeltaPosBlock::DeltaPosBlock(DeltaPosBlock& block_) {
	vp = block_.vp->clone();	
	bfrSize=block_.bfrSize/8;
	bfr=new byte[bfrSize];
	memcpy(bfr, block_.bfr, bfrSize);
	valueSorted=block_.valueSorted;
	reader=new BitReader(bfr, bfrSize);
	initDeltaPosBlock(bfr);
}

DeltaPosBlock::DeltaPosBlock(byte* bfr_, int bfrSize_, bool valueSorted_)
{
  //pair=new Pair();
	vp=new IntValPos();	
	bfr=bfr_;
	bfrSize=bfrSize_;
	valueSorted=valueSorted_;	
	if (bfr_==NULL)
		throw new CodingException("DeltaPosBlock: error, buffer passed in constructor is NULL");
	reader=new BitReader(bfr_, bfrSize_);
	initDeltaPosBlock(bfr_);
}

Block* DeltaPosBlock::clone(Block& block_)
{
  Block* newBlock = new DeltaPosBlock((DeltaPosBlock&)block_);
  return newBlock;
}

DeltaPosBlock::~DeltaPosBlock()
{
	// delete[] bfr;
	delete vp;
	if (reader!=NULL) delete reader;
}


void DeltaPosBlock::setDeltaBuffer(byte* bfr_, int bfrSize_) {
	if (bfr_==NULL)
		throw new CodingException("DeltaPosBlock: error, buffer passed in setDeltaBuffer is NULL");

	if (reader==NULL)
		reader=new BitReader(bfr_, bfrSize_);
	else
		reader->setBuffer(bfr_, bfrSize_);
		
	bfr=bfr_;
	bfrSize=bfrSize_;
	initDeltaPosBlock(bfr_);
}

void DeltaPosBlock::initDeltaPosBlock(byte* bfr_) {
	init=true;
	Log::writeToLog("DeltaPosBlock", 1, "Initializing a DeltaPosBlock...");
	bool readHeaderSuccess=true;
	DeltaPosHeader* header=((DeltaPosHeader*) bfr_);
	value=header->value;
	currPos=header->startPos;
	startPos=header->startPos;
	numValues=header->numValues;	
	Log::writeToLog("DeltaPosBlock", 1, "DeltaPosBlock.value=", value);
	Log::writeToLog("DeltaPosBlock", 1, "DeltaPosBlock.currPos=", currPos);
	Log::writeToLog("DeltaPosBlock", 1, "DeltaPosBlock.numValues=", numValues);
	reader->skipToBitPos(sizeof(DeltaPosHeader)*8);
	readHeaderSuccess&=reader->readBits(deltaSize,5);
	Log::writeToLog("DeltaPosBlock", 1, "DeltaPosBlock.deltaSize=", deltaSize);
	vp->set((byte*)&value);
	
	escapeInt=0xFFFFFFFF>>(32-deltaSize);
	currLoc=0;
	if (!readHeaderSuccess)
		throw new CodingException("DeltaPosBlock: Cannot initialize block, cannot read DeltaPos header");	

	Log::writeToLog("DeltaPosBlock", 1, "Finished a DeltaPosBlock...");
}

void DeltaPosBlock::resetBlock() {
	if (!init) throw new UnexpectedException("DeltaPosBlock: Error, block uninitialized");
	reader->skipToBitPos(sizeof(DeltaPosHeader)*8+5); // 5 for deltaSize
	currLoc=0;
}
// Iterator access to block
bool DeltaPosBlock::hasNext() {
	if (!init) return false;
	Log::writeToLog("DeltaPosBlock", 0, "hasNext(): Returning ", (currLoc+1<numValues));
	return (currLoc<numValues);
}

bool DeltaPosBlock::hasNext(int value_) {
  return (hasNext() && *vp == value_);
}

ValPos* DeltaPosBlock::getNext() {
	if (!init) return NULL;
	unsigned int delta;
	
	if (!hasNext())
		return NULL;
	if (currLoc==0) {
		currPos=startPos;
		vp->position=startPos;
		currLoc++;		
		Log::writeToLog("DeltaPosBlock", 0, "getNext(): Returning initial pair, pos=", startPos);
		return vp;
	}
	if(!reader->readBits(delta, deltaSize))
		throw new CodingException("DeltaPosBlock: getNext failed, could not read buffer");
		
	if (delta==escapeInt) {
		delta=0;
		if(!reader->readBits(delta, 32))
			throw new CodingException("DeltaPosBlock: getNext failed, could not read buffer");
		Log::writeToLog("DeltaPosBlock", 0, "getNext(): read escapeInt delta, reading 32 bit delta", delta);
	}
	
	Log::writeToLog("DeltaPosBlock", 0, "getNext(): Read delta=", delta);
	
	currPos+=delta;
	Log::writeToLog("DeltaPosBlock", 0, "getNext(): Read currPos=", currPos);
	vp->position=currPos;
	currLoc++;
	return vp;
}

ValPos* DeltaPosBlock::peekNext() {
	if (!init) return NULL;
	unsigned int delta;
	
	if (!hasNext())
		return NULL;
	if (currLoc==0) {
		currPos=startPos;
		vp->position=startPos;
		Log::writeToLog("DeltaPosBlock", 0, "peekNext(): Returning initial pair, pos=", startPos);
		return vp;
	}
	int oldPos=reader->getCurrentBitPosition();
	if(!reader->readBits(delta, deltaSize))
		throw new CodingException("DeltaPosBlock: peekNext failed, could not read buffer");
		
	if (delta==escapeInt) {
		delta=0;
		if(!reader->readBits(delta, 32))
			throw new CodingException("DeltaPosBlock: peekNext failed, could not read buffer");
		Log::writeToLog("DeltaPosBlock", 0, "peekNext(): read escapeInt delta, reading 32 bit delta", delta);
	}
	
	Log::writeToLog("DeltaPosBlock", 0, "peekNext(): Read delta=", delta);
	
	currPos+=delta;
	Log::writeToLog("DeltaPosBlock", 0, "peekNext(): Read currPos=", currPos);
	vp->position=currPos;
	reader->skipToBitPos(oldPos);
	return vp;
}


// Zero indexed, gets the pair at this loc
ValPos* DeltaPosBlock::getPairAtLoc(unsigned int loc_) {
  //Note, this actually changes the iterator ... this should not happen. fix if DeltaPosBlock ever used ...
	if (!init) return NULL;
	if (loc_>=numValues) return NULL;
	resetBlock();
	ValPos* result;
	for (unsigned int i=0; i<=loc_; i++) {
		result=getNext();
	}

	return result;
}

//Like getPairAtLoc except set the regular pair variable rather than 
//the utilityPair variable so can be called by getNext().
ValPos* DeltaPosBlock::getPairAtLocNotUtility(unsigned int loc_) {
  //Note, this actually changes the iterator ... this should not happen. fix if DeltaPosBlock ever used ...
	if (!init) return NULL;
	if (loc_>=numValues) return NULL;
	resetBlock();
	ValPos* result;
	for (unsigned int i=0; i<=loc_; i++) {
		result=getNext();
	}

	return result;
}


int DeltaPosBlock::getCurrLoc() { 
	if (!init) throw new UnexpectedException("DeltaPosBlock: Error, block uninitialized");
	return currLoc;
}

// return size of block		
int DeltaPosBlock::getSize() {
	if (!init) throw new UnexpectedException("DeltaPosBlock: Error, block uninitialized");
	return numValues;
}

// return size of block	in bytes	
int DeltaPosBlock::getSizeInBits() {
  //for now guess ... FIX LATER
	return PAGE_SIZE*8;	
}

ValPos* DeltaPosBlock::getStartPair() {
	if (!init) return NULL;
	//pair->value=value;
	vp->position=startPos;
	return vp;
}

unsigned int DeltaPosBlock::getEndPosition() {
  throw new UnimplementedException("Not done yet.");
}
	


bool DeltaPosBlock::isValueSorted() {
	return valueSorted;
}
bool DeltaPosBlock::isPosSorted() {
	return true;	
}
		
bool DeltaPosBlock::isPosContiguous() {
	return false;	
}
bool DeltaPosBlock::isOneValue() {
	return true;
}
bool DeltaPosBlock::isBlockValueSorted() {
	return true;
}
bool DeltaPosBlock::isBlockPosSorted() {
	return true;
}

int DeltaPosBlock::getValue() { 
	if (!init) throw new UnexpectedException("DeltaPosBlock: Error, block uninitialized");
	return value; 
} 
byte* DeltaPosBlock::getBuffer() { 
	return bfr; 
}
