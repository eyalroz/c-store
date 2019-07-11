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
#include "PosAndOperator.h"

PosAndOperator::PosAndOperator(Operator *leftOp_, int leftColIndex_, Operator *rightOp_, int rightColIndex_) : 
leftOp(leftOp_), leftColIndex(leftColIndex_), rightOp(rightOp_), rightColIndex(rightColIndex_)
{
	init();
}

void PosAndOperator::init()
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
  //0: get next L block from chile
  //1: get next R block from child
  //2: get both L and R blocks
  whatNext = 2;
}

PosAndOperator::~PosAndOperator() {
  if (m_BlockOut != NULL) delete m_BlockOut;
}

// ASSUMPTIONS:
//  positions stream is sorted
//  cardinality of input columns equal
PosBlock* PosAndOperator::getNextPosBlock() 
{

  if (whatNext == 0) {
    inputBlockL = (PosBlock*) leftOp->getNextPosBlock(leftColIndex);
    if (inputBlockL == NULL)
      return NULL;
    curPosL = inputBlockL->getEndPos();
  }
  else if (whatNext == 1) {
    inputBlockR = (PosBlock*) rightOp->getNextPosBlock(rightColIndex);
    if (inputBlockR == NULL)
      return NULL;
    curPosR = inputBlockR->getEndPos();
  }
  else if (whatNext == 2) {
    inputBlockL = (PosBlock*) leftOp->getNextPosBlock(leftColIndex);
    if (inputBlockL == NULL)
      return NULL;
    inputBlockR = (PosBlock*) rightOp->getNextPosBlock(rightColIndex);
    if (inputBlockR == NULL)
      return NULL;
    assert (inputBlockL->isPosSorted());
    assert (inputBlockR->isPosSorted());
    curPosL = inputBlockL->getEndPos();
    curPosR = inputBlockR->getEndPos();
    
    //Initialize m_BlockOut. This code assumes that inputBlocks on each column are all of the same type (i.e. all PosRLEBlocks or all PosType2Blocks, etc. If we want to change that assumption, this checking needs to occur at every call to getNextPosBlock() rather than just the first.
    if (m_BlockOut == NULL) {
      if (inputBlockL->isSparse()) {
	m_BlockOut = inputBlockL->clone(*inputBlockL);
	fromL = true;
      }
      else if (inputBlockR->isSparse()) {
	m_BlockOut = inputBlockR->getPosBlock()->clone();
	fromL = false;
      }
      else if (!inputBlockL->isPosContiguous()) {
	m_BlockOut = inputBlockL->getPosBlock()->clone();
	fromL = true;
      }
      else if (!inputBlockR->isPosContiguous()) {
	m_BlockOut = inputBlockR->getPosBlock()->clone();
	fromL = false;
      }
      else {
	m_BlockOut = inputBlockL->getPosBlock()->clone();
	fromL = true;
      }
    }
  }
  else throw new UnexpectedException("This should never happen");
  Log::writeToLog("PosAndOperator", 1, "getNextPosBlock");

  if (inputBlockL == NULL || inputBlockR == NULL) {
  	return NULL;
  }
  
  unsigned int leftStartPos = inputBlockL->getStartPos();
  unsigned int rightStartPos = inputBlockR->getStartPos();
  //find a intersection between leftOp and rightOp
  while ((curPosL < rightStartPos) || (curPosR < leftStartPos)) {						
    //case left range is smaller than right range and no overlap
    while (curPosL < rightStartPos) {			
      inputBlockL = leftOp->getNextPosBlock(leftColIndex);
      if (inputBlockL == NULL)
	return NULL;
      leftStartPos = inputBlockL->getStartPos();
      curPosL = inputBlockL->getEndPos();
    } 			
    //case right range is smaller than left range and no overlap
    while (curPosR < leftStartPos) {			
      inputBlockR = rightOp->getNextPosBlock(rightColIndex);
      if (inputBlockR == NULL)
	return NULL;
      rightStartPos = inputBlockR->getStartPos();
      curPosR = inputBlockR->getEndPos();
    }			 			
  }
  

  PosBlockIter* left;
  PosBlockIter* right;
  if (curPosL < curPosR) {
    left = inputBlockL->getIterator(prevPos+1, curPosL);
    right = inputBlockR->getIterator(prevPos+1, curPosL);
    prevPos = curPosL;
    whatNext = 0;
    if(!right->getEndPos())
      return getNextPosBlock();
    if(!left->getEndPos())
      return getNextPosBlock();
  }
  else if (curPosR < curPosL) {
    left = inputBlockL->getIterator(prevPos+1, curPosR);
    right = inputBlockR->getIterator(prevPos+1, curPosR);
    prevPos = curPosR;
    whatNext = 1;
    if(!right->getEndPos())
      return getNextPosBlock();
    if(!left->getEndPos())
      return getNextPosBlock();
  }
  else { //they must be the same
    left = inputBlockL->getIterator(prevPos+1, curPosL);
    right = inputBlockR->getIterator(prevPos+1, curPosL);
    assert(right->getEndPos());
    prevPos = curPosL;
    whatNext = 2;
    if(!right->getEndPos())
      return getNextPosBlock();
    if(!left->getEndPos())
      return getNextPosBlock();
  }
  if (fromL) {
//  	cerr << "Left is from " << left->getStartPos() << " to " << left->getEndPos() << endl;
// 	cerr << "Right is from " << right->getStartPos() << " to " << right->getEndPos() << endl;
//  	cerr << "m_BlockOut is from " << m_BlockOut->getStartPos() << " to " << m_BlockOut->getEndPos() << endl;
    left->posAnd(right, m_BlockOut);
//    cerr << "mblockout is now from " << m_BlockOut->getStartPos() << " to " << m_BlockOut->getEndPos() << endl;
  } else {
//  	cerr << "Left is from " << left->getStartPos() << " to " << left->getEndPos() << endl;
//  	cerr << "Right is from " << right->getStartPos() << " to " << right->getEndPos() << endl;
    right->posAnd(left, m_BlockOut);
//    cerr << "m_BlockOut is from " << m_BlockOut->getStartPos() << " to " << m_BlockOut->getEndPos() << endl;
    
	}
// cerr << "posand m_BlockOut has size = " << m_BlockOut->getSize() << " addr " << m_BlockOut << endl;
	assert(m_BlockOut->getEndPos() <= left->getEndPos() && m_BlockOut->getEndPos() <= right->getEndPos());
	assert(m_BlockOut->getStartPos() >= left->getStartPos() && m_BlockOut->getStartPos() >= right->getStartPos());
  return m_BlockOut;
}

Block* PosAndOperator::getNextValBlock(int val_)
{
  throw new UnexpectedException("Not supported.");
  return NULL;
}

PosBlock* PosAndOperator::getNextPosBlock(int val_)
{
  assert(val_ == 0);
  return getNextPosBlock();
}
