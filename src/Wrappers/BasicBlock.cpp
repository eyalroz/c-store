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
// Problems: migfer@mit.edu

#include "BasicBlock.h"

BasicBlock::BasicBlock(bool valSorted_, bool posContig_, bool posSorted_) : Block()
{
  //pair=new Pair();
  valSorted=valSorted_ ;
  posContig=posContig_;
  posSorted=posSorted_;
  currPos=1; // as there is no pair
  vp=NULL;
  utilityVP=NULL;
  setType(ValPos::INTTYPE);
}

BasicBlock::BasicBlock(bool valSorted_, bool posContig_, bool posSorted_, short type_) : Block()
{
  //pair=new Pair();
  valSorted=valSorted_ ;
  posContig=posContig_;
  posSorted=posSorted_;
  currPos=1; // as there is no pair
  vp=NULL;
  utilityVP=NULL;
  setType(type_);
}

BasicBlock::BasicBlock(bool valSorted_, bool posContig_, bool posSorted_, ValPos* vp_) : Block()
{
  //pair=new Pair();
  valSorted=valSorted_ ;
  posContig=posContig_;
  posSorted=posSorted_;
  currPos=1; // as there is no pair
  vp=NULL;
  utilityVP=NULL;
  setType(vp_);
}

BasicBlock::BasicBlock(const BasicBlock& block_) : Block(block_)
{
  //pair=new Pair();
  //pair->set(*block_.pair);
  vp = block_.vp->clone();
  utilityVP=NULL;
  valSorted=block_.valSorted;
  posContig=block_.posContig;
  posSorted=block_.posSorted;
  currPos=-1;
}

Block* BasicBlock::clone(Block& block_)
{
  Block* newBlock = new BasicBlock((BasicBlock&)block_);
  return newBlock;
}

BasicBlock::~BasicBlock()
{
  delete vp;
  delete utilityVP;
}

void BasicBlock::resetBlock() {
  currPos=-1;
}
void BasicBlock::setValue(int val_, int pos_) {
  currPos=-1;
  vp->set(pos_, (byte*)&val_);
  //pair->value=val_;
  //pair->position=pos_;
}
void BasicBlock::setValue(ValPos* val_) {
	currPos=-1;
	vp->set(val_->position, val_->value);
}

	
// Iterator access to block
bool BasicBlock::hasNext() {
	return (currPos<0);
}
bool BasicBlock::hasNext(int value_) {
  return ((currPos<0) && *(vp->value) == value_);
}
ValPos* BasicBlock::getNext() {
	if (currPos<0) {
		currPos=0;	
		return vp;
	}
	else 
		return NULL;
}
ValPos* BasicBlock::peekNext() {
	if (currPos<0) {
		return vp;
	}
	else 
		return NULL;
}

int BasicBlock::getValue() {
	if (vp->position==0) throw new CodingException("BasicBlock: cannot get value of empty basic block");
	return *(int*)vp->value;
}
unsigned int BasicBlock::getPosition() {
	if (vp->position==0) throw new CodingException("BasicBlock: cannot get position of empty basic block");	
	return vp->position;
}
unsigned int BasicBlock::getEndPosition() {
	if (vp->position==0) throw new CodingException("BasicBlock: cannot get position of empty basic block");	
	return vp->position;
}

// Zero indexed, gets the pair at this pos_
ValPos* BasicBlock::getPairAtLoc(unsigned int loc_) {
  if ((loc_==0) && (vp->position!=0))
    return vp;
  else 
    return NULL;
}

//Like getPairAtLoc except set the regular pair variable rather than 
//the utilityPair variable so can be called by getNext().
ValPos* BasicBlock::getPairAtLocNotUtility(unsigned int loc_) {
  if ((loc_==0) && (vp->position!=0))
    return vp;
  else 
    return NULL;
}

int BasicBlock::getCurrLoc() {
  return currPos+1;
}

// return size of block		
int BasicBlock::getSize() {
	return 1;	
}
// return buffer size of block in bits
int BasicBlock::getSizeInBits() {
	return sizeof(int)*8;	
}
ValPos* BasicBlock::getStartPair() {
	if (vp->position==0) return NULL;
	return vp;	
}
	
bool BasicBlock::isOneValue() {
	return true;	
}
bool BasicBlock::isValueSorted() {
	return valSorted;	
}
		
bool BasicBlock::isPosContiguous() {
	return posContig;	
}
bool BasicBlock::isPosSorted() {
	return posSorted;	
}
bool BasicBlock::isBlockValueSorted() {
	return true;	
}
bool BasicBlock::isBlockPosSorted() {
	return true;
}

