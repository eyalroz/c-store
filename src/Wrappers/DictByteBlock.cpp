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

#include "DictByteBlock.h"

DictByteBlock::DictByteBlock(bool valSorted_, bool posContig_, bool posSorted_) : MultiBlock(valSorted_, posContig_, posSorted_)
{
  fieldSize = 0;
  fieldBytes = 0;
  currIndex = 0;
  realCurrPosPtr=NULL;
  //entryValueMap = NULL;
}

DictByteBlock::DictByteBlock(const DictByteBlock& block_) : MultiBlock(block_)
{
  fieldSize = block_.fieldSize;
  fieldBytes = block_.fieldBytes;
  //entryValueMap = block_.entryValueMap;
  currIndex = block_.currIndex;
}

Block* DictByteBlock::clone(Block& block_)
{
  Block* newBlock = new DictByteBlock((DictByteBlock&)block_);
  return newBlock;
}

DictByteBlock::~DictByteBlock()
{
  /*if (pair)
    delete pair;
  if (utilityPair)
    delete utilityPair;
  if (buffer)
  delete[] buffer;*/
}

void DictByteBlock::setDictSpecs(int fs, unsigned int fb/*,unsigned int** evm*/) {
  fieldSize =fs;
  fieldBytes=fb;
  numvalsperentry = (fieldBytes*8)/fieldSize;
  //numvalsperint = numvalsperentry * (sizeof(int) / fieldBytes);
  //entryValueMap=evm;
  masks = new byte[numvalsperentry];
  for (int i = 0; i < numvalsperentry; i++)
    masks[i]=0 | (((1<<fieldSize)-1) << (fieldSize*i));
}

void DictByteBlock::resetBlock() {
  currPos=-1;
  currPosPtr=buffer;
  realCurrPosPtr=buffer;
  vp->set(startPos, (byte*)&startVal);
  //pair->value=startVal;//masks[0] & valb;//entryValueMap[valb][0];
  //pair->position=startPos;
  currIndex=0;

}
void DictByteBlock::setBuffer(int startpos_, int numValues_, byte* buffer_) {
  if (!fieldSize)
    throw new UnexpectedException("setDictSpecs should always be called first.");
  //MultiBlock::setBuffer(startpos_,numValues_,buffer_);
  if (fieldBytes == 1) {
    numValues=numValues_*4;
  }
  /*else if (fieldBytes == 2) {
    numValues=numValues_/2;
    }*/
  else throw new UnexpectedException("should never happen");
  numValues=numValues*numvalsperentry;

  currPos=-1;
  startPos=startpos_;
  int sz = numValues;
  if (buffer_size != sz) {
    if (buffer)
      delete[] buffer;
    buffer_size=sz;
    buffer=new byte[sz];
    }
  //memcpy(buffer, buffer_, numValues_*sizeof(int));
  //currPosPtr=(int*)buffer;
  //numValues=numValues_;
  //startVal=(*currPosPtr);
  //pair->value=(*currPosPtr);
  //pair->position=startpos_;
  realCurrPosPtr = buffer;
  byte valb;
  int maxi = (numValues)/numvalsperentry;
  if (numvalsperentry == 2) {
    for (int i = 0; i < maxi; i++) {
      valb=buffer_[i];
      *(buffer)=valb & masks[0];
      *(buffer+1)=(valb & masks[1])>>fieldSize;
      buffer+=2;
    }
  }
  else if (numvalsperentry == 4) {
    for (int i = 0; i < maxi; i++) {
      valb=buffer_[i];
      *(buffer)=valb & masks[0];
      *(buffer+1)=(valb & masks[1])>>fieldSize;
      *(buffer+2)=(valb & masks[2])>>(2*fieldSize);
      *(buffer+3)=(valb & masks[3])>>(3*fieldSize);
      buffer+=4;
    }
  }
  else if (numvalsperentry == 8) {
    for (int i = 0; i < maxi; i++) {
      valb=buffer_[i];
      *(buffer)=valb & masks[0];
      *(buffer+1)=(valb & masks[1])>>fieldSize;
      *(buffer+2)=(valb & masks[2])>>(2*fieldSize);
      *(buffer+3)=(valb & masks[3])>>(3*fieldSize);
      *(buffer+4)=(valb & masks[4])>>(4*fieldSize);
      *(buffer+5)=(valb & masks[5])>>(5*fieldSize);
      *(buffer+6)=(valb & masks[6])>>(6*fieldSize);
      *(buffer+7)=(valb & masks[7])>>(7*fieldSize);
      buffer+=8;
    }
  }
  buffer-=numValues;
  startVal=realCurrPosPtr[0];//masks[0] & valb;//entryValueMap[valb][0];
  //cout << startVal << endl;
  vp->set(startpos_, (byte*)&startVal);
  //pair->value=startVal;
  //pair->position=startpos_;
  currIndex=0;

}

//void DictByteBlock::setValue(Pair* val_) {
//	currPos=-1;
//	pair->set(*val_);
//}

	
// Iterator access to block
bool DictByteBlock::hasNext() {
  return (currPos<(numValues));
}
bool DictByteBlock::hasNext(int value_) {
  //  return ((currPos<(numValues)) && ((((*realCurrPosPtr) & masks[currIndex]) >>(currIndex*fieldSize)) == value_));
  return ((currPos<(numValues)) && ((*realCurrPosPtr) == value_));
}

ValPos* DictByteBlock::getNext() {
  if (currPos<0) {
    currPos=1;	
    //currIndex = (currIndex+1)%numvalsperentry;
    //if (numvalsperentry == 1)
      realCurrPosPtr++;
    return vp;
  }
  else if (currPos<(numValues)) {
    vp->set(currPos+startPos, realCurrPosPtr);
    //pair->value=*realCurrPosPtr;//((*realCurrPosPtr) & masks[currIndex])>>(currIndex*fieldSize);
    realCurrPosPtr++;//=currIndex;
    //currIndex = (currIndex+1)%numvalsperentry;
    //    if (!currIndex)
    //pair->position=currPos+startPos;
    currPos++;
    return vp;
  }
  else
    return NULL;
}

ValPos* DictByteBlock::peekNext() {
  if (currPos<0) {
    utilityVP->set(startPos, (byte*)&startVal);
    //utilityPair->value=pair->value;
    //utilityPair->position=pair->position;
    return utilityVP;
  }
  else if (hasNext()) {
    //utilityPair->value=((*realCurrPosPtr) & masks[currIndex])>>(currIndex*fieldSize);
    utilityVP->set(currPos+startPos, realCurrPosPtr);
    //utilityPair->value=(*realCurrPosPtr);
    //utilityPair->position=currPos+startPos;
    return utilityVP;
  }
  else
    return NULL;
}

// Zero indexed, gets the pair at this pos_ 
ValPos* DictByteBlock::getPairAtLoc(unsigned int loc_) {
  assert(loc_<(unsigned int)numValues);
  //utilityPair->value=((*(buffer+(loc_/numvalsperentry)))&masks[loc_%numvalsperentry])>>((loc_%numvalsperentry)*fieldSize);
  utilityVP->set(loc_+startPos,buffer+loc_); 
  //utilityPair->value=(*(buffer+loc_));
  //utilityPair->position=loc_+startPos;
  return utilityVP;
}

//Like getPairAtLoc except set the regular pair variable rather than 
//the utilityPair variable so can be called by getNext().
ValPos* DictByteBlock::getPairAtLocNotUtility(unsigned int loc_) {
  assert(loc_<(unsigned int)numValues);
  //pair->value=((*(buffer+(loc_/numvalsperentry)))&masks[loc_%numvalsperentry])>>((loc_%numvalsperentry)*fieldSize);
  vp->set(loc_+startPos,buffer+loc_); 
  //pair->value=(*(buffer+loc_));
  //pair->position=loc_+startPos;
  return vp;
}

/*void DictByteBlock::cutOffEnd(int endPos) {
  if (endPos < startPos+numValues)
    numValues=(endPos-startPos)+1;
}

int DictByteBlock::getCurrLoc() {
  if (currPos == -1)
    return 0;
  else
    return currPos-1;
    }

int DictByteBlock::getSizeInBits() {
  //doubt this is right but who cares since we don't actually use I don't think
  return (2+numValues)*sizeof(int);	
  }*/
ValPos* DictByteBlock::getStartPair() {
  if (buffer==NULL) return NULL;
  else return getPairAtLoc(0);
}
	
