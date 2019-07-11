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
// Delta on Position DataSource
// Problems: migfer@mit.edu


#include "DeltaPosDataSource.h"

DeltaPosDataSource::DeltaPosDataSource(AM* am_, bool valSorted_) : DataSource(am_) {
	outBlock=NULL;
	valDataSource=NULL;
	currColumnAM=NULL;
	currColumnDecoder=NULL;
	posContigNumValues=0;
	mode=DeltaPosDataSource::UNINIT;
	valSorted=valSorted_;
	posOutBlock=new PosDeltaBlock();
}
DeltaPosDataSource::DeltaPosDataSource(const DeltaPosDataSource& src_) : DataSource(src_) {
	outBlock=NULL;
	valDataSource=NULL;
	currColumnAM=NULL;
	currColumnDecoder=NULL;
	
	posContigNumValues=0;		
	mode=src_.mode;
	valSorted=src_.valSorted;
	posOutBlock=new PosDeltaBlock();
}
DeltaPosDataSource::~DeltaPosDataSource() {
	// cleanup depending on mode
	if (mode==DeltaPosDataSource::POS_CONTIG) {
		delete posContigDecoder;
		delete posContigOutBlock;

		delete valDataSource;


		for (BlockMapIterator iterPos=blockMap.begin();
						   iterPos!=blockMap.end(); 
						   ++iterPos) {
			DeltaPosBlock* currBlock=iterPos->second;
			if (currBlock!=NULL) delete currBlock;
		}
		
		for (AMMapIterator iterPos=valueAMMap.begin();
						   iterPos!=valueAMMap.end(); 
						   ++iterPos) {
			AM* currAM=iterPos->second;
			if (currAM!=NULL) delete currAM;	
		}		
	}
	else if (mode==DeltaPosDataSource::NORMAL) {
		delete valDataSource;
		delete currColumnDecoder;		
	}
	
}
// sets the mode of operation given the first call to getBlock
// calls to initialize
void DeltaPosDataSource::setMode(bool isPosContig_) {
	if (mode!=DeltaPosDataSource::UNINIT)
		throw new UnexpectedException("DeltaPosDataSource::setMode(): Error, mode was already set");
	if (isPosContig_) {
		initAMsPosContig();
		mode=DeltaPosDataSource::POS_CONTIG;
	}
	else {
		initAMsNormal();
		mode=DeltaPosDataSource::NORMAL;		
	}
}

	
// Just opens an AM for first value
void DeltaPosDataSource::initAMsNormal() {
  valDataSource=new IntDataSource(am, true, true);
  valDataSource->setSingles(true);
  currColumnDecoder=new DeltaPosDecoder(valSorted);
}
	
// Change of Value in normal, i.e. open of new column 
void DeltaPosDataSource::changeValueNormal(int val_) {
	if (currColumnAM!=NULL) delete currColumnAM;
	string tableName=am->getTableName();
	ostringstream nameStream;
	nameStream << tableName << val_;
	Log::writeToLog("DeltaPosDataSource", 0, "changeValueNormal(): Opening column with name: "+ nameStream.str());			
	currColumnAM=new ROSAM(nameStream.str(), 1);			
}

void DeltaPosDataSource::initAMsPosContig() {
	Log::writeToLog("DeltaPosDataSource", 0, "Initializing under POS CONTIG MODE");			
	if (am==NULL) throw new UnexpectedException("DeltaPosDataSource::initAMsPosContig(): Error, Am to read values from is NULL");

	posContigDecoder=new DeltaPosDecoder(valSorted);
	posContigOutBlock=new BasicBlock(valSorted, true, true);

	posContigNumValues=0;
	string tableName=am->getTableName();
	valDataSource=new IntDataSource(am, true, true);
	valDataSource->setSingles(true);
	Block* valBlock;
	valBlock=valDataSource->getNextValBlock(0);
	//for now assume values are integers ... fix later
	ValPos* vp;
	while (valBlock!=NULL) {
	  vp = valBlock->getNext();
	  assert(vp->type == ValPos::INTTYPE);
	  int value=*(int*)vp->value;
	  //int value=valBlock->getNext()->value;
		Log::writeToLog("DeltaPosDataSource", 0, "In posContig init: read value",value);				
		if (valueAMMap.count(value)==0) {
			ostringstream nameStream;
			nameStream << tableName << value;
			valueAMMap[value]=new ROSAM(nameStream.str(), 1);			
			blockMap[value]=new DeltaPosBlock(valSorted);
			posContigNumValues++;		
			Log::writeToLog("DeltaPosDataSource", 0, "In posContig init: opened am, name="+nameStream.str());				
		}
		else {
			throw new CodingException("DeltaPosDataSource: Duplicate values in value column");	
		}
		
		if (!valBlock->hasNext()) {
			valBlock=valDataSource->getNextValBlock(0);
		}
	}
}

bool DeltaPosDataSource::getNextValNormal(int& value_) {
	// value filters not implemented
	Log::writeToLog("DeltaPosDataSource", 0, "getNextValNormal(): finding new value subcolumn");				
	bool stopFlag=false;
	do {
		BasicBlock* block=(BasicBlock*) valDataSource->getNextValBlock(0);	
		if (block==NULL) {
			Log::writeToLog("DeltaPosDataSource", 0, "getNextValNormal(): none found returning false");				
			return false;
		}
		//for now assume values are integers ... fix later
		ValPos* vp = block->getNext();
		assert(vp->type == ValPos::INTTYPE);
		value_=*(int*)vp->value;
		//value_=block->getNext()->value;
		Log::writeToLog("DeltaPosDataSource", 0, "getNextValNormal(): found (but unsure it meets pred) w/ value=", value_);			
		if (pred!=NULL) {
			pred->setLHS(vp);
			stopFlag=pred->evalPredicate();
			Log::writeToLog("DeltaPosDataSource", 0, "getNextValNormal(): Predicate evaluated=", pred->evalPredicate());	
		}
		else {
			Log::writeToLog("DeltaPosDataSource", 0, "getNextValNormal(): Predicate NULL");	
			stopFlag=true;
		}
	} while (!stopFlag);	
	Log::writeToLog("DeltaPosDataSource", 0, "getNextValNormal(): found and meets pred w/ value=", value_);			
	return true;
}


Block* DeltaPosDataSource::getNextValBlockPosContig() {
	// Not implemented: value && position filters
	if (valFilterChanged || posFilterChanged) {
		Log::writeToLog("DeltaPosDataSource", 1, "getNextValBlockPosContig(): pred or filter changed, initializing cursors on AM");			
		valFilterChanged=false;
		posFilterChanged=false;	
	}	
	
	if (predChanged) {
		predChanged=false;
		// initialize all the cursors
		for (map<int, ROSAM*>::iterator iterPos=valueAMMap.begin();
									 iterPos!=valueAMMap.end(); 
									 ++iterPos) {
			iterPos->second->initCursors();
		}
	}
	
	// Now we need to build up the basic blocks to output
	// The stategy is to first get a page in all decoders, to put a block
	// In the block Map, we go through a pair in each single column and
	// place it in a map<position, value>
	// we then pop off this map to get contiguous postions
	// A coding expection is thrown if the positions were not contiguous
	
	
	populateBlocksMap();
	loadPairs();
	
	if (posValueMap.size()==0)
		return NULL;

	Log::writeToLog("DeltaPosDataSource", 0, "getNextValPosContig(): pushing off map");	
	int pos=posValueMap.begin()->first;	
	int val=posValueMap.begin()->second;	
	Log::writeToLog("DeltaPosDataSource", 0, "getNextValPosContig(): returning val=", val);	
	Log::writeToLog("DeltaPosDataSource", 0, "getNextValPosContig(): returning pos=", pos);	
	posValueMap.erase(pos);
	valPosMap.erase(val); // so we know this value no longer in buffer (i.e. we need to pull in another pair)
	posContigOutBlock->setValue(val, pos);			
	return posContigOutBlock;
}
void DeltaPosDataSource::populateBlocksMap() {
	if (pred==NULL)	{
		Log::writeToLog("DeltaPosDataSource", 0, "populateBlockMaps(): no pred: populate Maps with begin and end");
		populateBlockMaps(blockMap.begin(), blockMap.end());
	}
	else {
	  //assume type int for now .. fix later
	  assert(pred->getRHS()->type == ValPos::INTTYPE);
	  int rhsint = *(int*)pred->getRHS()->value;
		switch	(pred->getPredType()) {
			case Predicate::OP_GREATER_THAN: 	
				populateBlockMaps(blockMap.lower_bound(rhsint+1), blockMap.end());
				break;			
			case Predicate::OP_GREATER_THAN_OR_EQUAL: 					
				populateBlockMaps(blockMap.lower_bound(rhsint), blockMap.end());
				break;
			case Predicate::OP_LESS_THAN:
				populateBlockMaps(blockMap.begin(), blockMap.upper_bound(rhsint-1));
				break;
			case Predicate::OP_LESS_THAN_OR_EQUAL:
				populateBlockMaps(blockMap.begin(), blockMap.upper_bound(rhsint));
				break;
			case Predicate::OP_EQUAL: 	
				populateBlockMaps(blockMap.lower_bound(rhsint), blockMap.upper_bound(rhsint));
				break;
			default:
				populateBlockMaps(blockMap.begin(), blockMap.end());
				break;
		}
	}
}
void DeltaPosDataSource::populateBlockMaps(BlockMapIterator startIter,
											BlockMapIterator endIter) {
	for (BlockMapIterator iterPos=startIter;
					   iterPos!=endIter; 
					   ++iterPos) {
		int currValue=iterPos->first;
		DeltaPosBlock* currBlock=iterPos->second;
		if ((currBlock==NULL) || (!currBlock->hasNext())) {	
			Log::writeToLog("DeltaPosDataSource", 2, "Populating block map: getting page val=",currValue);			
			byte* page=(byte*) valueAMMap[currValue]->getNextPage();
			if (page!=NULL) {
				// as we know only one block per page
				currBlock->setDeltaBuffer(page, 8*PAGE_SIZE);
				posContigDecoder->setBuffer(page);
				Log::writeToLog("DeltaPosDataSource", 2, "Populating block map: page!=NULL with val=",currBlock->getValue());			
				Log::writeToLog("DeltaPosDataSource", 2, "Populating block map: page!=NULL with endPos=",posContigDecoder->getEndPos());				
				Log::writeToLog("DeltaPosDataSource", 2, "Populating block map: page!=NULL with startPos=",posContigDecoder->getStartPos());
				Log::writeToLog("DeltaPosDataSource", 2, "Populating block map: page!=NULL with blockSize=",currBlock->getSize());
			}
			else {
				Log::writeToLog("DeltaPosDataSource", 2, "Removing val from BlockMap, AM returned NULL, val",currValue); 
				blockMap.erase(currValue);	
			}			
		}
	}
}
void DeltaPosDataSource::loadPairs() {
	if (pred==NULL)	{
		loadPairs(blockMap.begin(), blockMap.end());
	}
	else {
	  //assume type int for now .. fix later
	  assert(pred->getRHS()->type == ValPos::INTTYPE);
	  int rhsint = *(int*)pred->getRHS()->value;
		switch	(pred->getPredType()) {
			case Predicate::OP_GREATER_THAN: 	
				loadPairs(blockMap.lower_bound(rhsint+1), blockMap.end());
				break;			
			case Predicate::OP_GREATER_THAN_OR_EQUAL: 					
				loadPairs(blockMap.lower_bound(rhsint), blockMap.end());
				break;
			case Predicate::OP_LESS_THAN:
				loadPairs(blockMap.begin(), blockMap.upper_bound(rhsint-1));
				break;
			case Predicate::OP_LESS_THAN_OR_EQUAL:
				loadPairs(blockMap.begin(), blockMap.upper_bound(rhsint));
				break;
			case Predicate::OP_EQUAL: 	
				loadPairs(blockMap.lower_bound(rhsint), blockMap.upper_bound(rhsint));
				break;
			default:
				loadPairs(blockMap.begin(), blockMap.end());
				break;
		}
	}
}
void DeltaPosDataSource::loadPairs(BlockMapIterator startIter, BlockMapIterator endIter) {
	for (BlockMapIterator iterPos=startIter; iterPos!=endIter; ++iterPos) {
		int currVal=iterPos->first;
		DeltaPosBlock* currBlock=blockMap[currVal];
		Log::writeToLog("DeltaPosDataSource", 1, "loadPairs for value",currVal);				
		int currValCount=valPosMap.count(currVal);
		Log::writeToLog("DeltaPosDataSource", 0, "loadPairs currValCount",currValCount);				
		if ((currBlock!=NULL) && (currValCount<1)) {
			Log::writeToLog("DeltaPosDataSource", 0, "loadPairs currBlock, value",currBlock->getValue());				
			Log::writeToLog("DeltaPosDataSource", 0, "loadPairs currBlock, size",currBlock->getSize());				

			if (currBlock->hasNext()) {
			  //assume type int for now .. fix later
				ValPos* pair=currBlock->getNext();
				assert(pair->type == ValPos::INTTYPE);
				//Pair* pair=currBlock->getNext();
				Log::writeToLog("DeltaPosDataSource", 0, "loadPairs: loading with pos",pair->position);				
				Log::writeToLog("DeltaPosDataSource", 0, "loadPairs: loading with value",*(int*)pair->value);				
				posValueMap[pair->position]=*(int*)pair->value;
				valPosMap[*(int*)pair->value]=pair->position;
			}
			else {
				Log::writeToLog("DeltaPosDataSource", 0, "loadPairs got NULL pair");							
			}
		}												
	}
}

Block* DeltaPosDataSource::getNextValBlockNormal() {
	Log::writeToLog("DeltaPosDataSource", 0, "called getNextValBlockNormal()");			
	// Not implemented: value filters && position filters
	if (predChanged || valFilterChanged || posFilterChanged) {
		Log::writeToLog("DeltaPosDataSource", 1, "getNextValBlockNormal(): pred or filter changed, initializing cursors on AM");			
		am->initCursors();
		predChanged=false;
		valFilterChanged=false;
		posFilterChanged=false;	
		currColumnAM=NULL;
	}
	
	if (currColumnAM==NULL) {
		Log::writeToLog("DeltaPosDataSource", 1, "getNextValBlockNormal(): currcolumnAM NULL, getting AM to new");			
		int value=1;
		if (!getNextValNormal(value)) {
			Log::writeToLog("DeltaPosDataSource", 1, "getNextValBlockNormal(): no more value subcolumns to open, returning NULL");			
			return NULL;	
		}
		changeValueNormal(value);
	}
	
	// as we know there is only one delta pos block per page
	byte* page=(byte*) currColumnAM->getNextPage();		
	while (page==NULL) {
		int value;
		if (!getNextValNormal(value)) {
			Log::writeToLog("DeltaPosDataSource", 0, "getNextValBlockNormal(): getNextValNormal returned false, therefore returning NULL");				
			return NULL;
		}
		changeValueNormal(value);	
		page=(byte*) currColumnAM->getNextPage();
	}
	Log::writeToLog("DeltaPosDataSource", 0, "getNextValBlockNormal(): have a page from currAM");			
	currColumnDecoder->setBuffer(page);
	return currColumnDecoder->getNextBlock(); //as we know only one delta pos block per page
}

// Gets the next value block from the operator 
Block* DeltaPosDataSource::getNextValBlock(int colIndex_) {
	Log::writeToLog("DeltaPosDataSource", 0, "called getNextValBlock()");			
	if (colIndex_==0) {
		if (mode==DeltaPosDataSource::UNINIT) {
			setMode(false);	
			return getNextValBlockNormal();
		}
		else if (mode==DeltaPosDataSource::POS_CONTIG) {
			throw new UnexpectedException("DeltaPosDataSource: Error cannot read data source in both modes simultaneously");
		}
		else 
			return getNextValBlockNormal();
	}
	else if (colIndex_==1) {
		if (mode==DeltaPosDataSource::UNINIT) {
			setMode(true);	
			return getNextValBlockPosContig();
		}
		else if (mode==DeltaPosDataSource::NORMAL) {
			throw new UnexpectedException("DeltaPosDataSource: Error cannot read data source in both modes simultaneously");
		}
		else {
			return getNextValBlockPosContig();
		}
	}
	else {
		return NULL;	
	}
}
	
	
// Skips to the first block with this value
// Returns NULL if the value is outside the range of the column
Block* DeltaPosDataSource::skipToValBlock(int colIndex_, int val_) {
	// not implemented
	return NULL;
}
	
	
// Gets the next position block (bitstring of positions) from the operator
PosBlock* DeltaPosDataSource::getNextPosBlock(int colIndex_) {
	if (colIndex_==0) {
		DeltaPosBlock* block=(DeltaPosBlock*) getNextValBlock(0);
		posOutBlock->setDeltaBlock(block);
		return posOutBlock;
	}
	else {
		return NULL;
	}
}	

// Skips to the first block with this position
// Returns NULL if the value is outside the range of the column
PosBlock* DeltaPosDataSource::skipToPosBlock(int colIndex_, int pos_) {
	// not implemented
	return NULL;
}		


//printColumn, print to stringstream instead of stdout. for testing purpose
void DeltaPosDataSource::printColumn(stringstream& ss) {
    using namespace std;
    	int numValues=0;
	DeltaPosBlock* blk=(DeltaPosBlock*) getNextValBlock(0);
	ss << "(Value,Size): "; 
	while (blk!=NULL) {
		ss << "(" << blk->getValue() << "," << blk->getSize() << "), ";
		numValues++;
		blk=(DeltaPosBlock*) getNextValBlock(0);
	}
	ss << endl;
}
