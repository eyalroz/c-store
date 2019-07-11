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
#include "BOrOperator.h"

BOrOperator::BOrOperator(Operator *leftOp_, int leftColIndex_, Operator *rightOp_, int rightColIndex_) : 
leftOp(leftOp_), leftColIndex(leftColIndex_), rightOp(rightOp_), rightColIndex(rightColIndex_)
{
	init();
}

void BOrOperator::init()
{
	inputBlockL = inputBlockR = NULL;
	m_BlockOut = NULL;
	m_areInputBlocksInitialized = false;
  m_prb = NULL;
  m_pbb = NULL;
    
  //postition holders
  prevPosL = curPosL = prevPosR = curPosR = 0;    
}

//Initialize input blocks and return true if successful. If failed, then there will be no results to this query.
bool BOrOperator::initInputBlocks()
{
	assert(m_areInputBlocksInitialized == false);
	
	Log::writeToLog("BOrOperator", 2, "initializing input blocks");
	
	// initialize input blocks
	inputBlockL = (PosBlock*) leftOp->getNextPosBlock(leftColIndex);
	inputBlockR = (PosBlock*) rightOp->getNextPosBlock(rightColIndex);
  
  bool  leftSorted = true, rightSorted = true;
  if (inputBlockL != NULL)  leftSorted = inputBlockL->isPosSorted();
  if (inputBlockR != NULL)  rightSorted = inputBlockR->isPosSorted();
        
  m_prb = new PosRLEBlock();
  m_pbb = new PosBasicBlock((leftSorted & rightSorted));
	
	m_areInputBlocksInitialized = true;
	return true;
}

BOrOperator::~BOrOperator() {
  if (m_prb != NULL) delete m_prb;
  if (m_pbb != NULL) delete m_pbb;
}

// ASSUMPTIONS:
//   The operator above will read blocks in parallel.  This assumption is not
//     strictly necessary, but makes debugging easier.
//   getNextPosBlock() should generate output in parallel
PosBlock* BOrOperator::getNextPosBlock() 
{
	Log::writeToLog("BOrOperator", 1, "getNextPosBlock");
	
	// check if we initialized the input yet
	if (!m_areInputBlocksInitialized) {
		bool temp = initInputBlocks();
		if (!temp)
		return NULL;
	}
		     
	generateBlocks();

	Log::writeToLog("BOrOperator", 1, "Returning BOr PosBlock");
	return m_BlockOut;
}

// ASSUMPTIONS:
//  positions always contiguous
//  positions across both block streams line up in rows
//  cardinality of input columns equal

void BOrOperator::generateBlocks() 
{
	Log::writeToLog("BOrOperator", 2, "Generating blocks...");
	
	m_BlockOut = NULL;
	
	//inputBlock should already be initialized and pointing to the next block from input
	if (inputBlockL == NULL && inputBlockR == NULL) return;
		
	//operator/datasource type
	enum OP_TYPE { RLE, DELTA, UNKNOWN};
	OP_TYPE leftOpType = UNKNOWN;
	OP_TYPE rightOpType = UNKNOWN;
	
	if ((inputBlockL != NULL) && (inputBlockL->getSize() > 1) && inputBlockL->isPosContiguous()) {
		leftOpType = RLE;
		if (!inputBlockL->isBlockPosSorted())
			throw new UnexpectedException("BOrOperator: left operator position not sorted!");
		if (!inputBlockL->isPosContiguous())
			throw new UnexpectedException("BOrOperator: left operator not contiguous!");
	}
		
	if ((inputBlockR != NULL) && (inputBlockR->getSize() > 1) && inputBlockR->isPosContiguous()) {
		rightOpType = RLE;
		if (!inputBlockR->isBlockPosSorted())
			throw new UnexpectedException("BOrOperator: right operator position not sorted!");
		if (!inputBlockR->isPosContiguous())
			throw new UnexpectedException("BOrOperator: right operator not contiguous!");
	}
	 		
	if ((leftOpType == RLE) && (rightOpType == RLE) || 
		(leftOpType == RLE) && (inputBlockR == NULL) ||
		(rightOpType == RLE) && (inputBlockL == NULL)) {
			int leftStartPos, leftEndPos, rightStartPos, rightEndPos;
			updateRightRange(rightStartPos, rightEndPos);			
			updateLeftRange(leftStartPos, leftEndPos);
			
			//if one of the range is null, copy the non-null range
			if (leftStartPos == -1) {
				leftStartPos = rightStartPos;
				leftEndPos = rightEndPos;
			}
			if (rightStartPos == -1) {
				rightStartPos = leftStartPos;
				rightEndPos = leftEndPos;
			}
							
			//(assume both ranges not null)
			//if no intersection between the two 
			//	return the range with smaller start position and
			//		also advance this input datasource to next position block
			//else
			//	start position = min (leftStartPos, rightStartPos)
			//	start position = max (leftStartPos, rightStartPos)
			//	advance both input datasources to next position block
			
			int startPos, endPos;
			
			if (leftEndPos < rightStartPos) {
				startPos = leftStartPos;
				endPos = leftEndPos;
				inputBlockL = leftOp->getNextPosBlock(leftColIndex);			
			} else if (rightEndPos < leftStartPos) {
				startPos = rightStartPos;
				endPos = rightEndPos;
				inputBlockR = rightOp->getNextPosBlock(rightColIndex);
			} else {
				startPos = (leftStartPos > rightStartPos ? rightStartPos : leftStartPos);
				endPos 	 = (leftEndPos > rightEndPos ? leftEndPos : rightEndPos);
				inputBlockL = leftOp->getNextPosBlock(leftColIndex);
				inputBlockR = rightOp->getNextPosBlock(rightColIndex);						
			}
			RLETriple* rt = m_prb->getTriple();
			rt->startPos = startPos;
			rt->reps = endPos-startPos+1;
			//rt->setTriple(new IntValPos(startPos,1), startPos, (endPos-startPos+1));
			m_prb->setTriple(rt);
			m_BlockOut = m_prb;
			return;
    } else {
      if (curPosL == 0 && inputBlockL != 0) curPosL = getNextPosition(leftOp, leftColIndex, inputBlockL);
      if (curPosR == 0 && inputBlockR != 0) curPosR = getNextPosition(rightOp, rightColIndex, inputBlockR);
      int outPos = 0;
      if (curPosL > curPosR) {
        if (curPosR == 0) {
          outPos = curPosL;
          curPosL = getNextPosition(leftOp, leftColIndex, inputBlockL);                
        } else {
          outPos = curPosR;
          curPosR = getNextPosition(rightOp, rightColIndex, inputBlockR);
        }
      } else if (curPosL < curPosR) {      
        if (curPosL == 0) {
          outPos = curPosR;
          curPosR = getNextPosition(rightOp, rightColIndex, inputBlockR);
        } else {
          outPos = curPosL;
          curPosL = getNextPosition(leftOp, leftColIndex, inputBlockL); 
        }
      } else {
        if (curPosL != 0) {
          outPos = curPosL;
          curPosL = getNextPosition(leftOp, leftColIndex, inputBlockL);
          curPosR = getNextPosition(rightOp, rightColIndex, inputBlockR);
        }
      }
      if (outPos > 0) {
        m_pbb->setPosition(outPos); 
        m_BlockOut = m_pbb;
      }
    }
}

Block* BOrOperator::getNextValBlock(int val_)
{
	throw new UnexpectedException("BOrOperator: Not implemented.");
}

PosBlock* BOrOperator::getNextPosBlock(int val_)
{
	return getNextPosBlock();
}

void BOrOperator::updateLeftRange(int& leftStartPos, int& leftEndPos)
{
	leftStartPos = leftEndPos = -1;
	if (inputBlockL == NULL) return;
	leftStartPos = inputBlockL->getStartPos();	
	leftEndPos = inputBlockL->getPosAtLoc(inputBlockL->getSize()-1);
	assert(leftEndPos >= leftStartPos);	
}

void BOrOperator::updateRightRange(int& rightStartPos, int& rightEndPos)
{
	rightStartPos = rightEndPos = -1;
	if (inputBlockR == NULL) return;
	rightStartPos = inputBlockR->getStartPos();	
	rightEndPos = inputBlockR->getPosAtLoc(inputBlockR->getSize()-1);
	assert(rightEndPos >= rightStartPos);
}

//helper function to get next position. return 0 if no more postion in the stream
unsigned int BOrOperator::getNextPosition(Operator* pOp, int& colIndex, PosBlock*& pBlock) {
  unsigned int nextPos = 0;
  while (pBlock != NULL) {
    if (pBlock->hasNext()) {
      nextPos = pBlock->getNext();
    }
    if (nextPos > 0) break;
    pBlock = pOp->getNextPosBlock(colIndex);
  }
  return nextPos;
}
