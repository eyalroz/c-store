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
// Writes Dict Blocks
// Problems: dna@mit.edu


#include "DictByteWriter.h"

/*This verstion of the Dictionary Writer works by:
1) assumes that it gets fields of size 2,4,8,16, or 32
2) Encodes multiple values in one 32 bit int
*/
DictByteWriter::DictByteWriter(Operator* blockSrc_, int colIndex_, unsigned int bfrSizeBits_, unsigned int fieldSize_) : Writer(blockSrc_, colIndex_) 
{
	fieldSize=fieldSize_;
	if (32 % fieldSize != 0 || fieldSize == 1)
	  throw new UnexpectedException("fieldSize must be a mod of 32 bits");
	numEncodedValsInInt = 32 / fieldSize;
	tableSize=1<<(fieldSize+numEncodedValsInInt);
	escapeInt=0xFFFFFFFF>>(32-fieldSize-numEncodedValsInInt);
	numDistinctValues=0;
	blockWriter=new BasicBlockWriter(blockSrc_, colIndex_);
	currBlock=(BasicBlock*) blockWriter->getNextValBlock(0);

	if (currBlock!=NULL) {
		outBlock=new BitBlock(currBlock->isValueSorted());
		byteWriter=new ByteWriter(bfrSizeBits_);	
		dictBlock=new BasicBlock(true, true, true);
		buffer=byteWriter->getBuffer();
		startPosPtr=(unsigned int*) (buffer+sizeof(int));
		numValsPtr=(unsigned int*) (buffer+2*sizeof(int));
	}
	else {
		outBlock=NULL;
		byteWriter=NULL;
		buffer=NULL;
		startPosPtr=NULL;
		numValsPtr=NULL;
	}
	init=true;
	readyToOutDict=false;	
	bfrSizeBits=bfrSizeBits_;
	currval=NULL;//new byte[64];
	//memset(currVal, 0, 64);
	currindex=0;
	curpos=1;
	Log::writeToLog("DictByteWriter", 0, "Finished constructor");
}

DictByteWriter::~DictByteWriter()
{
	if (outBlock!=NULL) delete outBlock;
	if (dictBlock!=NULL) delete dictBlock;
	if (blockWriter!=NULL) delete blockWriter;
	if (byteWriter!=NULL) delete byteWriter;
	if (currval !=NULL) delete [] currval;
}

void DictByteWriter::resetWriter() {
  //Log::writeToLog("DictByteWriter", 0, "Reset writer");
	byteWriter->resetBuffer();
	byteWriter->skipToBytePos(3*sizeof(int));		
	*((unsigned int*) (buffer))=fieldSize;
	startPosPtr=(unsigned int*) (buffer+sizeof(int));		
	*((unsigned int*) (buffer+2*sizeof(int)))=0;
	numValsPtr=(unsigned int*) (buffer+2*sizeof(int));
}

// Gets the next value block from the operator 
Block* DictByteWriter::getNextValBlock(int colIndex_) {
	if (colIndex_==0) {
		return getEntries();	
	}
	else if (colIndex_==1) {
		return getTable();	
	}
	else 
		return NULL;
}	
Block* DictByteWriter::getTable() {
	
	if (!readyToOutDict) throw new UnexpectedException("Error, you have not asked me for the entries yet");
	if ((entryValueMap.size()==0) && (currindex == 0)) {
		Log::writeToLog("DictByteWriter", 0, "entryValueMap empty returning NULL");
		return NULL;
	}
	//cout << "ndv: " << numDistinctValues << endl;
	if (currindex == 0) {
	  //unsigned int pos=entryValueMap.begin()->first;
	  vals = new int[numEncodedValsInInt];
	  currval=entryValueMap.begin()->second;
	  for (unsigned int i = 0; i < numEncodedValsInInt; i++)
	    vals[i] = currval[i];
	  entryValueMap.erase(entryValueMap.begin());
	  dictBlock->setValue(vals[0],curpos++);
	  Log::writeToLog("DictByteWriter", 0, "Returning table block: val", vals[currindex]);
	  currindex++;
	  if (currindex == numEncodedValsInInt)
	    currindex=0;
	}
	else {
	  dictBlock->setValue(vals[currindex],curpos++);
	  Log::writeToLog("DictByteWriter", 0, "Returning table block: val", vals[currindex]);
	  currindex++;
	  if (currindex == numEncodedValsInInt)
	    currindex=0;
	}

	Log::writeToLog("DictByteWriter", 0, "Returning table block: entry", curpos-1);
	return dictBlock;
}
Block* DictByteWriter::getEntries() {
	if (currBlock==NULL) {
		readyToOutDict=true;			
		Log::writeToLog("DictByteWriter", 1, "Data source dry returning NULL");				
		return NULL;
	}
	resetWriter();
	bool reusecurrval=false;
	if (currval == NULL) {
	  currval = new int[16];
	  memset(currval, 0, 64);
	}
	else
	  reusecurrval = true;
	bool writeSuccess=true;
	unsigned int numvals;
	unsigned int entry;
	while (writeSuccess) {	
	  /*	Pair* pair=currBlock->getStartPair();
		if (init) {
			*startPosPtr=pair->position;
			init=false;
		}	
		unsigned int entry;*/
	  if (reusecurrval) {
	    numvals=numEncodedValsInInt;
	    reusecurrval=false;
	  }
	  else
	    numvals = getEntry(currval);
	    if (valueEntryMap.count(currval)==0) {
	      if (numDistinctValues>escapeInt) {
		Log::writeToLog("DictByteWriter", 5, "Error: number of distinctValues larger than possible to encode in this fieldSize", fieldSize);
		throw new CodingException("Error: number of distinctValues larger than possible to encode in this fieldSize");
	      }
	      valueEntryMap[currval]=numDistinctValues;
	      entryValueMap[numDistinctValues]=currval;
	      currval = new int[16];
	      memset(currval, 0, 64);
	      entry=numDistinctValues;
	      //Log::writeToLog("DictByteWriter", 1, "No entry for value", pair->value);
	      //Log::writeToLog("DictByteWriter", 1, "Created entry", numDistinctValues);				
	      numDistinctValues++;
	    }
	    else {
	      entry=valueEntryMap[currval];
	      //Log::writeToLog("DictByteWriter", 0, "Writing entry for value", pair->value);				
	    }
	    writeSuccess=false;
	    /*
	      switch(fieldSize) {
	      case 8:
	      writeSuccess=byteWriter->writeByte((byte)entry);
	      break;
	      case 16:
	      writeSuccess=byteWriter->writeShort((unsigned short)entry);
	      break;
	      case 24:
	      writeSuccess=byteWriter->writeThree(entry);
	      break;
	      case 32:
	      writeSuccess=byteWriter->writeInt(entry);
	      break;
	      default: throw new UnexpectedException("Should be 8.16,24,or 32");
	      }
	    */
	    writeSuccess=byteWriter->writeInt(entry);
	    if (writeSuccess) {
		*numValsPtr+=numvals;		
		
	      //Log::writeToLog("DictByteWriter", 0, "Writing successfully entry", entry);				
	      //currBlock=(BasicBlock*) blockWriter->getNextValBlock(0);				
	      //Log::writeToLog("DictByteWriter", 0, "Got next block");				
	      if (currBlock==NULL) {
		Log::writeToLog("DictByteWriter", 1, "Data source dry returning current buffer (in block)");				
		outBlock->setBuffer(buffer, bfrSizeBits);
		return outBlock;
	      }					
	    }
	    //}
	  /*else {
	    writeSuccess=false;
	    writeSuccess=byteWriter->writeInt(currval[0]);
	    for (unsigned int i = 1; i < numvals; i++)
	      writeSuccess&=byteWriter->writeInt(currval[i]);

	    if (writeSuccess) {
	      *numValsPtr+=numvals;				
	      outBlock->setBuffer(buffer, bfrSizeBits);
	      return outBlock;
	    }
	    else throw new UnimplementedException("Haven't dealt with this case yet");
	  }*/
	}
	//Log::writeToLog("DictByteWriter", 0, "Filled current buffer, returning it (in block)");				
	outBlock->setBuffer(buffer, bfrSizeBits);
	init=true;
	return outBlock;
		
}

unsigned int DictByteWriter::getEntry(int* entryarray) {
  ValPos* pair=currBlock->getStartPair();
  if (init) {
    *startPosPtr=pair->position;
    init=false;
  }	
  //assume int type for now ... fix later
  assert(pair->type == ValPos::INTTYPE);
  entryarray[0]=*(int*)pair->value;
  currBlock=(BasicBlock*) blockWriter->getNextValBlock(0);
  bool ismore = (currBlock!=NULL);
  unsigned int i = 1;
  while (ismore  && i < numEncodedValsInInt) {
    ValPos* pair=currBlock->getStartPair();
    //assume int type for now ... fix later
    assert(pair->type == ValPos::INTTYPE);
    entryarray[i]=*(int*)pair->value;
    currBlock=(BasicBlock*) blockWriter->getNextValBlock(0);
    //if (pair->value == 78)
    //  cout << "value is: " << pair->value << endl;
    ismore = (currBlock!=NULL);
    i++;
  }
  if (i < numEncodedValsInInt)
    for (unsigned int j = i; j < numEncodedValsInInt; j++)
      entryarray[j]=0xffffffff;
  return i;
}
	
// Gets the next position block (bitstring of positions) from the operator
PosBlock* DictByteWriter::getNextPosBlock(int colIndex_) {
	return NULL;
}
