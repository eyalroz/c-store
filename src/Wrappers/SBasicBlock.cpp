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

#include "SBasicBlock.h"

SBasicBlock::SBasicBlock(bool valSorted_, bool posContig_, bool posSorted_) : SBlock()
{
	pair=new SPair();
	valSorted=valSorted_ ;
	posContig=posContig_;
	posSorted=posSorted_;
	currPos=1; // as there is no pair
}

SBasicBlock::SBasicBlock(const SBasicBlock& block_) : SBlock(block_)
{
	pair=new SPair();
	pair->set(*block_.pair);
	valSorted=block_.valSorted;
	posContig=block_.posContig;
	posSorted=block_.posSorted;
	currPos=-1;
}
/*
Block* SBasicBlock::clone(Block& block_)
{
  Block* newBlock = new SBasicBlock((SBasicBlock&)block_);
  return newBlock;
}*/

SBasicBlock::~SBasicBlock()
{
}
/*
void SBasicBlock::resetBlock() {
  currPos=-1;
  }*/
void SBasicBlock::setValue(char* val_, int pos_) {
	currPos=-1;
	pair->value=val_;
	pair->position=pos_;
}
void SBasicBlock::setValue(SPair* val_) {
	currPos=-1;
	pair->set(*val_);
	}

	
// Iterator access to block
bool SBasicBlock::hasNext() {
	return (currPos<0);
}/*
bool SBasicBlock::hasNext(int value_) {
  return ((currPos<0) && pair->value == value_);
  }*/
SPair* SBasicBlock::getNext() {
	if (currPos<0) {
		currPos=0;	
		return pair;
	}
	else 
		return NULL;
}/*
Pair* SBasicBlock::peekNext() {
	if (currPos<0) {
		return pair;
	}
	else 
		return NULL;
}

int SBasicBlock::getValue() {
	if (pair->position==0) throw new CodingException("SBasicBlock: cannot get value of empty basic block");
	return pair->value;
}
unsigned int SBasicBlock::getPosition() {
	if (pair->position==0) throw new CodingException("SBasicBlock: cannot get position of empty basic block");	
	return pair->position;
}

// Zero indexed, gets the pair at this pos_ 
Pair* SBasicBlock::getPairAtLoc(unsigned int loc_) {
	if ((loc_==0) && (pair->position!=0))
		return pair;
	else 
		return NULL;
}

int SBasicBlock::getCurrLoc() {
  return currPos+1;
}

// return size of block		
int SBasicBlock::getSize() {
	return 1;	
}
int SBasicBlock::getSizeInBits() {
	return 2*sizeof(int);	
}
Pair* SBasicBlock::getStartPair() {
	if (pair->position==0) return NULL;
	return pair;	
}
	
bool SBasicBlock::isOneValue() {
	return true;	
}
bool SBasicBlock::isValueSorted() {
	return valSorted;	
}
		
bool SBasicBlock::isPosContiguous() {
	return posContig;	
}
bool SBasicBlock::isPosSorted() {
	return posSorted;	
}
bool SBasicBlock::isBlockValueSorted() {
	return true;	
}
bool SBasicBlock::isBlockPosSorted() {
	return true;
	}*/

