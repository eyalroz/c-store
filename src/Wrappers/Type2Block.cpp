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
// Coded by DNA

#include "Type2Block.h"

Type2Block::Type2Block(bool valSorted_) : Block()
{
  init(valSorted_);
  //setType(ValPos::INTTYPE);
}

/*Type2Block::Type2Block(bool valSorted_, short type_) : Block()
{
  init(valSorted_);
  setType(type_);
}*/

void Type2Block::init(bool valSorted_) {
  vp=NULL;
  utilityVP = NULL;
  startPair = NULL;
	valSorted=valSorted_ ;
	posContig=false;
	posSorted=true;
	currPos=-1;
	buffer = NULL;
	bfrWithHeader = NULL;
	startPos=-1;
	endPos=-1;
	startVal=-1;
	numValues=0;
	currIndexInVal=-1;
	bits_in_16bits = new char[0x1u << 16];
	compute_bits_in_16bits () ;
	posIndex = new int[8*sizeof(int)+1];
	keepSame=false;
	playBuffer=NULL;
	calculatedSize=false;
	numOnBits=-1;
	peeked=false;
	bitStringInitialized = false;
	bitStringPtr=NULL;
	currEndPos = 0;
	GPALlastLoc = -1;
	GPALlastIndex = 0;
}

Type2Block::Type2Block(const Type2Block& block_) : Block(block_)
{
  vp = block_.vp->clone();
  utilityVP = block_.utilityVP->clone();
  startPair = block_.startPair->clone();
	valSorted=block_.valSorted;
	posContig=block_.posContig;
	posSorted=block_.posSorted;
	buffer=NULL;
	bfrWithHeader = NULL;
	endPos=-1;
	posIndex = new int[8*sizeof(int)+1];
	setBufferWithHeader(block_.bfrWithHeader);
	bits_in_16bits = new char[0x1u << 16];
	compute_bits_in_16bits () ;
	keepSame=false;	
	playBuffer=NULL;
	calculatedSize=block_.calculatedSize;
	numOnBits=block_.numOnBits;
	peeked=false;
	bitStringInitialized = false;
	bitStringPtr=NULL;
	currEndPos = 0;
	GPALlastLoc = -1;
	GPALlastIndex = 0;
}

Block* Type2Block::clone(Block& block_)
{
  Block* newBlock = new Type2Block((Type2Block&)block_);
  return newBlock;
}

Type2Block::~Type2Block()
{
  /*if (pair)
    delete pair;
  if (utilityPair)
    delete utilityPair;
  if (buffer)
  delete[] buffer;*/
  delete[] bits_in_16bits;
  if (playBuffer)
    delete[] playBuffer;
  delete[] bfrWithHeader;
  delete vp;
  delete utilityVP;
  delete startPair;
  delete[] posIndex;
}


int Type2Block::iterated_bitcount (unsigned int n)
{
    int count=0;    
    while (n)
    {
        count += n & 0x1u ;    
        n >>= 1 ;
    }
    return count ;
}

void Type2Block::compute_bits_in_16bits ()
{
    unsigned int i ;    
    for (i = 0; i < (0x1u<<16); i++)
        bits_in_16bits [i] = iterated_bitcount (i) ;
    return ;
}

int Type2Block::precomputed16_bitcount (unsigned int n)
{
    // works only for 32-bit int
    
    return bits_in_16bits [n         & 0xffffu]
        +  bits_in_16bits [(n >> 16) & 0xffffu] ;
}

void Type2Block::resetBlock() {
  currPos=-1;
  currPosPtr=(int*)buffer;
  currPosPtr++;
  currIndexInVal=-1;
  peeked=false;
  peekNext();
  vp->position=0;//realStartPos;
  GPALlastLoc = -1;
  GPALlastIndex = 0;
}

/*void Type2Block::setBuffer(int startpos_, int numValues_, byte* buffer_) {

  currPos=-1;
  startPos=startpos_;
  if (bfrWithHeader)
    delete[] bfrWithHeader;
  bfrWithHeader=new byte[(numValues_+3)*sizeof(int)];
  memcpy(bfrWithHeader, &startpos_, sizeof(int));
  memcpy(bfrWithHeader+sizeof(int), &numValues_, sizeof(int));
  buffer = bfrWithHeader + (2*sizeof(int));
  memcpy(buffer, buffer_, (numValues_+1)*sizeof(int));
  currPosPtr=(int*)buffer;
  numValues=numValues_;
  startVal=(*currPosPtr);
  currPosPtr++;
  //pair->value=(*currPosPtr);
  //pair->position=startpos_;
  currIndexInVal=-1;

  }*/
void Type2Block::setValue(ValPos** valindex) {
  if (bfrWithHeader==NULL)
    throw new CodingException("Type2Block: error, setting value when there is no buffer");
  if (startVal == -1)
    throw new CodingException("This method should only be called after setBufferWithHeader where startVal is set");
  unsigned int temppos = vp->position;
  delete vp;
  vp = valindex[startVal]->clone();
  vp->position = temppos;
  temppos = utilityVP->position;
  delete utilityVP;
  utilityVP = valindex[startVal]->clone();
  utilityVP->position = temppos;
  temppos = startPair->position;
  delete startPair;
  startPair = valindex[startVal]->clone();
  startPair->position = temppos;
  
}

void Type2Block::setBufferWithHeader(byte* buffer_) {
  //if (bfrWithHeader)
  //  delete[] bfrWithHeader;
  if (buffer_==NULL)
    throw new CodingException("Type2Block: error, buffer passed into setBufferWithHeader is NULL");
  //bfrWithHeader = buffer_;
  Type2Header* header=((Type2Header*) buffer_);
  startVal=header->value;
  startPos=header->startPos;
  endPos=header->endPos;
  numValues=header->numValues;
  delete[] bfrWithHeader;
  bfrWithHeader=new byte[sizeof(Type2Header) + numValues*sizeof(int)];
  memcpy(bfrWithHeader, buffer_, sizeof(Type2Header) + numValues*sizeof(int));
  buffer = bfrWithHeader + (3*sizeof(int));
  currPosPtr=(int*)buffer;
  currPosPtr++;
  currIndexInVal=-1;
  currPos=-1;
  //for now assume value type is an integer
  //it will stay this way unless setValue is called
  delete vp;
  vp = new IntValPos(0, startVal);
  delete utilityVP;
  utilityVP = new IntValPos(0, startVal);
  peeked=false;
  realStartPos = peekNext()->position;
  //pair->position = 0;//realStartPos;
  delete startPair;
  startPair = new IntValPos(realStartPos, startVal);
  //startPair->position = realStartPos;
  //startPair->value=startVal;
  GPALlastLoc = -1;
  GPALlastIndex = 0;
}

void Type2Block::setBuffer(DeltaPosBlock* dbp) {
  ValPos* p = dbp->getStartPair();
  /*  startPos = p->position;
  if (startPos % 32)
    startPos -= ((startPos % 32) - 1);
  else {
    //assert (false);
    //I'm changing from
    //startPos-=32;
    //to
    startPos-=31;
    //but I want to make sure that this doesn't kill something, so I assert(false) to see if we ever get here.
  }
  startVal = p->value;*/
  initEmptyBuffer(p->position, p);
  /*p = dbp->getPairAtLoc(dbp->getSize()-1);
    endPos = p->position;*/
  int cPos;
  //cout << "startval: " << startVal << "startpos: " << startPos << " endpos: " << endPos << endl;
  dbp->resetBlock();
  /*currPos=-1;*/
  //if (bfrWithHeader)
  //  delete[] bfrWithHeader;
  //bfrWithHeader=new byte[(((endPos-startPos+1)/8/sizeof(int))+5)*sizeof(int)];
  /*if (!bfrWithHeader)
    bfrWithHeader=new byte[PAGE_SIZE];
  memset(bfrWithHeader, 0, PAGE_SIZE);
  memcpy(bfrWithHeader, &startPos, sizeof(int));*/
  /*memcpy(bfrWithHeader+(2*sizeof(int)), &endPos, sizeof(int));*/
  /*buffer = bfrWithHeader + (3*sizeof(int));*/
  //memset(buffer, 0, (((endPos-startPos+1)/8/sizeof(int))+2)*sizeof(int));
  /*int currInt;
    int* bufferPtr = (int*)buffer;*/
  while (dbp->hasNext()) {
    cPos = dbp->getNext()->position;
    addPosition(cPos);
  }
  finishBufferEditing(cPos);

  //unsigned int x = 1;
  //x <<= 31;
  /*memcpy(buffer, &startVal, sizeof(int));*/
  //memcpy(buffer+4, &x, sizeof(int));
  /*currPosPtr=(int*)buffer;
  numValues=((endPos-startPos+1)/8/sizeof(int));
  if (((endPos-startPos+1)%(8*sizeof(int))))
    numValues++;
  memcpy(bfrWithHeader+sizeof(int), &numValues, sizeof(int));
  //startVal=(*currPosPtr);
  currPosPtr++;
  //pair->value=(*currPosPtr);
  //pair->position=startPos;
  currIndexInVal=-1;
  pair->value = startVal;
  utilityPair->value = startVal;
  peeked=false;
  realStartPos = peekNext()->position;
  pair->position = realStartPos;
  startPair->position = realStartPos;
  startPair->value=startVal;*/
  GPALlastLoc = -1;
  GPALlastIndex = 0;

}

void Type2Block::initEmptyBuffer(unsigned int startpos, ValPos* startVal) {
  startPos = startpos;
  if (startPos % 32)
    startPos -= ((startPos % 32) - 1);
  else {
    startPos-=31;
  }
  delete vp;
  delete utilityVP;
  delete startPair;
  vp = startVal->clone();
  utilityVP = startVal->clone();
  startPair = startVal->clone();
  //this->startVal = startVal;
  if (!bfrWithHeader)
    bfrWithHeader=new byte[PAGE_SIZE];
  else {
    delete[] bfrWithHeader;
    bfrWithHeader=new byte[PAGE_SIZE];
  }
  memset(bfrWithHeader, 0, PAGE_SIZE);
  memcpy(bfrWithHeader, &startPos, sizeof(int));
  buffer = bfrWithHeader + (3*sizeof(int));
  bufferPtrAsIntArr = (int*)buffer;
  currEndPos = startpos;
}

//returns what the numValues variable would be if we added this position (assming this would be the biggest position in the block.
int Type2Block::testAddPos(unsigned int pos) {
  int retval=((pos-startPos+1)/8/sizeof(int));
  if (((pos-startPos+1)%(8*sizeof(int))))
    retval++;
  return retval;
}

//updates the value of currEndPos even if there will be a 0 at that position
void Type2Block::setCurrEndPos(unsigned int pos) {
  currEndPos = pos;
}

void Type2Block::addPosition(unsigned int pos) {
  int currInt = ((pos - startPos) / 32)+1;
  bufferPtrAsIntArr[currInt] = bufferPtrAsIntArr[currInt] | 
    (1 << (31 - ((pos - startPos) % 32)));
  if (pos>currEndPos)
    currEndPos = pos;
}

void Type2Block::finishBufferEditing(unsigned int endpos) {
  endPos = endpos;
  currEndPos = 0;
  memcpy(bfrWithHeader+(2*sizeof(int)), &endpos, sizeof(int));
  currPos=-1;
  if ((vp->type != ValPos::NOTYPE) && (vp->getSize() == sizeof(int)))
    memcpy(buffer, vp->value, sizeof(int));
  else
    memset(buffer, 0, sizeof(int));
  currPosPtr=(int*)buffer;
  numValues=((endPos-startPos+1)/8/sizeof(int));
  if (((endPos-startPos+1)%(8*sizeof(int))))
    numValues++;
  memcpy(bfrWithHeader+sizeof(int), &numValues, sizeof(int));
  //startVal=(*currPosPtr);
  currPosPtr++;
  //pair->value=(*currPosPtr);
  //pair->position=startPos;
  currIndexInVal=-1;
  //pair->value = startVal;
  //utilityPair->value = startVal;
  peeked=false;
  realStartPos = peekNext()->position;
  vp->position = 0;//realStartPos;
  startPair->position = realStartPos;
  //startPair->value=startVal;
  
}

//void Type2Block::setValue(Pair* val_) {
//	currPos=-1;
//	pair->set(*val_);
//}

void Type2Block::setKeepSame() {
  keepSame=true;
}
	
// Iterator access to block
bool Type2Block::hasNext() {
  //return (currPos<(numValues));
  return (vp->position<(unsigned int)endPos);
}
bool Type2Block::hasNext(int value_) {
  if (*vp == value_)
    return hasNext();
  else
    return false;
}

ValPos* Type2Block::getNext() {
  int tempx;
  /*  if (keepSame) {
    keepSame=false;
    return pair;
    }*/
  if (peeked) {
    vp->position = utilityVP->position;
    peeked = false;
    return vp;
  }
  if (currPos<0) {
    tempx=*currPosPtr;
    if (tempx == 0) {
      currPos=1;	
      currPosPtr++;
      tempx = *currPosPtr;
    }
    else currPos=0;
    while ((tempx == 0) && (currPos < numValues)) {
      currPos++;	
      currPosPtr++;
      tempx = *currPosPtr;
    }
    if (!(currPos < numValues) && ((tempx == 0)))
      return NULL;
    else {
      setPosIndex(tempx, posIndex, currIndexInVal);
    }
    vp->position=(currPos*sizeof(int)*8)+startPos+posIndex[currIndexInVal];
    currIndexInVal++;
    if (posIndex[currIndexInVal] == 255) {
      currPos++;	
      currPosPtr++;
      currIndexInVal=0;
    }
    //printBits(pair->value);
    return vp;
  }
  else if (currPos<(numValues)) {
    if (currIndexInVal) {
      vp->position=(currPos*sizeof(int)*8)+startPos+posIndex[currIndexInVal];
    //pair->value=startVal;
      currIndexInVal++;
      if (posIndex[currIndexInVal] == 255) {
	currPos++;	
	currPosPtr++;
	currIndexInVal=0;
      }
    }
    else {
      tempx=*currPosPtr;
      while ((tempx == 0) && (currPos < numValues)) {
	currPos++;	
	currPosPtr++;
	tempx = *currPosPtr;
      }
      if (!(currPos < numValues) && ((tempx == 0)))
	return NULL;
      else {
	setPosIndex(tempx, posIndex, currIndexInVal);
      }
      vp->position=(currPos*sizeof(int)*8)+startPos+posIndex[currIndexInVal];
      currIndexInVal++;
      if (posIndex[currIndexInVal] == 255) {
	currPos++;	
	currPosPtr++;
	currIndexInVal=0;
      }
    }
    //printBits(pair->value);
    return vp;
  }
  else
    return NULL;
}

ValPos* Type2Block::peekNext() {
  int tempx;
  if (peeked) {
  	return utilityVP;
  }
  peeked = true;
  //lastPos = pair->position;
  /*  if (keepSame) {
    keepSame=false;
    return pair;
    }*/
  if (currPos<0) {
    tempx=*currPosPtr;
    if (tempx == 0) {
      currPos=1;	
      currPosPtr++;
      tempx = *currPosPtr;
    }
    else currPos=0;
    while ((tempx == 0) && (currPos < numValues)) {
      currPos++;	
      currPosPtr++;
      tempx = *currPosPtr;
    }
    if (!(currPos < numValues) && ((tempx == 0)))
      return NULL;
    else {
      setPosIndex(tempx, posIndex, currIndexInVal);
    }
    utilityVP->position=(currPos*sizeof(int)*8)+startPos+posIndex[currIndexInVal];
    currIndexInVal++;
    if (posIndex[currIndexInVal] == 255) {
      currPos++;	
      currPosPtr++;
      currIndexInVal=0;
    }
    //printBits(pair->value);
    return utilityVP;
  }
  else if (currPos<(numValues)) {
    if (currIndexInVal) {
      utilityVP->position=(currPos*sizeof(int)*8)+startPos+posIndex[currIndexInVal];
      currIndexInVal++;
      if (posIndex[currIndexInVal] == 255) {
	currPos++;	
	currPosPtr++;
	currIndexInVal=0;
      }
    }
    else {
      tempx=*currPosPtr;
      while ((tempx == 0) && (currPos < numValues)) {
	currPos++;	
	currPosPtr++;
	tempx = *currPosPtr;
      }
      if (!(currPos < numValues) && ((tempx == 0)))
	return NULL;
      else {
	setPosIndex(tempx, posIndex, currIndexInVal);
      }
      utilityVP->position=(currPos*sizeof(int)*8)+startPos+posIndex[currIndexInVal];
      currIndexInVal++;
      if (posIndex[currIndexInVal] == 255) {
	currPos++;	
	currPosPtr++;
	currIndexInVal=0;
      }
    }
    //printBits(pair->value);
    return utilityVP;
  }
  else
    return NULL;
}

ValPos* Type2Block::getValue() {
	if (startPos==-1) throw new CodingException("Type2Block: cannot get value of empty multi block");
	//if (!vp || (!(vp->type == ValPos::INTTYPE))) throw new CodingException("Type2Block: This is not an integer block");
	//return *(int *)vp->value;
	return vp;
}
unsigned int Type2Block::getStartPosition() {
  return realStartPos;
}
unsigned int Type2Block::getEndPosition() {
  if (endPos != -1)
    return (unsigned int) endPos;
  else
    throw new UnimplementedException("Actually, I think this will never happen now");
}
unsigned int Type2Block::getPosition() {
  if (vp->position==0) throw new CodingException("Type2Block: cannot get position of empty multi block");	
  return vp->position;
}
unsigned int Type2Block::getLastPosition() {
  return getEndPosition();//getPairAtLoc(getSize()-1)->position;
}

// Zero indexed, gets the pair at this pos_
ValPos* Type2Block::getPairAtLoc(unsigned int loc_) {
  //Note: casting loc_ as int is bad, but I really doubt a block will ever be that big that this will make a difference.
  assert((int)loc_ < getSize());
  int entrysize = (sizeof(int)*8);
  int nv = numValues;
  unsigned int* x = (unsigned int*)buffer;
  x++;
  int count, i;
  if ((int)loc_ > GPALlastLoc) {
    count = GPALlastLoc;
    i = GPALlastIndex;
  }
  else {
    count = -1;
    i = 1;
    count+=(precomputed16_bitcount(x[0]));
  }
  while (count < (int)loc_) {
    count+=precomputed16_bitcount(x[i]);
    i++;
  }
  assert(i<=nv);
  i--;
  count-=precomputed16_bitcount(x[i]);
  if ((int)loc_ > GPALlastLoc) {
    GPALlastLoc = count;
    GPALlastIndex = i;
  }
  
  assert(count < (int)loc_);
  int j = 0;
  while (j < entrysize) {
    if ((1<<(entrysize-j-1)) & x[i]) {
      count++;
      if (count==(int)loc_) {
	utilityVP->position=(i*sizeof(int)*8)+startPos+j;
	return utilityVP;
      }
    }
    j++;
  }
  assert(false);
}

//Like getPairAtLoc except set the regular pair variable rather than 
//the utilityPair variable so can be called by getNext().
ValPos* Type2Block::getPairAtLocNotUtility(unsigned int loc_) {
  throw new UnimplementedException("This will be really expensive to calculate ... do I really need to ever do this?");

  //OK, so this is a little HACKY, but getPairAtLoc doesn't mean much for type 2, so instead it will just return a pair with the number of on bits turned on + the startPos - 1 which is useful in the aggregator
  /*
  assert(buffer!=NULL);
  pair->value=startVal;
  pair->position=howManyOnBits(buffer+sizeof(int), numValues, 0, sizeof(int)*8 - 1) - 1 + getStartPair()->position;

  //Also HACKY and will probably break things if not called from aggregator
  currPos=numValues;
  
  */
  return vp;
}

/*void Type2Block::cutOffEnd(int endPos) {
  if (endPos < startPos+numValues)
    numValues=(endPos-startPos)+1;
    }*/

int Type2Block::getCurrLoc() {
  throw new UnimplementedException("This will be really expensive to calculate ... do I really need to ever do this?");
  if (currPos == -1)
    return 0;
  else
    return currPos-1;
}

// return size of block	in number of on positions
int Type2Block::getSize() {
  if (!calculatedSize)
    numOnBits = howManyOnBits(buffer+sizeof(int), numValues, 0, sizeof(int)*8 - 1);
  return numOnBits;
  //return numValues;	
}

// return size of block	in integers
int Type2Block::getNumValues() {
  int nv;
  if (currEndPos) {
    nv=((currEndPos-startPos+1)/8/sizeof(int));
    if (((currEndPos-startPos+1)%(8*sizeof(int))))
      nv++;
    return nv;
  }
  else
    return numValues;	
}
// return buffer size of block in bits
int Type2Block::getSizeInBits() {
  return PAGE_SIZE*8;	
}
ValPos* Type2Block::getStartPair() {
  return startPair;

    //throw new UnimplementedException("Not done yet");
  //if (buffer==NULL) return NULL;
  //else return getPairAtLoc(0);
}
	
bool Type2Block::isOneValue() {
    return true;
}
bool Type2Block::isValueSorted() {
	return valSorted;	
}
		
bool Type2Block::isPosContiguous() {
	return posContig;	
}
bool Type2Block::isPosSorted() {
	return posSorted;	
}
bool Type2Block::isBlockValueSorted() {
	return true;	
}
bool Type2Block::isBlockPosSorted() {
	return true;
}

void Type2Block::printBits(int bits) {
  for (int i = 0; i < 32; i++)
    cout << (bool)(bits & (((unsigned int)1) << (31-i)));
  cout << endl;
}

void Type2Block::setPosIndex(int v, int* pidx, int& currIndexInVal) {
  //printBits(v);
  currIndexInVal=0;
  for (int i = (8*sizeof(int))-1; i>=0; i--) {
    if ((1 << i) & v) {
      pidx[currIndexInVal] = ((8*sizeof(int))-1)-i;
      //cout << posIndex[currIndexInVal] << endl;
      currIndexInVal++;
    }
  }
  pidx[currIndexInVal]=255;
  currIndexInVal=0;
}

byte* Type2Block::getBuffer() { 
	return bfrWithHeader; 
}

int Type2Block::howManyOnBits(byte* buf, int nv, int startind, int endind) {
  //startind, endind are the indicies in the first and #nv entry in buf to start and end counting
  assert(buf!=NULL);
  int entrysize = (sizeof(int)*8);
  assert(startind < entrysize);
  assert(endind < entrysize);
  unsigned int temp1 = 0;
  for (int i = startind; i < entrysize; i++)
    temp1 = temp1 | (1 << (entrysize-i-1));
  unsigned int temp2 = 0;
  for (int i = 0; i <= endind; i++)
    temp2 = temp2 | (1 << (entrysize-i-1));
  unsigned int* x = (unsigned int*)buf;
  int count = 0;
  
  if (nv!=1) {
  count+=(precomputed16_bitcount(x[0] & temp1));
  count+=(precomputed16_bitcount(x[nv-1] & temp2));
  }
  else {
    temp1 = x[0] & temp1;
    count+=(precomputed16_bitcount(temp1 & temp2));
  }
    
  for (int i = 1; i < nv-1; i++)
    count+=precomputed16_bitcount(x[i]);
  return count;
}

void Type2Block::cutOffEnd(int endPos_) {
  endPos=endPos_;
  int adjustedEndPos = endPos_-startPos;
  int currIntWithPos = adjustedEndPos/(sizeof(int)*8);
  unsigned int* x = (unsigned int*)buffer;
  x++;
  unsigned int mask=0;
  for (unsigned int i =0; i < adjustedEndPos%(sizeof(int)*8); i++)
    mask = mask | (1 << (31 - i));
  x[currIntWithPos] = x[currIntWithPos] & mask;
  numValues=((endPos-startPos+1)/8/sizeof(int));
  if (((endPos-startPos+1)%(8*sizeof(int))))
    numValues++;
}

void Type2Block::cutOffStart(int begPos_) {
  int adjustedBegPos = begPos_-startPos;
  int currIntWithPos = adjustedBegPos/(sizeof(int)*8);
  unsigned int* x = (unsigned int*)buffer;
  x++;
  unsigned int mask=0;
  for (unsigned int i =0; i < adjustedBegPos%(sizeof(int)*8); i++)
    mask = mask | (1 << (31 - i));
  x[currIntWithPos] = x[currIntWithPos] & (~mask);
  for (int i=0; i < currIntWithPos; i++)
    x[i] = 0;
}

byte* Type2Block::makeNewBuffer(int value_, int startPos_, int endPos_) {
  if (!playBuffer)
    playBuffer=new byte[PAGE_SIZE];

  memset(playBuffer, 0, PAGE_SIZE);

  int newStartPos;

  if (startPos_ % 32)
    newStartPos = startPos_ - ((startPos_ % 32) - 1);
  else {
    // assert (false);
    //I'm changing from
    //newStartPos=startPos - 32;
    //to
    newStartPos=startPos_ - 31;
    //but I want to make sure that this doesn't kill something, so I assert(false) to see if we ever get here.
  }

  Type2Header* header=((Type2Header*) playBuffer);
  if (value_ != -1)
    header->value = value_;
  else
    header->value = startVal;
  header->startPos = newStartPos;
  header->endPos = endPos_;
  int newNumValues=((endPos_-newStartPos+1)/8/sizeof(int));
  if (((endPos_-newStartPos+1)%(8*sizeof(int))))
    newNumValues++;
  header->numValues = newNumValues;

  int adjustedBegPos = startPos_-startPos;
  int adjustedEndPos = endPos_-startPos;
  int currIntWithBegPos = adjustedBegPos/(sizeof(int)*8);
  int currIntWithEndPos = adjustedEndPos/(sizeof(int)*8);

  unsigned int* x = (unsigned int*)buffer;
  x++;

  byte* y = playBuffer + sizeof(Type2Header);
  assert(sizeof(Type2Header)==16);
  assert(((int)(adjustedBegPos%(sizeof(int)*8))) == ((startPos_-newStartPos)));
  unsigned int* z = (unsigned int*)y;

  assert(newNumValues == (currIntWithEndPos-currIntWithBegPos+1));

  int indexOfLastPos = -1;

  //Do everything except the last one (we don't want to update the if statement for the last one)
  for (int i = currIntWithBegPos; i < currIntWithEndPos; i++) {
    z[i-currIntWithBegPos]=x[i];
    if (x[i])
      indexOfLastPos = i-currIntWithBegPos;
  }
  z[currIntWithEndPos-currIntWithBegPos]=x[currIntWithEndPos];

  unsigned int mask=0;
  for (unsigned int i =0; i < adjustedBegPos%(sizeof(int)*8); i++)
    mask = mask | (1 << (31 - i));
  z[0] = z[0] & (~mask);

  mask = 0;
  for (unsigned int i =0; i <= adjustedEndPos%(sizeof(int)*8); i++)
    mask = mask | (1 << (31 - i));
  z[newNumValues-1] = z[newNumValues-1] & mask;

  if (z[newNumValues-1]) {
    // int lastPos = 0;
    // for (unsigned int i =0; i < (sizeof(int)*8); i++)
      // if (z[indexOfLastPos] & (1 << (31 - i)))
	// lastPos=i;
	// TODO: Is there really no use for lastPos here?
  }
  else if (indexOfLastPos == -1)
    header->endPos = 0;
  else {
    int lastPos = 0;
    for (unsigned int i =0; i < (sizeof(int)*8); i++)
      if (z[indexOfLastPos] & (1 << (31 - i)))
	lastPos=i;
    if (((indexOfLastPos*sizeof(int)*8)+newStartPos+lastPos) < (unsigned int)startPos_)
      header->endPos = 0;
    else
      header->endPos = (indexOfLastPos*sizeof(int)*8)+newStartPos+lastPos;
  }



  return playBuffer;
}

int Type2Block::initBitStringIterator(unsigned int pos) {
  //assert (((pos-1) % 32) == 0);
  assert (pos >= (unsigned int)startPos);
  assert (pos <= (unsigned int)endPos);
  if (pos % 32)
    pos -= ((pos % 32) - 1);
  else {
    pos-=31;
  }
  bitStringInitialized = true;
  bitStringPtr =(int*)buffer;
  bitStringPtr++;
  int temppos = ((pos - (unsigned int)startPos) / (8*sizeof(int)));
  bitStringPtr+=temppos;
  
  return pos;
}

int Type2Block::getNextBitString() {
  int temp = *bitStringPtr;
  bitStringPtr++;
  return temp;
}

Type2Block::Type2BlockIter::Type2BlockIter(Type2Block* t2Block_, unsigned int startPos_, unsigned int endPos_) : t2Block(t2Block_), startpos(startPos_), endpos(endPos_) {
  vp = t2Block_->vp->clone();
  utilityVP = t2Block_->utilityVP->clone();
  startPair = t2Block_->startPair->clone();
  //pair->value = t2Block_->startVal;
  //utilityPair->value = t2Block_->startVal;
  //startPair->value = t2Block_->startVal;
  if (startpos % 32)
    adjStartPos = startpos - ((startpos % 32) - 1);
  else
    adjStartPos = startpos - 31;
  posIndex=new int[8*sizeof(int)+1];  

  int diff = (adjStartPos - t2Block_->startPos);
  newBufferStart = t2Block->buffer + (diff/8) + (sizeof(int));

  //temporarily set numvalues
  numvalues=((endpos-adjStartPos+1)/8/sizeof(int));
  if (((endpos-adjStartPos+1)%(8*sizeof(int))))
    numvalues++;

  int* tempbuffer = (int*) newBufferStart;
  tempbuffer += (numvalues-1);
  unsigned int adjEndPos;
  if (endpos % 32)
    adjEndPos = endpos - ((endpos % 32) - 1);
  else
    adjEndPos = endpos - 31;

  //OK: endeavor to find the real end position (the position of the last '1' in the bitstring starting from endpos and going backwards). 
  int dontuse;
  t2Block->setPosIndex(*tempbuffer, posIndex, dontuse);
  int index = -1;
  for (unsigned int i = 0; (unsigned int)posIndex[i] <= (endpos - adjEndPos); i++) {
    //if (posIndex[i] != 255)
      index = i;
      //else
      //break;
  }
  if ((index != -1) && ((unsigned int)posIndex[index]<= (endpos - adjEndPos))) {
    endpos = adjEndPos + posIndex[index];
  }
  else {
    int j = numvalues-2;
    tempbuffer--;
    while ((j >=0) && (!(*tempbuffer))) {
      j--;
      tempbuffer--;
    }
    if (j==-1) {
      endpos=0;
    }
    else {
      t2Block->setPosIndex(*tempbuffer, posIndex, dontuse);
      for (unsigned int i = 0; i < sizeof(int)*8; i++) {
	if (posIndex[i] == 255) {
	  endpos = adjStartPos + (j*sizeof(int)*8) + posIndex[i-1];
	  break;
	}
	else if (i == sizeof(int)*8-1)
	  endpos = adjStartPos + (j*sizeof(int)*8) + posIndex[i];
      } 
    }
  }

  //reset numvalues
  numvalues=((endpos-adjStartPos+1)/8/sizeof(int));
  if (((endpos-adjStartPos+1)%(8*sizeof(int))))
    numvalues++;
  calculatedSize = false;
  peeked = false;
  resetIterator();
}

Type2Block::Type2BlockIter::Type2BlockIter(const Type2BlockIter& b) : t2Block(b.t2Block) {
  startpos = b.startpos;
  endpos = b.endpos;
  adjStartPos = b.adjStartPos;
  peeked = false;
  numvalues=b.numvalues;
  vp = b.vp->clone();
  utilityVP = b.utilityVP->clone();
  startPair = b.startPair->clone();
  //pair = new Pair(b.pair->value, b.pair->position);
  //utilityPair = new Pair(b.utilityPair->value, b.utilityPair->position);
  //startPair = new Pair(b.startPair->value, b.startPair->position);
  calculatedSize = false;
  newBufferStart = b.newBufferStart;
  posIndex = new int[8*sizeof(int)+1];
  resetIterator();
}


Type2Block::Type2BlockIter::~Type2BlockIter() {
  delete[] posIndex;
  delete vp;
  delete utilityVP;
  delete startPair;
}

Block* Type2Block::Type2BlockIter::clone(Block& block_) {
  Block* newBlock = new Type2BlockIter((Type2BlockIter&)block_);
  return newBlock;
}

bool Type2Block::Type2BlockIter::hasNext() {
  return (vp->position<(unsigned int)endpos);
}

bool Type2Block::Type2BlockIter::hasNext(int value_) {
  if (*vp == value_)
    return hasNext();
  else
    return false;
}

ValPos* Type2Block::Type2BlockIter::getNext() {
  int tempx;
  /*  if (keepSame) {
    keepSame=false;
    return pair;
    }*/
  if (peeked) {
    vp->position = utilityVP->position;
    peeked = false;
    return vp;
  }
  if (currPos<0) {
    tempx=*currPosPtr;
    if (tempx == 0) {
      currPos=1;	
      currPosPtr++;
      tempx = *currPosPtr;
    }
    else currPos=0;
    while ((tempx == 0) && (currPos < numvalues)) {
      currPos++;	
      currPosPtr++;
      tempx = *currPosPtr;
    }
    if (!(currPos < numvalues) && ((tempx == 0)))
      return NULL;
    else {
      t2Block->setPosIndex(tempx, posIndex, currIndexInVal);
    }
    vp->position=(currPos*sizeof(int)*8)+adjStartPos+posIndex[currIndexInVal];
    currIndexInVal++;
    if (posIndex[currIndexInVal] == 255) {
      currPos++;	
      currPosPtr++;
      currIndexInVal=0;
    }
    //printBits(pair->value);
    return vp;
  }
  else if (currPos<(numvalues)) {
    if (currIndexInVal) {
      vp->position=(currPos*sizeof(int)*8)+adjStartPos+posIndex[currIndexInVal];
    //pair->value=t2Block->startVal;
      currIndexInVal++;
      if (posIndex[currIndexInVal] == 255) {
	currPos++;	
	currPosPtr++;
	currIndexInVal=0;
      }
    }
    else {
      tempx=*currPosPtr;
      while ((tempx == 0) && (currPos < numvalues)) {
	currPos++;	
	currPosPtr++;
	tempx = *currPosPtr;
      }
      if (!(currPos < numvalues) && ((tempx == 0)))
	return NULL;
      else {
	t2Block->setPosIndex(tempx, posIndex, currIndexInVal);
      }
      vp->position=(currPos*sizeof(int)*8)+adjStartPos+posIndex[currIndexInVal];
      currIndexInVal++;
      if (posIndex[currIndexInVal] == 255) {
	currPos++;	
	currPosPtr++;
	currIndexInVal=0;
      }
    }
    //printBits(pair->value);
    return vp;
  }
  else
    return NULL;
}

ValPos* Type2Block::Type2BlockIter::peekNext() {
  int tempx;
  if (peeked) {
    return utilityVP;
  }
  peeked = true;
  //lastPos = pair->position;
  /*  if (keepSame) {
    keepSame=false;
    return pair;
    }*/
  if (currPos<0) {
    tempx=*currPosPtr;
    if (tempx == 0) {
      currPos=1;	
      currPosPtr++;
      tempx = *currPosPtr;
    }
    else currPos=0;
    while ((tempx == 0) && (currPos < numvalues)) {
      currPos++;	
      currPosPtr++;
      tempx = *currPosPtr;
    }
    if (!(currPos < numvalues) && ((tempx == 0)))
      return NULL;
    else {
      t2Block->setPosIndex(tempx, posIndex, currIndexInVal);
    }
    utilityVP->position=(currPos*sizeof(int)*8)+adjStartPos+posIndex[currIndexInVal];
    currIndexInVal++;
    if (posIndex[currIndexInVal] == 255) {
      currPos++;	
      currPosPtr++;
      currIndexInVal=0;
    }
    //printBits(pair->value);
    return utilityVP;
  }
  else if (currPos<(numvalues)) {
    if (currIndexInVal) {
      utilityVP->position=(currPos*sizeof(int)*8)+adjStartPos+posIndex[currIndexInVal];
      currIndexInVal++;
      if (posIndex[currIndexInVal] == 255) {
	currPos++;	
	currPosPtr++;
	currIndexInVal=0;
      }
    }
    else {
      tempx=*currPosPtr;
      while ((tempx == 0) && (currPos < numvalues)) {
	currPos++;	
	currPosPtr++;
	tempx = *currPosPtr;
      }
      if (!(currPos < numvalues) && ((tempx == 0)))
	return NULL;
      else {
	t2Block->setPosIndex(tempx, posIndex, currIndexInVal);
      }
      utilityVP->position=(currPos*sizeof(int)*8)+adjStartPos+posIndex[currIndexInVal];
      currIndexInVal++;
      if (posIndex[currIndexInVal] == 255) {
	currPos++;	
	currPosPtr++;
	currIndexInVal=0;
      }
    }
    //printBits(pair->value);
    return utilityVP;
  }
  else
    return NULL;
}
void Type2Block::Type2BlockIter::resetIterator() { 
  currPos=-1;
  /*currPosPtr=(int*)t2Block->buffer;
  currPosPtr++;
  int diff = (adjStartPos - t2Block->startPos);
  currPosPtr+=(diff/(8*sizeof(int)));
  */
  currPosPtr=(int*)newBufferStart;
  currIndexInVal=-1;
  peeked=false;
  ValPos* x = peekNext();
  while ((x) && (x->position < startpos)) {
    getNext();
    x = peekNext();
  }
  if (!x) {
    delete vp;
    vp = NULL;
    delete utilityVP;
    utilityVP = NULL;
    delete startPair;
    startPair = NULL;
    endpos=0;
  }
  else {
    vp->position=0;//utilityPair->position;
    startPair->position = utilityVP->position;
  }
  GPALlastLoc = -1;
  GPALlastIndex = 0;
}


int Type2Block::Type2BlockIter::getCurrLoc() {
  throw new UnimplementedException("This will be really expensive to calculate ... do I really need to ever do this?");

}

bool Type2Block::Type2BlockIter::skipToLoc(unsigned int loc_) {
  throw new UnimplementedException("This will be really expensive to calculate ... do I really need to ever do this?");
}

bool Type2Block::Type2BlockIter::skipToPos(unsigned int loc_) {
  assert (loc_ <= endpos);
  currPosPtr = (int*)newBufferStart;
  currPosPtr+=((loc_-adjStartPos)/(sizeof(int)*8));
  currPos=((loc_-adjStartPos)/(sizeof(int)*8));
  currIndexInVal=0;
  while(!(peekNext()->position >= loc_))
    getNext();
  return true;
}

Block* Type2Block::Type2BlockIter::getBlock() {
  return t2Block;
} 

// Zero indexed, gets the pair at this loc
ValPos* Type2Block::Type2BlockIter::getPairAtLoc(unsigned int loc_) {
  assert((int)loc_ < getSize());
  int entrysize = (sizeof(int)*8);
  int nv = numvalues;
  int count, i;
  unsigned int* x = (unsigned int*)newBufferStart;
  if (((int)loc_ > GPALlastLoc) && (GPALlastIndex>0)) {
    count = GPALlastLoc;
    i = GPALlastIndex;
  }
  else {
    int startind = startpos-adjStartPos;
    assert(startind < entrysize);
    //assert(endind < entrysize);
    unsigned int temp1 = 0;
    for (int j = startind; j < entrysize; j++)
      temp1 = temp1 | (1 << (entrysize-j-1));
    count = -1;
    i = 1;
    count+=(t2Block->precomputed16_bitcount(x[0] & temp1));
  }
  while (count < (int)loc_) {
    count+=t2Block->precomputed16_bitcount(x[i]);
    i++;
  }
  assert(i<nv);
  i--;
  count-=t2Block->precomputed16_bitcount(x[i]);
  if ((int)loc_ > GPALlastLoc) {
    GPALlastLoc = count;
    GPALlastIndex = i;
  }
  assert(count < (int)loc_);
  int j = 0;
  while (j < entrysize) {
    if ((1<<(entrysize-j-1)) & x[i]) {
      count++;
      if (count==(int)loc_) {
	utilityVP->position=(i*sizeof(int)*8)+adjStartPos+j;
	return utilityVP;
      }
    }
    j++;
  }
  assert(false);
}
	  
//Like getPairAtLoc except set the regular pair variable rather than 
//the utilityPair variable so can be called by getNext(). 
ValPos* Type2Block::Type2BlockIter::getPairAtLocNotUtility(unsigned int loc_) {
  throw new UnimplementedException("This will be really expensive to calculate ... do I really need to ever do this?");
}

// return size of block	(number of values)
int Type2Block::Type2BlockIter::getSize() {
  if (!calculatedSize) {
    /*int* x =(int*)t2Block->buffer;
    x++;
    int diff = (adjStartPos - t2Block->startPos);
    x+=(diff/(8*sizeof(int)));*/
    int adjEndPos = endpos;
    if (endpos % 32)
      adjEndPos = endpos - ((endpos % 32) - 1);
    else
      adjEndPos = endpos - 31;
    numOnBits = t2Block->howManyOnBits(newBufferStart, numvalues, startpos-adjStartPos, endpos-adjEndPos);
  }
  return numOnBits;
}
	
ValPos* Type2Block::Type2BlockIter::getStartPair() {
  return startPair;
}
	
unsigned int Type2Block::Type2BlockIter::getEndPosition() {
  return endpos;
}

int Type2Block::Type2BlockIter::initBitStringIterator(unsigned int pos) {
  
  //assert (((pos-1) % 32) == 0);
  assert (pos >= startpos);
  assert (pos <= endpos);
  if (pos % 32)
    pos -= ((pos % 32) - 1);
  else {
    pos-=31;
  }
  bitStringInitialized = true;
  bitStringPtr =(int*)newBufferStart;
  //bitStringPtr++;
  int temppos = ((pos - adjStartPos) / (8*sizeof(int)));
  bitStringPtr+=temppos;
  
  return pos;
}

int Type2Block::Type2BlockIter::getNextBitString() {
  int temp = *bitStringPtr;
  bitStringPtr++;
  return temp;
}

// Stream properties
bool Type2Block::Type2BlockIter::isValueSorted() {
  return t2Block->isValueSorted();
}

bool Type2Block::Type2BlockIter::isPosSorted() {
  return t2Block->isPosSorted();
}
	  
// Block properties
bool Type2Block::Type2BlockIter::isOneValue() {
  return t2Block->isOneValue();
}

bool Type2Block::Type2BlockIter::isPosContiguous() {
  return t2Block->isPosContiguous();
}

bool Type2Block::Type2BlockIter::isBlockValueSorted() {
  return t2Block->isBlockValueSorted();
}

bool Type2Block::Type2BlockIter::isBlockPosSorted() {
  return t2Block->isBlockPosSorted();
}
