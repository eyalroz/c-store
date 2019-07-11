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
// Encodes Sparse Pages

#include "BitSparseEncoder.h"

BitSparseEncoder::BitSparseEncoder(Operator* dataSrc_, int colIndex_, int fieldSize, int bfrSizeInBits_) : Encoder(dataSrc_, colIndex_)
{
  if (dataSrc_==NULL) mode=PUSH;
  currBlock=NULL;
  currVP=NULL;
  init=true;
  writer=new StringWriter(fieldSize,bfrSizeInBits_);
  buffer=writer->getBuffer();
  t2b = new Type2Block(false);
  lengthPtr=(int*) buffer;
  startPosPtr=lengthPtr+1;
  endPosPtr=lengthPtr+2;

  writer->skipToIntPos(3);

  mode=BitSparseEncoder::UNINIT;
  nullcount = 0;
  notnullcount = 0;
  currpos=0;
  ssize = fieldSize;
  nvp = new NullValPos();
}

BitSparseEncoder::~BitSparseEncoder()
{
  delete writer;
  delete nvp;
}

//DNA: this is ugly code fix later
void BitSparseEncoder::resetPair() {
  currVP=NULL;
}

bool BitSparseEncoder::isNull(byte* val) {
  bool isNULL=true;
  
  if (currVP->type==ValPos::STRINGTYPE) {
    if ((atoi((char*)val)) == NULLINT) {
      nullcount++;
    }
    else {
      notnullcount++;
      isNULL=false;
    }
  }
  else {
    if (val[ssize-1] !=NULLBYTE)
      isNULL=false;
    int x = ssize-1;
    for (int i = 0; i < x; i++)
      if (val[i] != 0)
	isNULL=false;
    if (isNULL)
      nullcount++;
    else
      notnullcount++;
  }
  return (isNULL);
}

int BitSparseEncoder::getNullCount() {
  return nullcount;
}

int BitSparseEncoder::getNotNullCount() {
  return notnullcount;
}

byte* BitSparseEncoder::getPage() {
  int test, nv;
  bool enoughSpace;
  Encoder::getPage();	
  if (mode==Encoder::PUSH) {
    init=true;
    writer->resetBuffer();
    return buffer;
  }
  Log::writeToLog("BitSparseEncoder", 0, "Called getPage()");
  while (true) {
    // get the next block
    if (((currBlock==NULL) || (!currBlock->hasNext())) && (currVP == NULL)) {			
      do {
	currBlock=dataSrc->getNextValBlock(colIndex);	
	if (currBlock==NULL) {
	  if (init)	{
	    Log::writeToLog("BitSparseEncoder", 1, "Block was NULL, we dried DataSrc: returning NULL");
	    return NULL; // signal we are really done
	  }
	  else {
	    init=true;
	    currVP=NULL;
	    Log::writeToLog("BitSparseEncoder", 1, "Block was NULL, we dried DataSrc: returning buffer, numInts=",*lengthPtr);
	    *endPosPtr=currpos;
	    refactorPage();
	    return buffer;	// we dried up dataSrc
	  }
	}
	Log::writeToLog("BitSparseEncoder", 0, "Got new block");
      } while (!currBlock->hasNext());
    }
		
    if (!currBlock->isPosContiguous())
      throw new UnexpectedException("BitSparseEncoder: cannot encode a non contiguous column");
    
    // write initial value to a blank page
    if (init) {
      writer->resetBuffer();
      writer->skipToIntPos(3);
      enoughSpace = writer->reserveBufferSpace(5);
      assert(enoughSpace);
      if (currVP==NULL) currVP=currBlock->getNext();
      if (currVP==NULL) return NULL;		
      *startPosPtr=currpos+1;	
      currpos = currVP->position;
      if (isNull(currVP->value)) {
	*lengthPtr=0;
	init=false;
	currVP=NULL;
	t2b->initEmptyBuffer(currpos, nvp);
	continue;
      }
      if (!writer->writeString((char*)currVP->value)) throw new UnexpectedException("BitSparseEncoder: Could not write initial value");
      t2b->initEmptyBuffer(*startPosPtr, nvp);
      nv = t2b->getNumValues();
      test = t2b->testAddPos(currVP->position);
      enoughSpace=writer->reserveBufferSpace(test-nv);
      if (enoughSpace)
	t2b->addPosition(currVP->position);
      else
	throw new UnexpectedException("BitSparseEncoder: Could not write initial position. This might actually happen legally for a really sparse page. Will deal with this later.");
      Log::writeToLog("BitSparseEncoder", 1, "Wrote in init: startPos=", currVP->position);
      //Log::writeToLog("BitSparseEncoder", 1, "Wrote in init: value=", *(int*)currVP->value);
      *lengthPtr=1;
      init=false;
      currVP=NULL;
    }
    // append more values to same page
    else {
      if (currVP==NULL) currVP=currBlock->getNext();
      if (isNull(currVP->value)) {
	nv = t2b->getNumValues();
	test = t2b->testAddPos(currVP->position);
	enoughSpace=writer->reserveBufferSpace(test-nv);
	if (enoughSpace) {
	  t2b->setCurrEndPos(currVP->position);
	  currpos = currVP->position;
	  currVP=NULL;
	  continue;
	}
	init=true;
	*endPosPtr=currpos;
	refactorPage();
	return buffer;
      }
      if (!writer->writeString((char*)currVP->value)) {
	init=true;
	Log::writeToLog("BitSparseEncoder", 1, "Page full, returning page, length",*lengthPtr);
	*endPosPtr=currpos;
	refactorPage();
	return buffer;
      }
      else if (!writer->reserveBufferSpace(t2b->testAddPos(currVP->position) - t2b->getNumValues())) {
	writer->unWriteString();
	init=true;
	Log::writeToLog("BitSparseEncoder", 1, "Page full, returning page, length",*lengthPtr);
	*endPosPtr=currpos;
	refactorPage();
	return buffer;
      }
      else {
	t2b->addPosition(currVP->position);
	currpos = currVP->position;
	*lengthPtr=*lengthPtr + 1; 
	currVP=NULL;
      }
    }
  }
}

void BitSparseEncoder::refactorPage() {
  bool x;
  t2b->finishBufferEditing(*endPosPtr);
  byte* buffer2 = t2b->getBuffer();
  //int* intbuffer = (int*) buffer2;
  writer->unReserveSpace();
  x = writer->writeString((char*)buffer2, (4+t2b->getNumValues())*sizeof(int));
  //for (int i = 0; i < 4+t2b->getNumValues(); i++) {
  //x = writer->writeInt(intbuffer[i]);
  //assert(x);
  //}
  assert(x);
}

