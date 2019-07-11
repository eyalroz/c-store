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

#include "SparseEncoder.h"

SparseEncoder::SparseEncoder(Operator* dataSrc_, int colIndex_, int fieldSize, int bfrSizeInBits_) : Encoder(dataSrc_, colIndex_)
{
  if (dataSrc_==NULL) mode=PUSH;
  currBlock=NULL;
  currVP=NULL;
  init=true;
  writer=new StringWriter(fieldSize+sizeof(int), bfrSizeInBits_);
  buffer=writer->getBuffer();
  writer2=new StringWriter(fieldSize+sizeof(int), bfrSizeInBits_);
  buffer2=writer2->getBuffer();
  lengthPtr=(int*) buffer;
  startPosPtr=lengthPtr+1;
  endPosPtr=lengthPtr+2;

  writer->skipToIntPos(3);
  mode=SparseEncoder::UNINIT;
  nullcount = 0;
  notnullcount = 0;
  currpos=0;
  ssize = fieldSize;
  playBuffer = new char[fieldSize+sizeof(int)];
}

SparseEncoder::~SparseEncoder()
{
  delete writer;
  delete writer2;
  delete[] playBuffer;
}

//DNA: this is ugly code fix later
void SparseEncoder::resetPair() {
  currVP=NULL;
}

bool SparseEncoder::isNull(byte* val) {
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

int SparseEncoder::getNullCount() {
  return nullcount;
}

int SparseEncoder::getNotNullCount() {
  return notnullcount;
}

byte* SparseEncoder::getPage() {
  Encoder::getPage();	
  if (mode==Encoder::PUSH) {
    init=true;
    writer->resetBuffer();
    return buffer;
  }
  Log::writeToLog("SparseEncoder", 0, "Called getPage()");
  while (true) {
    // get the next block
    if (((currBlock==NULL) || (!currBlock->hasNext())) && (currVP == NULL)) {
      do {
	currBlock=dataSrc->getNextValBlock(colIndex);	
	if (currBlock==NULL) {
	  if (init)	{
	    Log::writeToLog("SparseEncoder", 1, "Block was NULL, we dried DataSrc: returning NULL");
	    return NULL; // signal we are really done
	  }
	  else {
	    init=true;
	    currVP=NULL;
	    Log::writeToLog("SparseEncoder", 1, "Block was NULL, we dried DataSrc: returning buffer, numInts=",*lengthPtr);
	    *endPosPtr=currpos;
	    refactorPage();
	    return buffer2;	// we dried up dataSrc
	  }
	}
	Log::writeToLog("SparseEncoder", 0, "Got new block");
      } while (!currBlock->hasNext());
    }
		
    if (!currBlock->isPosContiguous())
      throw new UnexpectedException("SparseEncoder: cannot encode a non contiguous column");
    
    // write initial value to a blank page
    if (init) {
      writer->resetBuffer();
      writer->skipToIntPos(3);
      if (currVP==NULL) currVP=currBlock->getNext();
      if (currVP==NULL) return NULL;
      *startPosPtr=currpos+1;
      currpos = currVP->position;
      if (isNull(currVP->value)) {
	*lengthPtr=0;
	init=false;
	currVP=NULL;
	continue;
      }
      memcpy(playBuffer,currVP->value, ssize);
      memcpy(playBuffer+ssize,&currVP->position, sizeof(int));      
      if (!writer->writeString(playBuffer)) throw new UnexpectedException("SparseEncoder: Could not write initial value");
      //if (!writer->writeInt(currVP->position)) throw new UnexpectedException("SparseEncoder: Could not write initial position");
      Log::writeToLog("SparseEncoder", 1, "Wrote in init: startPos=", currVP->position);
      //Log::writeToLog("SparseEncoder", 1, "Wrote in init: value=", *(int*)currVP->value);
      *lengthPtr=1;
      init=false;
      currVP=NULL;
    }
    // append more values to same page
    else {
      if (currVP==NULL) currVP=currBlock->getNext();
      if (isNull(currVP->value)) {
	currpos = currVP->position;
	currVP=NULL;
	continue;
      }
      memcpy(playBuffer,currVP->value, ssize);
      memcpy(playBuffer+ssize,&currVP->position, sizeof(int)); 
      if (!writer->writeString(playBuffer)) {
	init=true;
	Log::writeToLog("SparseEncoder", 1, "Page full, returning page, length",*lengthPtr);
	*endPosPtr=currpos;
	refactorPage();
	return buffer2;
      }
      /*else if (!writer->writeInt(currVP->position)) {
	writer->unWriteInt();
	init=true;
	Log::writeToLog("SparseEncoder", 1, "Page full, returning page, length",*lengthPtr);
	*endPosPtr=currpos;
	refactorPage();
	return buffer2;
	}*/
      else {
	currpos = currVP->position;
	*lengthPtr=*lengthPtr + 1;
	currVP=NULL;
      }
    }
  }
}

void SparseEncoder::refactorPage() {
  int headerSize;
  headerSize=3*sizeof(int);
  memcpy(buffer2, buffer, headerSize);
  //should probably use the StringReader/StringWriter interface, but the below code will be faster.
  for (int i = 0; i < *lengthPtr; i++) {
    memcpy(buffer2+headerSize+(ssize*i), buffer+headerSize+((ssize+sizeof(int))*i), ssize);
    memcpy(buffer2+headerSize+((*lengthPtr)*ssize)+(sizeof(int)*i), buffer+headerSize+((ssize+sizeof(int))*i)+ssize, sizeof(int));
    /*writer2->skipToIntPos(3+i);
    writer2->writeInt(*page);
    page++;
    writer2->skipToIntPos((*lengthPtr)+i+3);
    writer2->writeInt(*page);
    page++;*/
  }
}

