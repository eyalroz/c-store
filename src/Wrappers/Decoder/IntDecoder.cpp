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
// Problems: migfer@mit.edu

#include "IntDecoder.h"

IntDecoder::IntDecoder(bool valSorted_) : Decoder() {
	outBlock=new MultiBlock(valSorted_, true, true, ValPos::INTTYPE);
	outPair=outBlock->vp;
}
IntDecoder::IntDecoder(byte* buffer_, bool valSorted_) : Decoder(buffer_) { 
	outBlock=new MultiBlock(valSorted_, true, true, ValPos::INTTYPE);
	setBuffer(buffer_);
	outPair=outBlock->vp;
}
IntDecoder::IntDecoder(IntDecoder& decoder_) : Decoder(decoder_) { 
	reader=decoder_.reader;
	numIntsPtr=decoder_.numIntsPtr;
	startPosPtr=decoder_.startPosPtr;
	outBlock=new MultiBlock(*decoder_.outBlock);
	outPair=outBlock->vp;
} 

IntDecoder::~IntDecoder()
{
  delete outBlock;
}
	
void IntDecoder::setBuffer(byte* buffer_) { 
	Decoder::setBuffer(buffer_);
	if (buffer==NULL) throw new CodingException("IntDecoder: Error, buffer passed in is NULL");
	numIntsPtr=(unsigned int*) buffer_;
	startPosPtr=(unsigned int*) (buffer_+sizeof(int));
	currPos=*startPosPtr;
	Log::writeToLog("IntDecoder", 1, "setBuffer(): buffer has numInts=", *numIntsPtr);
	Log::writeToLog("IntDecoder", 1, "setBuffer(): buffer has startPos=", *startPosPtr);

	reader.setBuffer(buffer_+2*sizeof(int), getPageLengthBits()-8*2*sizeof(int));
}

int IntDecoder::getValueSize(){ 
	return 8*sizeof(int);
}
int IntDecoder::getPageLengthBits() { 
	Log::writeToLog("IntDecoder", 0, "getPageLength returning leng=", 8*(2+((unsigned int) *numIntsPtr))*sizeof(int));
	return 8*(2+((unsigned int) *numIntsPtr))*sizeof(int);
}

bool IntDecoder::hasNextBlock() { 
	if (!initialized) return false;
	return (currPos-*startPosPtr<*numIntsPtr);
}

Block* IntDecoder::getNextBlock() {
  if (!initialized) return NULL; 
  //int value;
  int numInts;
  byte* buffer;
  if (!(buffer=reader.getBuffer(numInts))) return NULL;
  /*	outPair->value=value;
	outPair->position=currPos;
	outBlock->currPos=-1;*/
  //	outBlock->setValue(value, currPos);
  outBlock->setBuffer(currPos, numInts, buffer);
  currPos+=numInts;
  // Log::writeToLog("IntDecoder", 0, "getNext returning pair value", value);
  return outBlock;
	
}

Block* IntDecoder::getNextBlockSingle() {
  if (!initialized) return NULL; 
  int value;
  if (!(reader.readInt(value))) return NULL;
  //outPair->value=value;
  //outPair->position=currPos;
  //outBlock->currPos=-1;
  outBlock->setBuffer(currPos, 1, (byte*)&value);
  //	outBlock->setValue(value, currPos);
  currPos++;
  // Log::writeToLog("IntDecoder", 0, "getNext returning pair value", value);
  return outBlock;
	
}
Block* IntDecoder::peekNextBlock() {
	if (!initialized) return NULL; 
	int value;
	int oldPos=reader.getCurrentIntPos();
	if (!reader.readInt(value)) return NULL;
	reader.skipToIntPos(oldPos);
	outPair->set(currPos, (byte*)&value);
	//outPair->value=value;
	//outPair->position=currPos;
	outBlock->currPos=-1;
	return outBlock;
	
}
bool IntDecoder::skipToPos(unsigned int blockPos_) { 
  if (blockPos_>=*numIntsPtr) return false;//NULL;
  if (reader.skipToIntPos(blockPos_)) {
    currPos=*startPosPtr+blockPos_;
    return true;
  }
  else {
    return false;//NULL;	
  }
}

bool IntDecoder::skipToBlockOnValue(int value_) {
  Log::writeToLog("IntDecoder", 0, "skipToBlockOnValue() called, val", value_);
  // currently scan down, however can do binary search when sorted in future
  reader.resetPos();
  int value;
  while (reader.readInt(value)) {
    currPos++;
    //int value=((MultiBlock*) peekNextBlock())->getValue();
    if (value!=value_) {
      if (outBlock->isValueSorted()) {
	if (value>value_) {
	  Log::writeToLog("IntDecoder", 0, "Did not find value, returning false, val", value_);
	  return false;
	}
	//getNextBlock();
      }
      //else {
      //getNextBlock();
      //}
    }
    else {
      Log::writeToLog("IntDecoder", 0, "skipToBlockOnValue() found value", value_);
      skipToPos(currPos-*startPosPtr -1);
      return true;
    }
  }
  Log::writeToLog("IntDecoder", 0, "Did not find value, returning false, val", value_);
  return false;
}

	
ValPos* IntDecoder::getStartVal() {
  delete utilityPair;
  utilityPair = new IntValPos(0,reader.readFirst());
  return utilityPair;
}
unsigned int IntDecoder::getStartPos() { 
	return *startPosPtr;
}
ValPos* IntDecoder::getEndVal() { 
  delete utilityPair;
  utilityPair = new IntValPos(0,reader.readLast());
  return utilityPair;
}
unsigned int IntDecoder::getEndPos() { 
	return (*startPosPtr)+(*numIntsPtr)-1;
}
