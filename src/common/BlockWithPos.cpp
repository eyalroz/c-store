/* Copyright (c) 2005, Regentsthrow UnexpectedException(); of Massachusetts Institute of Technology, 
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
// Generalized block using PosBlockArray

#include "BlockWithPos.h"

BlockWithPos::BlockWithPos() : BlockPosNonContig()
{
 // posBlock=NULL;
  pbIter = NULL;
}

BlockWithPos::BlockWithPos(const BlockWithPos& block_) : BlockPosNonContig(block_)
{
//  posBlock=block_.posBlock;
//  pbIter = (PosBlockArray::PosBlockArrayIter*) posBlock->getIterator();
//  pbIter = posBlock->getIterator();
//  pbIter->skipToPos(block_.pbIter->getCurrLoc());
	baseBlock = block_.baseBlock->clone(*block_.baseBlock);
	pbIter = (PosBlockIter*) block_.pbIter->clone(*block_.pbIter);
	startPos = block_.startPos;
}

Block* BlockWithPos::clone(Block& block_)
{
  Block* newBlock = new BlockWithPos((BlockWithPos&)block_);
  return newBlock;
}

BlockWithPos::~BlockWithPos()
{
  //delete pbIter;
  //delete vp;
  //delete utilityVP;
}

void BlockWithPos::resetBlock() {
  baseBlock->resetBlock();
//  posBlock->resetBlock();
  pbIter->resetIterator();
}

void BlockWithPos::setBlock(Block* block) {
  baseBlock = block;
  startPos = baseBlock->getStartPair()->position;
}


// Assume:
// 1. posBlock has been set (or else will throw an exception)
// 2. position range covered by posBlock matches exactly position
//    range covered by this block
bool BlockWithPos::hasNext() {
  return pbIter->hasNext();
//	return posBlock->hasNext();
}
bool BlockWithPos::hasNext(int value_) {
  return ((pbIter->hasNext()) && (*(baseBlock->getPairAtLoc(pbIter->peekNext() - startPos)->value) == value_));
  //return ((posBlock->hasNext()) && ((baseBlock->getPairAtLoc(posBlock->peekNext() - startPos)->value) == value_));
}

ValPos* BlockWithPos::getNext() {
  //call getPairAtLocNotUtility rather than regular getPairAtLoc because we want this method to set the regular pair variable rather than the utilityPair variable (so we can call peekNext without changing the pointer returned by this method.
  assert(hasNext());
  return baseBlock->getPairAtLocNotUtility(pbIter->getNext() - startPos);
 //   return baseBlock->getPairAtLocNotUtility(posBlock->getNext() - startPos);
}

ValPos* BlockWithPos::peekNext() {
    return baseBlock->getPairAtLoc(pbIter->peekNext() - startPos);
//    return baseBlock->getPairAtLoc(posBlock->peekNext() - startPos);
}

/*int BlockWithPos::getValue() {
  return getPairAtLoc(posBlock->getPosAtLoc(posBlock->getCurrLoc()) - startPos)->value;
}
unsigned int BlockWithPos::getPosition() {
  return posBlock->getPosAtLoc(posBlock->getCurrLoc());
  }*/

unsigned int BlockWithPos::getLastPosition() {
  return pbIter->getEndPos();
}
unsigned int BlockWithPos::getEndPosition() {
  return pbIter->getEndPos();
}

// Zero indexed, gets the pair at this pos_ and points that 
//Pair* BlockWithPos::getPairAtLoc(unsigned int loc_) {
//  return baseBlock->getPairAtLoc(loc_);
//}

  //This method to set the regular pair variable rather than the utilityPair variable so can be called by getNext(). See comment in getNext() for reasoning.
// Zero indexed, gets the pair at this pos_ 
//Pair* BlockWithPos::getPairAtLocPair(unsigned int loc_) {
//  pair->value=*(((int*)(buffer))+loc_);
//  pair->position=loc_+startPos;
//  return pair;
//}

//Get pair at the derived loc ... i.e. if loc_ = 4 then get the pair not
// at the fourth position in the base block but at the position
// of the fourth position in the posBlock in the baseBlock
ValPos* BlockWithPos::getPairAtLoc(unsigned int loc_) {
  return baseBlock->getPairAtLoc(pbIter->getPosAtLoc(loc_) - startPos);
}

//Like getPairAtLoc except set the regular pair variable rather than 
//the utilityPair variable so can be called by getNext().
ValPos* BlockWithPos::getPairAtLocNotUtility(unsigned int loc_) {
  return baseBlock->getPairAtLocNotUtility(pbIter->getPosAtLoc(loc_) - startPos);
}

/*void BlockWithPos::cutOffEnd(int endPos) {
  if (endPos < startPos+numValues)
    numValues=(endPos-startPos)+1;
    }*/

int BlockWithPos::getCurrLoc() {
  return pbIter->getCurrLoc();
  //return posBlock->getCurrLoc();
}

// return size of block		
int BlockWithPos::getSize() {
  return pbIter->getSize();
}

//return buffer size in bits
int BlockWithPos::getSizeInBits() {
  return baseBlock->getSizeInBits() + pbIter->getSizeInBits();	
}

ValPos* BlockWithPos::getStartPair() {
  return baseBlock->getPairAtLoc(pbIter->getStartPos() - startPos);
}
	
bool BlockWithPos::isOneValue() {
  return baseBlock->isOneValue();
}

/*bool BlockWithPos::isPosContiguous() {
  return false;
  }*/

void BlockWithPos::setPosBlockIter(PosBlockIter* pbi){ //, bool save_old_bounds) {
/*	if (save_old_bounds && posBlock != NULL) {
		unsigned int startpos = getStartPair()->position;
		unsigned int endpos = getEndPosition();
		delete pbIter;
		pbIter = pba->getIterator(startpos, endpos);
	} else {*/;
//		assert(pbIter != pbi);
//		cerr << "BWP:setPosBlockiter: delete  " << pbIter << " to replace with " << pbi << endl;
		if (pbIter != pbi) {
//			delete pbIter; DSM fixme leak but screws up minicols
//			if (pbIter != NULL) {cerr << "BlockWithPos: delete old iter " << pbIter << endl;}
			pbIter = pbi; //pb->getIterator();
//			assert(baseBlock == NULL ? true : pbIter->getStartPos() >= baseBlock->getStartPair()->position);
//			assert(baseBlock == NULL ? true : pbIter->getEndPos() <= baseBlock->getEndPosition());
		}
//		cerr << "BWP:setPosBlockIter: pbIter now " << pbIter << " from " << pbIter->getStartPos() 
//				<< " to " << pbIter->getEndPos() << endl;
//	}
	//posBlock=pb;
  	//delete pbIter;
  	//pbIter = posBlock->getIterator(getStartPair()->position, getEndPosition());
}

PosBlockIter* BlockWithPos::getPosBlockIter() {
  return pbIter;
}


bool BlockWithPos::isValueSorted() {
	return baseBlock->isValueSorted();	
}
		
bool BlockWithPos::isPosSorted() {
	return baseBlock->isPosSorted();	
}
bool BlockWithPos::isBlockValueSorted() {
	return baseBlock->isBlockValueSorted();	
}
bool BlockWithPos::isBlockPosSorted() {
	return baseBlock->isBlockPosSorted();
}

BlockWithPos::BlockWithPosIter::BlockWithPosIter(BlockWithPos* bwp_) : bwp(bwp_), pbIter(bwp->pbIter), decomp_array(NULL) {
  assert( dynamic_cast<BlockWithPos*>( bwp ));
}	

BlockWithPos::BlockWithPosIter::BlockWithPosIter(BlockWithPos* bwp_, int startPos, int endPos) : bwp(bwp_), decomp_array(NULL) {
  pbIter = bwp->pbIter->getPosBlock()->getIterator(startPos, endPos);
  assert(  dynamic_cast<BlockWithPos*>( bwp ));
} 
			

BlockWithPos::BlockWithPosIter::BlockWithPosIter(BlockWithPos* bwp_, PosBlockIter* pbi) : bwp(bwp_), pbIter(pbi), decomp_array(NULL) {
  assert(  dynamic_cast<BlockWithPos*>( bwp ));
}

ValPos* BlockWithPos::BlockWithPosIter::getNext() {
  assert((pbIter->peekNext() >= bwp->baseBlock->getStartPair()->position));
  //cout << pbIter->peekNext() << " " << bwp->baseBlock->getStartPair()->position << " " << bwp->startPos<< endl;
  //bwp->startPos
  return bwp->baseBlock->getPairAtLocNotUtility(pbIter->getNext() - bwp->baseBlock->getStartPair()->position);
}

const byte* BlockWithPos::BlockWithPosIter::asArray() {
	if (decomp_array == NULL) {
		unsigned int sp = pbIter->getStartPos();
		int sz = getSize();
		int vs = bwp->baseBlock->getBlock()->valsize;
		decomp_array = new byte[sz*vs];
		const byte* vals = bwp->baseBlock->getBlock()->getIterator(sp,pbIter->getEndPos())->asArray();
		int i = 0;
		// If the BlockWithPos consists of contiguous positions, then we can just
		// use memcpy to efficiently grab the values we want.
		assert (pbIter->getStartPos() == sp);
		if (pbIter->isPosContiguous()) {
			memcpy(decomp_array, vals, sz*vs);
		} else {
			// Otherwise, we need to use the PosBlock iter in the BlockWithPos to select
			// the necessary values.
			pbIter->resetIterator();
			for (i = 0; pbIter->hasNext(); ++i) {
			  memcpy(decomp_array+(i*vs), vals + ((pbIter->getNext() - sp)*vs), vs);
			  //decomp_array[i] = vals[pbIter->getNext() - sp];
			}
			pbIter->resetIterator();
			assert(i == getSize());
			assert(pbIter->getStartPos() == pbIter->peekNext()); // check reset works properly
		}
	}
	return (const byte*) decomp_array;
}
	
Block* BlockWithPos::BlockWithPosIter::getBlock() { 
  //assert( !dynamic_cast<BlockWithPos::BlockWithPosIter*>( bwp ));
  //assert( !dynamic_cast<BlockWithPos*>( bwp ));
  return bwp; 
}
