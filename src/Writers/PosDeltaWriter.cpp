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
#include "PosDeltaWriter.h"

// Writes Position Delta Blocks
// Problems: migfer@mit.edu


PosDeltaWriter::PosDeltaWriter(Operator* blockSrc_, int colIndex_, int bfrSizeInBits_, int deltaSize_) : Writer(blockSrc_, colIndex_)
{
	deltaEncoder=new DeltaEncoder(NULL, 0, deltaSize_, bfrSizeInBits_, 1);
	bbWriter=new BasicBlockWriter(blockSrc_, colIndex_);
	isBlockAvail=false;
	done=false;
	outBlock=new PosDeltaBlock();
}

PosDeltaWriter::~PosDeltaWriter()
{
	delete deltaEncoder;
	delete bbWriter;
	delete outBlock;
}


// Gets the next value block from the operator 
Block* PosDeltaWriter::getNextValBlock(int colIndex_) {
	throw new UnexpectedException("PosDeltaWriter: cannot get a value block on a position writer");	
}
	
// Gets the next position block (bitstring of positions) from the operator
PosBlock* PosDeltaWriter::getNextPosBlock(int colIndex_) {
	if (colIndex_==0) {
		while (true) {
			if (!isBlockAvail) 	{
				BasicBlock* currBlock=((BasicBlock*) bbWriter->getNextValBlock(0));
				if (currBlock==NULL) {
					if (done) return NULL;
					done=true;
					byte* bfr=deltaEncoder->getPage();
					if (bfr==NULL) return NULL;
					else outBlock->setBuffer(bfr, deltaEncoder->getBufferSize());	 
					return outBlock;
				}
				else {				
					currPos=currBlock->getStartPair()->position;
				}
			}
			else {
				isBlockAvail=false;
			}

			if (!deltaEncoder->writeVal(currPos, currPos)) {
				isBlockAvail=true;
				outBlock->setBuffer(deltaEncoder->getPage(), deltaEncoder->getBufferSize());	
				return outBlock;					
			}			
			
		}
	}
	else {
		return NULL;	
	}
}
