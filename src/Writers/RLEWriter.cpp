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
// Writes RLE Blocks
// Problems: migfer@mit.edu


#include "RLEWriter.h"

RLEWriter::RLEWriter(Operator* blockSrc_, int colIndex_) : Writer(blockSrc_, colIndex_)
{
  currTriple=new RLETriple();
  outBlock=NULL;
  currBlock=blockSrc->getNextValBlock(colIndex);
  if (currBlock!=NULL) {
    outBlock=new RLEBlock(currBlock->isValueSorted());
    currValue = currBlock->peekNext()->clone();
  }
  init=true;
  //value=0;
  startPos=0;
  reps=0;
}

RLEWriter::~RLEWriter()
{
  delete outBlock;
  delete currValue;
}

// Gets the next value block from the operator 
Block* RLEWriter::getNextValBlock(int colIndex_) {
  if (outBlock==NULL) {
    Log::writeToLog("RLEWriter", 1, "Initial block is NULL returning NULL");
    return NULL;
  }
  // for initial case
  if (currBlock==NULL) {
    currBlock=blockSrc->getNextValBlock(colIndex);
    if (currBlock==NULL) return NULL; // we have no more blocks
  }
  
  RLETriple* currentTriple=processBlocks();
  if (currentTriple==NULL) return NULL;
  else {		
    //Log::writeToLog("RLEWriter", 1, "Returning triple, val=", currentTriple->value);
    //Log::writeToLog("RLEWriter", 1, "Returning triple, startPos=", currentTriple->startPos);
    //Log::writeToLog("RLEWriter", 1, "Returning triple, reps=", currentTriple->reps);
    outBlock->setTriple(currentTriple);
    return outBlock;
  }	
}

RLETriple* RLEWriter::processBlocks() {
  ValPos* currVP;
  while (true) {
    if (currBlock==NULL)
      return NULL;
    if (init) {
      if (currBlock->hasNext()) {
	currVP=currBlock->getNext();
	//assume int type for now ... fix later
	//assert(currVP->type == ValPos::INTTYPE);
	//value=*(int*)currVP->value;
	currValue->set(currVP->value);
	startPos=currVP->position;
	//Log::writeToLog("RLEWriter", 0, "Init Writing value=", value);
	Log::writeToLog("RLEWriter", 0, "Init Writing startPos=", startPos);
	reps=1;
	init=false;
	// continue in while loop
      }
      else {
	currBlock=blockSrc->getNextValBlock(colIndex);
	if (currBlock==NULL) return NULL;
	// else just continue in while loop	
      }
    }
    else {
      if (currBlock->hasNext()) {			
	currVP=currBlock->getNext();
	// two cases now, either the value matches or it doesn't
	//if (value==*(int*)currVP->value) {
	if (*currValue == currVP) {
	  reps++;
	  // continue in while loop
	}
	else {
	  //currValue->set(startPos, (byte*)&value);
	  currValue->position = startPos;
	  currTriple->setTriple(currValue, startPos, reps);
	  currValue->set(currVP->value);
	  //value=*(int*)currVP->value;
	  startPos=currVP->position;
	  //Log::writeToLog("RLEWriter", 0, "Writing value=", value);
	  Log::writeToLog("RLEWriter", 0, "Writing startPos=", startPos);
	  reps=1;			
	  return currTriple;
	}
      }
      else {
	currBlock=blockSrc->getNextValBlock(colIndex);
	if (currBlock==NULL) {
	  currValue->position = startPos;
	  //currValue->set(startPos, (byte*)&value);
	  currTriple->setTriple(currValue, startPos, reps);
	  return currTriple;
	}
	// else just continue in while loop	
      }
    }
  }
}

// Gets the next position block (bitstring of positions) from the operator
PosBlock* RLEWriter::getNextPosBlock(int colIndex_) {
  return NULL;	
}
