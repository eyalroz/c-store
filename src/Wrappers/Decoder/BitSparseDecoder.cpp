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

#include "BitSparseDecoder.h"

BitSparseDecoder::BitSparseDecoder(bool valSorted_, unsigned short type_, int fieldSize_) : Decoder() {
  if (type_ == ValPos::STRINGTYPE) {
    outPair = new StringValPos(0,fieldSize_);
    outBlock=new SparseBlock(valSorted_, true, true, outPair);
    delete outPair;
  }
  else
    outBlock=new SparseBlock(valSorted_, true, true, type_);
  outPosBlock=new PosType2Block();
  posBlockBaseBlock = new Type2Block(valSorted_);
  outPosBlock->setBlock(posBlockBaseBlock);
  outPair=outBlock->vp;
  fieldSize = fieldSize_;
  reader.setStringSize(fieldSize_);
  t2header=NULL;
  currIter=NULL;
}
BitSparseDecoder::BitSparseDecoder(byte* buffer_, bool valSorted_, unsigned short type_, int fieldSize_) : Decoder(buffer_) { 
  if (type_ == ValPos::STRINGTYPE) {
    outPair = new StringValPos(0,fieldSize_);
    outBlock=new SparseBlock(valSorted_, true, true, outPair);
    delete outPair;
  }
  else
    outBlock=new SparseBlock(valSorted_, true, true, type_);
  outPosBlock=new PosType2Block();
  posBlockBaseBlock = new Type2Block(valSorted_);
  outPosBlock->setBlock(posBlockBaseBlock);
  currIter=NULL;
  fieldSize = fieldSize_;
  reader.setStringSize(fieldSize_);
  setBuffer(buffer_);
  outPair=outBlock->vp;
}
BitSparseDecoder::BitSparseDecoder(BitSparseDecoder& decoder_) : Decoder(decoder_) { 
  reader=decoder_.reader;
  numValsPtr=decoder_.numValsPtr;
  startPosPtr=decoder_.startPosPtr;
  endPosPtr=decoder_.endPosPtr;
  fieldSize = decoder_.fieldSize;
  outBlock=new SparseBlock(*decoder_.outBlock);
  outPosBlock=new PosType2Block(*decoder_.outPosBlock);
  posBlockBaseBlock = new Type2Block(*decoder_.posBlockBaseBlock);
  outPosBlock->setBlock(posBlockBaseBlock);
  outPair=outBlock->vp;
  currIter=NULL;
  setBuffer(decoder_.buffer);
} 

BitSparseDecoder::~BitSparseDecoder()
{
  delete outBlock;
  delete outPosBlock;
  delete posBlockBaseBlock;
  delete currIter;
}
	
void BitSparseDecoder::setBuffer(byte* buffer_) { 
  Decoder::setBuffer(buffer_);
  if (buffer==NULL) throw new CodingException("IntDecoder: Error, buffer passed in is NULL");
  numValsPtr=(unsigned int*) buffer_;
  startPosPtr=(unsigned int*) (buffer_+sizeof(int));
  endPosPtr=(unsigned int*) (buffer_+2*sizeof(int));
  currPos=*startPosPtr;
  Log::writeToLog("IntDecoder", 1, "setBuffer(): buffer has numVals=", *numValsPtr);
  Log::writeToLog("IntDecoder", 1, "setBuffer(): buffer has startPos=", *startPosPtr);
  t2header = ((Type2Header*) (buffer_ + ((3*sizeof(int)) + ((*numValsPtr) * fieldSize))));
  
  posBlockBaseBlock->setBufferWithHeader(buffer_ + ((3*sizeof(int)) + ((*numValsPtr) * fieldSize)));
  delete currIter;
  currIter = outPosBlock->getIterator();
  reader.setBuffer(buffer_+3*sizeof(int), 8*fieldSize*(*numValsPtr));
}

int BitSparseDecoder::getValueSize(){ 
  return 8*fieldSize;
}
	
int BitSparseDecoder::getPageLengthBits() { 
  //Log::writeToLog("BitSparseDecoder", 0, "getPageLength returning leng=", 8*(2+((unsigned int) *numIntsPtr))*sizeof(int));
  return 8*(
	    3*sizeof(int) +
	    (*numValsPtr)*fieldSize +
	    sizeof(Type2Header) +
	    (t2header->numValues)*sizeof(int));
}

bool BitSparseDecoder::hasNextBlock() { 
  if (!initialized) return false;
  //return (currPos-*startPosPtr<*numIntsPtr);
  return (currPos<=*endPosPtr); //might need to fix later if getNextBlockSingle is implemented
}

Block* BitSparseDecoder::getNextBlock() {
  if (!initialized) return NULL; 
  //int value;
  int numVals;
  byte* buffer;
  //int* buffer2;
  //int* valbuffer;
  //int* posbuffer;
  if (!(buffer=reader.getBuffer(numVals))) return NULL;
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
  outBlock->setBuffer(currPos, *endPosPtr, numVals, buffer);
  //outPosBlock->setBuffer(buffer2+(*numIntsPtr), (numInts-*numIntsPtr));
  outBlock->setPosBlock(currIter);
  currPos=(*endPosPtr)+1;
  // Log::writeToLog("BitSparseDecoder", 0, "getNext returning pair value", value);
  return outBlock;
	
}

Block* BitSparseDecoder::getNextBlockSingle() {
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
  // Log::writeToLog("BitSparseDecoder", 0, "getNext returning pair value", value);*/
  return outBlock;
	
}
Block* BitSparseDecoder::peekNextBlock() {
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
bool BitSparseDecoder::skipToPos(unsigned int blockPos_) { 
  if (blockPos_> (*endPosPtr - *startPosPtr)) return false;//NULL;
  //could probably do binary search but I won't bother for now (unlikely to be performance bottleneck)
  
  assert(initialized); 
  bool x;
  int size = outPosBlock->getSize();
  delete currIter;
  currIter = outPosBlock->getIterator(*startPosPtr + blockPos_, *endPosPtr);
  x = reader.skipToStringPos(size-currIter->getSize());
  assert(x);
  return true;
}

bool BitSparseDecoder::skipToBlockOnValue(int value_) {
  throw new UnimplementedException("Not done yet."); 
  /*  Log::writeToLog("BitSparseDecoder", 0, "skipToBlockOnValue() called, val", value_);
  // currently scan down, however can do binary search when sorted in future
  reader.resetPos();
  int value;
  while (reader.readInt(value)) {
    currPos++;
    //int value=((MultiBlock*) peekNextBlock())->getValue();
    if (value!=value_) {
      if (outBlock->isValueSorted()) {
	if (value>value_) {
	  Log::writeToLog("BitSparseDecoder", 0, "Did not find value, returning false, val", value_);
	  return false;
	}
	//getNextBlock();
      }
      //else {
      //getNextBlock();
      //}
    }
    else {
      Log::writeToLog("BitSparseDecoder", 0, "skipToBlockOnValue() found value", value_);
      skipToPos(currPos-*startPosPtr -1);
      return true;
    }
  }
  Log::writeToLog("BitSparseDecoder", 0, "Did not find value, returning false, val", value_);
  return false;*/
}

	
ValPos* BitSparseDecoder::getStartVal() { 
  throw new UnimplementedException("Not done yet."); 
}
unsigned int BitSparseDecoder::getStartPos() { 
	return *startPosPtr;
}
ValPos* BitSparseDecoder::getEndVal() {
  throw new UnimplementedException("Not done yet."); 
}
unsigned int BitSparseDecoder::getEndPos() { 
	return (*endPosPtr);
}
