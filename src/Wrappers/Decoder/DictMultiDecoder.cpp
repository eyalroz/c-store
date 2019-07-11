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

#include "DictMultiDecoder.h"
#include "../IntDataSource.h"

DictMultiDecoder::DictMultiDecoder(AM* dictTableAM_, bool valSorted_) : Decoder() {
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


DictMultiDecoder::DictMultiDecoder(byte* buffer_, AM* dictTableAM_, bool valSorted_) : Decoder(buffer_) { 
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

DictMultiDecoder::DictMultiDecoder(DictMultiDecoder& decoder_) : Decoder(decoder_) { 
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
DictMultiDecoder::~DictMultiDecoder() {
	delete outBlock;	
	delete[] outBuffer;
}
	
void DictMultiDecoder::setBuffer(byte* buffer_){ 
  Decoder::setBuffer(buffer_);
  if (buffer==NULL) throw new CodingException("DictMultiDecoder: Error, buffer passed in is NULL");

  fieldSizePtr=(unsigned int*) buffer_;
  int fieldSize = *fieldSizePtr;

  if (fieldSize >16 || fieldSize == 0)
    throw new UnexpectedException("fieldSize must be <= 16 bits");
  if (fieldSize <=4)
    fieldBytes = 1;
  else fieldBytes = 2;
  //numEncodedValsInInt = (fieldBytes*8) / fieldSize;
  tableSize=1<<(fieldBytes*8);

  numvalsperint = (fieldBytes*8) / fieldSize;

  if (!generatedTable) {
    generatedTable=true;
    generateDictTable();
  }
  //fieldSizeBytes=(*fieldSizePtr)/8;
  numValsPtr=(unsigned int*) (buffer_+2*sizeof(int));
  startPosPtr=(unsigned int*) (buffer_+sizeof(int));
  //Log::writeToLog("DictMultiDecoder", 1, "setBuffer(): buffer has numBytes=", *numBytesPtr);
  //Log::writeToLog("DictMultiDecoder", 1, "setBuffer(): buffer has startPos=", *startPosPtr);
  currLoc=0;

  int temp = (((*numValsPtr)%numvalsperint) == 0)?0:1;
  if (reader==NULL) {
    reader=new ByteReader(buffer_+3*sizeof(int), (((*numValsPtr)/numvalsperint) + temp)*8*fieldBytes);
  }
  else {
    reader->setBuffer(buffer_+3*sizeof(int), (((*numValsPtr)/numvalsperint) + temp)*8*fieldBytes);
  }
  //bitDecoder->setBuffer(buffer_);
  bufferSet=true;
  //valsInBuffer=false;
  //initDecoder();
  
  currindexinentry = 0;
  currentry = NULL;
}
/*
void DictMultiDecoder::initDecoder() {
	readValues();
	numValues=0;
	if (maxValsInBuffer!=0) {
		startInt=values[0];
		//Log::writeToLog("DictMultiDecoder", 1, "initDecoder(): called, startInt=", startInt);
	}
	//else
	//Log::writeToLog("DictMultiDecoder", 1, "initDecoder(): called however src is dry");
	
	while (maxValsInBuffer!=0) {
		endInt=values[maxValsInBuffer-1];
		numValues+=maxValsInBuffer;	
		readValues();
	}
	//Log::writeToLog("DictMultiDecoder", 1, "initDecoder(): called, endInt=", endInt);
	//Log::writeToLog("DictMultiDecoder", 1, "initDecoder(): called, numValues=", numValues);
	
	// done
	reader->resetPos();
	currLoc=0;
	
}*/
int DictMultiDecoder::getValueSize(){ 
	return 8*sizeof(int);
}
int DictMultiDecoder::getPageLengthBits() { 
  int temp = (((*numValsPtr)%numvalsperint) == 0)?0:1;
  //Log::writeToLog("DictMultiDecoder", 0, "getPageLength returning leng=", 8*(2*sizeof(int)+((unsigned int) *numBytesPtr)));
	return 8*(3*sizeof(int)+(((*numValsPtr)/numvalsperint) + temp)*sizeof(int));
}

bool DictMultiDecoder::hasNextBlock() { 
  if (!bufferSet)
    return false;
  else return (currLoc < *numValsPtr);//bitDecoder->hasNextBlock();
}

/*bool DictMultiDecoder::getNextValue(int* value) { 
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
  //Log::writeToLog("DictMultiDecoder", 0, "getNext returning pair value", value);
  return true;
  }*/

Block* DictMultiDecoder::getNextBlock() {
  if (!hasNextBlock()) return NULL;
  int startPos = *startPosPtr+currLoc;
  //unsigned int vali;
  unsigned short vals;
  byte valb;
  //reader->readByte(valb);
  //*outBuffer=entryValueMap[valb];
  int i;

  if (fieldBytes==1) {
    int currPos = reader->getCurrentBytePos();
    byte* currPosPtr = reader->getCurrentPosPtr();
    int* outBufferPtr = outBuffer;
    int maxi = (*numValsPtr)/numvalsperint;
    //int sizeToCopy=numvalsperint*sizeof(int);
    if (numvalsperint == 2) {
      for (i = 0; i < maxi; i++) {
	valb=currPosPtr[i];
	//currPosPtr++;      
	*(outBufferPtr)=entryValueMap[valb][0];
	*(outBufferPtr+1)=entryValueMap[valb][1];
	//memcpy(outBufferPtr,entryValueMap[valb],sizeToCopy);
	outBufferPtr+=numvalsperint;
      } 
    }
    else if (numvalsperint == 4) {
      for (i = 0; i < maxi; i++) {
	valb=currPosPtr[i];
	//currPosPtr++;      
	*(outBufferPtr)=entryValueMap[valb][0];
	*(outBufferPtr+1)=entryValueMap[valb][1];
	*(outBufferPtr+2)=entryValueMap[valb][2];
	*(outBufferPtr+3)=entryValueMap[valb][3];
	//memcpy(outBufferPtr,entryValueMap[valb],sizeToCopy);
	outBufferPtr+=numvalsperint;
      } 
    }
    else if (numvalsperint ==8) {
      for (i = 0; i < maxi; i++) {
	valb=currPosPtr[i];
	//currPosPtr++;      
	*(outBufferPtr)=entryValueMap[valb][0];
	*(outBufferPtr+1)=entryValueMap[valb][1];
	*(outBufferPtr+2)=entryValueMap[valb][2];
	*(outBufferPtr+3)=entryValueMap[valb][3];
	*(outBufferPtr+4)=entryValueMap[valb][4];
	*(outBufferPtr+5)=entryValueMap[valb][5];
	*(outBufferPtr+6)=entryValueMap[valb][6];
	*(outBufferPtr+7)=entryValueMap[valb][7];
	//memcpy(outBufferPtr,entryValueMap[valb],sizeToCopy);
	outBufferPtr+=numvalsperint;
      } 
    }
    else throw new UnexpectedException("Should never happen");
    reader->setCurrentBytePos(currPos + i);
    reader->setCurrentPosPtr(currPosPtr + i);

    i*=numvalsperint;
    unsigned int j=0;
    reader->readByte(valb);
    //assert(result);
    for (; j <  (*numValsPtr) % numvalsperint; j++) {
      *(outBuffer+i+j)=entryValueMap[valb][j];
    }
    //outBuffer-=i;
    currLoc+=(i+j);
    outBlock->setBuffer(startPos, i+j, (byte*)outBuffer);
  }
  else if (fieldBytes==2) {
    int currPos = reader->getCurrentBytePos();
    unsigned short* currPosPtr = (unsigned short*)reader->getCurrentPosPtr();
    int* outBufferPtr = outBuffer;
    int maxi = (*numValsPtr)/numvalsperint;
    //int sizeToCopy=numvalsperint*sizeof(int);
    if (numvalsperint == 1) {
      for (i = 0; i < maxi; i++) {
	vals=currPosPtr[i];
	//currPosPtr++;      
	*(outBufferPtr)=entryValueMap[vals][0];
	//memcpy(outBufferPtr,entryValueMap[valb],sizeToCopy);
	outBufferPtr+=numvalsperint;
      } 
    }
    else if (numvalsperint == 2) {
      for (i = 0; i < maxi; i++) {
	vals=currPosPtr[i];
	//currPosPtr++;      
	*(outBufferPtr)=entryValueMap[vals][0];
	*(outBufferPtr+1)=entryValueMap[vals][1];
	//memcpy(outBufferPtr,entryValueMap[valb],sizeToCopy);
	outBufferPtr+=numvalsperint;
      } 
    }
    else if (numvalsperint == 3) {
      for (i = 0; i < maxi; i++) {
	vals=currPosPtr[i];
	//currPosPtr++;      
	*(outBufferPtr)=entryValueMap[vals][0];
	*(outBufferPtr+1)=entryValueMap[vals][1];
	*(outBufferPtr+2)=entryValueMap[vals][2];
	//memcpy(outBufferPtr,entryValueMap[valb],sizeToCopy);
	outBufferPtr+=numvalsperint;
      } 
    }
    else throw new UnexpectedException("Should never happen");
    reader->setCurrentBytePos(currPos + i*2);
    reader->setCurrentPosPtr(((byte*)currPosPtr) + i*2);

    i*=numvalsperint;
    unsigned int j=0;
    reader->readShort(vals);
    for (; j <  (*numValsPtr) % numvalsperint; j++) {
      *(outBuffer+i+j)=entryValueMap[vals][j];
    }
    //outBuffer-=i;
    currLoc+=(i+j);
    outBlock->setBuffer(startPos, i+j, (byte*)outBuffer);
  }/*
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
   
  for (i = 0; i <  (*numValsPtr)/numvalsperint; i++) {
    if (!reader->readInt(vali)) {
      //i++;
      break;
    }
    else {
      for (int j = 0; j < numvalsperint; j++)
	*(outBuffer+i*numvalsperint+j)=entryValueMap[(int)vali][j];
    }
  }*/
  return outBlock;
  
}

Block* DictMultiDecoder::getNextBlockSingle() { 
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
void DictMultiDecoder::readValues() {
	valuesCurrIndex=0;
	maxValsInBuffer=0;
	byte bitVector;	
	if (reader->readByte(bitVector)) {
	  //Log::writeToLog("DictMultiDecoder", 0, "readValues: bitVector is: ", bitVector);
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

/*bool DictMultiDecoder::readValue(unsigned int& value_, byte numBits_) {
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
		throw new UnexpectedException("DictMultiDecoder: error, numBits>3 in getValue()");
	}
	return returnFlag;
	}*/

/*bool DictMultiDecoder::skipADelta() {
	byte bitVector;
	unsigned int oldPos=reader->getCurrentBytePos();

	if (currLoc+4<numValues) {
		if (reader->readByte(bitVector)) {
			if (reader->skipToBytePos(oldPos+1+table[bitVector][4])) {
				currLoc+=table[bitVector][4];
				return true;
			}
			else {
				throw new CodingException("DictMultiDecoder::skipADelta(): error could not skip when numValues indicated we could");
			}
		}
		else {
			throw new CodingException("DictMultiDecoder::skipADelta(): error could not skip when numValues indicated we could");
		}
	}
	else {
		return false;
	}
	}*/

Block* DictMultiDecoder::skipToBlock(unsigned int blockPos_) { 
  /*
    if (blockPos_>=*numValsPtr) return NULL;
	reader->resetPos();
	currLoc=0;
	
	while (currLoc+4<blockPos_) {
		if (!skipADelta())
			throw new CodingException("DictMultiDecoder::skipToBlock: skipADelta failed, it shoudln't be");
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

bool DictMultiDecoder::skipToPos(unsigned int blockPos_) { 
  /*
  if (blockPos_>=numValues) return false;
  reader->resetPos();
  currLoc=0;
  
  while (currLoc+4<blockPos_) {
    if (!skipADelta())
      throw new CodingException("DictMultiDecoder::skipToBlock: skipADelta failed, it shoudln't be");
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
unsigned int DictMultiDecoder::getSize() {
	return *numValsPtr;	
}
ValPos* DictMultiDecoder::getStartVal() { 
  throw new UnimplementedException("");
  //return startInt;
}
unsigned int DictMultiDecoder::getStartPos() { 
	return *startPosPtr;
}
ValPos* DictMultiDecoder::getEndVal() { 
  throw new UnimplementedException("");
  //return endInt;
}
unsigned int DictMultiDecoder::getEndPos() { 
	return *startPosPtr+*numValsPtr-1;
}

void DictMultiDecoder::generateDictTable() {
  IntDataSource* ds = new IntDataSource(dictTableAM, false, true);
  ds->setSingles(true);
  int reallastpos = ds->getLastPosition();
  BasicBlock* block=(BasicBlock*) ds->getNextValBlock(0);
  int lastpos = (1<<(*fieldSizePtr));//ds->getLastPosition();
  cout << "lastpos: " << lastpos << endl;
  int realTableSize = 1;
  for (int i = 0; i < numvalsperint; i++)
    realTableSize*=lastpos;

  entryValueMap = new unsigned int*[realTableSize];
  for (int i = 0; i < realTableSize; i++){
    entryValueMap[i] = new unsigned int[numvalsperint];
    for (int j=0; j<numvalsperint; j++)
      entryValueMap[i][j]=0;
  }

  unsigned int* tempDic = new unsigned int[reallastpos];

  unsigned int curpos = 0;
  while (block!=NULL) {
      ValPos* vp = block->getStartPair();
      //assume int type for now ... fix later
      assert(vp->type == ValPos::INTTYPE);
      unsigned int value=*(int*)vp->value;
      curpos = (vp->position)-1;
        //cout << "pos is: " << curpos << endl;
      tempDic[curpos]=value;
      Log::writeToLog("DictDataSource", 1, "initTable(): got Entry", curpos);					
      Log::writeToLog("DictDataSource", 1, "initTable(): for value", value);
      
      block=(BasicBlock*) ds->getNextValBlock(0);
      //to fix strange bug where dictionary table page gets read in twice from the ROSAM (should probably fix for real at some point)
      //if (block->getStartPair()->position == 1)
      //block=NULL;
    //curpos++;
  }

  int dIndex;
  if (numvalsperint==1) {
    for (int z = 0; z < reallastpos; z++) {
      dIndex = setEntry(0,0,z);
      entryValueMap[dIndex][0] = tempDic[z];
    }
  }
  if (numvalsperint==2) {
    for (int z = 0; z < reallastpos; z++) {
      for (int y = 0; y < reallastpos; y++) {
	dIndex = setEntry(0,0,z);
	dIndex = setEntry(dIndex,1,y);
	entryValueMap[dIndex][0] = tempDic[z];
	entryValueMap[dIndex][1] = tempDic[y];
      }
    }
  }
  if (numvalsperint==3) {
    for (int z = 0; z < reallastpos; z++) {
      for (int y = 0; y < reallastpos; y++) {
	for (int x = 0; x < reallastpos; x++) {
	  dIndex = setEntry(0,0,z);
	  dIndex = setEntry(dIndex,1,y);
	  dIndex = setEntry(dIndex,2,x);
	  entryValueMap[dIndex][0] = tempDic[z];
	  entryValueMap[dIndex][1] = tempDic[y];
	  entryValueMap[dIndex][2] = tempDic[x];
	}
      }
    }
  }
  if (numvalsperint==4) {
    for (int z = 0; z < reallastpos; z++) {
      for (int y = 0; y < reallastpos; y++) {
	for (int x = 0; x < reallastpos; x++) {
	  for (int w = 0; w < reallastpos; w++) {
	    dIndex = setEntry(0,0,z);
	    dIndex = setEntry(dIndex,1,y);
	    dIndex = setEntry(dIndex,2,x);
	    dIndex = setEntry(dIndex,3,w);
	    entryValueMap[dIndex][0] = tempDic[z];
	    entryValueMap[dIndex][1] = tempDic[y];
	    entryValueMap[dIndex][2] = tempDic[x];
	    entryValueMap[dIndex][3] = tempDic[w];
	  }
	}
      }
    }
  }
  if (numvalsperint==8) {
    for (int z = 0; z < reallastpos; z++) {
      for (int y = 0; y < reallastpos; y++) {
	for (int x = 0; x < reallastpos; x++) {
	  for (int w = 0; w < reallastpos; w++) {
	    for (int v = 0; v < reallastpos; v++) {
	      for (int u = 0; u < reallastpos; u++) {
		for (int t = 0; t < reallastpos; t++) {
		  for (int s = 0; s < reallastpos; s++) {
		    dIndex = setEntry(0,0,z);
		    dIndex = setEntry(dIndex,1,y);
		    dIndex = setEntry(dIndex,2,x);
		    dIndex = setEntry(dIndex,3,w);
		    dIndex = setEntry(dIndex,4,v);
		    dIndex = setEntry(dIndex,5,u);
		    dIndex = setEntry(dIndex,6,t);
		    dIndex = setEntry(dIndex,7,s);
		    entryValueMap[dIndex][0] = tempDic[z];
		    entryValueMap[dIndex][1] = tempDic[y];
		    entryValueMap[dIndex][2] = tempDic[x];
		    entryValueMap[dIndex][3] = tempDic[w];
		    entryValueMap[dIndex][4] = tempDic[v];
		    entryValueMap[dIndex][5] = tempDic[u];
		    entryValueMap[dIndex][6] = tempDic[t];
		    entryValueMap[dIndex][7] = tempDic[s];
		  }
		}
	      }
	    }
	  }
	}
      }
    }
  }
}

unsigned int DictMultiDecoder::setEntry(unsigned int entry, unsigned int index, unsigned int value) {
  unsigned int newentry;
  newentry = entry | (value << (((*fieldSizePtr))*index));
  return newentry;
}
