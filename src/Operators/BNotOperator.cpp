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
#include "BNotOperator.h"

BNotOperator::BNotOperator(Operator *inputOp_, int inputColIndex_):
	inputOp(inputOp_), inputColIndex(inputColIndex_)
{
	init();
}

void BNotOperator::init()
{
	inputBlock = NULL;
	m_BlockOut = NULL;
	m_areInputBlocksInitialized = false;
	m_prb = NULL;
	m_pbb = NULL;
	lastInputBlockPos = 0;
	inputOpType = BNotOperator::UNKNOWN;
	prevInputBlockStartPos = prevInputBlockEndPos = 0;
	prevOutputBlockPos = 0;
}

//Initialize input blocks and return true if successful. If failed, then there will be no results to this query.
bool BNotOperator::initInputBlocks()
{
	assert(m_areInputBlocksInitialized == false);
	
	Log::writeToLog("BNotOperator", 2, "initializing input blocks");
	
	// initialize input blocks
	inputBlock = (PosBlock*) inputOp->getNextPosBlock(inputColIndex);
	if (inputBlock == NULL) {			
		return false;
	} else {
		m_areInputBlocksInitialized = true;
		lastInputBlockPos = inputOp->getLastPosition();
		setInputOpType();
		if (inputOpType == BNotOperator::RLE) {
			m_prb = new PosRLEBlock();
		} else {
			m_pbb = new PosBasicBlock(inputBlock->isPosSorted());
		}		
		return true;
	}
}

BNotOperator::~BNotOperator() {		
	if (m_prb != NULL) delete m_prb;
	if (m_pbb != NULL) delete m_pbb;	
}

// ASSUMPTIONS:
//   The operator above will read blocks in parallel.  This assumption is not
//     strictly necessary, but makes debugging easier.
//   getNextPosBlock() should generate output in parallel
PosBlock* BNotOperator::getNextPosBlock() 
{
	Log::writeToLog("BNotOperator", 1, "getNextPosBlock");
	
	// check if we initialized the input yet
	if (!m_areInputBlocksInitialized) {
		bool temp = initInputBlocks();
		if (!temp) return NULL;
	}
		     
	generateBlocks();

	Log::writeToLog("BNotOperator", 1, "Returning BNot PosBlock");
	return m_BlockOut;
}

// ASSUMPTIONS:
//  positions always contiguous
//  positions across both block streams line up in rows

void BNotOperator::generateBlocks()
{
	Log::writeToLog("BNotOperator", 2, "Generating blocks...");
	
	m_BlockOut = NULL;
					 		
	if (inputOpType == BNotOperator::RLE) {
		processPosRLEBlock();	
  } else if (inputOpType == BNotOperator::BASIC) {
    processPosBasicOrDeltaBlock();
  } else if (inputOpType == BNotOperator::DELTA) {
    processPosBasicOrDeltaBlock();   
  } else {
    throw UnimplementedException("Input stream type not recongnized");
	}
}

Block* BNotOperator::getNextValBlock(int val_)
{
	throw new UnexpectedException("BNotOperator: Not implemented.");
}

PosBlock* BNotOperator::getNextPosBlock(int val_)
{
	return getNextPosBlock();
}

void BNotOperator::updateInputRange(unsigned int& inputStartPos, unsigned int& inputEndPos)
{
	inputStartPos = inputBlock->getStartPos();	
	inputEndPos = inputBlock->getPosAtLoc(inputBlock->getSize()-1);
	assert(inputEndPos >= inputStartPos);
}

void BNotOperator::updatePrevInputRange(unsigned int& startPos, unsigned int& endPos)
{
	prevInputBlockStartPos = startPos;
	prevInputBlockEndPos = endPos;
}

void BNotOperator::setInputOpType()
{
	if (inputBlock == NULL) return;
	
	//assume blocks in the input stream have same properties
	int blockSize = 0;
	blockSize = inputBlock->getSize();
	bool blockIsPosContiguous = inputBlock->isPosContiguous();
	//bool blockIsPosSorted = inputBlock->isBlockPosSorted();
	
	// Stream properties
	//bool streamIsPosSorted = inputBlock->isPosSorted();	
	
	if (blockSize > 1) {
		if (blockIsPosContiguous) {
			inputOpType = BNotOperator::RLE;
		} else {
			inputOpType = BNotOperator::DELTA;
		}
	} else if (blockSize == 1) {
		inputOpType = BNotOperator::BASIC;	
	}
}

//input stream is RLE compressed
void BNotOperator::processPosRLEBlock()
{
	if ((inputBlock == NULL) && (prevInputBlockStartPos == 0)) return;
	//start-end positions for output block
	unsigned int startPos, endPos;
	unsigned int inputBlockStartPos, inputBlockEndPos;
	if (inputBlock != NULL) {
		updateInputRange(inputBlockStartPos, inputBlockEndPos);
		if (prevInputBlockEndPos == 0) {
			if (inputBlockStartPos == 1) {
				if (inputBlockEndPos != lastInputBlockPos) {
					startPos = inputBlockEndPos + 1;
					inputBlock = (PosBlock*) inputOp->getNextPosBlock(inputColIndex);
					if (inputBlock != NULL) {
						updateInputRange(inputBlockStartPos, inputBlockEndPos);
						endPos = inputBlockStartPos - 1;
					} else {
						endPos = lastInputBlockPos;				
					}
				} else {
					startPos = endPos = 0;									
				}
			} else {
				startPos = 1;
				endPos = inputBlockStartPos - 1;
			}
		} else {
			startPos = prevInputBlockEndPos + 1;
			endPos = inputBlockStartPos - 1;
		}
		if (inputBlockEndPos == lastInputBlockPos) {
			//last input block
			prevInputBlockStartPos = prevInputBlockEndPos = 0;				
		} else {
			updatePrevInputRange(inputBlockStartPos, inputBlockEndPos);
		}
	} else {
		if (prevInputBlockEndPos != 0) {
			startPos = prevInputBlockEndPos + 1;
			endPos = lastInputBlockPos;
			//this will be the last output
			prevInputBlockStartPos = prevInputBlockEndPos = 0;
		}
	}
		
	if (startPos > 0 && endPos > 0) {
    makeOutputBlock(inputOpType, startPos, endPos);
		m_BlockOut = m_prb;
	}				
	if (m_BlockOut != NULL) inputBlock = (PosBlock*) inputOp->getNextPosBlock(inputColIndex);
		
	return;
}

//input stream are basic blocks or delta blocks
void BNotOperator::processPosBasicOrDeltaBlock()
{
	//strategy:
	//	use prevInputBlockStartPos and prevInputBlockEndPos	as "range" pointers
	//	output PosBasicBlock if any gap(i.e. distance > 1) between these two pointers
	//	prevOutputBlockPos will be advanced accordingly
	//
	unsigned int outputBlockPos = 0;

	if ((prevInputBlockEndPos == lastInputBlockPos) || (prevOutputBlockPos == lastInputBlockPos)) return;

	if (inputBlock != NULL) {
		if (prevOutputBlockPos == 0) {
      prevInputBlockStartPos = getNextPosition();
			if (prevInputBlockStartPos > 1) {
				outputBlockPos = prevOutputBlockPos + 1;
				prevInputBlockEndPos = prevInputBlockStartPos;
				prevInputBlockStartPos = 0;
      } else if (prevInputBlockStartPos == 1) {
        prevInputBlockEndPos = getNextPosition();
				if (prevInputBlockEndPos == 0) prevInputBlockEndPos = prevInputBlockStartPos;
				findGap(outputBlockPos);
			}
		} else {
			if ((prevOutputBlockPos < prevInputBlockEndPos-1) && (prevOutputBlockPos > prevInputBlockStartPos)) {
				outputBlockPos = prevOutputBlockPos + 1;
			} else {
				prevInputBlockStartPos = prevInputBlockEndPos;
				findGap(outputBlockPos);
			}		
		}
		if ((outputBlockPos==0) && (inputBlock==NULL)) {
			outputBlockPos = prevInputBlockEndPos + 1;
		}
	} else {
		if (prevOutputBlockPos < lastInputBlockPos) {
			outputBlockPos = prevOutputBlockPos + 1;
		}	else {
			outputBlockPos = 0;
		}
	}
	
	if ((outputBlockPos > 0) && (outputBlockPos <= lastInputBlockPos)) {
    makeOutputBlock(inputOpType, outputBlockPos, outputBlockPos);
    prevOutputBlockPos = outputBlockPos;
	}
		
	return;	
	
}

//helper function to find gap between two input positions
void BNotOperator::findGap(unsigned int& outputBlockPos) {
  while (((prevInputBlockEndPos-prevInputBlockStartPos)<=1) && (inputBlock != NULL)) {
    prevInputBlockStartPos = prevInputBlockEndPos;
    unsigned int nextPos = getNextPosition();
    if (nextPos > 0) {
      prevInputBlockEndPos = nextPos;
    } else {
      prevInputBlockEndPos = prevInputBlockStartPos;
    }
    if ((prevInputBlockEndPos-prevInputBlockStartPos) > 1) outputBlockPos = prevInputBlockStartPos + 1;
  }
}

//helper function to get next position. return 0 if no more postion in the stream
unsigned int BNotOperator::getNextPosition() {
  unsigned int nextPos = 0;
  while (inputBlock != NULL) {
    if (inputBlock->hasNext()) {
      nextPos = inputBlock->getNext();
    }
    if (nextPos > 0) break;
    inputBlock = (PosBlock*) inputOp->getNextPosBlock(inputColIndex);
  }
  return nextPos;
}

void BNotOperator::makeOutputBlock(OP_TYPE outputType, const unsigned int startPos, const unsigned int endPos)
{
  if (outputType == BNotOperator::RLE) {		
    RLETriple* rt = m_prb->getTriple();
    rt->startPos = startPos;
    rt->reps = endPos-startPos+1;
    //rt->setTriple(new IntValPos(startPos, 1), startPos, (endPos-startPos+1));
		m_prb->setTriple(rt);
		m_BlockOut = m_prb;
  } else {
		m_pbb->setPosition(endPos);	
		m_BlockOut = m_pbb;
  }
	return;
}
