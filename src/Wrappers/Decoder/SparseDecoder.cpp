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

#include "SparseDecoder.h"

SparseDecoder::SparseDecoder(bool valSorted_, unsigned short type_, int fieldSize_) : Decoder() {
  if (type_ == ValPos::STRINGTYPE) {
    outPair = new StringValPos(0,fieldSize_);
    outBlock=new SparseBlock(valSorted_, true, true, outPair);
    delete outPair;
  }
  else
    outBlock=new SparseBlock(valSorted_, true, true, type_);
  outPosBlock=new PosArrayBlock(PAGE_SIZE/sizeof(int));
  outPair=outBlock->vp;
  fieldSize = fieldSize_;
  reader.setStringSize(fieldSize_);
}
SparseDecoder::SparseDecoder(byte* buffer_, bool valSorted_, unsigned short type_, int fieldSize_) : Decoder(buffer_) { 
  if (type_ == ValPos::STRINGTYPE) {
    outPair = new StringValPos(0,fieldSize_);
    outBlock=new SparseBlock(valSorted_, true, true, outPair);
    delete outPair;
  }
  else
    outBlock=new SparseBlock(valSorted_, true, true, type_);
  outPosBlock=new PosArrayBlock(PAGE_SIZE/sizeof(int));
  setBuffer(buffer_);
  outPair=outBlock->vp;
  fieldSize = fieldSize_;
  reader.setStringSize(fieldSize_);
}
SparseDecoder::SparseDecoder(SparseDecoder& decoder_) : Decoder(decoder_) { 
  reader=decoder_.reader;
  numValsPtr=decoder_.numValsPtr;
  startPosPtr=decoder_.startPosPtr;
  endPosPtr=decoder_.endPosPtr;
  fieldSize = decoder_.fieldSize;
  outBlock=new SparseBlock(*decoder_.outBlock);
  outPosBlock=new PosArrayBlock(*decoder_.outPosBlock);
  outPair=outBlock->vp;
  setBuffer(decoder_.buffer);
} 

SparseDecoder::~SparseDecoder()
{
  delete outBlock;
  delete outPosBlock;
}
	
void SparseDecoder::setBuffer(byte* buffer_) { 
  Decoder::setBuffer(buffer_);
  if (buffer==NULL) throw new CodingException("IntDecoder: Error, buffer passed in is NULL");
  numValsPtr=(unsigned int*) buffer_;
  startPosPtr=(unsigned int*) (buffer_+sizeof(int));
  endPosPtr=(unsigned int*) (buffer_+2*sizeof(int));
  currPos=*startPosPtr;
  Log::writeToLog("IntDecoder", 1, "setBuffer(): buffer has numInts=", *numValsPtr);
  Log::writeToLog("IntDecoder", 1, "setBuffer(): buffer has startPos=", *startPosPtr);
  
  reader.setBuffer(buffer_+3*sizeof(int), getPageLengthBits()-8*3*sizeof(int));
}

int SparseDecoder::getValueSize(){ 
  return 8*fieldSize;
}
	
int SparseDecoder::getPageLengthBits() { 
  return (8*3*sizeof(int)) +
    (8*((unsigned int) *numValsPtr)*(fieldSize+sizeof(int)));
}

bool SparseDecoder::hasNextBlock() { 
	if (!initialized) return false;
	//return (currPos-*startPosPtr<*numIntsPtr);
	return (currPos<=*endPosPtr);
}

Block* SparseDecoder::getNextBlock() {
  if (!initialized) return NULL; 
  //int value;
  int numVals;
  byte* buffer;
  //int* buffer2;
  //int* valbuffer;
  //int* posbuffer;
  if (!(buffer=reader.getBuffer(numVals))) return NULL;
  //assert(numInts%2==0);
  //buffer2 = (int*)buffer;
  //valbuffer = new int[numInts/2];
  //posbuffer = new int[numInts/2];
  //int index = 0;
  //for (int i = 0; i < numInts; i+=2) {
  //  valbuffer[index] = buffer2[i];
  //  posbuffer[index] = buffer2[i+1];
  //  index++;
  //}
  /*	outPair->value=value;
	outPair->position=currPos;
	outBlock->currPos=-1;*/
  //	outBlock->setValue(value, currPos);
  outBlock->setBuffer(currPos, *endPosPtr, *numValsPtr-(currPos-*startPosPtr), buffer);
  outPosBlock->setBuffer((int*)(buffer+(fieldSize*(*numValsPtr-(currPos-*startPosPtr)))), *numValsPtr-(currPos-*startPosPtr));
  outBlock->setPosBlock(outPosBlock);
  currPos=(*endPosPtr)+1;
  // Log::writeToLog("SparseDecoder", 0, "getNext returning pair value", value);
  return outBlock;
	
}

Block* SparseDecoder::getNextBlockSingle() {
  throw new UnimplementedException("Not done yet.");
  /*
  if (!initialized) return NULL; 
  int* value = new int[2];
  if (!(reader.readInt(value[0]))) return NULL;
  assert(reader.readInt(value[1]));
  //outPair->value=value;
  //outPair->position=currPos;
  //outBlock->currPos=-1;
  outBlock->setBuffer(value[1], 1, (byte*)value);
  //	outBlock->setValue(value, currPos);
  currPos++;
  delete[] value;
  // Log::writeToLog("SparseDecoder", 0, "getNext returning pair value", value);*/
  return outBlock;
	
}
Block* SparseDecoder::peekNextBlock() {
  throw new UnimplementedException("Not done yet.");
  /*
  if (!initialized) return NULL; 
  int* value = new int[2];
  int oldPos=reader.getCurrentIntPos();
  if (!reader.readInt(value[0])) return NULL;
  assert(reader.readInt(value[1]));
  reader.skipToIntPos(oldPos);
  outPair->value=value;
  outPair->position=currPos;
  outBlock->currPos=-1;
  */
  return outBlock;
}
bool SparseDecoder::skipToPos(unsigned int blockPos_) { 
  if (blockPos_> (*endPosPtr - *startPosPtr)) return false;//NULL;
  //could probably do binary search but I won't bother for now (unlikely to be performance bottleneck)
  
  assert(initialized); 
  unsigned int value;
  unsigned int currpos = *startPosPtr;
  //int oldPos;
  bool x;
  reader.skipToStringPos(*numValsPtr);
  int counter = 0;
  while (currpos < (*startPosPtr + blockPos_)) {
    //oldPos=reader.getCurrentIntPos();
    x = reader.readString((char*)&value,sizeof(int));
    assert(x);
    assert(value>=*startPosPtr && value <= *endPosPtr);
    currpos = value;
    counter++;
  }
  x = reader.skipToStringPos(counter-1);
  assert(x);
  return true;
}

bool SparseDecoder::skipToBlockOnValue(int value_) {
  throw new UnimplementedException("Not done yet."); 
  /*  Log::writeToLog("SparseDecoder", 0, "skipToBlockOnValue() called, val", value_);
  // currently scan down, however can do binary search when sorted in future
  reader.resetPos();
  int value;
  while (reader.readInt(value)) {
    currPos++;
    //int value=((MultiBlock*) peekNextBlock())->getValue();
    if (value!=value_) {
      if (outBlock->isValueSorted()) {
	if (value>value_) {
	  Log::writeToLog("SparseDecoder", 0, "Did not find value, returning false, val", value_);
	  return false;
	}
	//getNextBlock();
      }
      //else {
      //getNextBlock();
      //}
    }
    else {
      Log::writeToLog("SparseDecoder", 0, "skipToBlockOnValue() found value", value_);
      skipToPos(currPos-*startPosPtr -1);
      return true;
    }
  }
  Log::writeToLog("SparseDecoder", 0, "Did not find value, returning false, val", value_);*/
  return false;
}

	
ValPos* SparseDecoder::getStartVal() { 
  throw new UnimplementedException("Not done yet."); 
}
unsigned int SparseDecoder::getStartPos() { 
	return *startPosPtr;
}
ValPos* SparseDecoder::getEndVal() {
  throw new UnimplementedException("Not done yet."); 
}
unsigned int SparseDecoder::getEndPos() { 
	return (*endPosPtr);
}
