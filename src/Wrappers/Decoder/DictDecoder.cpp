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
// Decodes Dictionary Encoded pages

#include "DictDecoder.h"

DictDecoder::DictDecoder(AM* dictTableAM_, bool valSorted_) : Decoder() {
	dictTableAM = dictTableAM_, 
	generateDictTable();
	//assume int type for now ... fix later
	outBlock=new MultiBlock(valSorted_, true, true, ValPos::INTTYPE);
	bufferSize=16000;
	outBuffer = new int[bufferSize];
	bitDecoder=new BitDecoder(valSorted_);
	numBytesPtr=NULL;
	startPosPtr=NULL;
	bufferSet=false;
}


DictDecoder::DictDecoder(byte* buffer_, AM* dictTableAM_, bool valSorted_) : Decoder(buffer_) { 
	dictTableAM = dictTableAM_, 
	generateDictTable();
	setBuffer(buffer_);
	//assume int type for now ... fix later
	outBlock=new MultiBlock(valSorted_, true, true, ValPos::INTTYPE);
	bufferSize=16000;
	outBuffer = new int[bufferSize];
	bitDecoder=new BitDecoder(valSorted_);
	numBytesPtr=NULL;
	startPosPtr=NULL;
}

DictDecoder::DictDecoder(DictDecoder& decoder_) : Decoder(decoder_) { 
	generateDictTable();
	dictTableAM = decoder_.dictTableAM, 
	numBytesPtr=decoder_.numBytesPtr;
	startPosPtr=decoder_.startPosPtr;
	outBlock=decoder_.outBlock;
	setBuffer(decoder_.buffer);
	bufferSize=16000;
	outBuffer = new int[bufferSize];
	bitDecoder=new BitDecoder(outBlock->isValueSorted());
}
DictDecoder::~DictDecoder() {
	delete outBlock;	
	delete[] outBuffer;
}
	
void DictDecoder::setBuffer(byte* buffer_){ 
  Decoder::setBuffer(buffer_);
  if (buffer==NULL) throw new CodingException("DictDecoder: Error, buffer passed in is NULL");
  numBytesPtr=(unsigned int*) buffer_;
  startPosPtr=(unsigned int*) (buffer_+sizeof(int));
  //Log::writeToLog("DictDecoder", 1, "setBuffer(): buffer has numBytes=", *numBytesPtr);
  //Log::writeToLog("DictDecoder", 1, "setBuffer(): buffer has startPos=", *startPosPtr);
  currLoc=0;
  /*if (reader==NULL) {
    reader=new ByteReader(buffer_+2*sizeof(int), *numBytesPtr*8);
    }
    else {
    reader->setBuffer(buffer_+2*sizeof(int), *numBytesPtr*8);
    }*/
  bitDecoder->setBuffer(buffer_);
  bufferSet=true;
  //valsInBuffer=false;
  //initDecoder();
}
/*
void DictDecoder::initDecoder() {
	readValues();
	numValues=0;
	if (maxValsInBuffer!=0) {
		startInt=values[0];
		//Log::writeToLog("DictDecoder", 1, "initDecoder(): called, startInt=", startInt);
	}
	//else
	//Log::writeToLog("DictDecoder", 1, "initDecoder(): called however src is dry");
	
	while (maxValsInBuffer!=0) {
		endInt=values[maxValsInBuffer-1];
		numValues+=maxValsInBuffer;	
		readValues();
	}
	//Log::writeToLog("DictDecoder", 1, "initDecoder(): called, endInt=", endInt);
	//Log::writeToLog("DictDecoder", 1, "initDecoder(): called, numValues=", numValues);
	
	// done
	reader->resetPos();
	currLoc=0;
	
}*/
int DictDecoder::getValueSize(){ 
	return 8*sizeof(int);
}
int DictDecoder::getPageLengthBits() { 
  //Log::writeToLog("DictDecoder", 0, "getPageLength returning leng=", 8*(2*sizeof(int)+((unsigned int) *numBytesPtr)));
	return 8*(2*sizeof(int)+((unsigned int) *numBytesPtr));
}

bool DictDecoder::hasNextBlock() { 
  if (!bufferSet)
    return false;
  else return bitDecoder->hasNextBlock();
}

/*bool DictDecoder::getNextValue(int* value) { 
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
  //Log::writeToLog("DictDecoder", 0, "getNext returning pair value", value);
  return true;
  }*/

Block* DictDecoder::getNextBlock() {
  if (!bufferSet)
    return NULL;
  BasicBlock* currBlock=(BasicBlock*) bitDecoder->getNextBlockSingle();
  if (currBlock == NULL)
    return NULL;
  ValPos* vp = currBlock->getStartPair();
  //assume int type for now ... fix later
  assert(vp->type == ValPos::INTTYPE);
  int startPos = vp->position;
  *outBuffer=entryValueMap[*(int*)vp->value];
  int i;
  for (i = 1; i < bufferSize; i++) {
    if (!(currBlock = (BasicBlock*) bitDecoder->getNextBlockSingle())) {
      //i++;
      break;
    }
    else {
      *(outBuffer+i)=entryValueMap[*(int*)(currBlock->getStartPair()->value)];
    }
  }
  //outBuffer-=i;
  outBlock->setBuffer(startPos, i, (byte*)outBuffer);
  return outBlock;
  
}

Block* DictDecoder::getNextBlockSingle() { 
  if (!bufferSet)
    return NULL;
  BasicBlock* currBlock=(BasicBlock*) bitDecoder->getNextBlockSingle();
  if (currBlock == NULL)
    return NULL;
  ValPos* vp = currBlock->getStartPair();
  //assume int type for now ... fix later
  assert(vp->type == ValPos::INTTYPE);
  unsigned int newVal=entryValueMap[*(int*)vp->value];
  outBlock->setBuffer(vp->position, 1, (byte*)&newVal);
  return outBlock;
}

/*
void DictDecoder::readValues() {
	valuesCurrIndex=0;
	maxValsInBuffer=0;
	byte bitVector;	
	if (reader->readByte(bitVector)) {
	  //Log::writeToLog("DictDecoder", 0, "readValues: bitVector is: ", bitVector);
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
*/

/*bool DictDecoder::readValue(unsigned int& value_, byte numBits_) {
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
		throw new UnexpectedException("DictDecoder: error, numBits>3 in getValue()");
	}
	return returnFlag;
	}*/

/*bool DictDecoder::skipADelta() {
	byte bitVector;
	unsigned int oldPos=reader->getCurrentBytePos();

	if (currLoc+4<numValues) {
		if (reader->readByte(bitVector)) {
			if (reader->skipToBytePos(oldPos+1+table[bitVector][4])) {
				currLoc+=table[bitVector][4];
				return true;
			}
			else {
				throw new CodingException("DictDecoder::skipADelta(): error could not skip when numValues indicated we could");
			}
		}
		else {
			throw new CodingException("DictDecoder::skipADelta(): error could not skip when numValues indicated we could");
		}
	}
	else {
		return false;
	}
	}*/

Block* DictDecoder::skipToBlock(unsigned int blockPos_) { 
  /*
    if (blockPos_>=numValues) return NULL;
	reader->resetPos();
	currLoc=0;
	
	while (currLoc+4<blockPos_) {
		if (!skipADelta())
			throw new CodingException("DictDecoder::skipToBlock: skipADelta failed, it shoudln't be");
	}
	Block* returnBlock;
	while (currLoc!=blockPos_) {
		returnBlock=getNextBlock();
	}
	return returnBlock;
  */
  BasicBlock* currBlock = (BasicBlock*) bitDecoder->skipToBlock(blockPos_);
  if (currBlock == NULL)
    return NULL;
  ValPos* vp = currBlock->getStartPair();
  //assume int type for now ... fix later
  assert(vp->type == ValPos::INTTYPE);
  unsigned int newVal=entryValueMap[*(int*)vp->value];
  outBlock->setBuffer(vp->position, 1, (byte*)&newVal);
  return outBlock;
}

bool DictDecoder::skipToPos(unsigned int blockPos_) { 
  /*
  if (blockPos_>=numValues) return false;
  reader->resetPos();
  currLoc=0;
  
  while (currLoc+4<blockPos_) {
    if (!skipADelta())
      throw new CodingException("DictDecoder::skipToBlock: skipADelta failed, it shoudln't be");
  }
  Block* returnBlock;
  while (currLoc!=blockPos_) {
    returnBlock=getNextBlock();
  }
  return true;*/
  return bitDecoder->skipToPos(blockPos_);
}
unsigned int DictDecoder::getSize() {
	return numValues;	
}
ValPos* DictDecoder::getStartVal() { 
  //assume int type for now .. fix later
  delete utilityPair;
  utilityPair = new IntValPos(0,startInt);
  return utilityPair;
}
unsigned int DictDecoder::getStartPos() { 
	return *startPosPtr;
}
ValPos* DictDecoder::getEndVal() { 
  //assume int type for now .. fix later
  delete utilityPair;
  utilityPair = new IntValPos(0,endInt);
  return utilityPair;
}
unsigned int DictDecoder::getEndPos() { 
	return *startPosPtr+numValues-1;
}

void DictDecoder::generateDictTable() {
  IntDataSource* ds = new IntDataSource(dictTableAM, true, true);
  ds->setSingles(true);
  BasicBlock* block=(BasicBlock*) ds->getNextValBlock(0);
  entryValueMap = new unsigned int[ds->getLastPosition()];
  while (block!=NULL) {
    ValPos* vp = block->getStartPair();
    //assume int type for now ... fix later
    assert(vp->type == ValPos::INTTYPE);
    unsigned int value=*(int*)vp->value;
    unsigned int entry=(vp->position)-1;
    entryValueMap[entry]=value;
    Log::writeToLog("DictDataSource", 1, "initTable(): got Entry", entry);					
    Log::writeToLog("DictDataSource", 1, "initTable(): for value", value);					
    block=(BasicBlock*) ds->getNextValBlock(0);
  }
}
