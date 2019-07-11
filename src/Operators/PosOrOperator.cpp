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
#include "PosOrOperator.h"

PosOrOperator::PosOrOperator(Operator *leftOp_, int leftColIndex_, Operator *rightOp_, int rightColIndex_) : 
leftOp(leftOp_), leftColIndex(leftColIndex_), rightOp(rightOp_), rightColIndex(rightColIndex_)
{
	init();
}

void PosOrOperator::init()
{
  inputBlockL = inputBlockR = NULL;
  //inputVBlockR = NULL;
  m_BlockOut = NULL;
  //m_VBlockOut = NULL;
  //m_areInputBlocksInitialized = false;
	
  //postition holders
  prevPos = curPosL = curPosR = 0;
  fromL = true;

  //whatNext:
  //0: get next L block from child
  //1: get next R block from child
  //2: get both L and R blocks
  //3: get nothing
  whatNext = 2;
  doneL = false;
  doneR = false;
}

PosOrOperator::~PosOrOperator() {
  if (m_BlockOut != NULL) delete m_BlockOut;
}

// ASSUMPTIONS:
//  positions stream is sorted
//  cardinality of input columns equal
PosBlock* PosOrOperator::getNextPosBlock() 
{
  if (doneL && doneR)
    return NULL;
  if (doneL)
    return rightOp->getNextPosBlock(rightColIndex);
  if (doneR)
    return leftOp->getNextPosBlock(leftColIndex);

  if (whatNext == 0) {
    inputBlockL = (PosBlock*) leftOp->getNextPosBlock(leftColIndex);
    if (inputBlockL == NULL) {
      doneL = true;
      return inputBlockR->getIterator(prevPos+1, curPosR);
    }
    else
      curPosL = inputBlockL->getEndPos();
  }
  else if (whatNext == 1) {
    inputBlockR = (PosBlock*) rightOp->getNextPosBlock(rightColIndex);
    if (inputBlockR == NULL) {
      doneR =true;
      return inputBlockL->getIterator(prevPos+1, curPosL);
    }
    else
      curPosR = inputBlockR->getEndPos();
  }
  else if (whatNext == 2) {
    inputBlockL = (PosBlock*) leftOp->getNextPosBlock(leftColIndex);
    if (inputBlockL == NULL)
      doneL = true;
    inputBlockR = (PosBlock*) rightOp->getNextPosBlock(rightColIndex);
    if (inputBlockR == NULL)
      doneR = true;
    if (doneL && doneR) {
      return NULL;
    }
    if (doneL) {
      return inputBlockR;
    }
    if (doneR) {
      return inputBlockL;
    }
    assert (inputBlockL->isPosSorted());
    curPosL = inputBlockL->getEndPos();
    assert (inputBlockR->isPosSorted());
    curPosR = inputBlockR->getEndPos();
    //Initialize m_BlockOut. This code assumes that inputBlocks on each column are all of the same type (i.e. all PosRLEBlocks or all PosType2Blocks, etc. If we want to change that assumption, this checking needs to occur at every call to getNextPosBlock() rather than just the first.
    if (m_BlockOut == NULL) {
      if (inputBlockL->isSparse()) {
	m_BlockOut = inputBlockL->clone(*inputBlockL);
	fromL = false;
      }
      else if (inputBlockR->isSparse()) {
	m_BlockOut = inputBlockR->getPosBlock()->clone();
	fromL = true;
      }
      else if (!inputBlockL->isPosContiguous()) {
	m_BlockOut = inputBlockL->getPosBlock()->clone();
	fromL = false;
      }
      else if (!inputBlockR->isPosContiguous()) {
	m_BlockOut = inputBlockR->getPosBlock()->clone();
	fromL = true;
      }
      else {
	m_BlockOut = inputBlockL->getPosBlock()->clone();
	fromL = false;
      }
    }
  }
  else throw new UnexpectedException("This should never happen");
  
  Log::writeToLog("PosAndOperator", 1, "getNextPosBlock");

  
  unsigned int leftStartPos = inputBlockL->getStartPos();
  unsigned int rightStartPos = inputBlockR->getStartPos();
  
  //find a intersection between leftOp and rightOp
  if ((curPosL < rightStartPos) || (curPosR < leftStartPos)) {						
    //case left range is smaller than right range and no overlap
    if (curPosL < rightStartPos) {			
      leftStartPos = inputBlockL->getStartPos();
      curPosL = inputBlockL->getEndPos();
		whatNext = 0;
		int t = prevPos + 1;
		prevPos = curPosL;
      	return inputBlockL->getIterator(t, curPosL);
    } 	
    		
    //case right range is smaller than left range and no overlap
    if (curPosR < leftStartPos) {			
      rightStartPos = inputBlockR->getStartPos();
      curPosR = inputBlockR->getEndPos();
      whatNext = 1;
      int t = prevPos + 1;
      prevPos = curPosR;
	return inputBlockR->getIterator(t, curPosR);

    }			 			
  }
  
  PosBlockIter* left;
  PosBlockIter* right;
  if (curPosL < curPosR) {
    //whatNext = 3;
    int t = prevPos + 1;
    prevPos = curPosL;
    whatNext = 0;
    left = inputBlockL->getIterator(t, curPosL);
    right = inputBlockR->getIterator(t, curPosL);
    if(!right->getEndPos()) {
      if(!left->getEndPos()) {
	return getNextPosBlock();
      }
      else {
	return left;
      }
    }
    if(!left->getEndPos()) {
      return right;
    }
  }
  else if (curPosR < curPosL) {
    //whatNext = 3;
    int t = prevPos + 1;
    prevPos = curPosR;
    whatNext = 1;
    left = inputBlockL->getIterator(t, curPosR);
    right = inputBlockR->getIterator(t, curPosR);
    if(!right->getEndPos()) {
      if(!left->getEndPos()) {
	return getNextPosBlock();
      }
      else {
	return left;
      }
    }
    if(!left->getEndPos()) {
      return right;
    }
  }
  else { //they must be the same
    left = inputBlockL->getIterator(prevPos+1, curPosL);
    right = inputBlockR->getIterator(prevPos+1, curPosL);
    if(!right->getEndPos()) {
      if(!left->getEndPos()) {
	return getNextPosBlock();
      }
      else {
	return left;
      }
    }
    if(!left->getEndPos()) {
      return right;
    }
    prevPos = curPosL;
    whatNext = 2;
  }
  if (fromL) {
    left->posOr(right, m_BlockOut);
  } else {
    right->posOr(left, m_BlockOut);
	}
assert(m_BlockOut->getEndPos() >= left->getEndPos() && m_BlockOut->getEndPos() >= right->getEndPos());
assert(m_BlockOut->getStartPos() <= left->getStartPos() && m_BlockOut->getStartPos() <= right->getStartPos());
  return m_BlockOut;
}

Block* PosOrOperator::getNextValBlock(int val_)
{
  throw new UnexpectedException("Not supported.");
  return NULL;
}

PosBlock* PosOrOperator::getNextPosBlock(int val_)
{
  assert(val_ == 0);
  return getNextPosBlock();
}
