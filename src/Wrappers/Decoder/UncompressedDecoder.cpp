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

#include "UncompressedDecoder.h"

UncompressedDecoder::UncompressedDecoder(bool valSorted_, unsigned short type_) : Decoder() {
  type = type_;
  outBlock=NULL; //for now
  outPair=NULL;
  valSorted = valSorted_;
}
UncompressedDecoder::UncompressedDecoder(byte* buffer_, bool valSorted_, unsigned short type_) : Decoder(buffer_) { 
  type = type_;
  outBlock=NULL; //for now
  outPair=NULL;
  valSorted = valSorted_;
  setBuffer(buffer_);
}
UncompressedDecoder::UncompressedDecoder(UncompressedDecoder& decoder_) : Decoder(decoder_) { 
  type = decoder_.type;
  reader=decoder_.reader;
  numValsPtr=decoder_.numValsPtr;
  startPosPtr=decoder_.startPosPtr;
  valSorted = decoder_.valSorted;
  outBlock=new MultiBlock(*decoder_.outBlock);
  outPair=outBlock->vp;
} 

UncompressedDecoder::~UncompressedDecoder()
{
  delete outBlock;
}
	
void UncompressedDecoder::setBuffer(byte* buffer_) { 
  Decoder::setBuffer(buffer_);
  if (buffer==NULL) throw new CodingException("UncompressedDecoder: Error, buffer passed in is NULL");
  numValsPtr=(unsigned int*) buffer_;
  startPosPtr=(unsigned int*) (buffer_+sizeof(int));
  ssizePtr=(unsigned int*) (buffer_+(2*sizeof(int)));
  reader.setStringSize(*ssizePtr);
  currPos=*startPosPtr;
  Log::writeToLog("UncompressedDecoder", 1, "setBuffer(): buffer has numInts=", *numValsPtr);
  Log::writeToLog("UncompressedDecoder", 1, "setBuffer(): buffer has startPos=", *startPosPtr);
  if (!outBlock) {
    if (type != ValPos::STRINGTYPE) {
      outBlock=new MultiBlock(valSorted, true, true, type);
      outPair=outBlock->vp;
    }
    else {
      StringValPos* svp = new StringValPos(0,*(unsigned short*)ssizePtr);
      outBlock=new MultiBlock(valSorted, true, true, svp);
      outPair=outBlock->vp;
      delete svp;
    }
  }
  reader.setBuffer(buffer_+3*sizeof(int), getPageLengthBits()-8*3*sizeof(int));
}

int UncompressedDecoder::getValueSize(){ 
  return 8*(*ssizePtr);
}
int UncompressedDecoder::getPageLengthBits() { 
  return (8*((((unsigned int) *numValsPtr) * (*ssizePtr)) + (3*sizeof(int))));
}

bool UncompressedDecoder::hasNextBlock() { 
  if (!initialized) return false;
  return (currPos-*startPosPtr<*numValsPtr);
}

Block* UncompressedDecoder::getNextBlock() {
  if (!initialized) return NULL; 
  //int value;
  int numVals;
  byte* buffer;
  if (!(buffer=reader.getBuffer(numVals))) return NULL;
  /*	outPair->value=value;
	outPair->position=currPos;
	outBlock->currPos=-1;*/
  //	outBlock->setValue(value, currPos);
  outBlock->setBuffer(currPos, numVals, buffer);
  currPos+=numVals;
  // Log::writeToLog("UncompressedDecoder", 0, "getNext returning pair value", value);
  return outBlock;
	
}

Block* UncompressedDecoder::getNextBlockSingle() {
  if (!initialized) return NULL; 
  char* value =new char[*ssizePtr];
  if (!(reader.readString(value))) return NULL;
  outBlock->setBuffer(currPos, 1, (byte*)&value);
  currPos++;
  return outBlock;
}
Block* UncompressedDecoder::peekNextBlock() {
  throw new UnimplementedException("Not done yet");
}
bool UncompressedDecoder::skipToPos(unsigned int blockPos_) { 
  if (blockPos_>=*numValsPtr) return false;//NULL;
  if (reader.skipToStringPos(blockPos_)) {
    currPos=*startPosPtr+blockPos_;
    return true;
  }
  else {
    return false;//NULL;	
  }
}

bool UncompressedDecoder::skipToBlockOnValue(int value_) {
  throw new UnimplementedException("Not done yet");
}

	
ValPos* UncompressedDecoder::getStartVal() { 
  delete utilityPair;
  utilityPair = outBlock->vp->clone();
  utilityPair->set(*startPosPtr, buffer+3*sizeof(int));
  return utilityPair;
}
unsigned int UncompressedDecoder::getStartPos() { 
	return *startPosPtr;
}
ValPos* UncompressedDecoder::getEndVal() { 
  delete utilityPair;
  utilityPair = outBlock->vp->clone();
  utilityPair->set(*startPosPtr + (*numValsPtr) - 1, buffer+3*sizeof(int)+(((*numValsPtr)-1)*(*ssizePtr)));
  return utilityPair;
}
unsigned int UncompressedDecoder::getEndPos() { 
	return (*startPosPtr)+(*numValsPtr)-1;
}
