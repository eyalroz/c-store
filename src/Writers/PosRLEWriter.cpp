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
#include "PosRLEWriter.h"

// Writes positon RLEBlocks
//Problems: migfer@mit.edu

PosRLEWriter::PosRLEWriter(Operator* blockSrc_, int colIndex_) : Writer(blockSrc_, colIndex_)
{
	rleWriter=new RLEWriter(blockSrc_, colIndex_);
	triple=new RLETriple();
	outBlock=NULL;
	init=true;
	isTripleAvail=false;
}

PosRLEWriter::~PosRLEWriter()
{
	delete rleWriter;
	delete outBlock;
}

Block* PosRLEWriter::getNextValBlock(int colIndex_) {
	throw new UnexpectedException("PosRLEWriter: Error, position writer cannot write value blocks");
}
	
// Gets the next position block (bitstring of positions) from the operator
PosBlock* PosRLEWriter::getNextPosBlock(int colIndex_) {
	if (colIndex_==0) {
		while (true) {
			if (init) {
				outBlock=new PosRLEBlock();
				init=false;
				RLEBlock* blck=(RLEBlock*) rleWriter->getNextValBlock(0);
				if (blck==NULL) return NULL;
				tripleAvail=blck->getTriple();			
				isTripleAvail=true;
			}
			if (!isTripleAvail) {
				Log::writeToLog("PosRLEWriter", 0, "Got a new value block");
				RLEBlock* blck=(RLEBlock*) rleWriter->getNextValBlock(0);
				if (blck==NULL)  {
					if (outBlock!=NULL) {
						Log::writeToLog("PosRLEWriter", 0, "Returning last block, returning startPos", triple->startPos);
						Log::writeToLog("PosRLEWriter", 0, "Returning last block, Reps", triple->reps);
						outBlock->setTriple(triple);
						PosBlock* blk=outBlock;
						outBlock=NULL;
						return blk;	
					}
					return NULL;
				}
				tripleAvail=blck->getTriple();			

				if (tripleAvail==NULL)
					throw new CodingException("PosRLEWriter: error RLETriple was NULL");
				else if (tripleAvail->startPos==(triple->startPos+triple->reps)) {
					triple->reps+=tripleAvail->reps;	
				}
				else {
					isTripleAvail=true;
					outBlock->setTriple(triple);
					Log::writeToLog("PosRLEWriter", 0, "returning startPos", triple->startPos);
					Log::writeToLog("PosRLEWriter", 0, "returning reps", triple->reps);
					return outBlock;	
				}
			}
			else {
				triple->setTriple(tripleAvail);
				isTripleAvail=false;
			}
		}
	}
	
	return NULL;
}
	
void PosRLEWriter::writePair(int val_, unsigned int pos_) {
	throw new UnimplementedException("PosRLEWriter::writePair unimplemented");
}

