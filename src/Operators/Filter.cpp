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
#include "Filter.h"

Filter::Filter(Operator* inputs[], int ninputs, Predicate* preds[], int npreds, int* predindexes)
{
  this->inputs = inputs;
  this->ninputs = ninputs;
  this->preds = preds;
  this->npreds = npreds;
  this->predindexes = predindexes;
  /*  if (ninputs == 1) {
      this->inputBlocks = new Block*[2];
      inputBlocks[0] = inputBlocks[1] = NULL;
      this->sb1 = new SMultiBlock(PAGE_SIZE/4, (3)*sizeof(int));
      this->sb2 = new SMultiBlock(PAGE_SIZE/4, (3)*sizeof(int));
      curpos = 1;
      exhausted = new bool[2];
      for (int i = 0; i < 2; i++)
      exhausted[i] = true;
      endposes = new unsigned int[2];
      for (int i = 0; i < 2; i++)
      endposes[i] = 0;
      iterArray = new BlockIter*[2];
      for (int i = 0; i < 2; i++)
      iterArray[i] = NULL;
      done = false;
      } else {*/
  this->inputBlocks = new Block*[ninputs];
  for (int i = 0; i < ninputs; i++) {
    inputBlocks[i] = NULL;
  }
  this->sb1 = new SMultiBlock(PAGE_SIZE/2, (ninputs+1)*sizeof(int));
  this->sb2 = new SMultiBlock(PAGE_SIZE/2, (ninputs+1)*sizeof(int));
  curpos = 1;
  exhausted = new bool[ninputs];
  for (int i = 0; i < ninputs; i++)
    exhausted[i] = true;
  endposes = new unsigned int[ninputs];
  for (int i = 0; i < ninputs; i++)
    endposes[i] = 0;
  iterArray = new BlockIter*[ninputs];
  for (int i = 0; i < ninputs; i++)
    iterArray[i] = NULL;
  done = false;
  //}
  dumbEM=true;
  doParallel = false;
  specialPred=NULL;
}

/*Filter::Filter(Operator* input) {
  this->inputs = new Operator*[1];
  inputs[0] = input;
  this->ninputs = 1;
  this->numtouse = 1;
  this->inputBlocks = new Block*[ninputs];
  for (int i = 0; i < ninputs; i++) {
  inputBlocks[i] = NULL;
  }
  this->sb = new SMultiBlock(BLOCKSIZE, numtouse*sizeof(int));
  }*/
	

Filter::~Filter()
{
  delete [] inputBlocks;
  delete sb1;
  delete sb2;
}

void Filter::makeSmart() {
  dumbEM=false;
}

void Filter::setDoParallel() {
  doParallel=true;
}

// Gets the next value block from the operator 
Block* Filter::getNextValBlock(int colIndex_) {
  throw new UnexpectedException("concat does not handle getNextValBlock");
}

SBlock* Filter::getNextSValBlock(int colIndex_) {
  if (npreds) {
    if (doParallel)
      return getNextSValBlockParallel(colIndex_);
    else if (dumbEM) {
      return getNextSValBlockPredDumb(colIndex_);
    }
    else {
      return getNextSValBlockPredNotDumb(colIndex_);
    }
  }
  else
    return getNextSValBlockNoPredicate(colIndex_);
}

/**
 * This operator needs to return the next SBlock worth of tuples. Recall that
 * in CStore, for efficiency, iterators forward blocks of tuples, not single
 * tuples. colIndex_ is completely ignored by this function.
 */
SBlock* Filter::getNextSValBlockPredDumb(int colIndex_) {
  assert(colIndex_==0);
  int* values = new int[ninputs+1];
  bool atLeastOneResult=false;
  while (!atLeastOneResult) {
    updateInputBlocks();
    if (done)
      return NULL;
    sb1->resetBlock();
    sb1->setStartPosition(curpos);
    sb2->resetBlock();
    sb2->setStartPosition(curpos);
    ValPos* pair;
    //assume int type for now ... fix later ...
    ValPos* vp = new IntValPos();
    int* rhsints = new int[npreds];
    for (int i=0; i<npreds;i++) {
      assert(preds[i]->getRHS()->type == ValPos::INTTYPE);
      rhsints[i]=*(int*)(preds[i]->getRHS()->value);
    }
    preds[0]->setLHS(vp);
    
    int sbpos = 0;
    int* currarray;
    int currarraysize;
    int startpos;
    if (iterArray[predindexes[0]]->hasNext()) {
      currarray = (int*) iterArray[predindexes[0]]->asArray();
      currarraysize = iterArray[predindexes[0]]->getSize();
      pair = iterArray[predindexes[0]]->getNext();
      startpos=pair->position;
      for (int i = startpos; i < startpos+currarraysize; i++) {
	//preds[0]->setLHS(currarray[i-startpos]);
	vp->set((byte*)(currarray+(i-startpos)));
	if (preds[0]->evalPredicate(rhsints[0])) {
	  values[0]=i;
	  values[predindexes[0]+1]=currarray[i-startpos];
	  sb1->setValueAtPos((char*)values, sbpos);
	  sbpos++;
	}
      }
    }
    //int sbstartpos = sb1->getStartPosition();
    SPair* spair;
    SMultiBlock* tempsb;
    sbpos=0;
    for (int i = 1; i <npreds; i++) {
      tempsb = sb2;
      sb2=sb1;
      sb1=tempsb;
      startpos = iterArray[predindexes[i]]->getStartPair()->position;
      currarray = (int*) iterArray[predindexes[i]]->asArray();
      currarraysize = iterArray[predindexes[i]]->getSize();
      preds[i]->setLHS(vp);
      while (sb2->hasNext()) {
	spair = sb2->getNext();
	memcpy((byte*)values, spair->value, (ninputs+1)*sizeof(int));
	assert((values[0]-startpos) < currarraysize);
	////pair = iterArray[predindexes[i]]->getPairAtLoc(values[0]-startpos);
	//preds[i]->setLHS(currarray[values[0]-startpos]);  
	vp->set((byte*)(currarray+(values[0]-startpos)));
	if (preds[i]->evalPredicate(rhsints[i])) {
	  values[predindexes[i]+1]=currarray[values[0]-startpos];
	  sb1->setValueAtPos((char*)values, sbpos);
	  sbpos++;
	}
      }
    }
    if (sb1->getSize() > 0)
      atLeastOneResult = true;
  }

  /* ok, now would be the point where I fill in the rest of the tuple
     but for now since experiments have a predicate on every attribute, I
     don't need to worry about that for now ...
  */
    
  //sb->setRealStartPos(sb->peekNext()->position);
  delete[] values;

  return sb1;
}

void Filter::setSpecialPred(Predicate* p) {
  specialPred=p;
}


/**
 * This operator needs to return the next SBlock worth of tuples. Recall that
 * in CStore, for efficiency, iterators forward blocks of tuples, not single
 * tuples. colIndex_ is completely ignored by this function.
 */
SBlock* Filter::getNextSValBlockNoPredicate(int colIndex_) {
  assert(colIndex_==0);
  assert(ninputs==2);
  int* values = new int[ninputs+1];
  bool atLeastOneResult=false;
  while (!atLeastOneResult) {
    updateInputBlocks();
    if (done)
      return NULL;
    if (specialPred) {
      specialPred->setLHS(iterArray[1]->peekNext());
      if (!specialPred->evalPredicate())
	continue;
    }
    sb1->resetBlock();
    sb1->setStartPosition(curpos);
    //sb2->resetBlock();
    //sb2->setStartPosition(curpos);
    ValPos* pair;
    
    int sbpos = 0;
    int** currarray = new int*[ninputs];
    int* currarraysize = new int[ninputs];
    int* startposes = new int[ninputs];
    //int startpos;
    if (iterArray[0]->hasNext()) {
      for (int i = 0; i < ninputs; i++) {
	currarray[i] = (int*) iterArray[i]->asArray();
	currarraysize[i] = iterArray[i]->getSize();
	pair = iterArray[i]->getNext();
	startposes[i]=pair->position;
	if (i > 0) {
	  assert(startposes[i]==startposes[i-1]);
	  assert(currarraysize[i]==currarraysize[i-1]);
	}
      }
      for (int i = startposes[0]; i < startposes[0]+currarraysize[0]; i++) {
	values[0]=i;
	for (int j = 1; j <= ninputs; j++) {
	  values[j]=currarray[j-1][i-startposes[0]];
	}
	sb1->setValueAtPos((char*)values, sbpos);
	sbpos++;
      }
      atLeastOneResult=true;
    }      
  }

  //sb->setRealStartPos(sb->peekNext()->position);
  delete[] values;

  return sb1;
}


/**
 * This operator needs to return the next SBlock worth of tuples. Recall that
 * in CStore, for efficiency, iterators forward blocks of tuples, not single
 * tuples. colIndex_ is completely ignored by this function.
 */
SBlock* Filter::getNextSValBlockParallel(int colIndex_) {
  assert(colIndex_==0);
  assert(ninputs==2);
  int* values = new int[ninputs+1];
  bool atLeastOneResult=false;
  while (!atLeastOneResult) {
    updateInputBlocks();
    if (done)
      return NULL;
    sb1->resetBlock();
    sb1->setStartPosition(curpos);
    //sb2->resetBlock();
    //sb2->setStartPosition(curpos);
    ValPos* pair;
    //assume int type for now ... fix later ...
    ValPos* vp = new IntValPos();
    int* rhsints = new int[npreds];
    for (int i=0; i<npreds;i++) {
      assert(preds[i]->getRHS()->type == ValPos::INTTYPE);
      rhsints[i]=*(int*)(preds[i]->getRHS()->value);
      preds[i]->setLHS(vp);
    }
    
    int sbpos = 0;
    int** currarray = new int*[ninputs];
    int* currarraysize = new int[ninputs];
    int* startposes = new int[ninputs];
    bool predsPassed;
    //int startpos;
    if (iterArray[0]->hasNext()) {
      for (int i = 0; i < ninputs; i++) {
	currarray[i] = (int*) iterArray[i]->asArray();
	currarraysize[i] = iterArray[i]->getSize();
	pair = iterArray[i]->getNext();
	startposes[i]=pair->position;
	if (i > 0) {
	  assert(startposes[i]==startposes[i-1]);
	  assert(currarraysize[i]==currarraysize[i-1]);
	}
      }
      for (int i = startposes[0]; i < startposes[0]+currarraysize[0]; i++) {
	values[0]=i;
	for (int j = 1; j <= ninputs; j++) {
	  values[j]=currarray[j-1][i-startposes[0]];
	}
	predsPassed = true;
	for (int i = 0; i <npreds; i++) {
	  //preds[i]->setLHS(values[predindexes[i]+1]);  
	  vp->set((byte*)&values[predindexes[i]+1]);  
	  if (!preds[i]->evalPredicate(rhsints[i])) {
	    predsPassed = false;
	  }
	}
	if (predsPassed) {
	  sb1->setValueAtPos((char*)values, sbpos);
	  sbpos++;
	  atLeastOneResult=true;
	}
      }
    }      
  }

  //sb->setRealStartPos(sb->peekNext()->position);
  delete[] values;

  return sb1;
}
	
/**
 * This operator needs to return the next SBlock worth of tuples. Recall that
 * in CStore, for efficiency, iterators forward blocks of tuples, not single
 * tuples. colIndex_ is completely ignored by this function.
 */
SBlock* Filter::getNextSValBlockPredNotDumb(int colIndex_) {
  assert(colIndex_==0);
  int* values = new int[ninputs+1];
  bool atLeastOneResult=false;
  while (!atLeastOneResult) {
    updateInputBlocks();
    if (done)
      return NULL;
    sb1->resetBlock();
    sb1->setStartPosition(curpos);
    sb2->resetBlock();
    sb2->setStartPosition(curpos);
    ValPos* pair;
    //assume int type for now ... fix later ...
    ValPos* vp = new IntValPos();
    int* rhsints = new int[npreds];
    for (int i=0; i<npreds;i++) {
      assert(preds[i]->getRHS()->type == ValPos::INTTYPE);
      rhsints[i]=*(int*)(preds[i]->getRHS()->value);
      preds[i]->setLHS(vp);
    }
    
    int sbpos = 0;
    int* currarray;
    int currarraysize;
    int startpos;
    if (iterArray[predindexes[0]]->isOneValue()) {
      pair = iterArray[predindexes[0]]->getNext();
      preds[0]->setLHS(pair);
      if (preds[0]->evalPredicate(rhsints[0])) {
	unsigned int startpos = pair->position;
	unsigned int size = iterArray[predindexes[0]]->getSize();
	assert(pair->type == ValPos::INTTYPE);
	values[predindexes[0]+1]=*(int*)pair->value;
	for (unsigned int i = startpos; i < startpos + size; i++) {
	  values[0]=i;
	  //values[predindexes[0]+1]=pair->value;
	  sb1->setValueAtPos((char*)values, sbpos);
	  sbpos++;
	}
      }
      else {
	atLeastOneResult = false;
	continue;
      }
    }
    else {
      if (iterArray[predindexes[0]]->hasNext()) {
	currarray = (int*) iterArray[predindexes[0]]->asArray();
	currarraysize = iterArray[predindexes[0]]->getSize();
	pair = iterArray[predindexes[0]]->getNext();
	startpos=pair->position;
	for (int i = startpos; i < startpos+currarraysize; i++) {
	  //preds[0]->setLHS(currarray[i-startpos]);
	  vp->set((byte*)&currarray[i-startpos]);
	  if (preds[0]->evalPredicate(rhsints[0])) {
	    values[0]=i;
	    values[predindexes[0]+1]=currarray[i-startpos];
	    sb1->setValueAtPos((char*)values, sbpos);
	    sbpos++;
	  }
	}
      }
    }
    //int sbstartpos = sb1->getStartPosition();
    SPair* spair;
    SMultiBlock* tempsb;
    sbpos=0;
    for (int i = 1; i <npreds; i++) {
      tempsb = sb2;
      sb2=sb1;
      sb1=tempsb;
      pair = iterArray[predindexes[i]]->getStartPair();
      if (iterArray[predindexes[i]]->isOneValue()) {
	preds[i]->setLHS(pair);
	if (!preds[i]->evalPredicate(rhsints[i])) {
	  atLeastOneResult = false;
	  break;
	}
      }
      startpos = iterArray[predindexes[i]]->getStartPair()->position;
      currarray = (int*) iterArray[predindexes[i]]->asArray();
      currarraysize = iterArray[predindexes[i]]->getSize();
      while (sb2->hasNext()) {
	spair = sb2->getNext();
	memcpy((byte*)values, spair->value, (ninputs+1)*sizeof(int));
	assert((values[0]-startpos) < currarraysize);
	////pair = iterArray[predindexes[i]]->getPairAtLoc(values[0]-startpos);
	//preds[i]->setLHS(currarray[values[0]-startpos]); 
	vp->set((byte*)&currarray[values[0]-startpos]);
	if (preds[i]->evalPredicate(rhsints[i])) {
	  values[predindexes[i]+1]=currarray[values[0]-startpos];
	  sb1->setValueAtPos((char*)values, sbpos);
	  sbpos++;
	}
      }
    }
    if (sb1->getSize() > 0)
      atLeastOneResult = true;
  }

  /* ok, now would be the point where I fill in the rest of the tuple
     but for now since experiments have a predicate on every attribute, I
     don't need to worry about that for now ...
  */
    
  //sb->setRealStartPos(sb->peekNext()->position);
  delete[] values;

  return sb1;
}
	
/**
 * This operator needs to return the next SBlock worth of tuples. Recall that
 * in CStore, for efficiency, iterators forward blocks of tuples, not single
 * tuples. colIndex_ is completely ignored by this function.
 */
/*SBlock* Filter::getNextSValBlockPredNotDumb(int colIndex_) {
  assert(colIndex_==0);
  int* values = new int[ninputs+1];
  bool atLeastOneResult=false;
  while (!atLeastOneResult) {
  updateInputBlocks();
  if (done)
  return NULL;
  sb1->resetBlock();
  sb1->setStartPosition(curpos);
  sb2->resetBlock();
  sb2->setStartPosition(curpos);
  Pair* pair;
    
  int sbpos = 0;
  if (iterArray[predindexes[0]]->isOneValue()) {
  pair = iterArray[predindexes[0]]->getNext();
  if (preds[0]->evalPredicate()) {
  unsigned int startpos = pair->position;
  for (unsigned int i = startpos; i < startpos + iterArray[predindexes[0]]->getSize(); i++) {
  values[0]=i;
  values[predindexes[0]+1]=pair->value;
  sb1->setValueAtPos((char*)values, sbpos);
  sbpos++;
  }
  }
  }
  else {
  while (iterArray[predindexes[0]]->hasNext()) {
  pair = iterArray[predindexes[0]]->getNext();
  preds[0]->setLHS(pair->value);  
  if (preds[0]->evalPredicate()) {
  values[0]=pair->position;
  values[predindexes[0]+1]=pair->value;
  sb1->setValueAtPos((char*)values, sbpos);
  sbpos++;
  }
  }
  }
  int startpos;
  //int sbstartpos = sb1->getStartPosition();
  SPair* spair;
  SMultiBlock* tempsb;
  sbpos=0;
  for (int i = 1; i <npreds; i++) {
  tempsb = sb2;
  sb2=sb1;
  sb1=tempsb;
  startpos = iterArray[predindexes[i]]->getStartPair()->position;
  while (sb2->hasNext()) {
  spair = sb2->getNext();
  memcpy((byte*)values, spair->value, (ninputs+1)*sizeof(int));
  pair = iterArray[predindexes[i]]->getPairAtLoc(values[0]-startpos);
  preds[i]->setLHS(pair->value);  
  if (preds[i]->evalPredicate()) {
  values[predindexes[i]+1]=pair->value;
  sb1->setValueAtPos((char*)values, sbpos);
  sbpos++;
  }
  }
  }
  if (sb1->getSize() > 0)
  atLeastOneResult = true;
  }

  // ok, now would be the point where I fill in the rest of the tuple
  //   but for now since experiments have a predicate on every attribute, I
  //   don't need to worry about that for now ...
  //
    
  //sb->setRealStartPos(sb->peekNext()->position);
  delete[] values;

  return sb1;
  }*/
	
/**
 * This operator needs to return the next SBlock worth of tuples. Recall that
 * in CStore, for efficiency, iterators forward blocks of tuples, not single
 * tuples. colIndex_ is completely ignored by this function.
 */
/*SBlock* Filter::getNextSValBlockNoPred(int colIndex_) {
  assert(colIndex_==0);
  int* values = new int[ninputs+1];
  sb1->resetBlock();
  sb1->setStartPosition(curpos);
  //while (!atLeastOneResult) {
  updateInputBlocks();
  if (done)
  return NULL;
  Pair* pair;
  int sbpos = 0;
  while (iterArray[0]->hasNext()) {
  pair = iterArray[0]->getNext();
  values[0] = pair->position;
  values[1] = pair->value;
  for (int i = 1; i < ninputs; i++)
  values[i+1] = iterArray[i]->getNext()->value;
  sb1->setValueAtPos((char*)values, sbpos);
  sbpos++;
  }
  return sb1;
  }*/
	
// Gets the next position block (bitstring of positions) from the operator
PosBlock* Filter::getNextPosBlock(int colIndex_) {
  throw new UnexpectedException("concat doesn't handle getNextPosBlock");
}
	
// Gets the maximum position in the data source
int Filter::getLastPosition() 
{
  throw new UnexpectedException("concat doesn't handle getLastPosition");		
}

void Filter::updateInputBlocks() {
  for (int i = 0; i < ninputs; i++) {
    if (exhausted[i]) {
      // The zero below just assumes we want the first stream from
      // all operators (maybe should change later)
      //if (i == 0 && inputBlocks[i] != NULL) {
      //	cerr << "Replacing 0 block ending at " << inputBlocks[i]->getEndPosition() << endl;
      //}
      inputBlocks[i] = inputs[i]->getNextValBlock(0);
      if (inputBlocks[i] == NULL) {
	done = true;
	return;
      } else {
	if (i == 0 && !doParallel && inputBlocks[i]->isOneValue()) {
	  preds[0]->setLHS(inputBlocks[0]->peekNext());
	  // If pred doesn't match, keep skipping forward until it does
	  while (!preds[0]->evalPredicate()) {
	    inputBlocks[0] = inputs[0]->getNextValBlock(0);
	    if (inputBlocks[0] == NULL) {
	      done = true;
	      return;
	    }
	    preds[0]->setLHS(inputBlocks[i]->peekNext());
	  }
	  // Now, read forward in blocks 1 until we get to the right pos
	  curpos = inputBlocks[0]->peekNext()->position;
	  int ep = inputBlocks[1] == NULL ? 0 : inputBlocks[1]->getEndPosition();
	  //DNA commented out 7/11/06
	  /*
	  while(ep < curpos) {
	    inputBlocks[1] = inputs[1]->getNextValBlock(0);
	    if (inputBlocks[1] == NULL) {
	      done = true;
	      return;
	    }
	    ep = inputBlocks[1]->getEndPosition();
	    }*/
	  if (ep < curpos) {
	    inputBlocks[1] = inputs[1]->getNextValBlock(0);
	    ep = inputBlocks[1]->getEndPosition();
	    if (ep < curpos) {
	      inputBlocks[1] = inputs[1]->getNextValBlockAtPos(0, curpos);
	      if (inputBlocks[1] == NULL) {
		done = true;
		return;
	      }
	    }
	  }

	  exhausted[0]=exhausted[1]=false;
	  endposes[0] = inputBlocks[0]->getEndPosition();
	  endposes[1] = inputBlocks[1]->getEndPosition();
	  break;
	}
      }
					
      //if (inputBlocks[i]->getStartPair()->position==747409)
      //	cout << inputBlocks[i]->getStartPair()->position << endl;
	
      exhausted[i]=false;
      endposes[i] = inputBlocks[i]->getEndPosition();
    }
    delete iterArray[i];
  }
  unsigned int minendpos = endposes[0];
  for (int i = 1; i < ninputs; i++) {
    if (endposes[i] < minendpos)
      minendpos = endposes[i];
  }
    
  for (int i = 0; i < ninputs; i++) {
    if (endposes[i] == minendpos)
      exhausted[i] = true;
    iterArray[i] = inputBlocks[i]->getIterator(curpos, minendpos);
  }
  curpos = minendpos+1;
  //}
}
