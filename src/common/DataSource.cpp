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
#include "DataSource.h"

DataSource::DataSource(AM* am_) {
	am=am_;
	/*if (am->getNumIndices() == 1)
	  posPrimaryIndex=true;
	else
	posPrimaryIndex=false;*/
	posPrimaryIndex=true;

	/*		
		if (CatalogInstance::getCat())
		posAm = CatalogInstance::getCatalog()->getROSAM( ((ROSAM*)am)->getColumnName() );
		else
		posAm = new ROSAM(((ROSAM*)am)->getTableName(), ((ROSAM*)am)->getNumIndices());*/

	am->initCursors();
	valFilterChanged=true;
	posFilterChanged=true;
	predChanged=true;
	predLookup=true;

	valFilter=NULL;
	posFilterInit=true;
	posFilter=NULL;
	pred=NULL;

	getBlocksCurrPage=NULL;
	getBlocksCurrAM=NULL;
	getBlocksCurrDecoder=NULL;
	
	currPosBlock=NULL;
	havePosBlock=false;
	posFilterStart=0;
	posFilterEnd=0;
	posSkippedTo=0;
	isROS=true;
	getNextNextTime = true;
        RLEAlways=false;
	currBlockWP=NULL;
	posOutTripleOnPred=new RLETriple();
	matchedPred = NULL;
	matchedPredPos = NULL;
	pba=NULL;
	currRLETriple=NULL;
	theRLEBlock=NULL;
}

DataSource::DataSource(AM* am_, bool isROS_) {
  init(am_, isROS_);
  currBlockWP=NULL;
}

DataSource::DataSource(AM* am_, bool isROS_, bool valSorted_, Decoder* decoder_) {
  init(am_, isROS_);
  valSorted=valSorted_;
  if (am_==NULL) throw new UnexpectedException("IntDataSource::getNextValBlock: Error, am is NULL");
  decoder = decoder_;
  currBlock=NULL;
  posOutBlock=NULL;
  assert(isROS_); //for now generalized DataSource will only work with ROS
  //if (isROS)
  //  getSingles = false;
  //else
  //  getSingles = true;
  useWP=false;
  currBlockWP=new BlockWithPos();
}

void DataSource::init(AM* am_, bool isROS_) {
  am=am_;
  if (!isROS_)
    posPrimaryIndex=false;
  else {
    /*if (am->getNumIndices() == 1)
      posPrimaryIndex=true;
      else
      posPrimaryIndex=false;*/
    posPrimaryIndex=true;
  }
  isROS=isROS_;
  
  am->initCursors();
  valFilterChanged=true;
  posFilterChanged=true;
  predChanged=true;
  predLookup=true;
  
  valFilter=NULL;
  posFilterInit=true;
  posFilter=NULL;
  pred=NULL;
  
  getBlocksCurrPage=NULL;
  getBlocksCurrAM=NULL;
  getBlocksCurrDecoder=NULL;
  
  currPosBlock=NULL;
  havePosBlock=false;
  posFilterStart=0;
  posFilterEnd=0;
  posSkippedTo=0;	
  getNextNextTime = true;
  RLEAlways=false;
  matchedPred = new Type2Block(false);
  matchedPredPos = new PosType2Block();
  pba=NULL;
  posOutTripleOnPred=new RLETriple();
  currRLETriple=NULL;
  theRLEBlock=NULL;
}

DataSource::DataSource(const DataSource& src_) {
  //Want to do: am=src_.am->clone();
  //but since this is not implemented, just throw an exception for now ...
  throw new UnimplementedException("Is this ever used?");
  /*
	am->initCursors();
	valFilterChanged=true;
	posFilterChanged=true;
	predChanged=true;
	
	valFilter=src_.valFilter;
	posFilter=src_.posFilter;
	pred=src_.pred;
	
	getBlocksCurrPage=NULL;
	getBlocksCurrAM=NULL;
	getBlocksCurrDecoder=NULL;
	currPosBlock=NULL;
	getNextNextTime = true;
        RLEAlways=false;
	valSorted=src_.valSorted;
	decoder = src_.decoder;
	currBlock=NULL;
	posOutBlock=NULL;
	posOutTripleOnPred=new RLETriple;
	useWP=false;
	pba=NULL;
	currBlockWP=new BlockWithPos();
  */
}
DataSource::~DataSource() {	
  //cout << " Begin destructor of DataSource " << endl;
  /*  
      if (!( dynamic_cast<DVAM*>( am ) ))
      {
      if ( isROS )
      {
      if (CatalogInstance::getCat()) {
      CatalogInstance::getCatalog()->disposeOfROS( (ROSAM*)posAm );
      // second call is redundant, but should be safe,
      // since Catalog knows to check for nulls
      CatalogInstance::getCatalog()->disposeOfROS( (ROSAM*)am );
      }
      }
      else
      {
      if (CatalogInstance::getCat()) {
      CatalogInstance::getCatalog()->disposeOfWOS( (WOSAM*)posAm );
      CatalogInstance::getCatalog()->disposeOfWOS( (WOSAM*)am );
      }
      }
      }
      else
      {
      if (CatalogInstance::getCat()) 	
      CatalogInstance::getCatalog()->disposeOfDV( (DVAM*)am );
      else 
      {
      if (posAm!=NULL)
      delete posAm;
      }
      }
  */
  delete posOutTripleOnPred;
  delete currBlockWP;
  delete matchedPred;
  delete matchedPredPos;
  delete pba;
  delete currRLETriple;
  delete theRLEBlock;
  //  cout << " End destructor of DataSource " << endl;
}

// Changes the RHS binding for this datasource
void DataSource::changeRHSBinding(ValPos* rhs_) {
	if (pred==NULL) 
		throw new UnexpectedException("DataSource: Error, setting binding on NULL predicate");
	else { 
		pred->setRHS(rhs_);
		predChanged=true;
	}
}

// Changes the RHS binding for this datasource
void DataSource::changeRHSBinding(int rhsint) {
	if (pred==NULL) 
		throw new UnexpectedException("DataSource: Error, setting binding on NULL predicate");
	else { 
		pred->setRHS(rhsint);
		predChanged=true;
	}
}

// sets a new predicate for this datasource
void DataSource::setPredicate(Predicate* pred_) {
	Log::writeToLog("DataSource", 0, "Predicate set");
	predChanged=true;
	pred=pred_;
}	

// Sets a filter for values
void DataSource::setValueFilter(Operator* bitstringDataSource_, int colIndx_) {
	valFilterChanged=true;
	valFilter=bitstringDataSource_;
	valColIndex=colIndx_;
}	

// Sets a filter for postions
void DataSource::setPositionFilter(Operator* bitstringDataSource_, int colIndx_) {
	posFilterChanged=true;
	posFilter=bitstringDataSource_;
	posColIndex=colIndx_;
}

byte* DataSource::getValSortedGetPage(AM* am_) {
	byte* pageBuffer=NULL;
	bool lookupOnValue=false;
	//for now assume value is an integer ... fix later ...
	int value;
	if ((valFilter==NULL) && (posFilter==NULL) && (pred==NULL)) {
		Log::writeToLog("DataSource", 1, "Getting a page, no pred or filters...");
		pageBuffer=(byte*) getNextPageValue(am_);
		predChanged=false;
		posFilterChanged=false;
		valFilterChanged=false;
		if (pageBuffer==NULL) 
			return NULL;
		return pageBuffer;			
	}
		
	if ((pageBuffer==NULL  && predChanged)) {
		if (pred!=NULL) {
			predChanged=false;
			switch	(pred->getPredType()) {
				case Predicate::OP_GREATER_THAN: 	
				case Predicate::OP_GREATER_THAN_OR_EQUAL: 	
				case Predicate::OP_EQUAL:
					{
						lookupOnValue=true;
						//for now assume value is an integer ... fix later ...
						ValPos* vp = pred->getRHS();
						assert(vp->type == ValPos::INTTYPE);
						value=*(int*)vp->value;
					}
					break;
				case Predicate::OP_LESS_THAN:
				case Predicate::OP_LESS_THAN_OR_EQUAL:
					break;
				default:
					break;
			}
		}
		else {
			predChanged=false;
		}
	}
	
	if ((pageBuffer==NULL && valFilterChanged)) {
		if (valFilter!=NULL) {
			// worry about two situation:
			// Predicate is NULL or NOT
			// For now not implemented
			valFilterChanged=false;
		}	
		else {
			valFilterChanged=false;	
		}
	}
	
	if ((pageBuffer==NULL && posFilterChanged)) {
		if (posFilter!=NULL) {
			// not implemented
			valFilterChanged=false;
		}
		else {
			posFilterChanged=false;			
		}		
	}
	
	try {
		// now actually get the page
		if (lookupOnValue) {
			Log::writeToLog("DataSource", 1, "Getting a page on value=", value);
			pageBuffer=(byte*) skipToPageValue((char*)&value, am_);
			if (pageBuffer==NULL) return NULL;
			return pageBuffer;
		}
		else {
			Log::writeToLog("DataSource", 1, "Getting a page(post call on value) ...");
			pageBuffer=(byte*) getNextPageValue(am_);
			if (pageBuffer==NULL) return NULL;
			return pageBuffer;
		}
	}
	catch (AccessException& e_) {
		Log::writeToLog("DataSource", 5, e_.msg);
		return NULL;
	}
	
	if (pageBuffer==NULL) return NULL;
	else return pageBuffer;
}

Block* DataSource::getBlocks(AM* am_, Decoder* decoder_) {
	if (am_==NULL) throw new UnexpectedException("DataSource::getBlocks() AM arg was NULL");
	if (decoder_==NULL) throw new UnexpectedException("DataSource::getBlocks() Decoder arg was NULL");		

	if ((getBlocksCurrAM!=am_) || (getBlocksCurrAM==NULL)) {
		getBlocksCurrPage=NULL;
		getBlocksCurrAM=am_;
		am_->initCursors();
	}
	if ((getBlocksCurrDecoder!=decoder_) || (getBlocksCurrDecoder==NULL)) {
		getBlocksCurrPage=NULL;
		getBlocksCurrDecoder=decoder_;
	}
	
	
	if (getBlocksCurrPage==NULL) {
		getBlocksCurrPage=(byte*) getNextPageValue(am_);
		if (getBlocksCurrPage==NULL) {
			Log::writeToLog("DataSource", 1, "Got null page, returning NULL");
			return NULL;
		}
		decoder_->setBuffer(getBlocksCurrPage);
	}	

	while (!decoder_->hasNextBlock()) {
		getBlocksCurrPage=(byte*) getNextPageValue(am_);
		if (getBlocksCurrPage==NULL) {
			Log::writeToLog("DataSource", 1, "decoder had no more blocks, and got NULL page, returning NULL");
			return NULL;
		}
		decoder_->setBuffer(getBlocksCurrPage);
	}
	Log::writeToLog("DataSource", 1, "Returning next block in decoder");
	return decoder_->getNextBlockSingle();
}

/***************

getLastPosition returns the last position in the data source (column)

****************/
int DataSource::getLastPosition() {
  if (posPrimaryIndex)
    return *(int*)am->getLastIndexValuePrimary();
  else
    return *(int*)am->getLastIndexValueSecondary();
}

const void* DataSource::getNextPageValue() {
  if (posPrimaryIndex)
    return am->getNextPageSecondary();
  else
    return am->getNextPagePrimary();
}

const void* DataSource::skipToPageValue(char* key) {
  if (posPrimaryIndex)
    return am->skipToPageSecondary(key);
  else
    return am->skipToPagePrimary(key);
}

const void* DataSource::getNextPagePosition() {
  if (posPrimaryIndex)
    return am->getNextPagePrimary();
  else
    return am->getNextPageSecondary();
}

const void* DataSource::skipToPagePosition(int key) {
  if (posPrimaryIndex)
    return am->skipToPagePrimary((char*)&key);
  else
    return am->skipToPageSecondary((char*)&key);
}

const void* DataSource::getNextPageValue(AM* am) {
  if (posPrimaryIndex)
    return am->getNextPageSecondary();
  else
    return am->getNextPagePrimary();
}

const void* DataSource::skipToPageValue(char* key, AM* am) {
  if (posPrimaryIndex)
    return am->skipToPageSecondary(key);
  else
    return am->skipToPagePrimary(key);
}

const void* DataSource::getNextPagePosition(AM* am) {
  if (posPrimaryIndex)
    return am->getNextPagePrimary();
  else
    return am->getNextPageSecondary();
}

const void* DataSource::skipToPagePosition(int key, AM* am) {
  if (posPrimaryIndex)
    return am->skipToPagePrimary((char*)&key);
  else
    return am->skipToPageSecondary((char*)&key);
}


// checks to make sure that for a given page,
// we have the right block within that page
bool DataSource::checkOutBlock(bool& done_) {
  //bool currReturnVal = true;
  if (valFilter!=NULL) {
    
  }
  if ((posFilter!=NULL)) {
    if (isROS) {
      unsigned int pos=currBlock->peekNext()->position;
      unsigned filterStart=posFilterStart;
      unsigned filterEnd=posFilterEnd;
      if (filterEnd==0)
	if (!getPosFilterStartEnd(filterStart, filterEnd, false))
	  return false;    
      while (pos>filterEnd) {
	havePosBlock=false;
	if (!getPosFilterStartEnd(filterStart, filterEnd, true)) {
	  done_=true;
	  return false;
	}
	else {
	  skipToRightPosOnPage(decoder);
	}
      } 
      //right now, the way the code is written, I beleive the first half of this if statement is always true since we "find the right page" and the skip to the right position on that page
      //DNA: Actually, I think this code can be commented out since it is repeated below...
      /*	if (pos >= filterStart && ((MultiBlock*)currBlock)->getLastPosition() < posFilterEnd)
		getNextNextTime = false;*/
      
      // catch a corner case
      /*if (pos==filterEnd) {
	if (getPosFilterStartEnd(filterStart, filterEnd, true))
	skipToRightPosOnPage(decoder);
	return true;
	}*/
      
      //Again, see comment above, I believe filterStart===pos and since filterStart <= filterEnd, I don't see how this if statement is ever false
      if ((filterStart<=pos) && (pos<=filterEnd)) {
	if (RLEAlways) {
	  ValPos* tempPair = currBlock->getNext();
	  //int value = tempPair->value;
	  int position = tempPair->position;
	  delete currRLETriple;
	  delete theRLEBlock;
	  currRLETriple = new RLETriple(tempPair, position, currPosBlock->getSize());
	  theRLEBlock = new RLEBlock(currRLETriple, valSorted);
	}
	else {
	  
	  currBlockWP->setBlock(currBlock);
	  useWP=true;
	  if (pba) {
	    delete pba; //No longer done in BlockWithPos
	  	pba = NULL;
	  }
	  /* comment out for now
	     ((MultiBlock*)currBlock)->cutOffEnd(filterEnd);
	     posSkippedTo=0;
	  */
	  
	  //DNA: There seems to be a fundamental assumption here that a page = a block. If the end of the filter falls on the next page then gnnt gets set to false meaning that a new position block will not be fetched next time, rather the next value page is brought in. Otherwise it gets set to true, but how do we know we're really done with the position block? This is only true if a multiblock holds a whole page of data.
	  //DNA: maybe if I change decoder->getEndPos() to end pos in block ... does this negate above statement?		  
	  //realpba[0] = currPosBlock;
	  if (filterEnd > decoder->getEndPos()) {
	    getNextNextTime=false;
	    //pba = new PosBlockArray(realpba, 1, pos, decoder->getEndPos());
	    pba = currPosBlock->getIterator(pos, decoder->getEndPos());
	    if (!pba->getEndPos())
	      return false;
	  }
	  /*else if (filterEnd < decoder->getEndPos()) {
	    will probably have to fill this in later. For now, saying havePosBlock=false in block below will do ... not sure if this is unoptimal
	  }*/
	  else {
	    havePosBlock=false;
	    getNextNextTime=true;
	    //pba = new PosBlockArray(realpba, 1, pos, filterEnd);
	    pba = currPosBlock->getIterator(pos, filterEnd);
	    if (!pba->getEndPos())
	      return false;
	  }
	  currBlockWP->setPosBlockIter(pba);
	  
	}
	//return true;
      }
      else {
	return false;
      }
    } else {
      assert(false); //for now ...
      // skip to right pos on page? ... for now WOS just 1 page
      //skipToRightPosOnPage(decoder);
      //return true;
    }
  }
  if (pred!=NULL) {
    if (isROS) {
      Block* b;
      if (posFilter==NULL) {
	currBlockWP->setBlock(currBlock);
	useWP=true;
	b = currBlock;
      }
      else
	b = currBlockWP;

      bool x = b->applyPredicate(matchedPred,pred);
      if (!x) return false;
      matchedPredPos->setBlock(matchedPred);
      delete pba;
      pba = matchedPredPos->getIterator();
      if (!pba->getEndPos())
	return false;
      currBlockWP->setPosBlockIter(pba);
      assert (currBlockWP->hasNext());
      return true;
    }
  }
  return true;
}

Block* DataSource::getDecodedBlock(Decoder* decoder) {
  return decoder->getNextBlock();
}

Block* DataSource::getNextValBlock(int colIndex_) {
  if (colIndex_!=0) return NULL;
  else return getNextValBlock(am);
}

// Gets the next value block from the operator 
Block* DataSource::getNextValBlock(AM* am_) {
  bool done=false;
  //unsigned int fs, fe;
  do {
    //if (posFilter!=NULL && havePosBlock) {
    //if ((getPosFilterStartEnd(fs, fe, false)) && (fs==fe)) {
    currBlock=(MultiBlock*) getDecodedBlock(decoder);
    if (currBlock==NULL) {
      while (!decoder->hasNextBlock()) {
	byte* page=getRightPage(am_, decoder, valSorted, !valSorted);
	if (page==NULL) {
	  Log::writeToLog("IntDataSource", 0, "getNextValBlock(): got a NULL page, returning");			
	  return NULL;
	}
	decoder->setBuffer(page);
	if (posFilter!=NULL) 
	  skipToRightPosOnPage(decoder);			
	//Log::writeToLog("IntDataSource", 0, "getNextValBlock(): got a page with startPos:", decoder->getStartPos());
	currBlock=(MultiBlock*) getDecodedBlock(decoder);
	if (currBlock != NULL)
	  break;
      }
    }
  } while (!checkOutBlock(done) && !done);
  
  if (done) {
    return NULL;
  } else {
    //if (!valSorted) filterCurrBlockByPred();
    if (RLEAlways)
      return theRLEBlock;
    else if (useWP) {
      return currBlockWP;
      useWP=false;
    }
    else
      return currBlock;
  }
}

// Gets the next position block (bitstring of positions) from the operator
PosBlock* DataSource::getNextPosBlock(int colIndex_) {
  if (colIndex_!=0) return NULL;
  if (valSorted && isROS) {	
    //Log::writeToLog("IntDataSource", 0, "getNextPosBlock(), called on valSorted col");				
    if (!posOutTripleOnPred->isNull()) return NULL;
    if (posOutBlock==NULL) posOutBlock=new PosRLEBlock();
    
    if (pred!=NULL) {
      //Not sure how to write generalized code for this yet, so
      //I'll leave unimplemented for now
      throw new UnimplementedException("Figure out how to code this when it is actually needed");
    }
    else if (valFilter!=NULL)
      throw new UnimplementedException("Value filtering still unimplemented");					
    else {
      unsigned int minPos=1;
      unsigned int maxPos=getLastPosition();
      posOutTripleOnPred->setTriple(NULL, minPos, maxPos-minPos+1);
      ((PosRLEBlock*) posOutBlock)->setTriple(posOutTripleOnPred);
      cout << "Returning posTriple" << endl;
      return posOutBlock;
    }
  }
  // The column is not value sorted or is a WOS column
  else {
    Block* b;
    b = getNextValBlock(am);
    if (b==NULL) return NULL;
    else return currBlockWP->getPosBlockIter();
  }
}

ValPos** DataSource::createValueIndex() {
  Block* temp = getNextValBlock(0);
  if (decoder->getStartVal()->type != ValPos::INTTYPE) {
    ValPos** values = new ValPos*[TYPE2MAXVALUES];
    for (int i = 0; i < TYPE2MAXVALUES; i++) {
      values[i]=NULL;
    }
    int currindex = 0;
    while (temp) {
      if (!temp->hasNext()) {
	temp = getNextValBlock(0);
	if (!temp)
	  return values; //got NULL block, finished
      }
      values[currindex++] = temp->getNext();
    }
  }
  return NULL;
}
