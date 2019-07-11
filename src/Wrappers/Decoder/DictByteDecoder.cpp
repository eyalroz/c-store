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

#include "DictByteDecoder.h"

DictByteDecoder::DictByteDecoder(AM* dictTableAM_, bool valSorted_) : Decoder() {
	dictTableAM = dictTableAM_, 
	generatedTable=false;
	//assume int type for now ... fix later
	outBlock=new MultiBlock(valSorted_, true, true, ValPos::INTTYPE);
	bufferSize=65005;
	outBuffer = new int[bufferSize];
	//byteDecoder=new BitDecoder(valSorted_);
	numValsPtr=NULL;
	startPosPtr=NULL;
	fieldSizePtr=NULL;
	bufferSet=false;
	reader=NULL;
	
}


DictByteDecoder::DictByteDecoder(byte* buffer_, AM* dictTableAM_, bool valSorted_) : Decoder(buffer_) { 
	dictTableAM = dictTableAM_, 
	generatedTable=false;
	setBuffer(buffer_);
	//assume int type for now ... fix later
	outBlock=new MultiBlock(valSorted_, true, true, ValPos::INTTYPE);
	bufferSize=65005;
	outBuffer = new int[bufferSize];
	//bitDecoder=new BitDecoder(valSorted_);
	numValsPtr=NULL;
	startPosPtr=NULL;
	fieldSizePtr=NULL;
	reader=NULL;
}

DictByteDecoder::DictByteDecoder(DictByteDecoder& decoder_) : Decoder(decoder_) { 
	generatedTable=false;
	reader=NULL;
	dictTableAM = decoder_.dictTableAM, 
	numValsPtr=decoder_.numValsPtr;
	startPosPtr=decoder_.startPosPtr;
	fieldSizePtr=decoder_.fieldSizePtr;
	outBlock=decoder_.outBlock;
	setBuffer(decoder_.buffer);
	bufferSize=65005;
	outBuffer = new int[bufferSize];
	//bitDecoder=new BitDecoder(outBlock->isValueSorted());
}
DictByteDecoder::~DictByteDecoder() {
	delete outBlock;	
	delete[] outBuffer;
}
	
void DictByteDecoder::setBuffer(byte* buffer_){ 
  Decoder::setBuffer(buffer_);
  if (buffer==NULL) throw new CodingException("DictByteDecoder: Error, buffer passed in is NULL");
  fieldSizePtr=(unsigned int*) buffer_;
  if (32 % (*fieldSizePtr) != 0)
    throw new UnexpectedException("expecting fieldsize to evenly divide into 32");
  if (!generatedTable) {
    generatedTable=true;
    generateDictTable();
  }
  //fieldSizeBytes=(*fieldSizePtr)/8;
  numValsPtr=(unsigned int*) (buffer_+2*sizeof(int));
  startPosPtr=(unsigned int*) (buffer_+sizeof(int));
  //Log::writeToLog("DictByteDecoder", 1, "setBuffer(): buffer has numBytes=", *numBytesPtr);
  //Log::writeToLog("DictByteDecoder", 1, "setBuffer(): buffer has startPos=", *startPosPtr);
  currLoc=0;

  numvalsperint = (32/(*fieldSizePtr));
  int temp = (((*numValsPtr)%numvalsperint) == 0)?0:1;
  if (reader==NULL) {
    reader=new ByteReader(buffer_+3*sizeof(int), (((*numValsPtr)/numvalsperint) + temp)*8*sizeof(int));
  }
  else {
    reader->setBuffer(buffer_+3*sizeof(int), (((*numValsPtr)/numvalsperint) + temp)*8*sizeof(int));
  }
  //bitDecoder->setBuffer(buffer_);
  bufferSet=true;
  //valsInBuffer=false;
  //initDecoder();
  
  currindexinentry = 0;
  currentry = NULL;
}
/*
void DictByteDecoder::initDecoder() {
	readValues();
	numValues=0;
	if (maxValsInBuffer!=0) {
		startInt=values[0];
		//Log::writeToLog("DictByteDecoder", 1, "initDecoder(): called, startInt=", startInt);
	}
	//else
	//Log::writeToLog("DictByteDecoder", 1, "initDecoder(): called however src is dry");
	
	while (maxValsInBuffer!=0) {
		endInt=values[maxValsInBuffer-1];
		numValues+=maxValsInBuffer;	
		readValues();
	}
	//Log::writeToLog("DictByteDecoder", 1, "initDecoder(): called, endInt=", endInt);
	//Log::writeToLog("DictByteDecoder", 1, "initDecoder(): called, numValues=", numValues);
	
	// done
	reader->resetPos();
	currLoc=0;
	
}*/
int DictByteDecoder::getValueSize(){ 
	return 8*sizeof(int);
}
int DictByteDecoder::getPageLengthBits() { 
  int temp = (((*numValsPtr)%numvalsperint) == 0)?0:1;
  //Log::writeToLog("DictByteDecoder", 0, "getPageLength returning leng=", 8*(2*sizeof(int)+((unsigned int) *numBytesPtr)));
	return 8*(3*sizeof(int)+(((*numValsPtr)/numvalsperint) + temp)*sizeof(int));
}

bool DictByteDecoder::hasNextBlock() { 
  if (!bufferSet)
    return false;
  else return (currLoc < *numValsPtr);//bitDecoder->hasNextBlock();
}

/*bool DictByteDecoder::getNextValue(int* value) { 
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
  //Log::writeToLog("DictByteDecoder", 0, "getNext returning pair value", value);
  return true;
  }*/

Block* DictByteDecoder::getNextBlock() {
  if (!hasNextBlock()) return NULL;
  int startPos = *startPosPtr+currLoc;
  unsigned int vali;
  //unsigned short vals;
  //byte valb;
  //reader->readByte(valb);
  //*outBuffer=entryValueMap[valb];
  unsigned int i;

  /*  if (fieldSizeBytes==1) {
    for (i = 0; i < *numValsPtr; i++) {
      if (!reader->readByte(valb)) {
	//i++;
	break;
      }
      else
	*(outBuffer+i)=entryValueMap[(int)valb];
    }
  }
  else if (fieldSizeBytes==2) {
    for (i = 0; i <  *numValsPtr; i++) {
      if (!reader->readShort(vals)) {
	//i++;
	break;
      }
      else
	*(outBuffer+i)=entryValueMap[(int)vals];
    }
  }
  else if (fieldSizeBytes==3) {
    for (i = 0; i <  *numValsPtr; i++) {
      if (!reader->readThree(vali)) {
	//i++;
	break;
      }
      else
	*(outBuffer+i)=entryValueMap[vali];
    }
  }
  else if (fieldSizeBytes==4) {
    for (i = 0; i <  *numValsPtr; i++) {
      if (!reader->readInt(vali)) {
	//i++;
	break;
      }
      else
	*(outBuffer+i)=entryValueMap[(int)vali];
    }
  }
  else 
    throw new UnexpectedException("Expecting 1, 2, 3, or 4");
  */
  for (i = 0; i <  (*numValsPtr)/numvalsperint; i++) {
    if (!reader->readInt(vali)) {
      //i++;
      break;
    }
    else {
      for (int j = 0; j < numvalsperint; j++)
	*(outBuffer+i*numvalsperint+j)=entryValueMap[(int)vali][j];
    }
  }
  i*=numvalsperint;
  unsigned int j=0;
  reader->readInt(vali);
  for (; j <  (*numValsPtr) % numvalsperint; j++) {
    *(outBuffer+i+j)=entryValueMap[(int)vali][j];
  }
  //outBuffer-=i;
  currLoc+=(i+j);
  outBlock->setBuffer(startPos, i+j, (byte*)outBuffer);
  return outBlock;
  
}

Block* DictByteDecoder::getNextBlockSingle() { 
  if (!hasNextBlock()) return NULL;
  int startPos = *startPosPtr+currLoc;
  unsigned int vali;
  //unsigned short vals;
  //byte valb;
  //unsigned int newVal;
  /*
  if (fieldSizeBytes==1) {
    if (!reader->readByte(valb)) 
      return NULL;
    else
      newVal=entryValueMap[(int)valb];
  }
  else if (fieldSizeBytes==2) {
    if (!reader->readShort(vals)) 
      return NULL;
    else
      newVal=entryValueMap[(int)vals];
  }
  else if (fieldSizeBytes==3) {
    if (!reader->readThree(vali)) 
      return NULL;
    else
      newVal=entryValueMap[vali];
  }
  else if (fieldSizeBytes==4) {
    if (!reader->readInt(vali)) 
      return NULL;
    else
      newVal=entryValueMap[vali];
  }
  */
  if (currindexinentry == 0) {
    if (!reader->readInt(vali)) 
      return NULL;
    else 
      currentry=entryValueMap[vali];
  }
  currLoc++;
  outBlock->setBuffer(startPos, 1, (byte*)&currentry[currindexinentry]);
  currindexinentry++;
  if (currindexinentry == (32/(*fieldSizePtr)))
    currindexinentry=0;
  return outBlock;
}

/*
void DictByteDecoder::readValues() {
	valuesCurrIndex=0;
	maxValsInBuffer=0;
	byte bitVector;	
	if (reader->readByte(bitVector)) {
	  //Log::writeToLog("DictByteDecoder", 0, "readValues: bitVector is: ", bitVector);
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

/*bool DictByteDecoder::readValue(unsigned int& value_, byte numBits_) {
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
		throw new UnexpectedException("DictByteDecoder: error, numBits>3 in getValue()");
	}
	return returnFlag;
	}*/

/*bool DictByteDecoder::skipADelta() {
	byte bitVector;
	unsigned int oldPos=reader->getCurrentBytePos();

	if (currLoc+4<numValues) {
		if (reader->readByte(bitVector)) {
			if (reader->skipToBytePos(oldPos+1+table[bitVector][4])) {
				currLoc+=table[bitVector][4];
				return true;
			}
			else {
				throw new CodingException("DictByteDecoder::skipADelta(): error could not skip when numValues indicated we could");
			}
		}
		else {
			throw new CodingException("DictByteDecoder::skipADelta(): error could not skip when numValues indicated we could");
		}
	}
	else {
		return false;
	}
	}*/

Block* DictByteDecoder::skipToBlock(unsigned int blockPos_) { 
  /*
    if (blockPos_>=*numValsPtr) return NULL;
	reader->resetPos();
	currLoc=0;
	
	while (currLoc+4<blockPos_) {
		if (!skipADelta())
			throw new CodingException("DictByteDecoder::skipToBlock: skipADelta failed, it shoudln't be");
	}
	Block* returnBlock;
	while (currLoc!=blockPos_) {
		returnBlock=getNextBlock();
	}
	return returnBlock;
  */
  if (blockPos_>=*numValsPtr) return NULL;
  reader->resetPos();
  if (!reader->skipToBytePos(blockPos_/numvalsperint))
    return NULL;
  currLoc=blockPos_;
  if (blockPos_%numvalsperint == 0)
    return getNextBlockSingle();
  else {
    getNextBlockSingle();
    currindexinentry += ((blockPos_%numvalsperint)-1);
    return getNextBlockSingle();
  }
}

bool DictByteDecoder::skipToPos(unsigned int blockPos_) { 
  /*
  if (blockPos_>=numValues) return false;
  reader->resetPos();
  currLoc=0;
  
  while (currLoc+4<blockPos_) {
    if (!skipADelta())
      throw new CodingException("DictByteDecoder::skipToBlock: skipADelta failed, it shoudln't be");
  }
  Block* returnBlock;
  while (currLoc!=blockPos_) {
    returnBlock=getNextBlock();
  }
  return true;*/
  if (blockPos_>=*numValsPtr) return false;
  reader->resetPos();
  bool retval = reader->skipToBytePos(blockPos_/numvalsperint);
  currLoc=blockPos_;
  if (blockPos_%numvalsperint == 0)
    return retval;
  else {
    getNextBlockSingle();
    currindexinentry += ((blockPos_%numvalsperint)-1);
    return retval;
  }
}
unsigned int DictByteDecoder::getSize() {
	return *numValsPtr;	
}
ValPos* DictByteDecoder::getStartVal() { 
  throw new UnimplementedException("");
  //return startInt;
}
unsigned int DictByteDecoder::getStartPos() { 
	return *startPosPtr;
}
ValPos* DictByteDecoder::getEndVal() { 
  throw new UnimplementedException("");
  //return endInt;
}
unsigned int DictByteDecoder::getEndPos() { 
	return *startPosPtr+*numValsPtr-1;
}

void DictByteDecoder::generateDictTable() {
  IntDataSource* ds = new IntDataSource(dictTableAM, false, true);
  ds->setSingles(true);
  BasicBlock* block=(BasicBlock*) ds->getNextValBlock(0);
  int lastpos = ds->getLastPosition();
  cout << "lastpos: " << lastpos << endl;
  entryValueMap = new unsigned int*[lastpos];
  for (int i = 0; i < lastpos; i++){
    entryValueMap[i] = new unsigned int[32 / (*fieldSizePtr)];
  }
  unsigned int curpos = 0;
  while (block!=NULL) {
    for (unsigned int i = 0; i < 32 / (*fieldSizePtr); i++) {
      ValPos* vp = block->getStartPair();
      //assume int type for now ... fix later
      assert(vp->type == ValPos::INTTYPE);
      unsigned int value=*(int*)vp->value;
      curpos = (vp->position)-1;
      curpos/=(32 / (*fieldSizePtr));
      //cout << "pos is: " << curpos << endl;
      entryValueMap[curpos][i]=value;
      Log::writeToLog("DictDataSource", 1, "initTable(): got Entry", curpos);					
      Log::writeToLog("DictDataSource", 1, "initTable(): for value", value);
      
      block=(BasicBlock*) ds->getNextValBlock(0);
      //to fix strange bug where dictionary table page gets read in twice from the ROSAM (should probably fix for real at some point)
      if (vp->position == 1)
	block=NULL;
      if (block == NULL && i != ((32 / (*fieldSizePtr)) - 1))
	throw new UnexpectedException("something must have gone wrong in the encoding process");
    }
    //curpos++;
  }
}
