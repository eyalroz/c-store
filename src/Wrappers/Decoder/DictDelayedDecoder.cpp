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

#include "DictDelayedDecoder.h"

DictDelayedDecoder::DictDelayedDecoder(bool valSorted_) : IntDecoder(valSorted_) {
  db1 = new DictByteBlock(valSorted_,true,true);
  db2 = new DictShortBlock(valSorted_,true,true);
  bb = new BasicBlock(valSorted_,true,true);
  counter = 0;
  extra=0;
}

DictDelayedDecoder::DictDelayedDecoder(byte* buffer_, bool valSorted_) : IntDecoder(buffer_, valSorted_) {
  throw UnimplementedException("Not sure if I need the line below or not yet");
  setBuffer(buffer_);
}

DictDelayedDecoder::DictDelayedDecoder(DictDelayedDecoder& decoder_) : IntDecoder(decoder_) { 
	fieldSizePtr=decoder_.fieldSizePtr;
} 

DictDelayedDecoder::~DictDelayedDecoder()
{

}
	
void DictDelayedDecoder::setBuffer(byte* buffer_) { 
  //cout << "I was called." << endl;
  Decoder::setBuffer(buffer_);
  if (buffer==NULL) throw new CodingException("DictDelayedDecoder: Error, buffer passed in is NULL");
  fieldSizePtr=(unsigned int*) buffer_;
  int fieldSize = *fieldSizePtr;

  if (fieldSize >16 || fieldSize == 0)
    throw new UnexpectedException("fieldSize must be <= 16 bits");
  if (fieldSize <=4)
    fieldBytes = 1;
  else fieldBytes = 2;
  numvalsperint = ((fieldBytes*8) / fieldSize) * (sizeof(int) / fieldBytes);
  startPosPtr=(unsigned int*) (buffer_+sizeof(int));
  numIntsPtr=(unsigned int*) (buffer_+(2*sizeof(int)));
  int temp = (((*numIntsPtr)%numvalsperint) == 0)?0:1;
  realsize = (((*numIntsPtr)/numvalsperint) + temp);
  numIntsPtr=&realsize;
  currPos=*startPosPtr;
  Log::writeToLog("DictDelayedDecoder", 1, "setBuffer(): buffer has numInts=", *numIntsPtr);
  Log::writeToLog("DictDelayedDecoder", 1, "setBuffer(): buffer has startPos=", *startPosPtr);
  
  reader.setBuffer(buffer_+3*sizeof(int), getPageLengthBits()-8*3*sizeof(int));
  if (fieldBytes==1) {
    db1->setDictSpecs(fieldSize, fieldBytes);
  }
  else
    db2->setDictSpecs(fieldSize, fieldBytes);
}

int DictDelayedDecoder::getPageLengthBits() { 
	Log::writeToLog("IntDecoder", 0, "getPageLength returning leng=", 8*(2+((unsigned int) *numIntsPtr))*sizeof(int));
	return 8*(3+((unsigned int) *numIntsPtr))*sizeof(int);
}



Block* DictDelayedDecoder::getNextBlock() {
  if (!initialized) return NULL; 
  //int value;
  int numInts;
  byte* buffer;
  if (!(buffer=reader.getBuffer(numInts))) return NULL;
  if (fieldBytes==1) {
    db1->setBuffer(currPos, numInts, buffer);
    currPos+=numInts;
    while (extra!=0) {
      db1->getNext();
      extra--;
    }
    return db1;
  }
  else {
    db2->setBuffer(currPos, numInts, buffer);
    currPos+=numInts;
    while (extra!=0) {
      db2->getNext();
      extra--;
    }
    return db2;
  }
  // Log::writeToLog("IntDecoder", 0, "getNext returning pair value", value);
	
}

Block* DictDelayedDecoder::getNextBlockSingle() {
  if (!initialized) return NULL; 
  //int value;
  int numInts;
  byte* buffer;
  if (counter == 0) {
    if (!(buffer=reader.getBuffer(numInts))) return NULL;
    if (fieldBytes==1) {
      db1->setBuffer(currPos, numInts, buffer);
      currPos+=numInts;
      counter = db1->getSize()-1;
      bb->setValue(db1->getNext());
    }
    else {
      db2->setBuffer(currPos, numInts, buffer);
      currPos+=numInts;
      counter = db2->getSize()-1;
      bb->setValue(db2->getNext());
    }
  }
  else {
    if (fieldBytes==1) {
      counter--;
      bb->setValue(db1->getNext());
    }
    else {
      counter--;
      bb->setValue(db2->getNext());
    }
  }
  return bb;
  
}
Block* DictDelayedDecoder::peekNextBlock() {
  if (!initialized) return NULL; 
  int value;
  int oldPos=reader.getCurrentIntPos();
  if (!reader.readInt(value)) return NULL;
  reader.skipToIntPos(oldPos);
  //outPair->value=value;
  //outPair->position=currPos;
  outPair->set(currPos, (byte*)&value);
  if (fieldBytes==1) {
    db1->currPos=-1;
    return db1;
  }
  else {
    db2->currPos=-1;
    return db2;
  }
	
}

bool DictDelayedDecoder::skipToPos(unsigned int blockPos_) {
  extra=blockPos_%numvalsperint;
  blockPos_/=numvalsperint;
  if (blockPos_>=*numIntsPtr) return false;//NULL;
  if (reader.skipToIntPos(blockPos_)) {
    currPos=*startPosPtr+(blockPos_*numvalsperint);
    return true;
  }
  else {
    return false;//NULL;	
    }
}
