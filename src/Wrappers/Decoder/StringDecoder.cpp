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

#include "StringDecoder.h"

StringDecoder::StringDecoder(bool valSorted_) : Decoder() {
	outBlock=new SBasicBlock(valSorted_, true, true);
	outPair=outBlock->pair;
}
StringDecoder::StringDecoder(byte* buffer_, bool valSorted_) : Decoder(buffer_) { 
	outBlock=new SBasicBlock(valSorted_, true, true);
	setBuffer(buffer_);
	outPair=outBlock->pair;
}
StringDecoder::StringDecoder(StringDecoder& decoder_) : Decoder(decoder_) { 
	reader=decoder_.reader;
	numStringsPtr=decoder_.numStringsPtr;
	startPosPtr=decoder_.startPosPtr;
	ssizePtr=decoder_.ssizePtr;
	outBlock=new SBasicBlock(*decoder_.outBlock);
	outPair=outBlock->pair;
} 

StringDecoder::~StringDecoder()
{

}
	
void StringDecoder::setBuffer(byte* buffer_) { 
	Decoder::setBuffer(buffer_);
	if (buffer==NULL) throw new CodingException("StringDecoder: Error, buffer passed in is NULL");
	numStringsPtr=(unsigned int*) buffer_;
	startPosPtr=(unsigned int*) (buffer_+sizeof(int));
	ssizePtr=(unsigned int*) (buffer_+(2*sizeof(int)));
	currPos=*startPosPtr;
	Log::writeToLog("StringDecoder", 1, "setBuffer(): buffer has numInts=", *numStringsPtr);
	Log::writeToLog("StringDecoder", 1, "setBuffer(): buffer has startPos=", *startPosPtr);
	Log::writeToLog("StringDecoder", 1, "setBuffer(): buffer has ssize=", *ssizePtr);

	reader.setStringSize(*ssizePtr);
	reader.setBuffer(buffer_+3*sizeof(int), getPageLengthBits()-8*3*sizeof(int));
}

int StringDecoder::getValueSize(){ 
	return 8*(*ssizePtr);
}
int StringDecoder::getPageLengthBits() { 
	return (8*((((unsigned int) *numStringsPtr) * (*ssizePtr)) + (3*sizeof(int))));
}

bool StringDecoder::hasNextBlock() { 
	if (!initialized) return false;
	return (currPos-*startPosPtr<*numStringsPtr);
}

Block* StringDecoder::getNextBlock() {
  throw new UnimplementedException("Not done yet");
  /*
  if (!initialized) return NULL; 
  //int value;
  int numInts;
  byte* buffer;
  if (!(buffer=reader.getBuffer(numInts))) return NULL;*/
  /*	outPair->value=value;
	outPair->position=currPos;
	outBlock->currPos=-1;*/
  //	outBlock->setValue(value, currPos);
  /*outBlock->setBuffer(currPos, numInts, buffer);
  currPos+=numInts;
  // Log::writeToLog("StringDecoder", 0, "getNext returning pair value", value);
  return outBlock;*/
	
}
Block* StringDecoder::getNextBlockSingle() {
  throw new UnimplementedException("Not done yet");
}

SBlock* StringDecoder::getNextSBlockSingle() {
  if (!initialized) return NULL; 
  char* value =new char[*ssizePtr];
  if (!(reader.readString(value))) return NULL;
  //outPair->value=value;
  //outPair->position=currPos;
  //outBlock->currPos=-1;
  outBlock->setValue(value, currPos);
  //	outBlock->setValue(value, currPos);
  currPos++;
  // Log::writeToLog("StringDecoder", 0, "getNext returning pair value", value);
  return outBlock;
	
}
Block* StringDecoder::peekNextBlock() {
  throw new UnimplementedException("Not done yet"); /*
	if (!initialized) return NULL; 
	int value;
	int oldPos=reader.getCurrentIntPos();
	if (!reader.readInt(value)) return NULL;
	reader.skipToIntPos(oldPos);
	outPair->value=value;
	outPair->position=currPos;
	outBlock->currPos=-1;
	return outBlock;*/
	
}
bool StringDecoder::skipToPos(unsigned int blockPos_) { 
  if (blockPos_>=*numStringsPtr) return false;//NULL;
  if (reader.skipToStringPos(blockPos_)) {
    currPos=*startPosPtr+blockPos_;
    return true;
  }
  else {
    return false;//NULL;	
  }
}

bool StringDecoder::skipToBlockOnValue(int value_) {
  throw new UnimplementedException("Not done yet");
  /*
  Log::writeToLog("StringDecoder", 0, "skipToBlockOnValue() called, val", value_);
  // currently scan down, however can do binary search when sorted in future
  reader.resetPos();
  int value;
  while (reader.readInt(value)) {
    currPos++;
    //int value=((MultiBlock*) peekNextBlock())->getValue();
    if (value!=value_) {
      if (outBlock->isValueSorted()) {
	if (value>value_) {
	  Log::writeToLog("StringDecoder", 0, "Did not find value, returning false, val", value_);
	  return false;
	}
	//getNextBlock();
      }
      //else {
      //getNextBlock();
      //}
    }
    else {
      Log::writeToLog("StringDecoder", 0, "skipToBlockOnValue() found value", value_);
      skipToPos(currPos-*startPosPtr -1);
      return true;
    }
  }
  Log::writeToLog("StringDecoder", 0, "Did not find value, returning false, val", value_);
  return false;*/
}

	
ValPos* StringDecoder::getStartVal() {
  throw new UnimplementedException("Not done yet");
  //return reader.readFirst();
}
unsigned int StringDecoder::getStartPos() { 
	return *startPosPtr;
}
ValPos* StringDecoder::getEndVal() { 
  throw new UnimplementedException("Not done yet");
  //	return reader.readLast();
}
unsigned int StringDecoder::getEndPos() { 
	return (*startPosPtr)+(*numStringsPtr)-1;
}
