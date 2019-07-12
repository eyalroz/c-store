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
// Decodes NullSuprression pages
// Problems: migfer@mit.edu

#include "NSDecoder.h"

NSDecoder::NSDecoder(bool valSorted_) : Decoder() {
	reader=NULL;
	generateNSTable();
	//assume int type for now ... fix later if need be
	outBlock=new MultiBlock(valSorted_, true, true, ValPos::INTTYPE);
	bufferSize=16000;
	outBuffer = new int[bufferSize];
}


NSDecoder::NSDecoder(byte* buffer_, bool valSorted_) : Decoder(buffer_) { 
  //assume int type for now ... fix later if need be
	outBlock=new MultiBlock(valSorted_, true, true, ValPos::INTTYPE);
	reader=NULL;
	generateNSTable();
	setBuffer(buffer_);
	bufferSize=16000;
	outBuffer = new int[bufferSize];
}

NSDecoder::NSDecoder(NSDecoder& decoder_) : Decoder(decoder_) { 
	generateNSTable();
	reader=decoder_.reader;
	numBytesPtr=decoder_.numBytesPtr;
	startPosPtr=decoder_.startPosPtr;
	outBlock=decoder_.outBlock;
	setBuffer(decoder_.buffer);
	bufferSize=16000;
	outBuffer = new int[bufferSize];
}
NSDecoder::~NSDecoder() {
	delete outBlock;	
}
	
void NSDecoder::setBuffer(byte* buffer_){ 
	Decoder::setBuffer(buffer_);
	if (buffer==NULL) throw new CodingException("NSDecoder: Error, buffer passed in is NULL");
	numBytesPtr=(unsigned int*) buffer_;
	startPosPtr=(unsigned int*) (buffer_+sizeof(int));
	//Log::writeToLog("NSDecoder", 1, "setBuffer(): buffer has numBytes=", *numBytesPtr);
	//Log::writeToLog("NSDecoder", 1, "setBuffer(): buffer has startPos=", *startPosPtr);
	currLoc=0;
	if (reader==NULL) {
		reader=new ByteReader(buffer_+2*sizeof(int), *numBytesPtr*8);
	}
	else {
		reader->setBuffer(buffer_+2*sizeof(int), *numBytesPtr*8);
	}
	
	valsInBuffer=false;
	initDecoder();
}
void NSDecoder::initDecoder() {
	readValues();
	numValues=0;
	if (maxValsInBuffer!=0) {
		startInt=values[0];
		//Log::writeToLog("NSDecoder", 1, "initDecoder(): called, startInt=", startInt);
	}
	//else
	//Log::writeToLog("NSDecoder", 1, "initDecoder(): called however src is dry");
	
	while (maxValsInBuffer!=0) {
		endInt=values[maxValsInBuffer-1];
		numValues+=maxValsInBuffer;	
		readValues();
	}
	//Log::writeToLog("NSDecoder", 1, "initDecoder(): called, endInt=", endInt);
	//Log::writeToLog("NSDecoder", 1, "initDecoder(): called, numValues=", numValues);
	
	// done
	reader->resetPos();
	currLoc=0;
	
}
int NSDecoder::getValueSize(){ 
	return 8*sizeof(int);
}
int NSDecoder::getPageLengthBits() { 
  //Log::writeToLog("NSDecoder", 0, "getPageLength returning leng=", 8*(2*sizeof(int)+((unsigned int) *numBytesPtr)));
	return 8*(2*sizeof(int)+((unsigned int) *numBytesPtr));
}

bool NSDecoder::hasNextBlock() { 
	if (!initialized) return false;
	//Log::writeToLog("NSDecoder", 0, "hasNextBlock returning", (int) (currLoc<numValues));
	return (currLoc<numValues);
}

bool NSDecoder::getNextValue(int* value) { 
  if (!valsInBuffer) {
    readValues();	
    if (maxValsInBuffer==0)
      return false;
    valsInBuffer=true;
  }

  *value=values[valuesCurrIndex];		
  currLoc++; 
  valuesCurrIndex++;
  if (maxValsInBuffer==valuesCurrIndex) {
    valsInBuffer=false;
    valuesCurrIndex=0;
  }
  //Log::writeToLog("NSDecoder", 0, "getNext returning pair value", value);
  return true;
}
Block* NSDecoder::getNextBlock() { 
  if (!hasNextBlock()) return NULL;
  int startPos = *startPosPtr+currLoc;
  int i;
  for (i = 0; i < bufferSize; i++) {
    if (!(getNextValue(outBuffer+i))) {
      //i++;
      break;
    }
  }
  //outBuffer-=i;
  outBlock->setBuffer(startPos, i, (byte*)outBuffer);
  return outBlock;
  
}
Block* NSDecoder::getNextBlockSingle() { 
	if (!hasNextBlock()) return NULL;
	if (!valsInBuffer) {
		readValues();	
		if (maxValsInBuffer==0)
			 return NULL;
		valsInBuffer=true;
	}

	int value=values[valuesCurrIndex];		
	outBlock->setBuffer(*startPosPtr+currLoc, 1, (byte*)&value);
	currLoc++; 
	valuesCurrIndex++;
	if (maxValsInBuffer==valuesCurrIndex) {
		valsInBuffer=false;
		valuesCurrIndex=0;
	}
	//Log::writeToLog("NSDecoder", 0, "getNext returning pair value", value);
	return outBlock;
}
void NSDecoder::readValues() {
	valuesCurrIndex=0;
	maxValsInBuffer=0;
	byte bitVector;	
	if (reader->readByte(bitVector)) {
	  //Log::writeToLog("NSDecoder", 0, "readValues: bitVector is: ", bitVector);
		if (readValue(values[0], table[bitVector][0])) {
			if (readValue(values[1], table[bitVector][1])) {
				if (readValue(values[2], table[bitVector][2])) {
					if (readValue(values[3], table[bitVector][3])) {
						maxValsInBuffer=4; // read all values
						return;
					}
					else {
						maxValsInBuffer=3; // read two values only
						return;
					}
				}
				else {
					maxValsInBuffer=2; // read two values only
					return;
				}
			}
			else {
				maxValsInBuffer=1; // read one value only
				return;
			}
		}
		else {
			maxValsInBuffer=0; // read no values
			return; // read nothing
		}
	}	
	else {
		maxValsInBuffer=0; // read no values
		return; 	
	}
}
bool NSDecoder::readValue(unsigned int& value_, byte numBits_) {
	bool returnFlag;
	if (numBits_==0) {
		byte value;
		returnFlag=reader->readByte(value);
		value_=value;
	}
	else if (numBits_==1) {
		unsigned short value;
		returnFlag=reader->readShort(value);
		value_=value;
	}
	else if (numBits_==2) {
		return returnFlag=reader->readThree(value_);
	}
	else if (numBits_==3) {
		return reader->readInt(value_);
	}
	else {
		throw new UnexpectedException("NSDecoder: error, numBits>3 in getValue()");
	}
	return returnFlag;
}

bool NSDecoder::skipADelta() {
	byte bitVector;
	unsigned int oldPos=reader->getCurrentBytePos();

	if (currLoc+4<numValues) {
		if (reader->readByte(bitVector)) {
			if (reader->skipToBytePos(oldPos+1+table[bitVector][4])) {
				currLoc+=table[bitVector][4];
				return true;
			}
			else {
				throw new CodingException("NSDecoder::skipADelta(): error could not skip when numValues indicated we could");
			}
		}
		else {
			throw new CodingException("NSDecoder::skipADelta(): error could not skip when numValues indicated we could");
		}
	}
	else {
		return false;
	}
}

Block* NSDecoder::skipToBlock(unsigned int blockPos_) { 
	if (blockPos_>=numValues) return NULL;
	reader->resetPos();
	currLoc=0;
	
	while (currLoc+4<blockPos_) {
		if (!skipADelta())
			throw new CodingException("NSDecoder::skipToBlock: skipADelta failed, it shoudln't be");
	}
	Block* returnBlock;
	while (currLoc!=blockPos_) {
		returnBlock=getNextBlock();
	}
	return returnBlock;
}

bool NSDecoder::skipToPos(unsigned int blockPos_) { 
  if (blockPos_>=numValues) return false;
  reader->resetPos();
  currLoc=0;
  
  while (currLoc+4<blockPos_) {
    if (!skipADelta())
      throw new CodingException("NSDecoder::skipToBlock: skipADelta failed, it shoudln't be");
  }
  while (currLoc!=blockPos_) {
    getNextBlock();
  }
  return true;
}
unsigned int NSDecoder::getSize() {
	return numValues;	
}
ValPos* NSDecoder::getStartVal() { 
  delete utilityPair;
  utilityPair = new IntValPos(0,startInt);
  return utilityPair;
}
unsigned int NSDecoder::getStartPos() { 
	return *startPosPtr;
}
ValPos* NSDecoder::getEndVal() { 
  delete utilityPair;
  utilityPair = new IntValPos(0,endInt);
  return utilityPair;
}
unsigned int NSDecoder::getEndPos() { 
	return *startPosPtr+numValues-1;
}

void NSDecoder::generateNSTable() {
	for (short i=0; i<256; i++) {
		byte val=(i>>6)&0x03;
		table[i][0]=val;
	}
	for (short i=0; i<256; i++) {
		byte val=(i>>4)&0x03;
		table[i][1]=val;
	}
	for (short i=0; i<256; i++) {
		byte val=(i>>2)&0x03;
		table[i][2]=val;
	}
	for (short i=0; i<256; i++) {
		byte val=(i)&0x03;
		table[i][3]=val;
	}
	for (short i=0; i<256; i++) {
		table[i][4]=(table[i][1]+1)+(table[i][1]+1)+(table[i][2]+1)+(table[i][3]+1);
	}
}
