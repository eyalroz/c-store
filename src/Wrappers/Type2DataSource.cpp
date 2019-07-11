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
// Position DataSource


#include "Type2DataSource.h"

Type2DataSource::Type2DataSource(AM* am_, bool valSorted_, Decoder* valDecoder_) : DataSource(am_) {
	outBlock=NULL;
	valDataSource=NULL;
	valam=NULL;
	currColumnDecoder=NULL;
	posContigNumValues=0;
	mode=Type2DataSource::UNINIT;
	valSorted=valSorted_;
	posOutBlock=new PosType2Block();
	valSet=false;
	valDecoder = valDecoder_;
	currValBlock = NULL;
}
Type2DataSource::Type2DataSource(const Type2DataSource& src_) : DataSource(src_) {
	outBlock=NULL;
	valDataSource=NULL;
	valam=NULL;
	currColumnDecoder=NULL;
	
	posContigNumValues=0;		
	mode=src_.mode;
	valSorted=src_.valSorted;
	posOutBlock=new PosType2Block();
	valSet=false;
	valDecoder = src_.valDecoder;
	currValBlock = NULL;
}
Type2DataSource::~Type2DataSource() {
	// cleanup depending on mode
	if (mode==Type2DataSource::POS_CONTIG) {
		delete posContigDecoder;
		delete posContigOutBlock;

		delete valDataSource;


		for (BlockTMapIterator iterPos=blockMap.begin();
						   iterPos!=blockMap.end(); 
						   ++iterPos) {
			Type2Block* currBlock=iterPos->second;
			if (currBlock!=NULL) delete currBlock;
		}
		
		/*for (AMMapIterator iterPos=valueAMMap.begin();
						   iterPos!=valueAMMap.end(); 
						   ++iterPos) {
			AM* currAM=iterPos->second;
			if (currAM!=NULL) delete currAM;	
			}	*/
	}
	else if (mode==Type2DataSource::NORMAL) {
		delete valDataSource;
		delete currColumnDecoder;		
	}
	if (valam)
	  delete valam;
	
}
// sets the mode of operation given the first call to getBlock
// calls to initialize
void Type2DataSource::setMode(bool isPosContig_) {
	if (mode!=Type2DataSource::UNINIT)
		throw new UnexpectedException("Type2DataSource::setMode(): Error, mode was already set");
	if (isPosContig_) {
		initAMsPosContig();
		mode=Type2DataSource::POS_CONTIG;
	}
	else {
		initAMsNormal();
		mode=Type2DataSource::NORMAL;		
	}
}

	
// Just opens an AM for first value
void Type2DataSource::initAMsNormal() {
  string tableName=am->getTableName();

  ROSAM* rosam = new ROSAM(tableName, 1);
  DataSource* tempDS = new DataSource(rosam, true, false, valDecoder);
  ValPos** valindex = tempDS->createValueIndex();
  delete tempDS;
  delete rosam;

  ostringstream nameStream;
  nameStream << tableName << "All";
  valDataSource=new DataSource(am, true, false, valDecoder);
  //valDataSource->setSingles(true);
  currColumnDecoder=new Type2Decoder(valSorted, valindex);
  if (valindex && valindex[0])
    valam = new ROSAM(nameStream.str(), -1, valindex[0]->getSize(), sizeof(int), valindex[0]->type, ValPos::INTTYPE);
  else
    valam = new ROSAM(nameStream.str(), -1, sizeof(int), sizeof(int), ValPos::INTTYPE, ValPos::INTTYPE);
  //valam = new ROSAM(nameStream.str(), -1);
  valSet = false;
}
	
// Change of Value in normal, i.e. open of new column 
const void* Type2DataSource::changeValueNormal(char* val_) {
  /*if (currColumnAM!=NULL) delete currColumnAM;
	string tableName=am->getTableName();
	ostringstream nameStream;
	nameStream << tableName << val_;
	Log::writeToLog("Type2DataSource", 0, "changeValueNormal(): Opening column with name: "+ nameStream.str());			
	currColumnAM=new ROSAM(nameStream.str(), 1);*/
  valSet=true;
  return valam->skipToPagePrimary(val_);
}

void Type2DataSource::initAMsPosContig() {
	Log::writeToLog("Type2DataSource", 0, "Initializing under POS CONTIG MODE");			
	if (am==NULL) throw new UnexpectedException("Type2DataSource::initAMsPosContig(): Error, Am to read values from is NULL");

	posContigNumValues=0;
	string tableName=am->getTableName();
	

	ROSAM* rosam = new ROSAM(tableName, 1);
	DataSource* tempDS = new DataSource(rosam, true, false, valDecoder);
	ValPos** valindex = tempDS->createValueIndex();
	delete tempDS;
	delete rosam;

	posContigDecoder=new Type2Decoder(valSorted, valindex);
	posContigOutBlock=new BasicBlock(valSorted, true, true);
	ostringstream nameStream;
	nameStream << tableName << "All";
	valDataSource=new DataSource(am, true, false, valDecoder);
	throw new UnexpectedException("Want to setSingles below but can't now I changed valDataSource to be generic rather than IntDataSource, I can't do that anymore. But I don't think this path is called any more in Type2DataSource (we have Type2DecompDataSource now) so whatever.");
	//valDataSource->setSingles(true);
	valam = new ROSAM(nameStream.str(), -1);
	valam->setNumCursors(valDataSource->getLastPosition());
	Block* valBlock;
	valBlock=valDataSource->getNextValBlock(0);
	//for now assume values are integers ... fix later if need be ... but maybe don't need to since now there is Type2DecompDataSource
	ValPos* vp;
	while (valBlock!=NULL) {
	  vp = valBlock->getNext();
	  assert(vp->type == ValPos::INTTYPE);
	  int value=*(int*)vp->value;
		Log::writeToLog("Type2DataSource", 0, "In posContig init: read value",value);				
		byte* page;
		if (cursorMap.count(value)==0) {
			page=(byte*)valam->getCursorIndex((char*)&value, cursorMap[value]);
			blockMap[value]=new Type2Block(valSorted);
			blockMap[value]->setBufferWithHeader(page);
			posContigNumValues++;		
			Log::writeToLog("Type2DataSource", 0, "In posContig init: opened am, name="+nameStream.str());				
		}
		else {
			throw new CodingException("Type2DataSource: Duplicate values in value column");	
		}
		
		if (!valBlock->hasNext()) {
			valBlock=valDataSource->getNextValBlock(0);
		}
	}
}

bool Type2DataSource::getNextValNormal(char*& value_) {
	// value filters not implemented
	Log::writeToLog("Type2DataSource", 0, "getNextValNormal(): finding new value subcolumn");				
	bool stopFlag=false;
	do {
	  if ((!currValBlock) || (!currValBlock->hasNext())) {
	    currValBlock=valDataSource->getNextValBlock(0);	
	    if (currValBlock==NULL) {
	      Log::writeToLog("Type2DataSource", 0, "getNextValNormal(): none found returning false");				
	      return false;
	    }
	  }
		ValPos* vp = currValBlock->getNext();
		value_=(char*)vp->value;
		//Log::writeToLog("Type2DataSource", 0, "getNextValNormal(): found (but unsure it meets pred) w/ value=", value_);			
		if (pred!=NULL) {
			pred->setLHS(vp);
			stopFlag=pred->evalPredicate();
			Log::writeToLog("Type2DataSource", 0, "getNextValNormal(): Predicate evaluated=", pred->evalPredicate());	
		}
		else {
			Log::writeToLog("Type2DataSource", 0, "getNextValNormal(): Predicate NULL");	
			stopFlag=true;
		}
	} while (!stopFlag);	
	//Log::writeToLog("Type2DataSource", 0, "getNextValNormal(): found and meets pred w/ value=", value_);			
	return true;
}


Block* Type2DataSource::getNextValBlockPosContig() {
	// Not implemented: value && position filters
	if (valFilterChanged || posFilterChanged) {
		Log::writeToLog("Type2DataSource", 1, "getNextValBlockPosContig(): pred or filter changed, initializing cursors on AM");			
		valFilterChanged=false;
		posFilterChanged=false;	
	}	
	
	if (predChanged) {
	  //throw an exception, though the code below might work if you uncomment it
	  //throw new UnimplementedException("Untested so throw an exception for now");
	  	predChanged=false;
		byte* page;
		// initialize all the cursors
		for (map<int, int>::iterator iterPos=cursorMap.begin();
		     iterPos!=cursorMap.end(); 
		     ++iterPos) {
		  page=(byte*)valam->resetCursorIndex((char*)&iterPos->first, iterPos->second);
		  blockMap[iterPos->first]->setBufferWithHeader(page);
		  }
	  
		populateBlocksMap();
		loadPairs();

	}
	//else
	//  populateBlocksMap();
	
	// Now we need to build up the basic blocks to output
	// The stategy is to first get a page in all decoders, to put a block
	// In the block Map, we go through a pair in each single column and
	// place it in a map<position, value>
	// we then pop off this map to get contiguous postions
	// A coding expection is thrown if the positions were not contiguous
	
	
	
	if (posValueMap.size()==0)
		return NULL;
	
	//Log::writeToLog("Type2DataSource", 0, "getNextValPosContig(): pushing off map");
	map<int, int>::iterator x = posValueMap.begin();
	int pos=x->first;	
	int val=x->second;	
	//Log::writeToLog("Type2DataSource", 0, "getNextValPosContig(): returning val=", val);	
	//Log::writeToLog("Type2DataSource", 0, "getNextValPosContig(): returning pos=", pos);	
	posValueMap.erase(pos);
	//valPosMap.erase(val); // so we know this value no longer in buffer (i.e. we need to pull in another pair)
	posContigOutBlock->setValue(val, pos);
	Type2Block* currBlock=blockMap[val];
	if (currBlock!=NULL) {
	  //if (currBlock->hasNext()) {
	  ValPos* pair=currBlock->getNext();
	  if (pair) {
	    //assume type int for now ... fix later
	    assert(pair->type == ValPos::INTTYPE);
	    posValueMap[pair->position]=*(int*)pair->value;
	    //valPosMap[pair->value]=pair->position;
	  }
	  if (!currBlock->hasNext()) {
	    //Log::writeToLog("Type2DataSource", 0, "loadPairs got NULL pair");
	    populateBlocksMap();
	    //Type2Block* currBlock=blockMap[val];
	    //assert(currBlock!=NULL);
	    //Pair* pair=currBlock->getNext();
	    //posValueMap[pair->position]=pair->value; 
	  }
	}		
	return posContigOutBlock;
}
void Type2DataSource::populateBlocksMap() {
	if (pred==NULL)	{
		Log::writeToLog("Type2DataSource", 0, "populateBlockMaps(): no pred: populate Maps with begin and end");
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
void Type2DataSource::populateBlockMaps(BlockTMapIterator startIter,
											BlockTMapIterator endIter) {
	for (BlockTMapIterator iterPos=startIter;
					   iterPos!=endIter; 
					   ++iterPos) {
		int currValue=iterPos->first;
		Type2Block* currBlock=iterPos->second;
		if ((currBlock==NULL) || (!currBlock->hasNext())) {	
		  //Log::writeToLog("Type2DataSource", 2, "Populating block map: getting page val=",currValue);			
			byte* page=(byte*) valam->getNextPagePrimaryDup(cursorMap[currValue]);
			if (page!=NULL) {
				// as we know only one block per page
				currBlock->setBufferWithHeader(page);
				posContigDecoder->setBuffer(page);
				//Log::writeToLog("Type2DataSource", 2, "Populating block map: page!=NULL with val=",currBlock->getValue());			
				//Log::writeToLog("Type2DataSource", 2, "Populating block map: page!=NULL with endPos=",posContigDecoder->getEndPos());				
				//Log::writeToLog("Type2DataSource", 2, "Populating block map: page!=NULL with startPos=",posContigDecoder->getStartPos());
				//Log::writeToLog("Type2DataSource", 2, "Populating block map: page!=NULL with blockSize=",currBlock->getSize());
			}
			else {
			  //Log::writeToLog("Type2DataSource", 2, "Removing val from BlockMap, AM returned NULL, val",currValue); 
				blockMap.erase(currValue);	
			}			
		}
	}
}
void Type2DataSource::loadPairs() {
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
void Type2DataSource::loadPairs(BlockTMapIterator startIter, BlockTMapIterator endIter) {
	for (BlockTMapIterator iterPos=startIter; iterPos!=endIter; ++iterPos) {
		int currVal=iterPos->first;
		Type2Block* currBlock=blockMap[currVal];
		//Log::writeToLog("Type2DataSource", 1, "loadPairs for value",currVal);				
		//int currValCount=valPosMap.count(currVal);
		//Log::writeToLog("Type2DataSource", 0, "loadPairs currValCount",currValCount);				
		if ((currBlock!=NULL)/* && (currValCount<1)*/) {
		  //Log::writeToLog("Type2DataSource", 0, "loadPairs currBlock, value",currBlock->getValue());				
		  //Log::writeToLog("Type2DataSource", 0, "loadPairs currBlock, size",currBlock->getSize());				

			if (currBlock->hasNext()) {
			  //assume type int for now .. fix later
				ValPos* pair=currBlock->getNext();
				assert(pair->type == ValPos::INTTYPE);
				//Log::writeToLog("Type2DataSource", 0, "loadPairs: loading with pos",pair->position);				
				//Log::writeToLog("Type2DataSource", 0, "loadPairs: loading with value",pair->value);				
				posValueMap[pair->position]=*(int*)pair->value;
				//valPosMap[pair->value]=pair->position;
			}
			else {
			  //Log::writeToLog("Type2DataSource", 0, "loadPairs got NULL pair");							
			}
		}												
	}
}

bool Type2DataSource::checkOutBlock(bool& donewithval_) {
  if (posFilter!=NULL) {
    //for now assume 1 RLE triple
    donewithval_=false;
    unsigned int minpos=outBlock->getStartPosition();
    unsigned int maxpos=outBlock->getEndPosition();
    unsigned filterStart=DataSource::posFilterStart;
    unsigned filterEnd=DataSource::posFilterEnd;
    assert (filterEnd !=0);
    if (maxpos < filterStart)
      return false;
    if (minpos > filterEnd) {
      donewithval_ = true;
      return false;
    }
    if (minpos < filterStart) {
      outBlock->cutOffStart(filterStart);
      if (!outBlock->getStartPair())
	return false;
    }
    if (filterEnd < maxpos) {
      outBlock->cutOffEnd(filterEnd);
      if (!outBlock->getStartPair())
	return false;
      //donewithval_ = true;
    }
    return true;
	      
  } else {
      return true;
  }
}

Block* Type2DataSource::getNextValBlockNormal() {
  Log::writeToLog("Type2DataSource", 0, "called getNextValBlockNormal()");		
  unsigned int filterStart, filterEnd;
	// Not implemented: value filters && position filters
	if (predChanged || valFilterChanged || posFilterChanged) {
		Log::writeToLog("Type2DataSource", 1, "getNextValBlockNormal(): pred or filter changed, initializing cursors on AM");			
		am->initCursors();
		predChanged=false;
		valFilterChanged=false;
		posFilterChanged=false;	
		//currColumnAM=NULL;
		valSet=false;
		if (posFilter!=NULL) {
		  bool temp = getPosFilterStartEnd(filterStart, filterEnd, true);
		  assert(temp);
		}
	}
	byte* page;
	bool doneWithVal=false;
	do {
	  if (!valSet) {
	    Log::writeToLog("Type2DataSource", 1, "getNextValBlockNormal(): currcolumnAM NULL, getting AM to new");			
	    //int value=1;
	    char* value = NULL;
	    if (!getNextValNormal(value)) {
	      Log::writeToLog("Type2DataSource", 1, "getNextValBlockNormal(): no more value subcolumns to open, returning NULL");			
	      return NULL;	
	    }
	    page=(byte*) changeValueNormal(value);
	  }
	  /*else if (doneWithVal) {		
	    int value=1;
	    if (!getNextValNormal(value)) {
	      return NULL;	
	    }
	    page=(byte*) changeValueNormal(value);
	    }*/
	  // as we know there is only one delta pos block per page
	  else
	    page=(byte*) valam->getNextPagePrimaryDup();
	  while (page==NULL) {
	    char* value=NULL;
	    if (!getNextValNormal(value)) {
	      Log::writeToLog("Type2DataSource", 0, "getNextValBlockNormal(): getNextValNormal returned false, therefore returning NULL");				
	      return NULL;
	    }
	    page=(byte*) changeValueNormal(value);	
	    //page=(byte*) currColumnAM->getNextPage();
	  }
	  Log::writeToLog("Type2DataSource", 0, "getNextValBlockNormal(): have a page from currAM");			
	  currColumnDecoder->setBuffer(page);
	  outBlock = (Type2Block*)currColumnDecoder->getNextBlock();
	} while (!checkOutBlock(doneWithVal));
	  return outBlock; //as we know only one delta pos block per page
}

// Gets the next value block from the operator 
Block* Type2DataSource::getNextValBlock(int colIndex_) {
	Log::writeToLog("Type2DataSource", 0, "called getNextValBlock()");			
	if (colIndex_==0) {
		if (mode==Type2DataSource::UNINIT) {
			setMode(false);	
			return getNextValBlockNormal();
		}
		else if (mode==Type2DataSource::POS_CONTIG) {
			throw new UnexpectedException("Type2DataSource: Error cannot read data source in both modes simultaneously");
		}
		else 
			return getNextValBlockNormal();
	}
	else if (colIndex_==1) {
		if (mode==Type2DataSource::UNINIT) {
			setMode(true);	
			return getNextValBlockPosContig();
		}
		else if (mode==Type2DataSource::NORMAL) {
			throw new UnexpectedException("Type2DataSource: Error cannot read data source in both modes simultaneously");
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
Block* Type2DataSource::skipToValBlock(int colIndex_, int val_) {
	// not implemented
	return NULL;
}
	
	
// Gets the next position block (bitstring of positions) from the operator
PosBlock* Type2DataSource::getNextPosBlock(int colIndex_) {
	if (colIndex_==0) {
		Type2Block* block=(Type2Block*) getNextValBlock(0);
		if (!block)
		  return NULL;
		//byte* bwh = block->getBuffer();
		posOutBlock->setBlock(block);
		return posOutBlock;
	}
	else {
		return NULL;
	}
}	

// Skips to the first block with this position
// Returns NULL if the value is outside the range of the column
PosBlock* Type2DataSource::skipToPosBlock(int colIndex_, int pos_) {
	// not implemented
	return NULL;
}		


//printColumn, print to stringstream instead of stdout. for testing purpose
void Type2DataSource::printColumn(stringstream& ss) {
    using namespace std;
    	int numValues=0;
	Type2Block* blk=(Type2Block*) getNextValBlock(0);
	ss << "(Value,Size): "; 
	while (blk!=NULL) {
		ss << "(" << blk->getValue() << "," << blk->getSize() << "), ";
		numValues++;
		blk=(Type2Block*) getNextValBlock(0);
	}
	ss << endl;
}
