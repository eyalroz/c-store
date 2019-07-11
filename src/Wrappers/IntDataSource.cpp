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
// Uncompressed Int Datasource
// Problems: migfer@mit.edu

#include "IntDataSource.h"

IntDataSource::IntDataSource(AM* am_, bool valSorted_, bool isROS) : DataSource(am_, isROS)
{

#ifndef MINICSTORE
  if (!( dynamic_cast<DVAM*>( am ) ))
    {
      if (isROS)
	{
	  //cout << " Create rosam, create rosam, " << am->getTableName() << " indexes " << am->getNumIndices() << endl ;
	  if (CatalogInstance::getCat())
	    posAm = CatalogInstance::getCatalog()->getROSAM( ((ROSAM*)am)->getColumnName() );
	  else {
	    posAm=new ROSAM(((ROSAM*)am)->getTableName(), ((ROSAM*)am)->getNumIndices());
	  }
	  
	}
      else
	{
	  // back to calling old constructor.
	  // restructuring in progress.
	  //int before = StopWatch::ticks();
	  //posAm=new WOSAM(am->getTableName(), ((WOSAM*)am)->getNumColumns(), ((WOSAM*)am)->getFieldNum()/*, ((WOSAM*)am)->getEncoder(), ((WOSAM*)am)->getDecoder()*/);
	  //int after = StopWatch::ticks();
	  
	  //cout << " TIME " << (after-before)*StopWatch::secondsPerTick() << endl;
	  cout << " Creating posam in data source " << am->getTableName()<< " field num? " << ((WOSAM*)am)->getFieldNum() << endl;
	  if (CatalogInstance::getCat())
	    posAm=CatalogInstance::getCatalog()->getWOSAM( ((WOSAM*)am)->getTableName(), ((WOSAM*)am)->getFieldNum() );
	  else
	    posAm = new WOSAM(((WOSAM*)am)->getTableName(), ((WOSAM*)am)->getNumIndices(), ((WOSAM*)am)->getNumColumns(), ((WOSAM*)am)->getFieldNum()/*, ((WOSAM*)am)->getEncoder(), ((WOSAM*)am)->getDecoder()*/);
	}
    }
  else
    //cout << "DataSource: We will not construct posAM because this is a DVAM. " << endl;
    //cout << "DataSource: in order to do position filter posAm is needed ? yes " << endl;
    //+ let velen know if you posAm get you in trouble - We need it for posFilter
    posAm = am;
#else
  posAm=am;
#endif
  
	valSorted=valSorted_;

	if (am_==NULL) throw new UnexpectedException("IntDataSource::getNextValBlock: Error, am is NULL");
	am->initCursors();
	currBlock=NULL;
	init=true;
	decoder=new IntDecoder(valSorted_);
	//valOutBlock=new MultiBlock(valSorted_, true, true);
	posOutBlock=NULL;
	posOutTripleOnPred=new RLETriple;
	if (isROS)
	  getSingles = false;
	else
	  getSingles = true;
	//tempBlock=NULL;
	useWP=false;
	pba=NULL;
	currBlockWP=new BlockWithPos();
	matchedPred = new Type2Block(false);
	matchedPredPos = new PosType2Block();
	//realpba = new PosBlock*[1];
}

IntDataSource::IntDataSource(const IntDataSource& src_) : DataSource(src_) {
  //Want to do: am=src_.am->clone();
  //but since this is not implemented, just throw an exception for now ...
  throw new UnimplementedException("Is this ever used?");

  /*  if (!( dynamic_cast<DVAM*>( src_.am ) ))
    {
      if (isROS)
	{
	  //cout << " Create rosam, create rosam, " << am->getTableName() << " indexes " << am->getNumIndices() << endl ;
	  posAm=new ROSAM(((ROSAM*)am)->getTableName(), ((ROSAM*)am)->getNumIndices());
	}
      else
	{
	  //cout << " Creating posam in data source " << endl;
	  if (CatalogInstance::getCat())
	    posAm= CatalogInstance::getCatalog()->getWOSAM( ((WOSAM*)am)->getTableName(), ((WOSAM*)am)->getFieldNum() );
	  else
	  posAm = new WOSAM(((WOSAM*)am)->getTableName(), ((WOSAM*)am)->getNumIndices(), ((WOSAM*)am)->getNumColumns(), ((WOSAM*)am)->getFieldNum()*//*, ((WOSAM*)am)->getEncoder(), ((WOSAM*)am)->getDecoder()*//*);
	}
    }
  else
    cout << "DataSource: We will not construct posAM because this is a DVAM. " << endl;
  
	currBlock=NULL;
	valSorted=src_.valSorted;
	decoder=new IntDecoder(valSorted);
	posOutBlock=NULL;
	//valOutBlock=new MultiBlock(valSorted, true, true);
	posOutTripleOnPred=new RLETriple;
	getSingles = src_.getSingles;
	tempBlock=new MultiBlock(valSorted, true, true);
	currBlockWP=new BlockWithPos();
	useWP=src_.useWP;
	pba=NULL;
	matchedPred = new Type2Block(false);
	matchedPredPos = new PosType2Block();
	//realpba = new PosBlock*[1];
*/
}
IntDataSource::~IntDataSource()
{

  /*if (decoder!=NULL) delete decoder;
	if (currBlock!=NULL) delete currBlock;
	delete valOutBlock;
	if (posOutBlock!=NULL) delete posOutBlock;
	delete posOutTripleOnPred;*/
  //if (tempBlock) delete tempBlock;
	//delete currBlockWP;
	//delete[] realpba;
}

// checks to make sure that for a given page,
// we have the right block within that page
// NOTE: this code is terrible...
bool IntDataSource::checkOutBlock(bool& done_) {
  //bool currReturnVal = true;
  if (valFilter!=NULL) {
    
  }
  if ((posFilter!=NULL)) {
    if (isROS) {
      unsigned int pos=currBlock->peekNext()->position;
      unsigned filterStart=DataSource::posFilterStart;
      unsigned filterEnd=DataSource::posFilterEnd;
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
	  theRLEBlock = new RLEBlock(new  RLETriple(tempPair, position, currPosBlock->getSize()), valSorted);
	}
	else {
	  
	  currBlockWP->setBlock(currBlock);
	  useWP=true;
	  //if (pba) {
	  //  delete pba; DSM we'll do this in BlockWithPos now
	  //	pba = NULL;
	  //}
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
      // skip to right pos on page? ... for now WOS just 1 page
      //skipToRightPosOnPage(decoder);
      //return true;
    }
  }
  if (pred!=NULL) {
    if (isROS && !getSingles) {
      Block* b;
      if (posFilter==NULL) {
	currBlockWP->setBlock(currBlock);
	useWP=true;
	b = currBlock;
      }
      else
	b = currBlockWP;

#ifndef MATERIAL_EXPS
      bool x = b->applyPredicate(matchedPred,pred);
      if (!x) return false;
#else 
      //t2b = new Type2Block(false);
      ValPos* pair=b->getNext();
      //currBlock->resetBlock();
      //int value=pair->value;		
      pred->setLHS(pair);
      int rhsint = *(int*)pred->getRHS()->value;
      //material experiments so I know int only
      assert(pair->type == ValPos::INTTYPE);
      while ((!pred->evalPredicate(rhsint)) && (b->hasNext())) {
	pair = b->getNext();		
	pred->setLHS(pair); 
      }
      if (pred->evalPredicate(rhsint)) { //we stopped while loop via 1st end condition
	matchedPred->initEmptyBuffer(pair->position, pair);
	matchedPred->addPosition(pair->position);
      }
      else return false;
      int currEndPos = pair->position;
      while (b->hasNext()) {
	pair=b->getNext();		
	pred->setLHS(pair);
	if (pred->evalPredicate(rhsint)) {
	  matchedPred->addPosition(pair->position);
	  currEndPos = pair->position;
	}
      }
      matchedPred->finishBufferEditing(currEndPos);

#endif

      matchedPredPos->setBlock(matchedPred);
      pba = matchedPredPos->getIterator();
      if (!pba->getEndPos())
	return false;
      currBlockWP->setPosBlockIter(pba);
      assert (currBlockWP->hasNext());
      return true;
    }
    else {
      ValPos* pair=currBlock->peekNext();
      //int value=pair->value;		
      pred->setLHS(pair);
      //assume int type
      assert(pred->getRHS()->type == ValPos::INTTYPE);
      int rhsint = *(int*)pred->getRHS()->value;
      if (!pred->evalPredicate(rhsint)) {
	//Log::writeToLog("IntDataSource", 0, "checkBlock(): FALSE");
	while (currBlock->hasNext()) {
	  pred->setLHS(currBlock->peekNext());
	  if (pred->evalPredicate(rhsint))
	    break;
	  currBlock->getNext();
	}
	if (currBlock->hasNext()) {
	  //pred->setLHS(currBlock->getPairAtLoc(currBlock->getSize()-1)->value);
	  return true;
	} else
	  return false;
      }
      
    }
  }
  // Log::writeToLog("IntDataSource", 0, "checkBlock(): TRUE");			
  return true;
}

Block* IntDataSource::getDecodedBlock(Decoder* decoder) {
  if (getSingles)
    return decoder->getNextBlockSingle();
  else
    return decoder->getNextBlock();
}
Block* IntDataSource::getNextValBlock(int colIndex_) {
	if (colIndex_!=0) return NULL;
	else return getNextValBlock(am);
}
// Gets the next value block from the operator 
Block* IntDataSource::getNextValBlock(AM* am_) {
  bool done=false;
  //unsigned int fs, fe;
  do {
    //if (posFilter!=NULL && havePosBlock) {
    //if ((getPosFilterStartEnd(fs, fe, false)) && (fs==fe)) {
    if (getSingles && posFilter!=NULL) {
      
      posFilterChanged=true;
      havePosBlock=false;
      posFilterEnd=0;
      byte* page=getRightPage(am_, decoder, valSorted, !valSorted);
      if (page==NULL) {
	Log::writeToLog("IntDataSource", 0, "getNextValBlock(): got a NULL page, returning");			
	return NULL;
      }
      decoder->setBuffer(page);
      if (posFilter!=NULL)
	skipToRightPosOnPage(decoder);
      currBlock=(MultiBlock*) getDecodedBlock(decoder);
    }
    //}
    else
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

// Gets the value block from the operator that contains position pos
Block* IntDataSource::getNextValBlockAtPos(int colIndex_, unsigned int pos) {
  if (colIndex_!=0) return NULL;
  byte* page = (byte*)skipToPagePosition(pos);
  if (page==NULL) {
    return NULL;
  }
  decoder->setBuffer(page);
  bool x = decoder->skipToPos(pos-decoder->getStartPos());
  assert(x);
  //Log::writeToLog("IntDataSource", 0, "getNextValBlock(): got a page with startPos:", decoder->getStartPos());
  currBlock=(MultiBlock*) getDecodedBlock(decoder);
  assert(currBlock);
  bool done = false;
  x = checkOutBlock(done);
  assert(x);
  assert(!done);
  //if (currBlock != NULL)
  //  break;
  return currBlock;
}

// Skips to the first block with this value
// Returns NULL if the value is outside the range of the column
Block* IntDataSource::skipToValBlock(int colIndex_, int val_) {
	// not implemented
	return NULL;	
}
// Gets the next position block (bitstring of positions) from the operator
PosBlock* IntDataSource::getNextPosBlock(int colIndex_) {
  if (colIndex_!=0) return NULL;
  if (valSorted && isROS) {	
    //Log::writeToLog("IntDataSource", 0, "getNextPosBlock(), called on valSorted col");				
    if (!posOutTripleOnPred->isNull()) return NULL;
    if (posOutBlock==NULL) posOutBlock=new PosRLEBlock();
    
    if (pred!=NULL) {
      // I need to find the first or last postion for a given value, depending on predicate
      // find position where binding on predicate occured
      IntDecoder* decoder=new IntDecoder(valSorted);
      unsigned int position=0;
      unsigned int minPos=1;
      unsigned int maxPos=getLastPosition();
      byte* page=NULL;
      //assume integer type for now ... fix later (unless DataSource replaces IntDataSource) .. acutally since this is 'IntDataSource' it is probably ok to assume ints
      assert(pred->getRHS()->type == ValPos::INTTYPE);
      int rhsint = *(int*)pred->getRHS()->value;
      int temp = rhsint+1;
      switch	(pred->getPredType()) {
      case Predicate::OP_GREATER_THAN:
	page=(byte*) skipToPageValue((char*)&temp, posAm);
	if (page!=NULL) {
	  decoder->setBuffer(page);
	  if (decoder->skipToBlockOnValue(rhsint+1))
	    position=((MultiBlock*) getDecodedBlock(decoder))->getPosition();
	}
	else return NULL;
	if (position==0) posOutTripleOnPred->setTriple(NULL, minPos, maxPos-minPos+1);
	if (position==maxPos) return NULL;
	else posOutTripleOnPred->setTriple(NULL, position, maxPos-position+1);
	break;
      case Predicate::OP_GREATER_THAN_OR_EQUAL: 	
	page=(byte*) skipToPageValue((char*)&rhsint, posAm);
	if (page!=NULL) {
	  decoder->setBuffer(page);
	  if (decoder->skipToBlockOnValue(rhsint))
	    position=((MultiBlock*) getDecodedBlock(decoder))->getPosition();
	}
	if (position==0) posOutTripleOnPred->setTriple(NULL, minPos, maxPos-minPos+1);
	if (position==maxPos) return NULL;
	else posOutTripleOnPred->setTriple(NULL, position, maxPos-position+1);
	break;
      case Predicate::OP_EQUAL: 	
	page=(byte*) skipToPageValue((char*)&rhsint, posAm);
	if (page!=NULL) {
	  decoder->setBuffer(page);
	  if (decoder->skipToBlockOnValue(rhsint))
	    position=((MultiBlock*) getDecodedBlock(decoder))->getPosition();
	}
	if (position==0) posOutTripleOnPred->setTriple(NULL, minPos, maxPos-minPos+1);
	if (position==maxPos) return NULL;
	
	unsigned int end;
	page=(byte*) skipToPageValue((char*)&temp, posAm);
	if (page!=NULL) {
	  decoder->setBuffer(page);
	  if (decoder->skipToBlockOnValue(temp))
	    end=((MultiBlock*) getDecodedBlock(decoder))->getPosition()-1;
	}
	else
	  end=0;
	if (position==0) position=minPos;
	if (end==0) posOutTripleOnPred->setTriple(NULL, position, maxPos-position+1);
	else if (position==maxPos) return NULL;					
	else posOutTripleOnPred->setTriple(NULL, position, end-position+1);
	break;
      case Predicate::OP_LESS_THAN:
	if (position==minPos) return NULL;
	page=(byte*) skipToPageValue((char*)&rhsint, posAm);
	if (page!=NULL) {
	  decoder->setBuffer(page);
	  if (decoder->skipToBlockOnValue(rhsint))
	    position=((MultiBlock*) getDecodedBlock(decoder))->getPosition();
	}
	else posOutTripleOnPred->setTriple(NULL, minPos, maxPos-minPos+1);
	position--;
	if (position<=0) return NULL;
	else posOutTripleOnPred->setTriple(NULL, minPos, position-minPos+1);
	break;
      case Predicate::OP_LESS_THAN_OR_EQUAL:
	if (position==minPos) return NULL;
	page=(byte*) skipToPageValue((char*)&temp, posAm);
	if (page!=NULL) {
	  decoder->setBuffer(page);
	  if (decoder->skipToBlockOnValue(rhsint+1))
	    position=((MultiBlock*) getDecodedBlock(decoder))->getPosition();
	}
	else posOutTripleOnPred->setTriple(NULL, minPos, maxPos-minPos+1);
	position--;
	if (position<=0) return NULL;
	else posOutTripleOnPred->setTriple(NULL, minPos, position-minPos+1);
	break;
	break;
      default:
	return NULL;
	break;
      }
      delete decoder;
      ((PosRLEBlock*) posOutBlock)->setTriple(posOutTripleOnPred);
      cout << "Returning posTriple: {" << posOutTripleOnPred->startPos
	   << ","
	   << posOutTripleOnPred->reps
	   << "}" << endl;
      return posOutBlock;
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
    //if (!currBlock || !currBlock->hasNext())
    assert(!RLEAlways);
    if ((!isROS) || getSingles) {
      setSingles(true);
      currBlock = (MultiBlock*)getNextValBlock(posAm);
      if (currBlock==NULL) return NULL;
      if (posOutBlock==NULL) posOutBlock=new PosMultiBlock(currBlock);
      else
	((PosMultiBlock*)posOutBlock)->setMB(currBlock);
      //Pair* p = currBlock->getNext();
      //((PosBasicBlock*) posOutBlock)->setPosition(p->position);
      return posOutBlock;
    }
    else {
      Block* b;
      b = getNextValBlock(posAm);
      if (b==NULL) return NULL;
      else return currBlockWP->getPosBlockIter();
    }
  }
}
// Skips to the first block with this position
// Returns NULL if the value is outside the range of the column
PosBlock* IntDataSource::skipToPosBlock(int colIndex_, int pos_) {
	// not implemented
	return NULL;	
}

//printColumn, print to stringstream instead. For testing purpose
void IntDataSource::printColumn(stringstream& ss) {
  using namespace std;
  int count=0;
  int total=0;
  int prevVal=-1;
  
  Block* blk=this->getNextValBlock(0);
  
  while (blk!=NULL) {
    while (blk->hasNext()) {
      int currVal=*(int*)blk->getNext()->value;
      if (prevVal!=currVal)  {
	if (count%10==0) ss  << endl << "Read uniques value: ";
	ss << currVal << ", ";
	count++;
	prevVal=currVal;
      }
      total++;
    }
    blk=this->getNextValBlock(0);
  }
  ss << endl;
  ss << "Total number of values read: " << total << endl;
}

/**
 * utility function to filter MultiBlock with predicate
 *
 */

/* Do not use any more!!!!! 
void IntDataSource::filterCurrBlockByPred() {
	if (pred != NULL  &&  currBlock != NULL  &&  !valSorted  &&  !getSingles) {
		if (tempBlock == NULL) tempBlock = new MultiBlock(valSorted, true, true);
		std::vector<int> vi;
		vi.reserve(currBlock->getSize());	
		Pair* pair = NULL;
		while (currBlock->hasNext()) {
			pair=currBlock->peekNext();
			pred->setLHS(pair->value);
			if (pred->evalPredicate()) {
				//cout << "\tpair->value:" << pair->value << " pos:" << pair->position <<endl;
				vi.push_back(pair->value);
			}
			currBlock->getNext();
		}
		if (vi.size() >0) {
			//cout << "\t\tvi.size():" << vi.size() << endl;
			int* buffer = new int[vi.size()];
			memset(buffer, 0, vi.size()*sizeof(int));
			for (unsigned int i=0; i < vi.size(); ++i) {
				buffer[i] = vi[i];
				//cout << "\tbuffer["<< i << "]:" << buffer[i] << endl;
			}
			tempBlock->setBuffer(1, vi.size(), (byte*)buffer);
			delete[] buffer;
		}
		currBlock = tempBlock;
	}
}
*/
