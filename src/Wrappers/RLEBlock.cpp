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
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, F
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
// RLE Block 


#include "RLEBlock.h"

RLEBlock::RLEBlock(bool valSorted_) : Block() {
	triple=NULL;	
	//pair=new Pair();
	//utilityPair=new Pair();
	vp = NULL;
	utilityVP = NULL;
	valSorted=valSorted_;
	adjustedCurPos = 0;
	valsize = 0;
	createdTriple = false;
}

RLEBlock::RLEBlock(RLETriple* trip_, bool valSorted_) : Block() {	
	if (trip_==NULL)
		throw new CodingException("RLEBlock: Constructing with a NULL triple");
	triple=trip_;	
	//currentPos=triple->startPos;
	//pair=new Pair();
	//utilityPair=new Pair();
	vp=NULL;
	utilityVP=NULL;
	if (trip_->value)
	  setType(trip_->value);
	else
	  valsize=0;
	valSorted=valSorted_;
	adjustedCurPos = 0;
	createdTriple = false;
}
RLEBlock::RLEBlock(const RLEBlock& block_) : Block(block_)
{
  //pair=new Pair();
  //utilityPair=new Pair();
  vp = NULL;
  utilityVP = NULL;
  setType(block_.vp);
  //currentPos=block_.triple->startPos;
  triple=block_.triple == NULL ? NULL : new RLETriple(*block_.triple);
  createdTriple = true;
  valSorted=block_.valSorted;
  adjustedCurPos = 0;
}

Block* RLEBlock::clone(Block& block_)
{
  Block* newBlock = new RLEBlock((RLEBlock&)block_);
  return newBlock;
}

RLEBlock::~RLEBlock()
{
  //cout << " RLEBlock is joining its ancestors !" << endl;
  //cout << " Let's delete pair " << pair << " triple " << triple << endl;
  delete vp;
  delete utilityVP;
  if (createdTriple)
    delete triple;
}


// Iterator access to block
bool RLEBlock::hasNext() {
  //assert(triple!=NULL);
  return ((adjustedCurPos)<(triple->reps));
}
bool RLEBlock::hasNext(int value_) {
  //assume for now that MultiBlock stores integers only
  return ((adjustedCurPos<triple->reps) && *(triple->value) == value_);
}
ValPos* RLEBlock::getNext() {
  //assert(triple!=NULL);
	if ((adjustedCurPos)<(triple->reps)) {
	  vp->position = adjustedCurPos+triple->startPos;
	  //pair->value=triple->value;
	  //pair->position=adjustedCurPos+triple->startPos;
	  adjustedCurPos++;
	  return vp;
	}
	else {
		throw new CodingException("RLEBlock: No next pair");	
	}		
}

ValPos* RLEBlock::peekNext() {
  assert(triple!=NULL);
  if (hasNext()) {			
    //pair->value=triple->value;
    utilityVP->position = adjustedCurPos+triple->startPos;
    //pair->position=adjustedCurPos+triple->startPos;
    return utilityVP;
  }
  else {
    throw new CodingException("RLEBlock: No next pair");	
  }		
}
// Zero indexed, gets the pair at this pos_
ValPos* RLEBlock::getPairAtLoc(unsigned int loc_) {
	if (triple==NULL) throw new UnexpectedException("RLEBlock: error triple is NULL");	
	if ((loc_<triple->reps) && (loc_>=0)) {			
	  //utilityPair->value=triple->value;
	  utilityVP->position=triple->startPos+loc_;
	  //adjustedCurPos=loc_+1;
	  return utilityVP;
	}
	else {
		throw new CodingException("RLEBlock: No pair at this position, position invalid.");
	}
	
	return utilityVP;
}

//Like getPairAtLoc except set the regular pair variable rather than 
//the utilityPair variable so can be called by getNext().
ValPos* RLEBlock::getPairAtLocNotUtility(unsigned int loc_) {
	if (triple==NULL) throw new UnexpectedException("RLEBlock: error triple is NULL");	
	if ((loc_<triple->reps) && (loc_>=0)) {			
	  //pair->value=triple->value;
	  vp->position=triple->startPos+loc_;
	  //adjustedCurPos=loc_+1;
	  return vp;
	}
	else {
		throw new CodingException("RLEBlock: No pair at this position, position invalid.");
	}
	
	return vp;
}

bool RLEBlock::skipToLoc(unsigned int loc_) {
	if (triple==NULL) throw new UnexpectedException("RLEBlock: error triple is NULL");	
	if ((loc_<triple->reps) && (loc_>=0)) {			
	  //adjustedCurPos=loc_+1;		
	  adjustedCurPos=loc_;
	  return true;
	}
	else {
		throw new CodingException("RLEBlock: No pair at this position, position invalid.");
	}
	
	return false;
}

int RLEBlock::getCurrLoc() {
	return adjustedCurPos;
}
void RLEBlock::resetBlock() {
  //urrentPos=triple->startPos;
  adjustedCurPos=0;
}	
// return size of block (number of values)
int RLEBlock::getSize() {
	if (triple==NULL) throw new UnexpectedException("RLEBlock: error triple is NULL");	
	return triple->reps;	
}

//return buffer size in block
int RLEBlock::getSizeInBits() {
  throw new UnimplementedException("RLEBlock: Not sure what this means in light of the new type generality");	
  //return sizeof(RLETriple)*8;	
}

ValPos* RLEBlock::getStartPair() {
  //pair->value=triple->value;
  //pair->position=triple->startPos;	
  vp->position = triple->startPos;
  return vp;
}

unsigned int RLEBlock::getEndPosition() {
  return triple->startPos + triple->reps -1;
}

bool RLEBlock::isOneValue() {
	return true;
}
	
bool RLEBlock::isValueSorted() {
	return valSorted;	
}

bool RLEBlock::isPosSorted() {
	return true;
}

bool RLEBlock::isPosContiguous() {
	return true;
}
bool RLEBlock::isBlockValueSorted() {
	return true;	
}
bool RLEBlock::isBlockPosSorted() {
	return true;
}

RLETriple* RLEBlock::getTriple() 
{
	if (triple==NULL) throw new UnexpectedException("RLEBlock: error triple is NULL");	
	return triple;	
}

void RLEBlock::setTriple(RLETriple* trip_) {
  if (trip_==NULL)
    throw new CodingException("RLEBlock: Setting with a NULL triple");
  if (createdTriple)
    delete triple;
  createdTriple = false;
  if ((triple == NULL) || (!triple->value)) {
    triple=trip_;
    if (trip_->value)
      setType(trip_->value);
    else {
      delete vp;
      delete utilityVP;
      vp = new NullValPos();
      utilityVP = new NullValPos();
    }
  }
  else {
    triple = trip_;
    vp ->set(trip_->value->value);
    utilityVP->set(trip_->value->value);
  }
  //currentPos=trip_->startPos;
  adjustedCurPos = 0;
}

bool RLEBlock::isSplittable() {
	return true;
}

// We keep the low values and spill the high values
Block* RLEBlock::split(int npositions) {
	// Make sure the split is reasonable--both blocks non-empty
	assert((unsigned int)npositions < triple->reps);
	
	// Create the triple for the leftover block
	RLETriple* leftoverTrip = new RLETriple(triple->value, 
								triple->startPos + npositions,
								triple->reps - npositions);
	// Update our triple
	triple->reps = npositions;

	return new RLEBlock(leftoverTrip, true);
}

bool RLEBlock::RLEBlockIter::hasNext() {
	return ((adjustedCurPos)<(rleBlock->triple->reps));
//			return rleBlock->hasNext();
}
bool RLEBlock::RLEBlockIter::hasNext(int value_) {
	return ((adjustedCurPos<rleBlock->triple->reps) && *(rleBlock->triple->value) == value_);
//				return rleBlock->hasNext(value_);
}
ValPos* RLEBlock::RLEBlockIter::getNext() {
	if ((adjustedCurPos)<(rleBlock->triple->reps)) {			
	  //pair->value=rleBlock->triple->value;
	  //pair->position=adjustedCurPos+rleBlock->triple->startPos;
	  vp->position = adjustedCurPos+rleBlock->triple->startPos;
	  adjustedCurPos++;
	  return vp;
	}
	else {
		throw new CodingException("RLEBlock: No next pair");	
	}
	//return rleBlock->getNext();		
}
ValPos* RLEBlock::RLEBlockIter::peekNext() {
	if (hasNext()) {			
	  //utilityPair->value=rleBlock->triple->value;
	  //utilityPair->position=adjustedCurPos+rleBlock->triple->startPos;
	  utilityVP->position = adjustedCurPos+rleBlock->triple->startPos;
	  return utilityVP;
	} else {
		throw new CodingException("RLEBlock: No next pair");	
	}	
//				return rleBlock->peekNext();	
}
void RLEBlock::RLEBlockIter::resetIterator() { adjustedCurPos = 0; }
int RLEBlock::RLEBlockIter::getCurrLoc() {
	return adjustedCurPos;
//				return rleBlock->getCurrLoc();
}
bool RLEBlock::RLEBlockIter::skipToLoc(unsigned int loc_) {
	if ((loc_<rleBlock->triple->reps) && (loc_>=0)) {		
	  //adjustedCurPos=loc_+1;		
	  adjustedCurPos=loc_;
	  return true;
	} else {
	  throw new CodingException("RLEBlock: No pair at this position, position invalid.");
	}
}

// Zero indexed, gets the pair at this pos_
ValPos* RLEBlock::RLEBlockIter::getPairAtLoc(unsigned int loc_) {
  if (rleBlock->triple==NULL) throw new UnexpectedException("RLEBlock: error triple is NULL");	
  if ((loc_<rleBlock->triple->reps) && (loc_>=0)) {			
    //utilityPair->value=rleBlock->triple->value;
    //utilityPair->position=rleBlock->triple->startPos+loc_;
    utilityVP->position=rleBlock->triple->startPos+loc_;
    //adjustedCurPos=loc_+1;
    return utilityVP;
  }
  else {
    throw new CodingException("RLEBlock: No pair at this position, position invalid.");
  }
}

//Like getPairAtLoc except set the regular pair variable rather than 
//the utilityPair variable so can be called by getNext().
ValPos* RLEBlock::RLEBlockIter::getPairAtLocNotUtility(unsigned int loc_) {
	if (rleBlock->triple==NULL) throw new UnexpectedException("RLEBlock: error triple is NULL");	
	if ((loc_<rleBlock->triple->reps) && (loc_>=0)) {		       
	  //pair->value=rleBlock->triple->value;
	  //pair->position=rleBlock->triple->startPos+loc_;
	  vp->position=rleBlock->triple->startPos+loc_;
	  //adjustedCurPos=loc_+1;
	  return vp;
	}
	else {
	  throw new CodingException("RLEBlock: No pair at this position, position invalid.");
	}
}

const byte* RLEBlock::RLEBlockIter::asArray() {
  int vs = rleBlock->valsize;
  if (decomp_array == NULL) {
    int sz = getSize();
    decomp_array = new byte[sz*vs];
    for (int i = 0; i < sz; ++i) {
      memcpy(decomp_array+(i*vs), rleBlock->triple->value, vs);
      //decomp_array[i] = val;
    }
  }
  return (const byte*) decomp_array;
}

RLEBlock::RangedRLEBlockIter::RangedRLEBlockIter(RLEBlock* rleBlock_, unsigned int startPos_, unsigned int endPos_) 
									: RLEBlockIter(rleBlock_)
{
  int sp = rleBlock_->getStartPair()->position;
  startLoc = startPos_-sp;
  endLoc = endPos_-sp;
  startpos = startPos_;
  //if (startLoc > 0 /*&& startLoc - 1 > 0*/ && !skipToLoc(startLoc - 1)) {
  if (!skipToLoc(startLoc)) {
    throw UnexpectedException("cannot skip to loc");
  }
}

//bool RLEBlock::RangedRLEBlockIter::hasNext() {
//	return adjustedCurPos<=endLoc;
//}
	
bool RLEBlock::RangedRLEBlockIter::hasNext(int value_) {
	return hasNext() && *(rleBlock->triple->value) == value_;
}

int RLEBlock::RangedRLEBlockIter::getCurrLoc() {
	return adjustedCurPos-startLoc;
}

void RLEBlock::RangedRLEBlockIter::resetIterator() {
  //if (startLoc > 0) {
		skipToLoc(startLoc);
		//} else {
		//adjustedCurPos = 0;
		//}
}

bool RLEBlock::RangedRLEBlockIter::skipToLoc(unsigned int loc_) {
  //assert(loc_ >= startLoc - 1 && loc_ < endLoc);
	assert(loc_ >= startLoc && loc_ <= endLoc);
	//DNA: This seems wrong ...
	return RLEBlock::RLEBlockIter::skipToLoc(loc_);
}

ValPos* RLEBlock::RangedRLEBlockIter::getPairAtLoc(unsigned int loc_) {
  if (rleBlock->triple==NULL) throw new UnexpectedException("RLEBlock: error triple is NULL");	
  if ((loc_<=endLoc) && (loc_>=0)) {			
    //utilityPair->value=rleBlock->triple->value;
    //utilityPair->position=startpos+loc_;
    utilityVP->position=startpos+loc_;
    //adjustedCurPos=loc_+1;
    return utilityVP;
  }
  else {
    throw new CodingException("RLEBlock: No pair at this position, position invalid.");
  }
}

ValPos* RLEBlock::RangedRLEBlockIter::getPairAtLocNotUtility(unsigned int loc_) {
  if (rleBlock->triple==NULL) throw new UnexpectedException("RLEBlock: error triple is NULL");	
  if ((loc_<=endLoc) && (loc_>=0)) {			
    //pair->value=rleBlock->triple->value;
    //pair->position=startpos+loc_;
    vp->position=startpos+loc_;
    //adjustedCurPos=loc_+1;
    return vp;
  }
  else {
    throw new CodingException("RLEBlock: No pair at this position, position invalid.");
  }
}	
