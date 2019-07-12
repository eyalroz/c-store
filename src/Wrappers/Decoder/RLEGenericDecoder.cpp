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
// Decode RLE Pages with RLETriples
#include "RLEGenericDecoder.h"

RLEGenericDecoder::RLEGenericDecoder(bool valSorted_, ValPos* templateValPos) : RLESuperDecoder() {
	valSorted=valSorted_;
	result=new RLETriple();
	reader=NULL;
	block=new RLEBlock(valSorted);
	trip = NULL;
	result->value = templateValPos->clone();
}

RLEGenericDecoder::RLEGenericDecoder(byte* buffer_, bool valSorted_, ValPos* templateValPos) : RLESuperDecoder(buffer_)
{
	valSorted=valSorted_;
	result=new RLETriple();
	reader=NULL;
	block=new RLEBlock(valSorted);
	trip = NULL;
	result->value = templateValPos->clone();
	setBuffer(buffer_);
}
RLEGenericDecoder::~RLEGenericDecoder()
{
  delete reader;
  delete result;
  delete block;
  delete[] trip;
  //delete vp;
}

void RLEGenericDecoder::setBuffer(byte* buffer_) {
  Decoder::setBuffer(buffer_);
  if (buffer==NULL)
    throw new CodingException("RLEGenericDecoder: Input to RLEGenericDecoder constructor is NULL");
  
  data=(buffer+sizeof(short)+sizeof(int));
  valueSize=*((short*) buffer);
  assert(result->value->getSize() == valueSize);
  startPosSize=sizeof(int);
  repsSize=sizeof(int);
  
  //Log::writeToLog("RLEGenericDecoder", 0, "Buffer valueSize=", valueSize);
  //Log::writeToLog("RLEGenericDecoder", 0, "Buffer startPosSize=", startPosSize);
  //Log::writeToLog("RLEGenericDecoder", 0, "Buffer repsSize=", repsSize);
  
  tripleSize=valueSize+startPosSize+repsSize;
  trip = new char[tripleSize];
  int pageLength=(this->getPageLengthBits());
  
  if (reader==NULL) {
    reader=new StringReader();
    reader->setStringSize(tripleSize);
  }
  reader->setBuffer(data, pageLength);
  //Log::writeToLog("RLEGenericDecoder", 0, "Initializing reader with page length", pageLength);
}

bool RLEGenericDecoder::hasNextTriple() {
  return (reader->hasNextString());	
}

RLETriple* RLEGenericDecoder::getNextTriple() {
  // to guarantee atomicity in reading if there is a failure
  /*int oldBitPos=reader->getCurrentBitPosition();
  unsigned int value;
  if (!reader->readBits(value, valueSize)) {
    reader->skipToBitPos(oldBitPos);	
    Log::writeToLog("RLEGenericDecoder", 0, "getNextTriple() Could not read value");
    return NULL;
  }	
  
  if (!reader->readBits(result->startPos, startPosSize)) {
    reader->skipToBitPos(oldBitPos);	
    Log::writeToLog("RLEGenericDecoder", 0, "getNextTriple() Could not read startPos");
    return NULL;
  }
  if (!reader->readBits(result->reps, repsSize)) {
    reader->skipToBitPos(oldBitPos);	
    Log::writeToLog("RLEGenericDecoder", 0, "getNextTriple() Could not read reps");
    return NULL;
    }*/
  bool x;
  if (reader->hasNextString()) {
    x = reader->readString(trip);
    assert(x);
  }
  else return NULL;
  //result->value=(unsigned int) value; // assuming encoding for all positive values
  result->startPos=*(int*)(trip+valueSize);
  result->reps=*(int*)(trip+valueSize+sizeof(int));
  result->value->set(result->startPos, (byte*)trip);
  //result->value = vp;
  return result;
}

RLETriple* RLEGenericDecoder::peekNextTriple() {
  RLETriple* retTrip=getNextTriple();
  if (retTrip) {
    reader->unReadString();
    return retTrip;
  }
  else
    return NULL;
}

bool RLEGenericDecoder::skipToTriple(unsigned int triplePos_) {
  if (triplePos_>=getSize()) return false;
  return reader->skipToStringPos(triplePos_);
}

RLETriple* RLEGenericDecoder::getTriple(unsigned int triplePos_) {	
  if (triplePos_>=getSize()) return NULL;
  // to guarantee atomicity in reading if there is a failure
  int oldPos=reader->getCurrentPos();
  reader->skipToStringPos(triplePos_);
  RLETriple* t = getNextTriple();
  /*unsigned int value;
  if (!reader->readBits(value, valueSize)) {
    reader->skipToBitPos(oldBitPos);	
    Log::writeToLog("RLEGenericDecoder", 0, "getTriple() Could not read value");
    return NULL;
  }	
  
  if (!reader->readBits(result->startPos, startPosSize)) {
    reader->skipToBitPos(oldBitPos);	
    Log::writeToLog("RLEGenericDecoder", 0, "getTriple() Could not read startPos");
    return NULL;
  }
  
  if (!reader->readBits(result->reps, repsSize)) {
    reader->skipToBitPos(oldBitPos);	
    Log::writeToLog("RLEGenericDecoder", 0, "getTriple() Could not read reps");
    return NULL;
    }*/
  
  //result->value=(unsigned int) value; // assuming encoding for all positive values		
  //result->value->set(result->startPos, (byte*)&value);
  //result->value = vp;
  // return to old position for sequential read
  reader->skipToStringPos(oldPos);	
  return t;	
}

bool RLEGenericDecoder::hasNextBlock() {
  if (!initialized) return false;
  return hasNextTriple();
}
Block* RLEGenericDecoder::getNextBlock() {
  if (!initialized) throw new CodingException("RLEGenericDecoder: Error, decoder not initialized with buffer");
  RLETriple* triple=getNextTriple();
  if (triple==NULL) return NULL;
  
  block->setTriple(triple);	
  return block;
}
Block* RLEGenericDecoder::skipToBlock(unsigned int blockPos_) {
  if (!initialized) throw new CodingException("RLEGenericDecoder: Error, decoder not initialized with buffer");
  if (!skipToTriple(blockPos_)) return NULL;
  RLETriple* triple=getNextTriple();
  if (triple==NULL) return NULL;
  block->setTriple(triple);	
  return block;
}

//consume the block wih position blockPos_
bool RLEGenericDecoder::skipToPos(unsigned int blockPos_) {
  throw new CodingException("DNA 7/2/06: I don't think this method is correct but I don't think it matters since this method never gets called in runtime code. Thus, I'll place this here to see if it gets called.");
  if (!initialized) throw new CodingException("RLEGenericDecoder: Error, decoder not initialized with buffer");
  if (!skipToTriple(blockPos_)) return false;
  RLETriple* triple=getNextTriple();
  if (triple==NULL) return false;
  
  return true;
}
bool RLEGenericDecoder::skipToBlockOnValue(ValPos* value_) {
  //Log::writeToLog("RLEGenericDecoder", 0, "skipToBlockOnValue() called, val", value_);
  // currently scan down, however can do binary search when sorted in future
  reader->resetPos();
  while (hasNextTriple()) {
    RLETriple* triple=peekNextTriple();
    if (*(triple->value)!=value_) {
      if (valSorted) {
	if (*(triple->value)>value_) {
	  //Log::writeToLog("RLEGenericDecoder", 0, "Did not find value, returning false, val", value_);
	  return false;
	}
	getNextBlock();
      }
      else {
	getNextBlock();
      }
    }
    else {
      //Log::writeToLog("RLEGenericDecoder", 0, "skipToBlockOnValue() found value", value_);
      return true;
    }
  }
  //Log::writeToLog("RLEGenericDecoder", 0, "Did not find value, returning false, val", value_);
  return false;
}

bool RLEGenericDecoder::skipToBlockOnPos(unsigned int pos) {
  // currently scan down, however can do binary search when sorted in future
  reader->resetPos();
  RLETriple* triple = NULL;
  while (hasNextTriple()) {
    triple=peekNextTriple();
    if ((triple->startPos + triple->reps -1) < pos)
      getNextBlock();
    else
      break;
  }
  if ((triple) && (triple->startPos<=pos) && (pos<=(triple->startPos+triple->reps-1))) 
    return true;
  else
    return false;
}


Block* RLEGenericDecoder::getBlock(unsigned int blockPos_) {
  if (!initialized) throw new CodingException("RLEGenericDecoder: Error, decoder not initialized with buffer");
  RLETriple* triple=getTriple(blockPos_);
  if (triple==NULL) return NULL;  
  block->setTriple(triple);	
  return block;
}
	
ValPos* RLEGenericDecoder::getStartVal() {
  throw new CodingException("RLEGenericDecoder: Do later");
  /*	if (!initialized) throw new CodingException("RLEGenericDecoder: Error, decoder not initialized with buffer");
	int oldBitPos=reader->getCurrentBitPosition();
	unsigned int startInt; 
	reader->resetPos();
	if (!reader->readBits(startInt, valueSize)) {
		// throw exception
		throw new CodingException("RLEGenericDecoder: Cannot read Start Int");
	}
	
	reader->skipToBitPos(oldBitPos);	
	Log::writeToLog("RLEGenericDecoder", 0, "Start int=", startInt);
	return ((int) startInt); // assuming encoding for all positive values		*/
}
unsigned int RLEGenericDecoder::getStartPos() {
  return *(int*)(data+valueSize);
}

ValPos* RLEGenericDecoder::getEndVal() {
  throw new CodingException("RLEGenericDecoder: Do later");

  /*	if (!initialized) throw new CodingException("RLEGenericDecoder: Error, decoder not initialized with buffer");
	int oldBitPos=reader->getCurrentBitPosition();
	unsigned int endInt;
	int endIntPos=reader->getNumBitsInBuffer()-tripleSize;	
	reader->skipToBitPos(endIntPos);	
	
	if (!reader->readBits(endInt, valueSize)) {
		// throw exception
		throw new CodingException("RLEGenericDecoder: Cannot read End Int");
	}	
	reader->skipToBitPos(oldBitPos);	
	return ((int) endInt);	// assuming encoding for all positive values		*/
}
unsigned int RLEGenericDecoder::getEndPos() {
  int endPos = *(int*)(data+tripleSize*getSize()-2*sizeof(int));
  int endReps = *(int*)(data+tripleSize*getSize()-sizeof(int));
  return (endPos+endReps-1);		
}
int RLEGenericDecoder::getPageLengthBits() {
	if (!initialized) throw new CodingException("RLEGenericDecoder: Error, decoder not initialized with buffer");	
	return *((int*) (buffer+sizeof(short)));
}
unsigned int RLEGenericDecoder::getSize() {
	if (!initialized) throw new CodingException("RLEGenericDecoder: Error, decoder not initialized with buffer");
	return (getPageLengthBits()-8*(sizeof(short)+sizeof(int)))/(tripleSize*8);
}
