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

#include "PosType2Block.h"

PosType2Block::PosType2Block() : PosBlock()
{
  vp=NULL;
  numOccCounter=0;
  block=NULL;
  //block=new Type2Block(false);
  
  posContig=false;
  posSorted=true;
}

PosType2Block::PosType2Block(const PosType2Block& block_) : PosBlock(block_)
{
  numOccCounter=0;
  block=new Type2Block(*block_.block);
  posContig=block_.posContig;
  posSorted=block_.posSorted;
  vp=NULL;
}

PosBlock* PosType2Block::clone(PosBlock& block_)
{
  PosBlock* newBlock = new PosType2Block((PosType2Block&)block_);
  return newBlock;
}

PosType2Block::~PosType2Block()
{
  /*if (pair)
    delete pair;
  if (utilityPair)
    delete utilityPair;
  if (buffer)
  delete[] buffer;*/
  //delete block;
}

void PosType2Block::resetBlock() {	
  block->resetBlock();
  numOccCounter=0;
  vp=NULL;
}

void PosType2Block::setBlock(Type2Block* block_) {
  block=block_;
}

Type2Block* PosType2Block::getBlock() {
  return block;
}

// Iterator access to block
bool PosType2Block::hasNext() {
  if (numOccCounter==0)
    return block->hasNext();
  else 
    return true;
}

/*bool PosType2Block::hasNext(int value_) {
  if (startVal == value_)
    return hasNext();
  else
    return false;
    }*/

unsigned int PosType2Block::getNext() {
  if (numOccCounter==0) {
    vp=block->getNext();
  }
  if (vp==NULL) throw new CodingException("PosType2Block: error no next position");
  numOccCounter=(numOccCounter+1)%numOccurences;
  return vp->position;
}

unsigned int PosType2Block::peekNext() {
  if (numOccCounter==0) {
    return block->peekNext()->position;
  }
  if (vp==NULL) throw new CodingException("PosType2Block: error no next position");
  return vp->position;
}

// Zero indexed, gets the pair at this pos_ 
unsigned int PosType2Block::getPosAtLoc(unsigned int loc_) {
  //pair=block->getPairAtLoc(loc_%(block->getSize()));
  //numOccCounter=loc_%numOccurences;
  vp=block->getPairAtLoc(loc_/numOccurences);
  if (vp==NULL) throw new CodingException("PosType2Block: error no next position");
  return vp->position;	
}

unsigned int PosType2Block::getCurrLoc() {
  return block->getCurrLoc()*numOccurences+numOccCounter;
}

// return size of block		
int PosType2Block::getSize() {
	return block->getSize()*numOccurences;
}
int PosType2Block::getSizeInBits() {
  throw new UnexpectedException("Don't think this is used anymore");
  //doubt this is right but who cares since we don't actually use I don't think
  //return (2+numValues)*sizeof(int);	
}
unsigned int PosType2Block::getStartPos() {
	return block->getStartPair()->position;
}
unsigned int PosType2Block::getEndPos() {
	return block->getEndPosition();
}

PosBlock* PosType2Block::copySubset(int fromPos, int untilPos) {
  int startPos = getStartPos();
  int endPos = getEndPos();
  assert(startPos <= fromPos);
  assert(startPos <= untilPos);
  assert(fromPos <= endPos);
  assert(untilPos <= endPos);
  assert(fromPos <= untilPos);
  Type2Block* temp = new Type2Block(block->isValueSorted());
  //assume for now that value is an integer ... fix later ...
  temp->setBufferWithHeader(block->makeNewBuffer(-1,fromPos,untilPos));
  PosType2Block* temp2 = new PosType2Block();
  temp2->setBlock(temp);
  return temp2;
}

/*void PosType2Block::printBits(int bits) {
  for (int i = 0; i < 32; i++)
    cout << (bool)(bits & (((unsigned int)1) << (31-i)));
  cout << endl;
  }*/

// Stream properties
bool PosType2Block::isPosSorted() {  
	return posSorted;
}
bool PosType2Block::isSparse() {  
	return false;
}
	
// Block properties
bool PosType2Block::isPosContiguous() {  
	return false;
}
bool PosType2Block::isBlockPosSorted() {  
	return true;
}


byte* PosType2Block::makeNewBuffer(int value_, int startPos_, int endPos_) {
  //ick ... assumes that value is an integer ... fix later
  return block->makeNewBuffer(value_, startPos_, endPos_);
}

unsigned int PosType2Block::initBitStringIterator(unsigned int pos) {
  return block->initBitStringIterator(pos);
}

int PosType2Block::getNextBitString() {
  return block->getNextBitString();
}




/*************************
for now ignore numOccCounter in all methods
*************************/

bool PosType2Block::PosType2BlockIter::hasNext() {
  return /*numOccCounter == 0 ? */blockIter->hasNext()/* : true*/; 
}

unsigned int PosType2Block::PosType2BlockIter::getNext() {
	
  /*	if (numOccCounter == 0) {
		currPair = blockIter->getNext();	
	} 
	assert(currPair != NULL);
	numOccCounter=(numOccCounter+1)%pType2Block->numOccurences;
	return currPair->position;*/
  return blockIter->getNext()->position;
}
		
unsigned int PosType2Block::PosType2BlockIter::peekNext() {
  /*if (numOccCounter==0) {
		return blockIter->peekNext()->position;
	}
	assert(currPair != NULL);
	return currPair->position;*/
  return blockIter->peekNext()->position;
}

unsigned int PosType2Block::PosType2BlockIter::getCurrLoc() {
  //return blockIter->getCurrLoc()*pType2Block->numOccurences+numOccCounter; 
  return blockIter->getCurrLoc();
}

void PosType2Block::PosType2BlockIter::resetIterator() {
  /*numOccCounter = 0;
    currPair = NULL;*/
  blockIter->resetIterator();
  FLAPlastPos=0;
  FLAPlastCount=0;
}

bool PosType2Block::PosType2BlockIter::skipToPos(unsigned int pos) {
  return ((Type2Block::Type2BlockIter*)blockIter)->skipToPos(pos);
  //throw new UnimplementedException("This will be really expensive to calculate ... do I really need to ever do this?");
}


int PosType2Block::PosType2BlockIter::findLocAtPosAndSkip(unsigned int pos) {
  skipToPos(pos);
  int numvalues;
  int adjStartPos, adjEndPos;
  int sp;
  if ((FLAPlastPos) && (pos > FLAPlastPos)) {
    sp = FLAPlastPos; //start at the last position since we never actually counted it
  }
  else {
    sp = startpos;
  }
  if (sp % 32)
    adjStartPos = sp - ((sp % 32) - 1);
  else
    adjStartPos = sp - 31;
  //posIndex=new int[8*sizeof(int)+1];  
 
  byte* bufferwithheader = ((Type2Block*)blockIter->getBlock())->getBuffer();
  int diff = (adjStartPos - *((int*)bufferwithheader));

  //assumes buffer header size ... fix later
  byte* newBufferStart = bufferwithheader + (4*sizeof(int)) + (diff/8);

  //temporarily set numvalues
  numvalues=((pos-adjStartPos+1)/8/sizeof(int));
  if (((pos-adjStartPos+1)%(8*sizeof(int))))
    numvalues++;

  if (pos % 32)
    adjEndPos = pos - ((pos % 32) - 1);
  else
    adjEndPos = pos - 31;
  
  int numToAdd = 0;
  if ((FLAPlastPos) && (pos > FLAPlastPos)) {
    numToAdd = FLAPlastCount;
  }
  int count;
  if (peekNext() == pos) {
    count = ((Type2Block*)(blockIter->getBlock()))->howManyOnBits(newBufferStart, numvalues, sp-adjStartPos, pos-adjEndPos)-1+numToAdd;
    if (pos > FLAPlastPos) {
      FLAPlastPos = pos;
      FLAPlastCount = count;
    }
    return count;
  }
  else {
    count = ((Type2Block*)(blockIter->getBlock()))->howManyOnBits(newBufferStart, numvalues, sp-adjStartPos, pos-adjEndPos)+numToAdd;
    if (pos > FLAPlastPos) {
      FLAPlastPos = pos;
      FLAPlastCount = count;
    }
    return count;
  }
  
}

int PosType2Block::PosType2BlockIter::getSize() {
  //return (endpos-startpos)+1;
  return blockIter->getSize();
}

unsigned int PosType2Block::PosType2BlockIter::getStartPos() {
  return blockIter->getStartPair()->position;
}

unsigned int PosType2Block::PosType2BlockIter::getEndPos() {
  return blockIter->getEndPosition();
}

unsigned int PosType2Block::PosType2BlockIter::initBitStringIterator(unsigned int pos) {
  return ((Type2Block::Type2BlockIter*)blockIter)->initBitStringIterator(pos);
}

int PosType2Block::PosType2BlockIter::getNextBitString() {
  return ((Type2Block::Type2BlockIter*)blockIter)->getNextBitString();
}

// DSM NB: or operator changes start/end pos calculations! And may extend past the ends of a block,
// so if does, need to grab parts of longer block directly
void PosType2Block::PosType2BlockIter::posAnd(PosBlockIter* other, PosBlock* toWrite) {
  assert (other->isPosSorted());
  if (other->isSparse())
    throw new UnexpectedException("It isn't supposed to work this way. PosType2Block can only and with something if it can produce a PosType2Block as a result.");
  PosType2Block* tw = (PosType2Block*) toWrite;
  Type2Block* outBlock = tw->getBlock();
  int sp1 = blockIter->getStartPair()->position;
  int sp2 = other->getStartPos();
  int ep1 = blockIter->getEndPosition();
  int ep2 = other->getEndPos();
  int sp3;
  int ep3;
  if (sp1 < sp2) 
    sp3 = sp2;
  else
    sp3 = sp1;
  if (ep1 < ep2) 
    ep3 = ep1;
  else
    ep3 = ep2;
  Type2Block* t2b = (Type2Block*)blockIter->getBlock();
  if (other->isPosContiguous()) { // all 1s-> no need to do the anding--it'll just be (a subset of) our poslist.
    //assumes value is an integer ... fix later ...
    outBlock->setBufferWithHeader(t2b->makeNewBuffer(-1,sp3,ep3));
  }
  else {
    unsigned int pos1 = initBitStringIterator(sp3);
    unsigned int pos2 = other->initBitStringIterator(sp3);
    assert(pos1==pos2);
    byte* playBuffer=new byte[PAGE_SIZE];
    memset(playBuffer, 0, PAGE_SIZE);
    Type2Header* header=((Type2Header*) playBuffer);
    header->value = ((Type2Header*)t2b->getBuffer())->value;
    header->startPos = pos1;
    header->endPos = ep3;
    int newNumValues=((ep3-pos1+1)/8/sizeof(int));
    if (((ep3-pos1+1)%(8*sizeof(int))))
      newNumValues++;
    header->numValues = newNumValues;
    byte* y = playBuffer + sizeof(Type2Header);
    //fix later
    assert(sizeof(Type2Header)==16);
    int* z = (int*)y;
    z[0] = (getNextBitString() & other->getNextBitString());

    int tempAdjStartPos = sp3;
    if (tempAdjStartPos % 32)
      tempAdjStartPos -= ((tempAdjStartPos % 32) - 1);
    else {
      tempAdjStartPos-=31;
    }

    unsigned int mask=0;
    for (unsigned int i =0; i < (sp3-tempAdjStartPos)%(sizeof(int)*8); i++)
      mask = mask | (1 << (31 - i));
    z[0] = z[0] & (~mask);
    for (int i = 1; i < newNumValues; i++)
      z[i] = (getNextBitString() & other->getNextBitString()); // AND operator
    
    mask = 0;
    for (unsigned int i =0; i <= (ep3-tempAdjStartPos)%(sizeof(int)*8); i++)
      mask = mask | (1 << (31 - i));
    z[newNumValues-1] = z[newNumValues-1] & mask;
    outBlock->setBufferWithHeader(playBuffer);
    delete[] playBuffer;
     cerr << "PosAND outblock from " << outBlock->getStartPair()->position << " to " << outBlock->getEndPosition() << endl;
  }
}

void PosType2Block::PosType2BlockIter::posOr(PosBlockIter* other, PosBlock* toWrite) {
  assert (other->isPosSorted());
  assert(other->getPosBlock() != this);
  if (other->isSparse())
    throw new UnexpectedException("It isn't supposed to work this way. PosType2Block can only and with something if it can produce a PosType2Block as a result.");
    
   // FIXME proper start positions
  PosType2Block* tw = (PosType2Block*) toWrite;
  Type2Block* outBlock = tw->getBlock();
  int sp1 = blockIter->getStartPair()->position;
  int sp2 = other->getStartPos();
  int ep1 = blockIter->getEndPosition();
  int ep2 = other->getEndPos();
  assert(ep1 >= sp2);
  assert(ep2 >= sp1);
  unsigned int sp3;
  unsigned int ep3;
  unsigned int minsp, maxsp;
  unsigned int minep, maxep;  
  PosBlockIter* minIter = NULL;
  PosBlockIter* maxIter = NULL;
  PosBlockIter* minepIter = NULL;
  PosBlockIter* maxepIter = NULL;

  if (sp1 < sp2) { // For OR we use the EARLIER startpos
    sp3 = sp1;
    minsp = sp1;
   	maxsp = sp2;
	minIter = this;
	maxIter = other;
  } else {
    sp3 = sp2;
    minsp = sp2;
   	maxsp = sp1;
	minIter = other;
	maxIter = this;
  }
  if (ep1 < ep2) { // For OR we use the LATER endPos. 
    ep3 = ep2;
    minep = ep1;
    maxep = ep2;
    minepIter = this;
    maxepIter = other;
  } else {
    ep3 = ep1;
    minep = ep2;
    maxep = ep1;
    minepIter = other;
    maxepIter = this;
  }
  Type2Block* t2b = (Type2Block*)blockIter->getBlock();
  
//  cerr << "will por from " << sp3 << " to " << ep3 << endl;
  
  // If the other block is pos contiguous, then its bitstring is all 1's. The oring of all 1's
  // with anything is all 1's, so there's no need to actually do the or.
  // t2b is our block. FIXME FIXME FIXME.
  // outBlock is the Type2Block in the PosType2Block we're writing
  if (other->isPosContiguous()) {
  	throw UnexpectedException("not implemented correctly");
    outBlock->setBufferWithHeader(t2b->makeNewBuffer(-1,sp3,ep3));
  }
  else {
	unsigned int pos1 = minIter->initBitStringIterator(minsp);
	unsigned int pos2 = maxIter->initBitStringIterator(maxsp);
  //  cerr << "pos1 = " << pos1 << " pos2 = " << pos2 << endl;

    byte* playBuffer=new byte[PAGE_SIZE];
    memset(playBuffer, 0, PAGE_SIZE);
    Type2Header* header=((Type2Header*) playBuffer);
    //assumes integer ... fix later ...
    header->value = ((Type2Header*)t2b->getBuffer())->value;
    header->startPos = pos1;
    header->endPos = ep3;
    unsigned int newNumValues=((ep3-pos1+1)/8/sizeof(int));
    if (((ep3-pos1+1)%(8*sizeof(int))))
      newNumValues++;
    header->numValues = newNumValues;
 //   cerr << "using " << newNumValues << " new num values (assume this means ints)" << endl;
    byte* y = playBuffer + sizeof(Type2Header);
    assert(sizeof(Type2Header)==16);
    int* z = (int*)y;

    unsigned int minAdjEndPos = minep;
    if (minAdjEndPos % 32)
      minAdjEndPos -= ((minAdjEndPos % 32) - 1);
    else {
      minAdjEndPos-=31;
    }

    unsigned int maxAdjEndPos = maxep;
    if (maxAdjEndPos % 32)
      maxAdjEndPos -= ((maxAdjEndPos % 32) - 1);
    else {
      maxAdjEndPos-=31;
    }

    // Copy until consensus start
    unsigned int i = pos1;
    for (; i < pos2; i+=sizeof(int)*8) {
      z[(i-pos1)/(sizeof(int)*8)] = minIter->getNextBitString();
    }
    
    // Mask out first block of consensus read from maxiter

    unsigned int mask=0;
    for (unsigned int j = 0; j < (maxsp - pos2)%(sizeof(int)*8); j++) {
      mask |= (1 << (31 - j));	// zero out until consensus (inter-block) start pos 
    }

    if (i < minAdjEndPos) {
      z[(i-pos1)/(sizeof(int)*8)] = maxIter->getNextBitString() & (~mask);
      z[(i-pos1)/(sizeof(int)*8)] |= minIter->getNextBitString();
      i+=sizeof(int)*8;

    
      for (; i < minAdjEndPos; i+=sizeof(int)*8)
	z[(i-pos1)/(sizeof(int)*8)] = (minIter->getNextBitString() | maxIter->getNextBitString()); // OR operator
      
      
      mask=0;
      for (unsigned int j =0; j <= (minep-pos2)%(sizeof(int)*8); j++)
	mask = mask | (1 << (31 - j));
      
      z[(i-pos1)/(sizeof(int)*8)] = minepIter->getNextBitString() & mask;
      z[(i-pos1)/(sizeof(int)*8)] |= maxepIter->getNextBitString();
      i+=sizeof(int)*8;
    }
    else {
      assert(i==pos2);
      assert(minAdjEndPos==pos2);
      unsigned int temp = maxIter->getNextBitString() & (~mask);
      mask=0;
      for (unsigned int j =0; j <= (minep-pos2)%(sizeof(int)*8); j++)
	mask = mask | (1 << (31 - j));
      
      //z[(i-pos1)/(sizeof(int)*8)] = minepIter->getNextBitString() & mask;
      //z[(i-pos1)/(sizeof(int)*8)] |= maxepIter->getNextBitString();
      //i+=sizeof(int)*8;
      if (maxIter == minepIter) {
	z[(i-pos1)/(sizeof(int)*8)] = temp & mask;
	z[(i-pos1)/(sizeof(int)*8)] |= maxepIter->getNextBitString();
      }
      else {
	z[(i-pos1)/(sizeof(int)*8)] = temp;
	temp = minepIter->getNextBitString() & mask;
	z[(i-pos1)/(sizeof(int)*8)] |= temp;
      }
      i+=sizeof(int)*8;
    }
    maxAdjEndPos+=sizeof(int)*8;
    for (; i < maxAdjEndPos; i+=sizeof(int)*8)
      z[(i-pos1)/(sizeof(int)*8)] = (maxepIter->getNextBitString());
    
    
    // Fixup z[0]
    mask=0;
    for (unsigned int j = 0; j < (minsp - pos1)%(sizeof(int)*8); j++) {
      mask |= (1 << (31 - j));	// zero out until consensus (inter-block) start pos 
    }
    z[0] = z[0] & (~mask);	// Masking AND

    // Fixup z[(i-pos1)/(sizeof(int)*8)]
    mask = 0;
    for (unsigned int j = 0; j <= (maxep - pos1)%(sizeof(int)*8); j++) {
      mask |= (1 << (31 - j));	// zero out until consensus (inter-block) start pos 
    }
    z[(i-pos1)/(sizeof(int)*8)] = z[(i-pos1)/(sizeof(int)*8)] & (mask);	// Masking AND
    assert((newNumValues) == ((i-pos1)/(sizeof(int)*8)));

    
    outBlock->setBufferWithHeader(playBuffer);
 //   cerr << "PosOR outblock from " << outBlock->getStartPair()->position << " to " << outBlock->getEndPosition() << endl;
    delete[] playBuffer;
  }
}

bool PosType2Block::PosType2BlockIter::isPosSorted() {
  return pType2Block->isPosSorted();
}

bool PosType2Block::PosType2BlockIter::isPosContiguous() {
  return pType2Block->isPosContiguous();
}

bool PosType2Block::PosType2BlockIter::isBlockPosSorted() {
  return pType2Block->isBlockPosSorted();
}

bool PosType2Block::PosType2BlockIter::isSparse() {
  return pType2Block->isSparse();
}

PosBlock* PosType2Block::PosType2BlockIter::clone(PosBlock& pb) {
	return new PosType2BlockIter((PosType2BlockIter&)pb);
}
