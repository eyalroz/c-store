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
#include "SMultiBlock.h"

SMultiBlock::SMultiBlock(int capacity, int tupwidth) // tupwidth in BYTES
{
	this->capacity = capacity;
	this->nelements = 0;
	this->tupwidth = tupwidth;
	this->valSorted = true;
	this->startPos = -1;
	this->buffer = new char[capacity * tupwidth];
	this->utilityTuple = new SPair(new char[tupwidth], -1);
	this->curidx = 0;
}

SMultiBlock::~SMultiBlock()
{
	delete[] buffer;
	delete utilityTuple;
}

SMultiBlock::SMultiBlock(const SMultiBlock& block_) : SBlock(block_) {
	capacity = block_.capacity;
	nelements = block_.nelements;
	tupwidth = block_.tupwidth;
	valSorted = block_.valSorted;
	startPos = block_.startPos;
	utilityTuple = new SPair(new char[tupwidth], -1);
	curidx = 0;

	// Copy the contents
	buffer = new char[capacity * tupwidth];
	memcpy(buffer, block_.buffer, capacity * tupwidth);
}
	
void SMultiBlock::resetBlock() {
	curidx = 0;
	nelements = 0;
	startPos = -1;
}

void SMultiBlock::resetIterator() {
	curidx = 0;
}

SMultiBlock* SMultiBlock::clone(SMultiBlock& block_)
{
  SMultiBlock* newBlock = new SMultiBlock(block_);
  return newBlock;
}

void SMultiBlock::setStartPosition(unsigned int startPos) {
  this->startPos = startPos;
}

unsigned int SMultiBlock::getStartPosition() {
  return startPos;
}

bool SMultiBlock::hasNext() {
	bool result = curidx < ((nelements) * tupwidth);
	return result;
}

bool SMultiBlock::hasNext(int value) {
	throw new UnexpectedException("hasNext(value) not supported");
}

const int SMultiBlock::getTupwidth() {
	return tupwidth;
}

SPair* SMultiBlock::getNext() {
	SPair* retval = peekNext();
	curidx += tupwidth;
	return retval;
}

SPair* SMultiBlock::peekNext() {
  /*if (!hasNext()) {
		cerr << "Attempt to peekNext when no next available; " 
			<< " curidx = " << curidx << " nelts = " << nelements 
			<< " tupwidth = " << tupwidth << endl;
		throw new UnexpectedException("no next element to peek");
		}*/
  assert(hasNext());

	char* dst = utilityTuple->value;
	char* src = &(buffer[curidx]);
	int len = tupwidth;
	//cerr << "dst = " << (void*) dst << " src = " << (void*) src << " len = " << len << endl;
//	memcpy(utilityTuple->value, &buffer[curidx], tupwidth);
	//cerr << (void*)this << " use curidx = " << curidx << "/" << (nelements-1) * tupwidth << endl;
	memcpy(dst, src, len);
	utilityTuple->position = startPos + (curidx / tupwidth);
	return utilityTuple;
}

SPair* SMultiBlock::getPairAtLoc(unsigned int loc_) {
	if (loc_ < (unsigned int) nelements) {
		memcpy(utilityTuple->value, &buffer[loc_*tupwidth], tupwidth);
		utilityTuple->position = startPos + loc_;
		return utilityTuple;
	} else {
		return NULL;
	}
}

void SMultiBlock::setValueAtPos(char* value, unsigned int loc) {
	memcpy(&buffer[loc*tupwidth], value, tupwidth);
	if (loc >= (unsigned int)nelements) {
		++nelements;
	}
}

int SMultiBlock::getCurrLoc() {
	return curidx == 0 ? 0 : curidx - tupwidth;
}
		
int SMultiBlock::getSize() {
	return nelements;
}

int SMultiBlock::getSizeInBits() {
	return nelements * tupwidth * 8;
}

SPair* SMultiBlock::getStartPair() {
	return getPairAtLoc(0);
}

bool SMultiBlock::isValueSorted() {
	return valSorted;
}

bool SMultiBlock::isPosContiguous() {
	return true;
}

bool SMultiBlock::isPosSorted() {
	return true;
}

bool SMultiBlock::isOneValue() {
	return nelements == 1;
}

bool SMultiBlock::isBlockValueSorted() {
	return false;
}

bool SMultiBlock::isBlockPosSorted() {
	return false;
}

int SMultiBlock::getCapacity() {
	return capacity;
}

int SMultiBlock::getNElements() {
	return nelements;
}
		
	

