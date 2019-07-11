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
// Problems: migfer@mit.edu


#include "DictWriter.h"

DictWriter::DictWriter(Operator* blockSrc_, int colIndex_, unsigned int bfrSizeBits_, unsigned int fieldSize_) : Writer(blockSrc_, colIndex_) 
{
	fieldSize=fieldSize_;
	escapeInt=0xFFFFFFFF>>(32-fieldSize);
	numDistinctValues=0;
	blockWriter=new BasicBlockWriter(blockSrc_, colIndex_);
	currBlock=(BasicBlock*) blockWriter->getNextValBlock(0);

	if (currBlock!=NULL) {
		outBlock=new BitBlock(currBlock->isValueSorted());
		bitWriter=new BitWriter(bfrSizeBits_);	
		dictBlock=new BasicBlock(true, true, true);
		buffer=bitWriter->getBuffer();
		startPosPtr=(unsigned int*) (buffer+sizeof(int));
		numValsPtr=(unsigned int*) (buffer+2*sizeof(int));
	}
	else {
		outBlock=NULL;
		bitWriter=NULL;
		buffer=NULL;
		startPosPtr=NULL;
		numValsPtr=NULL;
	}
	init=true;
	readyToOutDict=false;	
	bfrSizeBits=bfrSizeBits_;

	Log::writeToLog("DictWriter", 0, "Finished constructor");
}

DictWriter::~DictWriter()
{
	if (outBlock!=NULL) delete outBlock;
	if (dictBlock!=NULL) delete dictBlock;
	if (blockWriter!=NULL) delete blockWriter;
	if (bitWriter!=NULL) delete bitWriter;
}

void DictWriter::resetWriter() {
	Log::writeToLog("DictWriter", 0, "Reset writer");
	bitWriter->clearBuffer();
	bitWriter->skipBits(8*3*sizeof(int));		
	*((unsigned int*) (buffer))=fieldSize;
	startPosPtr=(unsigned int*) (buffer+sizeof(int));
	numValsPtr=(unsigned int*) (buffer+2*sizeof(int));
}

// Gets the next value block from the operator 
Block* DictWriter::getNextValBlock(int colIndex_) {
	if (colIndex_==0) {
		return getEntries();	
	}
	else if (colIndex_==1) {
		return getTable();	
	}
	else 
		return NULL;
}	
Block* DictWriter::getTable() {
	
	if (!readyToOutDict) throw new UnexpectedException("Error, you have not asked me for the entries yet");
	if (entryValueMap.size()==0) {
		Log::writeToLog("DictWriter", 0, "entryValueMap empty returning NULL");
		return NULL;
	}
	unsigned int pos=entryValueMap.begin()->first;
	unsigned int val=entryValueMap.begin()->second;
	entryValueMap.erase(entryValueMap.begin());
	dictBlock->setValue(val,pos+1);
	Log::writeToLog("DictWriter", 0, "Returning table block: val", val);
	Log::writeToLog("DictWriter", 0, "Returning table block: entry", pos);
	return dictBlock;
}
Block* DictWriter::getEntries() {
	if (currBlock==NULL) {
		readyToOutDict=true;			
		Log::writeToLog("DictWriter", 1, "Data source dry returning NULL");				
		return NULL;
	}
	resetWriter();
	bool writeSuccess=true;
	while (writeSuccess) {	
		ValPos* pair=currBlock->getStartPair();
		//assume int type for now ... fix later
		assert(pair->type == ValPos::INTTYPE);
		if (init) {
			*startPosPtr=pair->position;
			init=false;
		}	
		unsigned int entry;
		if (valueEntryMap.count(*(int*)pair->value)==0) {
			if (numDistinctValues>escapeInt) {
				Log::writeToLog("DictWriter", 5, "Error: number of distinctValues larger than possible to encode in this fieldSize", fieldSize);
				throw new CodingException("Error: number of distinctValues larger than possible to encode in this fieldSize");
			}
			valueEntryMap[*(int*)pair->value]=numDistinctValues;
			entryValueMap[numDistinctValues]=*(int*)pair->value;
			entry=numDistinctValues;
			Log::writeToLog("DictWriter", 1, "No entry for value", *(int*)pair->value);
			Log::writeToLog("DictWriter", 1, "Created entry", numDistinctValues);				
			numDistinctValues++;
		}
		else {
			entry=valueEntryMap[*(int*)pair->value];
			Log::writeToLog("DictWriter", 0, "Writing entry for value", *(int*)pair->value);				
		}
		writeSuccess=bitWriter->writeBits(entry, fieldSize);
		if (writeSuccess) {
			*numValsPtr+=1;				
			Log::writeToLog("DictWriter", 0, "Writing successfully entry", entry);				
			currBlock=(BasicBlock*) blockWriter->getNextValBlock(0);				
			Log::writeToLog("DictWriter", 0, "Got next block");				
			if (currBlock==NULL) {
				Log::writeToLog("DictWriter", 1, "Data source dry returning current buffer (in block)");				
				outBlock->setBuffer(buffer, bfrSizeBits);
				return outBlock;
			}					
		}
	}
	Log::writeToLog("DictWriter", 0, "Filled current buffer, returning it (in block)");				
	outBlock->setBuffer(buffer, bfrSizeBits);
	init=true;
	return outBlock;
		
}
	
// Gets the next position block (bitstring of positions) from the operator
PosBlock* DictWriter::getNextPosBlock(int colIndex_) {
	return NULL;
}
