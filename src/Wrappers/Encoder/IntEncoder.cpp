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
// Encodes Int Pages
// Problems: migfer@mit.edu

#include "IntEncoder.h"

IntEncoder::IntEncoder(Operator* dataSrc_, int colIndex_, int bfrSizeInBits_) : Encoder(dataSrc_, colIndex_)
{
  if (dataSrc_==NULL) mode=PUSH;
  currBlock=NULL;
  currVP=NULL;
  init=true;
  writer=new IntWriter(bfrSizeInBits_);
  buffer=writer->getBuffer();
  lengthPtr=(int*) buffer;
  startPosPtr=lengthPtr+1;
  writer->skipToIntPos(2);
  mode=IntEncoder::UNINIT;
}

IntEncoder::~IntEncoder()
{
  delete writer;
}

//DNA: this is ugly code fix later
void IntEncoder::resetPair() {
  currVP=NULL;
}

byte* IntEncoder::getPage() {
  Encoder::getPage();	
  if (mode==Encoder::PUSH) {
    init=true;
    writer->resetBuffer();
    return buffer;
  }
  Log::writeToLog("IntEncoder", 0, "Called getPage()");
  while (true) {
    // get the next block
    if (((currBlock==NULL) || (!currBlock->hasNext())) && (currVP == NULL)) {			
      do {
	currBlock=dataSrc->getNextValBlock(colIndex);	
	if (currBlock==NULL) {
	  if (init)	{
	    Log::writeToLog("IntEncoder", 1, "Block was NULL, we dried DataSrc: returning NULL");
	    return NULL; // signal we are really done
	  }
	  else {
	    init=true;
	    currVP=NULL;
	    Log::writeToLog("IntEncoder", 1, "Block was NULL, we dried DataSrc: returning buffer, numInts=",*lengthPtr);
	    return buffer;	// we dried up dataSrc
	  }
	}
	Log::writeToLog("IntEncoder", 0, "Got new block");
      } while (!currBlock->hasNext());
    }
		
    if (!currBlock->isPosContiguous())
      throw new UnexpectedException("IntEncoder: cannot encode a non contiguous column");
		
    // write initial value to a blank page
    if (init) {
      writer->resetBuffer();
      writer->skipToIntPos(2);
      if (currVP==NULL) currVP=currBlock->getNext();
      assert(currVP->type == ValPos::INTTYPE);
      if (currVP==NULL) return NULL;		
      *startPosPtr=currVP->position;	
      if (!writer->writeInt(*(int*)currVP->value)) throw new UnexpectedException("IntEncoder: Could not write initial value");
      Log::writeToLog("IntEncoder", 1, "Wrote in init: startPos=", currVP->position);
      Log::writeToLog("IntEncoder", 1, "Wrote in init: value=", *(int*)currVP->value);
      *lengthPtr=1;
      init=false;
      currVP=NULL;
    }
    // append more values to same page
    else {
      if (currVP==NULL) currVP=currBlock->getNext();
      assert(currVP->type == ValPos::INTTYPE);
      
      if (!writer->writeInt(*(int*)currVP->value)) {
	init=true;
	Log::writeToLog("IntEncoder", 1, "Page full, returning page, length",*lengthPtr);
	return buffer;
      }
      else {
	Log::writeToLog("IntEncoder", 0, "Wrote: value=", *(int*)currVP->value);
	currVP=NULL;
	*lengthPtr=*lengthPtr+1;
      }
    }
  }
}

bool IntEncoder::writeVal(int val_, unsigned int pos_)  {
  Encoder::writeVal(val_,pos_);
  if (writer->writeInt(val_)) {
    if (init) {
      init=false;
      *startPosPtr=pos_;		
      *lengthPtr=0;			
      writer->resetBuffer();
      if (!writer->skipToIntPos(2)) throw new UnexpectedException("IntEncoder: cannot write int, buffer to small to initialize");		
      if (!writer->writeInt(val_)) throw new UnexpectedException("IntEncoder: cannot write int, buffer to small to initialize");		
      Log::writeToLog("IntEncoder", 0, "IntWriter PUSH mode, finished init");
    }
    *lengthPtr=*lengthPtr+1;
    return true;
  }
  else {
    return false;
  }	
}

