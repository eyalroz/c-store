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
// Writes DeltaPos Blocks
// Problems: migfer@mit.edu


#include "DeltaPosWriter.h"

DeltaPosWriter::DeltaPosWriter(Operator* blockSrc_, int colIndex_, int bfrSizeInBits_, int deltaSize_) : Writer(blockSrc_, colIndex_), bufferMap(0)
{
	bfrSizeInBits=bfrSizeInBits_;
	currBlock=new DeltaPosBlock(false);
	inBlock=NULL;
	currVP=NULL;
	deltaSize=deltaSize_;
	Log::writeToLog("DeltaPosWriter", 0, "initializing delta pos writer");							
}

DeltaPosWriter::~DeltaPosWriter()
{
	delete currBlock;
}


// Gets the next value block from the operator 
Block* DeltaPosWriter::getNextValBlock(int colIndex_) {
	Log::writeToLog("DeltaPosWriter", 0, "Called getNextValBlock() colIndx=", colIndex_);							
	if (colIndex_!=0) 
		return NULL;
	else {
		while (true) {
			currVP=getNextPair();
			//assume int type for now ... fix later
			assert(currVP==NULL || currVP->type == ValPos::INTTYPE);
			if (currVP==NULL) {
				if (bufferMap.empty()) {
					Log::writeToLog("DeltaPosWriter", 1, "getNextValBlock(): DataSource ran dry, Buffers also dry, returning NULL");							
					return NULL;
				}
				Log::writeToLog("DeltaPosWriter", 1, "getNextValBlock(): DataSource ran dry, returning unfilled buffers");							
				DeltaPosValue* dPV=((DeltaPosValue*) bufferMap.removeFirstObj());
				currBlock->setDeltaBuffer(dPV->getBuffer(), bfrSizeInBits);
				return currBlock;
			}
		
			int val=*(int*)currVP->value;
			unsigned int pos=currVP->position;
		
			// two cases: we have page already being created for this value 
			//			  or we don't
			
			if ((bufferMap.getNumObjs(val))<=0) {
				Log::writeToLog("DeltaPosWriter", 1, "getNextValBlock(): Creating new DeltaPosValue for value=", val);		
				bufferMap.putObj(val, new DeltaPosValue(bfrSizeInBits,val, pos, deltaSize),bfrSizeInBits);				
			}
			else {
				Log::writeToLog("DeltaPosWriter", 0, "getNextValBlock(): Using DeltaPosValue for value=", val);		
				DeltaPosValue* currDeltaBfr=((DeltaPosValue*) bufferMap.getObj(val));
				if (currDeltaBfr->writePos(pos)) {
					// do noting we, have not yet filled buffer	
				}
				else {
					// we filled: thus we remove it from buffer map and 
					// construct a block using it					
					
					currBlock->setDeltaBuffer(currDeltaBfr->getBuffer(), bfrSizeInBits);
					Log::writeToLog("DeltaPosWriter", 1, "getNextValBlock(): We filled buffer, returning block, for value=", val);		

					Log::writeToLog("DeltaPosWriter", 1, "getNextValBlock(): Creating another DeltaPosValue for value=", val);		


					// currDeltaBfr->reset(val, pos);
										
					// trying to reuse memory (above) (old code follows)
					bufferMap.removeObj(val);
					bufferMap.putObj(val, new DeltaPosValue(bfrSizeInBits,val, pos, deltaSize),bfrSizeInBits);				
					
					return currBlock;
				}		
			}	
		}
	}
}

ValPos* DeltaPosWriter::getNextPair() {
	if (inBlock==NULL) {
		Log::writeToLog("DeltaPosWriter", 0, "getNextPair(): Current Block is NULL, pulling another block");
		inBlock=blockSrc->getNextValBlock(colIndex);
		if (inBlock==NULL) {
			Log::writeToLog("DeltaPosWriter", 1, "getNextPair(): Block returned is NULL, returning NULL pair");
			return NULL;
		}
	}
	ValPos* pair=inBlock->getNext();
	if (pair==NULL) {
		Log::writeToLog("DeltaPosWriter", 0, "getNextPair(): Pair is NULL from inBlock, pulling next Block");
		inBlock=blockSrc->getNextValBlock(colIndex);
		do {
			if (inBlock==NULL) return NULL;
			pair=inBlock->getNext();
			if (pair==NULL) inBlock=blockSrc->getNextValBlock(colIndex);
		} while (pair==NULL);
		//Log::writeToLog("DeltaPosWriter", 0, "getNextPair(): Got a pair with value", pair->value);
		//Log::writeToLog("DeltaPosWriter", 0, "getNextPair(): Got a pair with position", pair->position);
		return pair;
	}
	else {
	  //Log::writeToLog("DeltaPosWriter", 0, "getNextPair(): Got a pair with value", pair->value);
	  //Log::writeToLog("DeltaPosWriter", 0, "getNextPair(): Got a pair with position", pair->position);
		return pair;
	}
}

// Gets the next position block (bitstring of positions) from the operator
PosBlock* DeltaPosWriter::getNextPosBlock(int colIndex_) {
	return NULL;
}

