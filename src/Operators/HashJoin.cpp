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
#include "HashJoin.h"
#include "../common/BlockWithPos.h"

HashJoin::HashJoin(Operator **left_, int* predColIndex1_, Operator **right_, int* predColIndex2_, int larraysize_, int rarraysize_, bool outputKey_) : l_src(left_), l_predcol(predColIndex1_), r_src(right_), r_predcol(predColIndex2_), larraysize(larraysize_), rarraysize(rarraysize_), outputKey(outputKey_) {
  
  init();
}

void HashJoin::init() {
  // set both blocks dirty
  m_isLBlockConsumed = true;
  m_isRBlockConsumed = true;
  blocks = NULL;
  m_currOutputPosition = 1;

  lBlockOut = NULL;
  rBlockOut = NULL;
  lBlockIn = NULL;
  rBlockIn = NULL;
  firstCall = true;
      
  rpbo = NULL;
  isRightOneValue = false;
  isLeftOneValue = false;
  mode = UNINIT;
  rMap = NULL;
  hf=NULL;
  playpbb = new PosBasicBlock(true);
  lValBlockOut = new Block*[larraysize];
  int i;
  for (i = 0; i < larraysize; i++)
    lValBlockOut[i] = NULL;
  rValBlockOut = new Block*[rarraysize-1]; //-1 for no key
  for (i = 0; i < (rarraysize-1); i++)
    rValBlockOut[i] = NULL;
  numOutputColumns = larraysize+rarraysize-2; //-2 is for each key
  if (outputKey)
    numOutputColumns++;
  m_iskBlockConsumed = new bool[numOutputColumns];
  for (i = 0; i < numOutputColumns; i++)
    m_iskBlockConsumed[i] = true;
  lBlockIn = new Block*[larraysize];
  rBlockIn = new Block*[rarraysize];
  currVPs = new ValPos*[larraysize];
  triples = NULL;
}

HashJoin::~HashJoin() {
  /*if (aggData != NULL) {
    delete aggData;
  }*/
  if (lBlockOut != NULL) {
    delete lBlockOut;
  }
  if (rBlockOut != NULL) {
    delete rBlockOut;
  }
  for (int i = 0; i < larraysize; i++) 
    delete lValBlockOut[i];
  delete[] lValBlockOut;
  for (int i = 0; i < (rarraysize-1); i++)
    delete rValBlockOut[i];
  delete[] rValBlockOut;
  delete[] lBlockIn;
  delete[] rBlockIn;
  delete[] currVPs;
  delete[] m_iskBlockConsumed;
  delete playpbb;
  if (mode == VALMODE) {
    if (rMap) {
      ValPos* vp;
      byte** bufferset;
      while (!rMap->isEmpty()) {
	vp = rMap->getKey();
	bufferset = (byte**)rMap->remove(vp);
	for (int i = 0; i < (rarraysize+1); i++)
	  delete [] bufferset[i];
	delete[] bufferset;
      }
    }
    for (int i = 0; i < larraysize; i++) {
      triples[i]->value = NULL;
      delete triples[i];
    }
    delete[] triples;
  }
  else if (mode == POSMODE) {
    if (rMap) {
      ValPos* vp;
      if (isRightOneValue) {
	PBObject* pbo;
	PosBlock* currpb;
	while (!rMap->isEmpty()) {
	  vp = rMap->getKey();
	  pbo= (PBObject*)rMap->remove(vp);
	  while (pbo->hasNext()) {
	    currpb = pbo->getNext();
	    delete currpb;
	  }
	  delete pbo;
	}
      }
      else {
	PosArrayBlock* pab;
	while (!rMap->isEmpty()) {
	  vp = rMap->getKey();
	  pab= (PosArrayBlock*)rMap->remove(vp);
	  delete pab;
	}
      }
    }
  }
  delete hf;
  delete rMap;
}

// assume that operator above will read blocks in parallel
// getNextValBlock generates output in parallel
Block* HashJoin::getNextValBlock(int colIndex_) {
  assert(colIndex_<numOutputColumns);
  if (mode == UNINIT) {
    prepareRightHashTable();
    if (!rMap)
      return NULL;
    mode = VALMODE;
    for (int i = 0; i < larraysize; i++)
      lBlockIn[i]=l_src[i]->getNextValBlock(l_predcol[i]);
    if (lBlockIn[0]->isOneValue()) {
      isLeftOneValue = true;
      if (!lBlockIn[0]->isPosContiguous())
	throw new UnimplementedException("Not done yet");
      triples = new RLETriple*[larraysize];
      for (int i = 0; i < larraysize; i++) {
	lValBlockOut[i] = new RLEBlock(lBlockIn[i]->isValueSorted());
	triples[i] = new RLETriple();
      }
    }
    else {  
      isLeftOneValue = false;
      triples = new RLETriple*[larraysize];
      for (int i = 0; i < larraysize; i++) {
	lValBlockOut[i] = new RLEBlock(lBlockIn[i]->isValueSorted());
	triples[i] = new RLETriple();
      }
    }
  }
  else if (mode == POSMODE)
    throw new UnexpectedException("Illegal to switch modes on the fly.");
  bool allConsumed = true;
  for (int i = 0; i < numOutputColumns; i++)
    if (m_iskBlockConsumed[i] == false)
      allConsumed = false;

  if (allConsumed) {
    for (int i = 0; i < numOutputColumns; i++)
      m_iskBlockConsumed[i] = false;
    generateBlocks();
    assert(lValBlockOut[0]==NULL || (rarraysize == 1) || (lValBlockOut[0]->getSize() == rValBlockOut[0]->getSize()));
  }
  if (m_iskBlockConsumed[colIndex_] == false) {
    m_iskBlockConsumed[colIndex_] = true;
    if ((outputKey?colIndex_:colIndex_+1) < larraysize)
      return lValBlockOut[outputKey?colIndex_:colIndex_+1];
    else
      return rValBlockOut[outputKey?colIndex_-larraysize:colIndex_+1-larraysize];
  }
  else {
      throw new UnexpectedException("HashJoin: already consumed asked for block");

  }
}

void HashJoin::updateCurrVPs() {
  currVPs[0] = getNextLPair();
  for (int i = 1; i < larraysize; i++)
    currVPs[i] = getNextLVP(i);
}

// ASSUMPTIONS:
//  positions across both block streams line up in rows

// INVARIANT:
// lBlockIn always set to next block to be read
void HashJoin::generateBlocks() {
  //if (isLeftOneValue) {
  //  if (!lBlockIn[0])
  //    currLPair = NULL;
  //  else
  //    currLPair = lBlockIn[0]->getStartPair();
  //}
  //else
  updateCurrVPs();
  if (currLPair == NULL) {
    for (int i = 0; i < larraysize; i++) {
      delete lValBlockOut[i];
      lValBlockOut[i] = NULL;
    }
    for (int i = 0; i < (rarraysize-1); i++) {
      delete rValBlockOut[i];
      rValBlockOut[i] = NULL;
    }
    return;
  }
  byte** bufferset;
  while (!(bufferset = (byte**) rMap->get(currLPair))) {
    updateCurrVPs();
    if (currLPair == NULL) {
    for (int i = 0; i < larraysize; i++)
      lValBlockOut[i] = NULL;
    for (int i = 0; i < (rarraysize-1); i++)
      rValBlockOut[i] = NULL;
    return;
    }
  }
  int bfrsize = *(int*)(bufferset[rarraysize]);
  for (int i = 0; i < larraysize; i++) {
    triples[i]->value = currVPs[i];
    triples[i]->startPos = m_currOutputPosition;
    triples[i]->reps = bfrsize;
    ((RLEBlock*)lValBlockOut[i])->setTriple(triples[i]);
  }
  for (int i = 1; i < rarraysize; i++) {
    ((MultiBlock*)rValBlockOut[i-1])->setBufferDirect(m_currOutputPosition, bfrsize, bufferset[i]+sizeof(int));
  }
  m_currOutputPosition+=bfrsize;
}

ValPos* HashJoin::getCurrLPair() {
  return currLPair;
}
ValPos* HashJoin::getNextLPair() {
  /*  ValPos* p;
  if (!(p=lBlockIn[0]->getNext())) {
    lBlockIn[0]=l_src[0]->getNextValBlock(l_predcol[0]);
    if (lBlockIn[0]) {
      currLPair = lBlockIn[0]->getNext();
      return currLPair;
    }
    else
      return NULL;
  }
  else {
    currLPair=p;
    return p;
    }*/
  if (!lBlockIn[0]->hasNext()) {
    lBlockIn[0]=l_src[0]->getNextValBlock(l_predcol[0]);
    if (!lBlockIn[0]) {
      currLPair = NULL;
      return NULL;
    }
  }
  currLPair = lBlockIn[0]->getNext();
  return currLPair;
}

PosBlock* HashJoin::posify(Block* b) {
  //I don't like this method ... there's got to be a more general way to do this ...
  if (b->isPosContiguous()) {
    RLETriple trip(b->getStartPair(), b->getStartPair()->position, b->getSize());
    PosRLEBlock* rle = new PosRLEBlock(&trip);
    return rle;
  }
  else {
    PosType2Block* t2 = new PosType2Block();
    t2->setBlock((Type2Block*)b);//ICK
    return t2;
  }
}

void HashJoin::prepareRightHashTable() {
  for (int i = 0; i < rarraysize; i++) {
    rBlockIn[i] = r_src[i]->getNextValBlock(r_predcol[i]);
    if (rBlockIn[i] == NULL || (!rBlockIn[i]->hasNext()))
      return;
    if (i!=(rarraysize-1))
      rValBlockOut[i] = new MultiBlock(rBlockIn[i]->isValueSorted(), true, true);
  }
  if (!rBlockIn[0]->isPosSorted()) {
    throw new UnimplementedException("HashJoin: For now join only works for pos sorted data");
  }
  rMap = new HashMap(1500000);
  hf = new IdentityHashFunction(1500000);
  rMap->setHashFunction(hf);
  // Right now, we need to clone Blocks because we can't pin them.
  // Down the road, we can just pin them.
  ValPos* vp;
  /*if (rBlockIn->isOneValue()) {
    isRightOneValue = true;
    rpbo = new PBObject();
    rpbo->addPosBlock(posify(rBlockIn->clone(*rBlockIn)));      
    rMap->put(rBlockIn->getStartPair(), rpbo, sizeof(PBObject));
    while (rBlockIn=r_src[0]->getNextValBlock(r_predcol[0])) {
      assert(rBlockIn->isOneValue());
      vp = rBlockIn->getStartPair();
      if (rpbo = (PBObject*) rMap->get(vp))
	rpbo->addPosBlock(posify(rBlockIn->clone(*rBlockIn)));
      else {
	rpbo = new PBObject();
	rpbo->addPosBlock(posify(rBlockIn->clone(*rBlockIn)));
	rMap->put(vp, rpbo, sizeof(PBObject));
      }
    }
  }
  else { 
  isRightOneValue = false; */
  ValPos* temp;
  int curloc, curcap, cursize;
  byte** bufferset;
  cout << "hi dan" << endl;
  while (rBlockIn[0]) {
    vp = getNextRVP(0);
    if(!vp)
      break;
    if (vp->position % 100000 == 0)
      cout << "hi dan" << vp->position << endl;
    if (bufferset = (byte**) rMap->get(vp)) {
      curloc = *(int*)(bufferset[rarraysize]);
      for (int i = 0; i < rarraysize; i++) {
	curcap = *(int*)(bufferset[i]);
	cursize = temp->getSize();
	if (((curloc*cursize)+(int)sizeof(int)) >= curcap) {
	  curcap=curcap*2;
	  byte* temp2 = new byte[curcap];
	  memcpy(temp2, bufferset[i], curcap/2);
	  delete[] bufferset[i];
	  bufferset[i] = temp2;
	  (*(int*)(bufferset[i])) = curcap;
	}
	if (i == 0)
	  temp = vp;
	else {
	  temp = getNextRVP(i);
	  assert(temp->position == vp->position);
	}
	memcpy(bufferset[i]+(curloc*cursize)+sizeof(int),temp->value,cursize);
      }
      (*((int*)(bufferset[rarraysize])))++;
    }
    else {
      bufferset = new byte*[rarraysize+1];
      int tempsize = 0;
      for (int i = 0; i < rarraysize; i++) {
	if (i == 0)
	  temp = vp;
	else {
	  temp = getNextRVP(i);
	  assert(temp->position == vp->position);
	  if (vp->position == 9998)
	    cout << "hi dan" << endl;
	}
	tempsize = temp->getSize()+sizeof(int)+ 256;
	bufferset[i] = new byte[tempsize];
	(*(int*)bufferset[i]) = tempsize;
	cursize = temp->getSize();
	memcpy(bufferset[i]+sizeof(int), temp->value, cursize);
      }
      bufferset[rarraysize] = new byte[sizeof(int)];
      (*(int*)bufferset[rarraysize]) = 1;
      rMap->put(vp, bufferset, tempsize*rarraysize+sizeof(int));
    }
  }
  //}
}

ValPos* HashJoin::getNextRVP(int index) {
  if (!rBlockIn[index]->hasNext()) {
    rBlockIn[index] = r_src[index]->getNextValBlock(r_predcol[index]);
    if (!rBlockIn[index])
      return NULL;
  }
  return rBlockIn[index]->getNext();
  
}

ValPos* HashJoin::getNextLVP(int index) {
  if (!lBlockIn[index]->hasNext()) {
    lBlockIn[index] = l_src[index]->getNextValBlock(l_predcol[index]);
    if (!lBlockIn[index])
      return NULL;
  }
  return lBlockIn[index]->getNext();
  
}

void HashJoin::prepareRightHashTablePos() {
  rBlockIn[0] = r_src[0]->getNextValBlock(r_predcol[0]);
  if (rBlockIn[0] == NULL || (!rBlockIn[0]->hasNext()))
    return;
  if (!rBlockIn[0]->isPosSorted()) {
    throw new UnimplementedException("HashJoin: For now join only works for pos sorted data");
  }
  rMap = new HashMap(1500000);
  hf = new IdentityHashFunction(1500000);
  rMap->setHashFunction(hf);
  // Right now, we need to clone Blocks because we can't pin them.
  // Down the road, we can just pin them.
  ValPos* vp;
  if (rBlockIn[0]->isOneValue()) {
    isRightOneValue = true;
    rpbo = new PBObject();
    rpbo->addPosBlock(posify(rBlockIn[0]->clone(*rBlockIn[0])));      
    rMap->put(rBlockIn[0]->getStartPair(), rpbo, sizeof(PBObject));
    while (rBlockIn[0]=r_src[0]->getNextValBlock(r_predcol[0])) {
      assert(rBlockIn[0]->isOneValue());
      vp = rBlockIn[0]->getStartPair();
      if (rpbo = (PBObject*) rMap->get(vp))
	rpbo->addPosBlock(posify(rBlockIn[0]->clone(*rBlockIn[0])));
      else {
	rpbo = new PBObject();
	rpbo->addPosBlock(posify(rBlockIn[0]->clone(*rBlockIn[0])));
	rMap->put(vp, rpbo, sizeof(PBObject));
      }
    }
    //make sure rpbo does not have next so that first time generatePosBlocks
    //is called, it goes to the right part of the if statement
    while (rpbo->hasNext()) 
      rpbo->getNext();
  }
  else {
    isRightOneValue = false;
    while (rBlockIn[0]) {
      if (!rBlockIn[0]->hasNext()) {
	rBlockIn[0]=r_src[0]->getNextValBlock(r_predcol[0]);
	continue;
      }
      vp = rBlockIn[0]->getNext();
      if (rBlockOut = (PosArrayBlock*) rMap->get(vp))
	((PosArrayBlock*)rBlockOut)->setPosVarCap(vp->position);
      else {
	rBlockOut = new PosArrayBlock(100);
	((PosArrayBlock*)rBlockOut)->setPosVarCap(vp->position);
	rMap->put(vp, rBlockOut, sizeof(PosArrayBlock));
      }
    }
  }
}

void HashJoin::generatePosBlocks() {
  if (isLeftOneValue) {
    if (!lBlockIn[0])
      currLPair = NULL;
    else
      currLPair = lBlockIn[0]->getStartPair();
  }
  else
    getNextLPair();
  if (currLPair == NULL) {
    if ((isLeftOneValue) && (rBlockOut->getSize() == rBlockOut->getNumOcurrences()))
      ((PosRLEBlock*)lBlockOut)->getTriple()->value = NULL;
    delete lBlockOut;
    lBlockOut = NULL;
    rBlockOut = NULL;
    return;
  }
  if (isRightOneValue) {
    if (rpbo->hasNext()) {
      rBlockOut = rpbo->getNext();
      ((PosBasicBlock*)lBlockOut)->setPosition(currLPair->position);
      lBlockOut->setNumOcurrences(rBlockOut->getSize());
    }
    else {
      while (!(rpbo = (PBObject*) rMap->get(currLPair))) {
	getNextLPair();
	if (currLPair == NULL) {
	  if ((isLeftOneValue) && (rBlockOut->getSize() == rBlockOut->getNumOcurrences()))
	    ((PosRLEBlock*)lBlockOut)->getTriple()->value = NULL;
	  delete lBlockOut;
	  lBlockOut = NULL;
	  rBlockOut = NULL;
	  return;
	}
      }
      rpbo->resetIterator();
      assert(rpbo->hasNext());
      rBlockOut = rpbo->getNext();
      ((PosBasicBlock*)lBlockOut)->setPosition(currLPair->position);
      lBlockOut->setNumOcurrences(rBlockOut->getSize());
    }
  }
  else {
    while (!(rBlockOut = (PosArrayBlock*) rMap->get(currLPair))) {
      if (isLeftOneValue) {
	lBlockIn[0]=l_src[0]->getNextValBlock(l_predcol[0]);
	if (!lBlockIn[0]) 
	  currLPair = NULL;
	else
	  currLPair = lBlockIn[0]->getStartPair();
      }
      else
	getNextLPair();
      if (currLPair == NULL) {
	if ((isLeftOneValue) && (rBlockOut->getSize() == rBlockOut->getNumOcurrences()))
	  ((PosRLEBlock*)lBlockOut)->getTriple()->value = NULL;
	delete lBlockOut;
	lBlockOut = NULL;
	rBlockOut = NULL;
	return;
      }
    }
    if ((isLeftOneValue) && (rBlockOut->getSize() == 1)) {
      RLETriple* trip = ((PosRLEBlock*)lBlockOut)->getTriple();
      trip->value = currLPair;
      trip->startPos = currLPair->position;
      trip->reps = lBlockIn[0]->getSize();
      //lBlockOut->setNumOcurrences(rBlockOut->getSize()/((PosArrayBlock*)rBlockOut)->getRepeats());
      //((PosArrayBlock*)rBlockOut)->setRepeats(lBlockIn[0]->getSize());
      playpbb->setPosition(rBlockOut->getStartPos());
      playpbb->setNumOcurrences(lBlockIn[0]->getSize());
      rBlockOut = playpbb;
      lBlockIn[0]=l_src[0]->getNextValBlock(l_predcol[0]);
      
    }
    else {
      ((PosBasicBlock*)lBlockOut)->setPosition(currLPair->position);
      lBlockOut->setNumOcurrences(rBlockOut->getSize());
    }
  } 
}

PosBlock* HashJoin::getNextPosBlock(int colIndex_) {
  //if (!doEM)
  //  return getNextPosBlockMC(colIndex_);
  // check if we need to generate output new output blocks
  if (mode == UNINIT) {
    prepareRightHashTablePos();
    if (!rMap)
      return NULL;
    mode = POSMODE;
    lBlockIn[0]=l_src[0]->getNextValBlock(l_predcol[0]);
    if (lBlockIn[0]->isOneValue() && !isRightOneValue) {
      isLeftOneValue = true;
      if (!lBlockIn[0]->isPosContiguous())
	throw new UnimplementedException("Not done yet");
      lBlockOut = new PosRLEBlock();
    }
    else {  
      isLeftOneValue = false;
      lBlockOut = new PosBasicBlock(true);
    }
  }
  else if (mode == VALMODE)
    throw new UnexpectedException("Illegal to switch modes on the fly.");
  if (m_isLBlockConsumed && m_isRBlockConsumed) {
    m_isLBlockConsumed = false;
    m_isRBlockConsumed = false;
    generatePosBlocks();
    assert(lBlockOut==NULL || (lBlockOut->getSize() == rBlockOut->getSize()));
  }
  if (colIndex_ == L_COL_INDEX && !m_isLBlockConsumed) {

    m_isLBlockConsumed = true;
    //if (dontcheck)
    //  m_isRBlockConsumed = true;
    return (lBlockOut);

  } else if (colIndex_ == R_COL_INDEX && !m_isRBlockConsumed) {

    m_isRBlockConsumed = true;
    //if (dontcheck)
    //  m_isLBlockConsumed = true;
    return (rBlockOut);

  } else {
    // operator above is reading a block without having read other block
    if (!m_isLBlockConsumed) {
      throw new UnexpectedException("HashJoin: already read R Block but haven't read L Block");
    } else {
      throw new UnexpectedException("HashJoin: already read L Block but haven't read R Block");
    }
  }
  //return NULL;
}

//unsupported
PosBlock* HashJoin::getNextPosBlock(int colIndex_, int val_) {
  return NULL;
}
