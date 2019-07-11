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
#include "BAndOperator.h"

BAndOperator::BAndOperator(Operator *leftOp_, int leftColIndex_, Operator *rightOp_, int rightColIndex_) : 
leftOp(leftOp_), leftColIndex(leftColIndex_), rightOp(rightOp_), rightColIndex(rightColIndex_)
{
	init();
}

void BAndOperator::init()
{
  inputBlockL = inputBlockR = NULL;
  inputVBlockR = NULL;
  m_BlockOut = NULL;
  m_VBlockOut = NULL;
  m_areInputBlocksInitialized = false;
	
  m_prb = NULL;
  m_pbb = NULL;
  m_ptb = NULL;
  m_rb = NULL;
  m_bb = NULL;
  m_tb = NULL;
    
  //postition holders
  prevPosL = curPosL = prevPosR = curPosR = 0;  
}

//Initialize input blocks and return true if successful. If failed, then there will be no results to this query.
bool BAndOperator::initInputBlocks()
{
  assert(m_areInputBlocksInitialized == false);
  
  Log::writeToLog("BAndOperator", 2, "initializing input blocks");
  // initialize input blocks
  inputBlockL = (PosBlock*) leftOp->getNextPosBlock(leftColIndex);
  if (inputBlockL == NULL)
    return false;
  inputBlockR = (PosBlock*) rightOp->getNextPosBlock(rightColIndex);
  if (inputBlockR == NULL)
    return false;
  
  m_prb = new PosRLEBlock();
  m_pbb = new PosBasicBlock((inputBlockL->isPosSorted() && inputBlockR->isPosSorted()));
  m_ptb = new PosType2Block();
  
  //DNA: note, this assertion is false for a null query ... 
  //assert(inputBlock != NULL);
  
  m_areInputBlocksInitialized = true;
  return true;
}

//Initialize input blocks and return true if successful. If failed, then there will be no results to this query.
bool BAndOperator::initInputVBlocks()
{
  assert(m_areInputBlocksInitialized == false);
  
  Log::writeToLog("BAndOperator", 2, "initializing input blocks");
  // initialize input blocks
  inputBlockL = (PosBlock*) leftOp->getNextPosBlock(leftColIndex);
  if (inputBlockL == NULL)
    return false;
  inputVBlockR = rightOp->getNextValBlock(rightColIndex);
  if (inputVBlockR == NULL)
    return false;
  
  //m_prb = new PosRLEBlock();
  //m_pbb = new PosBasicBlock((inputBlockL->isPosSorted() && inputBlockR->isPosSorted()));
  //m_ptb = new PosType2Block();
  m_rb = new RLEBlock(new RLETriple(0,0,1), inputVBlockR->isValueSorted());
  //m_bb = new BasicBlock((inputBlockL->isPosSorted() && inputBlockR->isPosSorted()));
  m_tb = new Type2Block(inputVBlockR->isValueSorted());

  if (inputBlockL->getSize() == 1)
    leftOpType = TYPE3;
  else if (inputBlockL->isPosContiguous())
    leftOpType = TYPE1;
  else
    leftOpType = TYPE2;

  if (inputVBlockR->getSize() == 1)
    rightOpType = TYPE3;
  else if (!inputVBlockR->isOneValue())
    rightOpType = TYPE4;
  else if (inputVBlockR->isPosContiguous())
    rightOpType = TYPE1;
  else
    rightOpType = TYPE2;
  
  //DNA: note, this assertion is false for a null query ... 
  //assert(inputBlock != NULL);
  
  m_areInputBlocksInitialized = true;
  return true;
}

BAndOperator::~BAndOperator() {
  if (m_prb != NULL) delete m_prb;
  if (m_pbb != NULL) delete m_pbb;
  if (m_pbb != NULL) delete m_ptb;
  if (m_prb != NULL) delete m_rb;
  if (m_pbb != NULL) delete m_bb;
  if (m_pbb != NULL) delete m_tb;
}

// ASSUMPTIONS:
//   The operator above will read blocks in parallel.  This assumption is not
//     strictly necessary, but makes debugging easier.
//   getNextPosBlock() should generate output in parallel
PosBlock* BAndOperator::getNextPosBlock() 
{
	Log::writeToLog("BAndOperator", 1, "getNextPosBlock");
	
	// check if we initialized the input yet
	if (!m_areInputBlocksInitialized) {
		bool temp = initInputBlocks();
		if (!temp)
		return NULL;
	}
		     
	generateBlocks();

	Log::writeToLog("BAndOperator", 1, "Returning BAnd PosBlock");
	return m_BlockOut;
}

// ASSUMPTIONS:
//  positions always contiguous
//  positions across both block streams line up in rows
//  cardinality of input columns equal

void BAndOperator::generateBlocks() 
{
	Log::writeToLog("BAndOperator", 2, "Generating blocks...");
	
	m_BlockOut = NULL;
	
	//inputBlock should already be initialized and pointing to the next block from input
	if (inputBlockL == NULL || inputBlockR == NULL) return;
		
	//operator/datasource type
	enum OP_TYPE { RLE, DELTA};
	OP_TYPE leftOpType, rightOpType;
	
	if ((inputBlockL->getSize() > 1) && inputBlockL->isPosContiguous()) leftOpType = RLE;
	if ((inputBlockR->getSize() > 1) && inputBlockR->isPosContiguous()) rightOpType = RLE;
 		
	// leftOp: RLE block		rightOp: RLE block
	if ((leftOpType == RLE) && (rightOpType == RLE)) {
			int leftStartPos, leftEndPos, rightStartPos, rightEndPos;
			updateRightRange(rightStartPos, rightEndPos);			
			updateLeftRange(leftStartPos, leftEndPos);			
			
			//find a intersection between leftOp and rightOp
			while ((leftEndPos < rightStartPos) || (rightEndPos < leftStartPos)) 
			{											
				//case left range is smaller than right range and no overlap
				while (leftEndPos < rightStartPos) {			
					inputBlockL = leftOp->getNextPosBlock(leftColIndex);
					if (inputBlockL == NULL)
						return;
					updateLeftRange(leftStartPos, leftEndPos);			
				} 			
				//case right range is smaller than left range and no overlap
				while (rightEndPos < leftStartPos) {			
					inputBlockR = rightOp->getNextPosBlock(rightColIndex);
					if (inputBlockR == NULL)
						return;			
					updateRightRange(rightStartPos, rightEndPos);			
				}			 			
			}
			
			if ((inputBlockL == NULL || inputBlockR == NULL)) {
				return;
			} else {
				//a intersection found, determine output block range - five cases
				int startPos, endPos;
				//case 1: left range is equal to right range	1-10   1-10
				//case 2: left range contains right range		1-20   10-15 
				//case 3: right range contains left range		10-15   1-20 
				//case 4: right range "behind" left range		10-20	19-30
				//case 5: left range "behind" right range		19-30	10-20
				//any case not thought about ? 
				startPos = (leftStartPos > rightStartPos ? leftStartPos : rightStartPos);
				endPos 	 = (leftEndPos > rightEndPos ? rightEndPos : leftEndPos);
				if (endPos == leftEndPos)
					inputBlockL = leftOp->getNextPosBlock(leftColIndex);
				if (endPos == rightEndPos)
					inputBlockR = rightOp->getNextPosBlock(rightColIndex);					
				RLETriple* rt = m_prb->getTriple();
				rt->setTriple(NULL, startPos, (endPos-startPos+1));
				m_prb->setTriple(rt);
				m_BlockOut = m_prb;
				return;
			}
  } else {
    if ((curPosL == 0 && curPosR == 0) && (inputBlockL != NULL && inputBlockR != NULL)) {
      curPosL = getNextPosition(leftOp, leftColIndex, inputBlockL);
      curPosR = getNextPosition(rightOp, rightColIndex, inputBlockR);
    }
    while (curPosL != curPosR) {
      if (curPosL > curPosR) {
        prevPosR = curPosR;
        curPosR = getNextPosition(rightOp, rightColIndex, inputBlockR);
        if (curPosR == 0) break;        
      } else if (curPosL < curPosR) {
        prevPosL = curPosL;
        curPosL = getNextPosition(leftOp, leftColIndex, inputBlockL);
        if (curPosL == 0) break;        
      }
    }
    if (curPosL > 0 && curPosR > 0) {
      m_pbb->setPosition(curPosL); 
      m_BlockOut = m_pbb;      
      prevPosR = curPosR;
      prevPosL = curPosL;
      curPosL = getNextPosition(leftOp, leftColIndex, inputBlockL);
      curPosR = getNextPosition(rightOp, rightColIndex, inputBlockR);
    }    
  }
}

// ASSUMPTIONS:
//  positions always contiguous
//  positions across both block streams line up in rows
//  cardinality of input columns equal
//  get value block stream from right child position from left child

void BAndOperator::generateVBlocks() 
{
  m_VBlockOut = NULL;
  
  //inputBlock should already be initialized and pointing to the next block from input
  if (inputBlockL == NULL || inputVBlockR == NULL) {
    m_VBlockOut = NULL;
    return;
  }
  int leftStartPos, leftEndPos, rightStartPos, rightEndPos;
  
  updateVRightRange(rightStartPos, rightEndPos);			
  updateLeftRange(leftStartPos, leftEndPos);			
  
  //find a intersection between leftOp and rightOp
  while ((leftEndPos < rightStartPos) || (rightEndPos < leftStartPos)) 
    {											
      //case left range is smaller than right range and no overlap
      while (leftEndPos < rightStartPos) {			
	inputBlockL = leftOp->getNextPosBlock(leftColIndex);
	if (inputBlockL == NULL) {
	  m_VBlockOut = NULL;
	  return;
	}
	updateLeftRange(leftStartPos, leftEndPos);			
      } 			
      //case right range is smaller than left range and no overlap
      while (rightEndPos < leftStartPos) {			
	inputVBlockR = rightOp->getNextValBlock(rightColIndex);
	if (inputVBlockR == NULL) {
	  m_VBlockOut = NULL;
	  return;
	}			
	updateVRightRange(rightStartPos, rightEndPos);			
      }			 			
    }
  
  if ((inputBlockL == NULL || inputVBlockR == NULL)) {
    m_VBlockOut = NULL;
    return;
  } else {
    //Both size > 1 but just 1 value
      
      //a intersection found, determine output block range - five cases
      int startPos, endPos;
      //case 1: left range is equal to right range	1-10   1-10
      //case 2: left range contains right range		1-20   10-15 
      //case 3: right range contains left range		10-15   1-20 
      //case 4: right range "behind" left range		10-20	19-30
      //case 5: left range "behind" right range		19-30	10-20
      //any case not thought about ? 
      startPos = (leftStartPos > rightStartPos ? leftStartPos : rightStartPos);
      endPos 	 = (leftEndPos > rightEndPos ? rightEndPos : leftEndPos);

    if ((leftOpType == TYPE1) && (rightOpType == TYPE1)) {
      RLETriple* rt = m_rb->getTriple();
      rt->setTriple(inputVBlockR->getStartPair(), startPos, (endPos-startPos+1));
      m_rb->setTriple(rt);
      m_VBlockOut = m_rb;
    }
    
    else if ((leftOpType == TYPE1) && (rightOpType == TYPE2)) {
      m_tb->setBufferWithHeader(((Type2Block*)inputVBlockR)->makeNewBuffer(-1, startPos, endPos));
      //RLETriple* rt = m_rb->getTriple();
      //rt->setTriple(inputVBlockR->getStartPair()->value, startPos, (endPos-startPos+1));
      //m_rb->setTriple(rt);
      m_VBlockOut = m_tb;
    }
    
    else if ((leftOpType == TYPE1) && (rightOpType == TYPE3)) {
      m_VBlockOut = inputVBlockR;
    }
    
    if (endPos == leftEndPos)
      inputBlockL = leftOp->getNextPosBlock(leftColIndex);
    if (endPos == rightEndPos)
      inputVBlockR = rightOp->getNextValBlock(rightColIndex);	
    if ((m_VBlockOut) && (!m_VBlockOut->getStartPair()))
      generateVBlocks();
    return;
    
    //Left size > 1 but just 1 value, right same but not pos. contig
    /*if ((leftOpType == TYPE1) && (rightOpType == TYPE2)) {
      if ((rightStartPos<=leftStartPos) && (leftEndPos<=rightEndPos)) {
	outT2Block->setBufferWithHeader(pb->makeNewBuffer(currTriple->value, leftStartPos, leftEndPos));
	if (!outT2Block->getStartPair())
	  return false;
	return true;
      }
      else if ((rightStartPos<=leftStartPos) && (rightEndPos<=leftEndPos)) {
	outT2Block->setBufferWithHeader(pb->makeNewBuffer(currTriple->value, leftStartPos, rightEndPos));
	getNextNextTime = true;
	if (rightEndPos+1<=leftEndPos)
	  nextFilteredTriple->setTriple(currTriple->value, rightEndPos+1, leftEndPos-rightEndPos);
	if (!outT2Block->getStartPair())
	  return false;
	
	return true;
      }
      else if ((leftStartPos<=rightStartPos) && (leftEndPos<=rightEndPos) && (leftEndPos>=rightStartPos)) {
	getNextNextTime = false;
	outT2Block->setBufferWithHeader(pb->makeNewBuffer(currTriple->value, rightStartPos, leftEndPos));
	if (!outT2Block->getStartPair())
	  return false;
	return true;
      }
      else if ((leftStartPos<=rightStartPos) && (rightEndPos<=leftEndPos) && (leftStartPos<=rightEndPos)) {
	outT2Block->setBufferWithHeader(pb->makeNewBuffer(currTriple->value, rightStartPos, rightEndPos));
	getNextNextTime = true;	    
	if (rightEndPos+1<=leftEndPos)
	  nextFilteredTriple->setTriple(currTriple->value, rightEndPos+1, leftEndPos-rightEndPos);
	if (!outT2Block->getStartPair())
	  return false;
	return true;
      }
      else {
	if (leftEndPos<rightEndPos)
	  getNextNextTime = false;
	else
	  getNextNextTime = true;
	
	return false;
      }
    }*/
  }
  /*  else {
    if ((curPosL == 0 && curPosR == 0) && (inputBlockL != NULL && inputBlockR != NULL)) {
      curPosL = getNextPosition(leftOp, leftColIndex, inputBlockL);
      curPosR = getNextPosition(rightOp, rightColIndex, inputBlockR);
    }
    while (curPosL != curPosR) {
      if (curPosL > curPosR) {
	prevPosR = curPosR;
	curPosR = getNextPosition(rightOp, rightColIndex, inputBlockR);
	if (curPosR == 0) break;        
      } else if (curPosL < curPosR) {
	prevPosL = curPosL;
	curPosL = getNextPosition(leftOp, leftColIndex, inputBlockL);
	if (curPosL == 0) break;        
      }
    }
    if (curPosL > 0 && curPosR > 0) {
      m_pbb->setPosition(curPosL); 
      m_BlockOut = m_pbb;      
      prevPosR = curPosR;
      prevPosL = curPosL;
      curPosL = getNextPosition(leftOp, leftColIndex, inputBlockL);
      curPosR = getNextPosition(rightOp, rightColIndex, inputBlockR);
    }    
    }*/
}

Block* BAndOperator::getNextValBlock(int val_)
{
  //Log::writeToLog("BAndOperator", 1, "getNextValBlock");
	
	// check if we initialized the input yet
	if (!m_areInputBlocksInitialized) {
	  bool temp = initInputVBlocks();
	  if (!temp)
	    return NULL;
	}
	
	generateVBlocks();

	//Log::writeToLog("BAndOperator", 1, "Returning BAnd PosBlock");
	return m_VBlockOut;
}

PosBlock* BAndOperator::getNextPosBlock(int val_)
{
	return getNextPosBlock();
}

void BAndOperator::updateLeftRange(int& leftStartPos, int& leftEndPos)
{
  leftStartPos = inputBlockL->getStartPos();
	leftEndPos = inputBlockL->getPosAtLoc(inputBlockL->getSize()-1);
	assert(leftEndPos >= leftStartPos);	
}

void BAndOperator::updateRightRange(int& rightStartPos, int& rightEndPos)
{
	rightStartPos = inputBlockR->getStartPos();	
	rightEndPos = inputBlockR->getPosAtLoc(inputBlockR->getSize()-1);
	assert(rightEndPos >= rightStartPos);	
}

void BAndOperator::updateVRightRange(int& rightStartPos, int& rightEndPos)
{
	rightStartPos = inputVBlockR->getStartPair()->position;	
	rightEndPos = inputVBlockR->getEndPosition();//getPairAtLoc(inputVBlockR->getSize()-1)->position;
	assert(rightEndPos >= rightStartPos);	
}

//helper function to get next position. return 0 if no more postion in the stream
unsigned int BAndOperator::getNextPosition(Operator* pOp, int& colIndex, PosBlock*& pBlock) {
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
