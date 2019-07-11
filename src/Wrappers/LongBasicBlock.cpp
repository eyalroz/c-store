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
// Single value block with ong long type

#include "LongBasicBlock.h"

LongBasicBlock::LongBasicBlock(bool valSorted_, bool posContig_, bool posSorted_) : Block()
{
	pair=new Pair();
	valSorted=valSorted_ ;
	posContig=posContig_;
	posSorted=posSorted_;
	currPos=1; // as there is no pair
}

LongBasicBlock::LongBasicBlock(const LongBasicBlock& block_) : Block(block_)
{
	pair=new Pair();
	pair->set(*block_.pair);
	valSorted=block_.valSorted;
	posContig=block_.posContig;
	posSorted=block_.posSorted;
	currPos=-1;
}

Block* LongBasicBlock::clone(Block& block_)
{
  Block* newBlock = new LongBasicBlock((LongBasicBlock&)block_);
  return newBlock;
}

LongBasicBlock::~LongBasicBlock()
{
}

void LongBasicBlock::resetBlock() {
  currPos=-1;
}
void LongBasicBlock::setValue(long long val_, int pos_) {
	currPos=-1;
	pair->value=val_;
	pair->position=pos_;
}
void LongBasicBlock::setValue(Pair* val_) {
	currPos=-1;
	pair->set(*val_);
}

	
// Iterator access to block
bool LongBasicBlock::hasNext() {
	return (currPos<0);
}
bool LongBasicBlock::hasNext(int value_) {
  return ((currPos<0) && ((int)pair->value) == value_);
}
Pair* LongBasicBlock::getNext() {
	if (currPos<0) {
		currPos=0;	
		return pair;
	}
	else 
		return NULL;
}
Pair* LongBasicBlock::peekNext() {
	if (currPos<0) {
		return pair;
	}
	else 
		return NULL;
}

long long LongBasicBlock::getValue() {
	if (pair->position==0) throw new CodingException("LongBasicBlock: cannot get value of empty basic block");
	return pair->value;
}
unsigned int LongBasicBlock::getPosition() {
	if (pair->position==0) throw new CodingException("LongBasicBlock: cannot get position of empty basic block");	
	return pair->position;
}
unsigned int LongBasicBlock::getEndPosition() {
	if (pair->position==0) throw new CodingException("LongBasicBlock: cannot get position of empty basic block");	
	return pair->position;
}

// Zero indexed, gets the pair at this pos_
Pair* LongBasicBlock::getPairAtLoc(unsigned int loc_) {
  if ((loc_==0) && (pair->position!=0))
    return pair;
  else 
    return NULL;
}

//Like getPairAtLoc except set the regular pair variable rather than 
//the utilityPair variable so can be called by getNext().
Pair* LongBasicBlock::getPairAtLocNotUtility(unsigned int loc_) {
  if ((loc_==0) && (pair->position!=0))
    return pair;
  else 
    return NULL;
}

int LongBasicBlock::getCurrLoc() {
  return currPos+1;
}

// return size of block		
int LongBasicBlock::getSize() {
	return 1;	
}
int LongBasicBlock::getSizeInBits() {
	return 2*sizeof(int);	
}
Pair* LongBasicBlock::getStartPair() {
	if (pair->position==0) return NULL;
	return pair;	
}
	
bool LongBasicBlock::isOneValue() {
	return true;	
}
bool LongBasicBlock::isValueSorted() {
	return valSorted;	
}
		
bool LongBasicBlock::isPosContiguous() {
	return posContig;	
}
bool LongBasicBlock::isPosSorted() {
	return posSorted;	
}
bool LongBasicBlock::isBlockValueSorted() {
	return true;	
}
bool LongBasicBlock::isBlockPosSorted() {
	return true;
}

