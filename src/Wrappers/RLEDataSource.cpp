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

// RLE Datasource
// Problems: migfer@mit.edu

#include "RLEDataSource.h"

// Construct an rle data source that :
//		-access data through am_
//		-writes blocks using writer_
RLEDataSource::RLEDataSource(AM* am_, bool valSorted_, RLESuperDecoder* decoder_)  : DataSource(am_) {	
	pageBuffer=NULL;
	//decoder=new RLEDecoder(valSorted_);
	if (decoder_) {
	  decoder=decoder_;
	  createdOwnDecoder = false;
	}
	else {
	  decoder=new RLEDecoder(valSorted_);
	  createdOwnDecoder = true;
	}
	outBlock=new RLEBlock(valSorted_);
	//outT2Block=new Type2Block(valSorted_);
	currTriple=NULL;
	nextFilteredTriple=new RLETriple();
	valSorted=valSorted_;
	getNewPosBlock=false;
 	posBlock=NULL;
 	 	
 	posInit=true;
	posDone=false;
	posIsTripleAvail=false;
	posTripleAvail=new RLETriple();
	posOutTriple=new RLETriple();
	currBlockIter = NULL;
	useWP=false;
	pba=NULL;
	currBlockWP=new BlockWithPos();
	getSingles=false;
	currPos=0;
}
	
// Copy constructor
RLEDataSource::RLEDataSource(const RLEDataSource& datasource_) : DataSource((DataSource) datasource_) {
	pageBuffer=NULL;	
	valSorted=datasource_.valSorted;
 	posBlock=NULL;
	currTriple=NULL;
	//decoder=new RLEDecoder(valSorted);
	decoder=datasource_.decoder;//really we should clone it
	outBlock=new RLEBlock(valSorted);
	nextFilteredTriple=new RLETriple();

 	posInit=true;
	posDone=false;
	posIsTripleAvail=false;
	posTripleAvail=new RLETriple();
	posOutTriple=new RLETriple();

	//outT2Block=new Type2Block(valSorted);
	currBlockIter = NULL;
	useWP=false;
	pba=NULL;
	currBlockWP=new BlockWithPos();
	getSingles=false;
	currPos=0;
}
	
// Destructor
RLEDataSource::~RLEDataSource()  
{
  if (currBlockIter)
    delete currBlockIter;
  delete currBlockWP;
  delete pba;
  delete posBlock;
  delete outBlock;
  delete nextFilteredTriple;
	
  delete posTripleAvail;
  delete posOutTriple;
  if (createdOwnDecoder)
    delete decoder;
  
}

// Gets the next value block from the am 
// (a block with a set of contiguous values but not necessarily contiguous positions)
Block* RLEDataSource::getNextValBlock(int colIndex_) {
	if (colIndex_!=0) return NULL;
	
	RLETriple* triplePtr=getNextTriple(am);
	if (triplePtr==NULL) {
		return NULL;
	}
	else {
	  if ((posFilter==NULL) || (!useWP)) {
	    outBlock->setTriple(triplePtr);
	    return outBlock;	
	  }
	  else
	    return currBlockWP;
	} 	
}

bool RLEDataSource::checkOutBlock(bool& done_) {
	ValPos* value=currTriple->value;
	//unsigned int pos=currTriple->startPos;
	//unsigned int reps=currTriple->reps;
	
	//Log::writeToLog("RLEDataSource", 0, "checkBlock(): checking value", value);			
	//Log::writeToLog("RLEDataSource", 0, "checkBlock(): checking startPos", pos);			
	//Log::writeToLog("RLEDataSource", 0, "checkBlock(): checking reps", reps);			
	if (valFilter!=NULL) {
		throw UnimplementedException("not done");
	}
	if (pred!=NULL) {
		pred->setLHS(value);
		ValPos* p = pred->getRHS();
		if (p->type==ValPos::INTTYPE) {
		  if (!pred->evalPredicate(*(int*)p->value)) {
		  //Log::writeToLog("RLEDataSource", 0, "checkBlock(): pred FALSE");			
			return false;
		  }
		}
		else if (!pred->evalPredicate()) {
		  //Log::writeToLog("RLEDataSource", 0, "checkBlock(): pred FALSE");			
		  return false;
		}
	}
	if (posFilter!=NULL) {
		return checkTripleOnPos(done_);
	}
	//Log::writeToLog("RLEDataSource", 0, "checkBlock(): pred TRUE");			
	return true;
}

bool RLEDataSource::checkTripleOnPos(bool& done_) {
  unsigned int startPos=currTriple->startPos;
  unsigned int endPos=currTriple->startPos+currTriple->reps-1;
  unsigned int filterStart;
  unsigned int filterEnd;
  
  getPosFilterStartEnd(filterStart, filterEnd, false);
  //filterStart = currPosBlock->getStartPos();
  //filterEnd = currPosBlock->getEndPos();

  while (startPos>filterEnd) {
    havePosBlock=false;
    if (!getPosFilterStartEnd(filterStart, filterEnd, true)) {
      done_=true;
      return false;
    }
    havePosBlock=true;
    //filterStart = currPosBlock->getStartPos();
    //filterEnd = currPosBlock->getEndPos();
  }
  
  outBlock->setTriple(currTriple);
  if (currBlockIter) 
    delete currBlockIter;
  if (filterStart<=startPos) {    
    if (filterEnd > endPos) {
      getNextNextTime=false;
      delete pba;
      pba = currPosBlock->getIterator(startPos, endPos);
      currBlockIter = outBlock->getIterator();
    }
    else {
      havePosBlock=false;
      getNextNextTime=true;
      //if (filterEnd > endPos) {
      //pba = currPosBlock->getIterator(startPos, endPos);
      //currBlockIter = outBlock->getIterator();
      //}
      //else {
      delete pba;
      pba = currPosBlock->getIterator(startPos, filterEnd);
      currBlockIter = outBlock->getIterator(startPos, filterEnd);
      if (endPos!=filterEnd)
	nextFilteredTriple->setTriple(currTriple->value, filterEnd+1, endPos-filterEnd);
      //}
    }
    if (!pba->getEndPos())
      return false;
    currBlockWP->setBlock(currBlockIter);
    useWP=true;
    currBlockWP->setPosBlockIter(pba);
    return true;
  }
  else { //startPos < filterStart
    if (endPos < filterStart) {
      currBlockIter = NULL;
      return false;
    }
    if (filterEnd > endPos) {
      getNextNextTime=false;
      delete pba;
      pba = currPosBlock->getIterator(filterStart, endPos);
      currBlockIter = outBlock->getIterator(filterStart, endPos);
    }
    else {
      havePosBlock=false;
      getNextNextTime=true;
      //if (filterEnd > endPos) {
      //pba = currPosBlock->getIterator(filterStart, endPos);
      //currBlockIter = outBlock->getIterator(filterStart, endPos);
      //}
      //else {
      delete pba;
      pba = currPosBlock->getIterator();
      currBlockIter = outBlock->getIterator(filterStart, filterEnd);
      if (endPos!=filterEnd)
	nextFilteredTriple->setTriple(currTriple->value, filterEnd+1, endPos-filterEnd);
      //}
    }
    if (!pba->getEndPos())
      return false;
    currBlockWP->setBlock(currBlockIter);
    useWP=true;
    currBlockWP->setPosBlockIter(pba);
    return true;
    
  }

  /*	if (currPosBlock->isPosContiguous()) {
	  
	  getPosFilterStartEnd(filterStart, filterEnd, false);
	  
	  //Log::writeToLog("RLEDataSource", 0, "PosFilter start is ", filterStart);
	  //Log::writeToLog("RLEDataSource", 0, "PosFilter end is ", filterEnd);
	  //Log::writeToLog("RLEDataSource", 0, "StartPos", startPos);			
	  //Log::writeToLog("RLEDataSource", 0, "EndPos", endPos);			
	  
	  while (startPos>filterEnd) {
	    //Log::writeToLog("RLEDataSource", 0, "Reading more PosFilter blocks");
	    // we need to get a new position block
	    if (!getPosFilterStartEnd(filterStart, filterEnd, true)) {
	      done_=true;
	      //Log::writeToLog("RLEDataSource", 0, "PosFilter exhausted returning FALSE");
	      return false;
	    }
	  }
	  
	  //lets now look at where triple fell within the filter
	  if ((filterStart<=startPos) && (endPos<=filterEnd) && (endPos>=filterStart)) {
	    getNextNextTime = false;
	    //Log::writeToLog("RLEDataSource", 0, "PosFilter returned TRUE: Case 1");
	    return true;
	  }
	  else if ((filterStart<=startPos) && (filterEnd<=endPos)) {
	    currTriple->setTriple(currTriple->value, startPos, filterEnd-startPos+1);	
	    if (filterEnd+1<=endPos)
	      nextFilteredTriple->setTriple(currTriple->value, filterEnd+1, endPos-filterEnd);
	    //Log::writeToLog("RLEDataSource", 0, "PosFilter returned TRUE, triple truncated: Case 2");
	    return true;
	  }
	  else if ((startPos<=filterStart) && (endPos<=filterEnd) && (endPos>=filterStart)) {
	    getNextNextTime = false;
	    currTriple->setTriple(currTriple->value, filterStart, endPos-filterStart+1);
	    //Log::writeToLog("RLEDataSource", 0, "PosFilter returned TRUE: Case 3");
	    return true;
	  }
	  else if ((startPos<=filterStart) && (filterEnd<=endPos) && (startPos<=filterEnd)) {
	    currTriple->setTriple(currTriple->value, filterStart, filterEnd-filterStart+1);
	    if (filterEnd+1<=endPos)
	      nextFilteredTriple->setTriple(currTriple->value, filterEnd+1, endPos-filterEnd);
	    //Log::writeToLog("RLEDataSource", 0, "PosFilter returned TRUE, triple truncated: Case 4");
	    return true;
	  }
	  else {
	    getNextNextTime = false;
	    //Log::writeToLog("RLEDataSource", 0, "PosFilter returned FALSE");
	    return false;
	  }
	}
	else {
	  //PosType2Block* pb = (PosType2Block*)currPosBlock;
	  PosBlock* pb = currPosBlock;
	  filterStart = pb->getStartPos();
	  filterEnd = pb->getEndPos();
	  while (startPos>filterEnd) {
	    havePosBlock=false;
	    if (!getPosFilterStartEnd(filterStart, filterEnd, true)) {
	      done_=true;
	      return false;
	    }
	    filterStart = pb->getStartPos();
	    filterEnd = pb->getEndPos();
	  }
	  if ((filterStart<=startPos) && (endPos<=filterEnd)) {
	    getNextNextTime = false;
	    outT2Block->setBufferWithHeader(pb->makeNewBuffer(currTriple->value, startPos, endPos));
	    if (!outT2Block->getStartPair())
	      return false;
	    return true;
	  }
	  else if ((filterStart<=startPos) && (filterEnd<=endPos)) {
	    outT2Block->setBufferWithHeader(pb->makeNewBuffer(currTriple->value, startPos, filterEnd));
	    getNextNextTime = true;
	    if (filterEnd+1<=endPos)
	      nextFilteredTriple->setTriple(currTriple->value, filterEnd+1, endPos-filterEnd);
	    if (!outT2Block->getStartPair())
	      return false;

	    return true;
	  }
	  else if ((startPos<=filterStart) && (endPos<=filterEnd) && (endPos>=filterStart)) {
	   getNextNextTime = false;
	    outT2Block->setBufferWithHeader(pb->makeNewBuffer(currTriple->value, filterStart, endPos));
	    if (!outT2Block->getStartPair())
	      return false;
	    return true;
	  }
	  else if ((startPos<=filterStart) && (filterEnd<=endPos) && (startPos<=filterEnd)) {
	    outT2Block->setBufferWithHeader(pb->makeNewBuffer(currTriple->value, filterStart, filterEnd));
	    getNextNextTime = true;	    
	    if (filterEnd+1<=endPos)
	      nextFilteredTriple->setTriple(currTriple->value, filterEnd+1, endPos-filterEnd);
	    if (!outT2Block->getStartPair())
	      return false;
	    return true;
	  }
	  else {
	    if (endPos<filterEnd)
	      getNextNextTime = false;
	    else
	      getNextNextTime = true;

	    return false;
	  }
	}
  */
}

RLETriple* RLEDataSource::getNextTripleFromDisk(AM* am_) {
	if (nextFilteredTriple->isNull()) {
		while (!decoder->hasNextBlock()) {
			byte* page=getRightPage(am_, decoder, valSorted, !valSorted);
			if (page==NULL) {
			  //Log::writeToLog("RLEDataSource", 0, "getNextValBlock(): got a NULL page, returning");			
				return NULL;
			}
			decoder->setBuffer(page); 	
			//if (posFilter!=NULL) skipToRightPosOnPage(decoder);
			if (posFilter!=NULL) {
			  bool temp;
			  temp = decoder->skipToBlockOnPos(currPosBlock->getStartPos());
			  //assert(temp);
			}
						
			//Log::writeToLog("RLEDataSource", 0, "getNextValBlock(): got a page with startPos:", decoder->getStartPos());			
		}
		Block* blk=decoder->getNextBlock();
		if (blk==NULL) throw new CodingException("RLEDatasource: decoder said I had blocks, but returned NULL");
		currTriple=((RLEBlock*) blk)->getTriple();
		return currTriple;
	}
	else {
	  //Log::writeToLog("RLEDataSource", 0, "getNextTripleFromDisk(), reading remaining part of triple", decoder->getStartPos());			
		currTriple->setTriple(nextFilteredTriple);
		nextFilteredTriple->setNull();
		return currTriple;
	}
	
}

RLETriple* RLEDataSource::getNextTriple(AM* am_) {
  bool done=false;
  if (getSingles && posFilter!=NULL) {
    havePosBlock=false;
    getNextNextTime=true;
    byte* page=getRightPage(am_, decoder, valSorted, !valSorted);
    if (page==NULL) {
      return NULL;
    }
    assert( dynamic_cast<PosBasicBlock*>( currPosBlock ));
    decoder->setBuffer(page); 	
    if (posFilter!=NULL) {
      bool temp;
      temp = decoder->skipToBlockOnPos(currPosBlock->getStartPos());
      assert(temp);
    }
    Block* blk=decoder->getNextBlock();
    if (blk==NULL) throw new CodingException("RLEDatasource: decoder said I had blocks, but returned NULL");
    currTriple=((RLEBlock*) blk)->getTriple();
    posOutTripleOnPred->setTriple(currTriple->value, currPos, currPosBlock->getNumOcurrences());
    //posOutTripleOnPred->value=currTriple->value;
    //posOutTripleOnPred->reps = currPosBlock->getNumOcurrences();
    //posOutTripleOnPred->startPos=currPos;
    currPos+=posOutTripleOnPred->reps;
    return posOutTripleOnPred;
  }
  else {
    do {
      currTriple=getNextTripleFromDisk(am_);		
      if (currTriple==NULL) {
	//Log::writeToLog("RLEDataSource", 0, "getNextTriple(): got NULL, returning NULL");
	return NULL;
      }
    } while (!checkOutBlock(done) && !done);
    if (done) {
      //Log::writeToLog("RLEDataSource", 0, "getNextTriple(): done is true, returning NULL");
      return NULL;
    }
    else {
      //Log::writeToLog("RLEDataSource", 0, "getNextTriple(): done is false, returning currTriple, val", currTriple->value);	
      return currTriple;	
    }
  }
}

// Gets the value block from the operator that contains position pos
Block* RLEDataSource::getNextValBlockAtPos(int colIndex_, unsigned int pos) {
  if (colIndex_!=0) return NULL;
  Block* currBlock;
  byte* page = (byte*)skipToPagePosition(pos);
  if (page==NULL) {
    return NULL;
  }
  decoder->setBuffer(page);
  bool x = decoder->skipToBlockOnPos(pos);
  assert(x);
  //Log::writeToLog("IntDataSource", 0, "getNextValBlock(): got a page with startPos:", decoder->getStartPos());
  currBlock=decoder->getNextBlock();
  assert(currBlock);
  currTriple=((RLEBlock*) currBlock)->getTriple();
  assert(currTriple);
  bool done = false;
  x = checkOutBlock(done);
  assert(x);
  assert(!done);
  outBlock->setTriple(currTriple);
  //if (currBlock != NULL)
  //  break;
  return outBlock;
}
	
// Skips to the first block with this value
// Returns false if the value is outside the range of the column
Block* RLEDataSource::skipToValBlock(int colIndex_, int val_) {
	if (colIndex_!=0) return NULL;
	return NULL;
}
	
// Gets the next pos block from the am 
// (a block with a set of contiguous positions but not necessarily contiguous values)
PosBlock* RLEDataSource::getNextPosBlock(int colIndex_) {
  if (colIndex_!=0) return NULL;
  // okay if we are sorted, we can be smart
  if (valSorted) {	
    //Log::writeToLog("RLEDataSource", 0, "getNextPosBlock(), called on valSorted col");				
    if (!posOutTripleOnPred->isNull()) return NULL;
    if (posBlock==NULL) posBlock=new PosRLEBlock();
    
    if (pred!=NULL) {
      // find position where binding on predicate occured
      RLEDecoder* decoder=new RLEDecoder(valSorted);
      //assume type int for now .. fix later
      assert(pred->getRHS()->type == ValPos::INTTYPE);
      int rhsint = *(int*)pred->getRHS()->value;
      byte* page=(byte*) skipToPageValue((char*)&rhsint, am);
      RLETriple* triple=NULL; 
      if (page!=NULL) {
	decoder->setBuffer(page);
	if (decoder->skipToBlockOnValue(rhsint))
	  {
	    triple=((RLEBlock*) decoder->getNextBlock())->getTriple();
	  }
	else 
	  triple=NULL;
      }
      
      
      unsigned int minPos=1;
      unsigned int maxPos=getLastPosition();
      
      if (triple==NULL) {
	// EDMOND: if there's no triple, we should return rather than everything
	/*
	  posOutTripleOnPred->setTriple(1, minPos, maxPos-minPos+1);
	  posBlock->setTriple(posOutTripleOnPred);
	  return posBlock;
	*/
	return NULL;
      }
      else {
	//Log::writeToLog("RLEDataSource", 0, "getNextPosBlock(), found triple with val:",triple->value);							
	unsigned int tripleStart=triple->startPos;
	unsigned int tripleEnd=triple->startPos+triple->reps-1;
	//Log::writeToLog("RLEDataSource", 0, "getNextPosBlock(), maximum pos is", maxPos);
	switch	(pred->getPredType()) {
	case Predicate::OP_GREATER_THAN: 	
	  if (tripleEnd==maxPos) return NULL;
	  posOutTripleOnPred->setTriple(NULL, tripleEnd+1, maxPos-tripleEnd);
	  break;
	case Predicate::OP_GREATER_THAN_OR_EQUAL: 	
	  posOutTripleOnPred->setTriple(NULL, tripleStart, maxPos-tripleStart+1);
	  break;
	case Predicate::OP_EQUAL: 	
	  posOutTripleOnPred->setTriple(NULL, tripleStart, triple->reps);
	  break;
	case Predicate::OP_LESS_THAN:
	  if (tripleStart==minPos) return NULL;
	  posOutTripleOnPred->setTriple(NULL, minPos, tripleStart-minPos);
	  break;
	case Predicate::OP_LESS_THAN_OR_EQUAL:
	  posOutTripleOnPred->setTriple(NULL, minPos, tripleEnd-minPos+1);						
	  break;
	default:
	  return NULL;
	  break;
	}
      }	
      delete decoder;
      posBlock->setTriple(posOutTripleOnPred);
      return posBlock;
    }
    else if (valFilter!=NULL)
      throw new UnimplementedException("Value filtering still unimplemented");					
    else {
      unsigned int minPos=1;
      unsigned int maxPos=getLastPosition();
      posOutTripleOnPred->setTriple(NULL, minPos, maxPos-minPos+1);
      posBlock->setTriple(posOutTripleOnPred);
      return posBlock;
    }
  }
  // The column is not value sorted
  else {
    if (posBlock==NULL) posBlock=new PosRLEBlock();
    if (posOutTriple==NULL) posOutTriple=new RLETriple();
    while (true) {
      if (posInit) {
	posInit=false;
	RLETriple* triple=this->getNextTriple(am);
	if (triple==NULL) return NULL;
	posTripleAvail->setTriple(triple);
	posIsTripleAvail=true;
      }
      if (!posIsTripleAvail) {
	RLETriple* triple=this->getNextTriple(am);
	if (triple==NULL)  {
	  if (!posOutTriple->isNull()) {
	    if (posDone) return NULL;
	    posBlock->setTriple(posOutTriple);						
	    posDone=true;
	    return posBlock;	
	  }
	  return NULL;
	}
	if (triple->startPos==(posOutTriple->startPos+posOutTriple->reps)) {
	  posOutTriple->reps+=triple->reps;	
	}
	else {
	  posTripleAvail->setTriple(triple);
	  posIsTripleAvail=true;
	  posBlock->setTriple(posOutTriple);
	  return posBlock;	
	}
      }
      else {
	posOutTriple->setTriple(posTripleAvail);
	posIsTripleAvail=false;
	
      }
    }		
  }
}

	
// Skips to the first block with this position
// Returns false if the value is outside the range of the column
// Returns false if the value does not exist in the column
// Skips to the previous block if position cannot be found for this value
PosBlock* RLEDataSource::skipToPosBlock(int colIndex_, int pos_) {
	if (colIndex_!=0) return NULL;
	return NULL;
}

	

void RLEDataSource::printColumn() {
	using namespace std;
	RLEDecoder* dcd=new RLEDecoder(valSorted);
	int count=0;
	byte* page=(byte*) getNextPageValue();
	while (page!=NULL) {
		dcd->setBuffer(page);
		while (dcd->hasNextBlock()) {
			RLETriple* trip=((RLEBlock*) dcd->getNextBlock())->getTriple();
			cout << "Triple #" << count << " val=" << trip->value
										<< " \tstPos=" << trip->startPos
										<< " \tReps=" << trip->reps << endl;
			count++;
		}

		page=(byte*) getNextPageValue();
	}
	delete dcd;
}

//another version of printColumn, but print to stringstream instead
void RLEDataSource::printColumn(stringstream& ss) {
    using namespace std;
	RLEDecoder* dcd=new RLEDecoder(valSorted);
    int count=0;
	byte* page=(byte*) getNextPageValue();
	while (page!=NULL) {
                   dcd->setBuffer(page);
                   while (dcd->hasNextBlock()) {
		     RLETriple* trip=((RLEBlock*) dcd->getNextBlock())->getTriple();
		     ss << "Triple #" << count << " val=" << trip->value
			<< " \tstPos=" << trip->startPos
			<< " \tReps=" << trip->reps << endl;
		     count++;
		   }
		   
		   page=(byte*) getNextPageValue();
	}
    delete dcd;
}

