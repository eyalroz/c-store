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
// Writes Type2 Blocks
// Problems: migfer@mit.edu


#include "Type2Writer.h"

Type2Writer::Type2Writer(Operator* blockSrc_, int colIndex_, int bfrSizeInBits_, int deltaSize_) : Writer(blockSrc_, colIndex_), bufferMap(0)
{
	bfrSizeInBits=bfrSizeInBits_;
	currBlockD=new DeltaPosBlock(false);
	currBlockT2=new Type2Block(false);
	inBlock=NULL;
	currVP=NULL;
	deltaSize=deltaSize_;
	Log::writeToLog("Type2Writer", 0, "initializing delta pos writer");
	emptyBuffers=false;
	prevmod=0;
	indexes = new int[TYPE2MAXVALUES];
	for (int i = 0; i < TYPE2MAXVALUES; i++) {
	  indexes[i]=i;
	}
	values = new ValPos*[TYPE2MAXVALUES];
	for (int i = 0; i < TYPE2MAXVALUES; i++) {
	  values[i]=NULL;
	}
	valindex = 0;
	hashTable = new HashMap(2*TYPE2MAXVALUES);
	hashTable->setHashFunction(new IdentityHashFunction(2*TYPE2MAXVALUES));
}

Type2Writer::~Type2Writer()
{
  delete currBlockD;
  delete currBlockT2;
}

ValPos** Type2Writer::getValueIndex() {
  return values;
}

// Gets the next value block from the operator 
Block* Type2Writer::getNextValBlock(int colIndex_) {
  Log::writeToLog("Type2Writer", 0, "Called getNextValBlock() colIndx=", colIndex_);							
  if (colIndex_!=0) 
    return NULL;
  else {
    while (true) {
      if (emptyBuffers)
	currVP=NULL;
      else
	currVP=getNextPair();
      if (currVP==NULL) {
	if (bufferMap.empty()) {
	  Log::writeToLog("Type2Writer", 1, "getNextValBlock(): DataSource ran dry, Buffers also dry, returning NULL");							
	  if (emptyBuffers) {
	    emptyBuffers=false;
	    currVP=getNextPair();
	    if (!currVP)
	      return NULL;
	  }
	  else
	    return NULL;
	}
	else {
	  Log::writeToLog("Type2Writer", 1, "getNextValBlock(): DataSource ran dry, returning unfilled buffers");							
	  DeltaPosValue* dPV=((DeltaPosValue*) bufferMap.removeFirstObj());
	  currBlockD->setDeltaBuffer(dPV->getBuffer(), bfrSizeInBits*4);
	  currBlockT2->setBuffer(currBlockD);
	  delete dPV;
	  return currBlockT2;
	}
      }
      
      int val;
      if (currVP->type == ValPos::INTTYPE)
	val=*(int*)currVP->value;
      else {
	int* v = (int*)hashTable->get(currVP);
	if (v)
	  val = *v;
	else {
	  hashTable->put(currVP, indexes+valindex, sizeof(int));
	  values[valindex] = currVP->clone();
	  val = *(indexes+valindex);
	  valindex++;
	}
      }
      unsigned int pos=currVP->position;
      
      // two cases: we have page already being created for this value 
      //			  or we don't
      
      if ((bufferMap.getNumObjs(val))<=0) {
	Log::writeToLog("Type2Writer", 1, "getNextValBlock(): Creating new Type2Value for value=", val);		
	bufferMap.putObj(val, new DeltaPosValue(bfrSizeInBits*4,val, pos, deltaSize),bfrSizeInBits*4);				
      }
      else {
	Log::writeToLog("Type2Writer", 0, "getNextValBlock(): Using DeltaPosValue for value=", val);		
	DeltaPosValue* currDeltaBfr=((DeltaPosValue*) bufferMap.getObj(val));
	if (currDeltaBfr->writePos(pos)) {
	  // do noting we, have not yet filled buffer	
	}
	else {
	  // we filled: thus we remove it from buffer map and 
	  // construct a block using it					
	  throw new UnexpectedException("Should never happen");
	}
	int curmod = ((pos + 1) % ((bfrSizeInBits/sizeof(int)) - (5*8*sizeof(int))));
	//if (curmod % 10000 == 0)
	//  cout << curmod << endl;
	//int curmod = ((pos + 1) % (8*PAGE_SIZE));
	if ((curmod == 1) || ((curmod < prevmod) && (curmod))) {
	  emptyBuffers=true;
	  currBlockD->setDeltaBuffer(currDeltaBfr->getBuffer(), bfrSizeInBits*4);
	  currBlockT2->setBuffer(currBlockD);
	  Log::writeToLog("Type2Writer", 1, "getNextValBlock(): We filled buffer, returning block, for value=", val);		
	  
	  Log::writeToLog("Type2Writer", 1, "getNextValBlock(): Creating another DeltaPosValue for value=", val);		
	  
	  
	  //currDeltaBfr->reset(val, pos);
	  
	  // trying to reuse memory (above) (old code follows)
	  //currDeltaBfr->clearWriterSpace();
	  delete currDeltaBfr;
	  bufferMap.removeObj(val);
	  //bufferMap.putObj(val, new DeltaPosValue(bfrSizeInBits,val, pos, deltaSize),bfrSizeInBits);				
	  
	  return currBlockT2;
	}		
	prevmod=curmod;
      }	
    }
  }
}

ValPos* Type2Writer::getNextPair() {
	if (inBlock==NULL) {
		Log::writeToLog("Type2Writer", 0, "getNextPair(): Current Block is NULL, pulling another block");
		inBlock=blockSrc->getNextValBlock(colIndex);
		if (inBlock==NULL) {
			Log::writeToLog("Type2Writer", 1, "getNextPair(): Block returned is NULL, returning NULL pair");
			return NULL;
		}
	}
	ValPos* pair=inBlock->getNext();
	if (pair==NULL) {
		Log::writeToLog("Type2Writer", 0, "getNextPair(): Pair is NULL from inBlock, pulling next Block");
		inBlock=blockSrc->getNextValBlock(colIndex);
		do {
			if (inBlock==NULL) return NULL;
			pair=inBlock->getNext();
			if (pair==NULL) inBlock=blockSrc->getNextValBlock(colIndex);
		} while (pair==NULL);
		//Log::writeToLog("Type2Writer", 0, "getNextPair(): Got a pair with value", pair->value);
		//Log::writeToLog("Type2Writer", 0, "getNextPair(): Got a pair with position", pair->position);
		return pair;
	}
	else {
	  //Log::writeToLog("Type2Writer", 0, "getNextPair(): Got a pair with value", pair->value);
	  //Log::writeToLog("Type2Writer", 0, "getNextPair(): Got a pair with position", pair->position);
		return pair;
	}
}

// Gets the next position block (bitstring of positions) from the operator
PosBlock* Type2Writer::getNextPosBlock(int colIndex_) {
	return NULL;
}

