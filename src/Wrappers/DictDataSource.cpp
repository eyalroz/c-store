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
// Dictionary DataSource

#include "DictDataSource.h"

DictDataSource::DictDataSource(AM* dictTableAM_, AM* entriesAM_, bool valSorted_) : IntDataSource(entriesAM_, valSorted_, true)
{
  dictTableAM=dictTableAM_;
  decoder=new DictDecoder(dictTableAM_, valSorted_);
}

/*
DictDataSource::DictDataSource(AM* dictTableAM_, AM* entriesAM_, bool valSorted_) : DataSource (entriesAM_)
{
	dictTableAM=dictTableAM_;
	entriesAM=entriesAM_;
	valSorted=valSorted_;
	bitDecoder=new BitDecoder(valSorted_);
	intDecoder=new IntDecoder(valSorted_);
	outBlock=new BasicBlock(valSorted_, true, true);
	initTable();		
	init=true;
	}
*/

DictDataSource::DictDataSource(const DictDataSource& src_) : IntDataSource(src_)
{
  //valSorted=src_.valSorted;
  dictTableAM=src_.dictTableAM;
  /*entriesAM=src_.entriesAM;
	bitDecoder=new BitDecoder(valSorted);
	intDecoder=new IntDecoder(false);
	outBlock=new BasicBlock(valSorted, true, true);
	initTable();		
	init=true;*/
  decoder=new DictDecoder(dictTableAM, src_.valSorted);
}

DictDataSource::~DictDataSource()
{
  /*delete bitDecoder;
	delete intDecoder;
	delete outBlock;*/
}
/*
void DictDataSource::initTable() {
	BasicBlock* block=(BasicBlock*) getBlocks(dictTableAM, intDecoder);
	while (block!=NULL) {
		unsigned int value=block->getStartPair()->value;
		unsigned int entry=(block->getStartPair()->position)-1;
		entryValueMap[entry]=value;
		Log::writeToLog("DictDataSource", 1, "initTable(): got Entry", entry);					
		Log::writeToLog("DictDataSource", 1, "initTable(): for value", value);					
		block=(BasicBlock*) getBlocks(dictTableAM, intDecoder);
	}
}
// Gets the next value block from the operator 
Block* DictDataSource::getNextValBlock(int colIndex_) {
	if (colIndex_!=0) return NULL;
	
	if (predChanged || valFilterChanged || posFilterChanged) {
		Log::writeToLog("DictDataSource", 1, "getNextValBlockSorted(): pred or filter changed, initializing cursors on AM");					
		entriesAM->initCursors();
		init=true;
		if (!valSorted) {
			predChanged=false;
			valFilterChanged=false;
			posFilterChanged=false;	
		} // else getValSortedGetPage takes care of altering these flags
	}
	do {	
		if (init) {
			init=false;
			byte* page=NULL;
			if (valSorted) {
				page=getValSortedGetPage(entriesAM);
			}
			else {
				page=(byte*) entriesAM->getNextPagePrimary();
			}
			if (page==NULL) return NULL;
			bitDecoder->setBuffer(page); 	
		}
	
		while (!bitDecoder->hasNextBlock()) {
			byte* page=NULL;
			//if (valSorted)
				page=(byte*) entriesAM->getNextPagePrimary();
			//else
			//	page=(byte*) entriesAM->getNextPage();
				
			if (page==NULL) return NULL;
			Log::writeToLog("DictDataSource", 0, "read new page, as old ran out of blocks, setting buffer in decoder");			
			bitDecoder->setBuffer(page); 	
		}
		// Do translation
		BasicBlock* currBlock=(BasicBlock*) bitDecoder->getNextBlockSingle();
		unsigned int newVal=entryValueMap[currBlock->getStartPair()->value];
		outBlock->setValue(newVal, currBlock->getStartPair()->position);		
	} while (!checkOutBlock());
	outBlock->resetBlock();
	return outBlock;		
};

bool DictDataSource::checkOutBlock() {
	Pair* pair=outBlock->getNext();
	outBlock->resetBlock();
	int value=pair->value;
	int pos=pair->position;
	//Log::writeToLog("DictDataSource", 0, "checkBlock(): checking value", value);			
	//Log::writeToLog("DictDataSource", 0, "checkBlock(): checking pos", pos);			
	if (valFilter!=NULL) {
		// not implemented	
	}
	if (posFilter!=NULL) {
		// not implemented	
	}
	if (pred!=NULL) {
		pred->setLHS(value);
		if (!pred->evalPredicate()) return false;
	}
	//Log::writeToLog("DictDataSource",0,"DataSource checkOutBlock returning true");
	return true;
}
	
// Skips to the first block with this value
// Returns NULL if the value is outside the range of the column
Block* DictDataSource::skipToValBlock(int colIndex_, int val_) {
	return NULL;
};
	
	
// Gets the next position block (bitstring of positions) from the operator
PosBlock* DictDataSource::getNextPosBlock(int colIndex_) {
	return NULL;
};
	
// Skips to the first block with this position
// Returns NULL if the value is outside the range of the column
PosBlock* DictDataSource::skipToPosBlock(int colIndex_, int pos_) {
	return NULL;
};
*/


