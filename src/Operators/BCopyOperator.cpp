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
#include "BCopyOperator.h"
#include <stdio.h>

BCopyOperator::BCopyOperator(Operator *inOp_, int inColIndex_, int numParents)  
  : m_input(inOp_), m_ColIndex(inColIndex_), numParents_(numParents) {
  init();
}

void BCopyOperator::init() {
  // set both blocks to consumed; both blocks need to be generated
  m_iskBlockConsumed = new bool[numParents_];
  int i;
  for (i = 0; i < numParents_; i++)
    m_iskBlockConsumed[i] = true;


  //m_currOutputPosition = 0;

  inputBlock = NULL;
  inputPBlock = NULL;

  m_BlockOut = new Block*[numParents_];
  m_PBlockOut = new PosBlock*[numParents_];

  for (i = 0; i < numParents_; i++) {
    m_BlockOut[i] = NULL;
    m_PBlockOut[i] = NULL;
  }

  //m_areInputBlocksInitialized = false;

}

//Initialize input value blocks and return true if successful. If failed, then there will be no results to this query.
/*
bool BCopyOperator::initInputValBlocks() {
  assert(m_areInputBlocksInitialized == false);

  Log::writeToLog("BCopyOperator", 2, "initializing input blocks");
  // initialize input blocks
  inputBlock = m_input->getNextValBlock(m_ColIndex);

  //DNA: note, this assertion is false for a null query ... 
  //assert(inputBlock != NULL);
  if (inputBlock == NULL)
    return false;

  m_areInputBlocksInitialized = true;
  return true;
}

//Initialize input position blocks and return true if successful. If failed, then there will be no results to this query.
bool BCopyOperator::initInputPosBlocks() {
  assert(m_areInputBlocksInitialized == false);

  Log::writeToLog("BCopyOperator", 2, "initializing input blocks");
  // initialize input blocks
  inputPBlock = m_input->getNextPosBlock(m_ColIndex);

  //DNA: note, this assertion is false for a null query ... 
  //assert(inputBlock != NULL);
  if (inputPBlock == NULL)
    return false;

  m_areInputBlocksInitialized = true;
  return true;
}
*/

BCopyOperator::~BCopyOperator() {
  cout << " Bcopy Opertator begins destructor " << endl;
  delete[] m_iskBlockConsumed;

  for (int i = 0; i < numParents_; i++) {
    if (m_BlockOut[i] != NULL) {
      delete m_BlockOut[i];
    }
    if (m_PBlockOut[i] != NULL) {
      delete m_PBlockOut[i];
    }
  }

  delete[] m_BlockOut;
  delete[] m_PBlockOut;
  cout << " Bcopy Opertator done destructor " << endl;
}

// ASSUMPTIONS:
//   The operator above will read blocks in parallel.  This assumption is not
//     strictly necessary, but makes debugging easier.
//   getNextValBlock() should generate output in parallel
Block* BCopyOperator::getNextValBlock(int colIndex_) {
  Log::writeToLog("BCopyOperator", 1, "getNextValBlock with colIndex", colIndex_);

  if (colIndex_ >= numParents_)
    throw new UnexpectedException("BCopyOperator: asked for colIndex that doesn't exist.");

  // check if we initialized the input yet
  //if (!m_areInputBlocksInitialized) {
  //  bool temp = initInputValBlocks();
  //  if (!temp)
  //    return NULL;
  //}

  // check if we need to generate new output blocks
  bool allConsumed = true;
  int i;
  for (i = 0; i < numParents_; i++)
    if (m_iskBlockConsumed[i] == false)
      allConsumed = false;
  
  if (allConsumed) {
    for (i = 0; i < numParents_; i++)
      m_iskBlockConsumed[i] = false;
    generateBlocks();
  }

  if (m_iskBlockConsumed[colIndex_] == false) {
    m_iskBlockConsumed[colIndex_] = true;
    Log::writeToLog("BCopyOperator", 1, "Returning Copied Block");
    return m_BlockOut[colIndex_];
  }
  else {
  	cerr << "Bcopy: already consumed asked for block in column " << colIndex_ << endl;
      throw new UnexpectedException("BCopyOperator: already consumed asked for block");

  }
}

// INVARIANT:
// inputBlock is always set to next pair to be read
void BCopyOperator::generateBlocks() {
  Log::writeToLog("BCopyOperator", 2, "Generating blocks...");
  
  //inputBlock should already be initialized and pointing to the next block from input

  int i;
  inputBlock = m_input->getNextValBlock(m_ColIndex);
  if (inputBlock == NULL) {
    for (i = 0; i < numParents_; i++) {
      m_BlockOut[i] = NULL;
    }
    return;
  }

  for (i = 0; i < numParents_; i++) {
    if (!( dynamic_cast<BasicBlock*>( inputBlock ) )) {
      delete m_BlockOut[i];
      if (inputBlock->getBlock() == inputBlock)
	m_BlockOut[i] = inputBlock->getIterator();
      else
	m_BlockOut[i] = inputBlock->getIterator(inputBlock->getStartPair()->position, inputBlock->getEndPosition());
    }
    else
      m_BlockOut[i] = inputBlock->clone(*inputBlock);
  }
}

// INVARIANT:
// inputPBlock is always set to next pair to be read
void BCopyOperator::generatePBlocks() {
  Log::writeToLog("BCopyOperator", 2, "Generating position blocks...");
  
  //inputPBlock should already be initialized and pointing to the next block from input

  int i;
  inputPBlock = m_input->getNextPosBlock(m_ColIndex);
  if (inputPBlock == NULL) {
    for (i = 0; i < numParents_; i++) {
      m_PBlockOut[i] = NULL;
    }
    return;
  }
  
  for (i = 0; i < numParents_; i++) {
    if (!( dynamic_cast<PosBasicBlock*>( inputPBlock ) )) {
      delete m_PBlockOut[i];
      if (inputPBlock->getPosBlock() == inputPBlock)
	m_PBlockOut[i] = inputPBlock->getIterator();
      else
	m_PBlockOut[i] = inputPBlock->getIterator(inputPBlock->getStartPos(), inputPBlock->getEndPos());
    }
    else
      m_PBlockOut[i] = inputPBlock->clone(*inputPBlock);
    //cout << " creation of the m_PBlockOut " << m_PBlockOut[i] << " and i " << i << " input bloc? " << inputPBlock <<  endl;
    //cout << " What's my triple??? " << ((RLEBlock*)m_PBlockOut[i])->getTriple() <<  endl;
  }
}

PosBlock* BCopyOperator::getNextPosBlock(int colIndex_) {
  Log::writeToLog("BCopyOperator", 1, "getNextPosBlock with colIndex", colIndex_);

  if (colIndex_ >= numParents_)
    throw new UnexpectedException("BCopyOperator: asked for colIndex that doesn't exist.");

  /*// check if we initialized the input yet
  if (!m_areInputBlocksInitialized) {
    bool temp = initInputPosBlocks();
    if (!temp)
      return NULL;
      }*/

  

  // check if we need to generate new output blocks
  bool allConsumed = true;
  int i;
  for (i = 0; i < numParents_; i++)
    if (m_iskBlockConsumed[i] == false)
      allConsumed = false;
  
  if (allConsumed) {
    for (i = 0; i < numParents_; i++)
      m_iskBlockConsumed[i] = false;
    generatePBlocks();
  }

  if (m_iskBlockConsumed[colIndex_] == false) {
    m_iskBlockConsumed[colIndex_] = true;
    Log::writeToLog("BCopyOperator", 1, "Returning Copied Block");
    return m_PBlockOut[colIndex_];
  }
  else {
      throw new UnexpectedException("BCopyOperator: already consumed asked for block");

  }
}

PosBlock* BCopyOperator::getNextPosBlock(int colIndex_, int val_) {
	return NULL;
}

/*bool BCopyOperator::isInputSourceEmpty() {
  // Check if input source is empty
  // If it is, there are no more blocks to generate

  if (m_aggBlockIn == NULL || m_groupBlockIn == NULL) {
    // check if in bogus case? is this case bogus?
    if (m_aggBlockIn != NULL || m_groupBlockIn != NULL) {
      throw new UnexpectedException("BCopyOperator: different number of blocks for Agg and Group By");
    }
    return true;
  }   
  return false;
  }*/


/*void BCopyOperator::logOutputBlock(int value_) {
  Log::writeToLog("BCopyOperator", 1, "Output Position", m_currOutputPosition);
  Log::writeToLog("BCopyOperator", 1, "  value", value_);
}

void BCopyOperator::logOutputBlock(int group_, int value_) {
  Log::writeToLog("BCopyOperator", 1, "Output Position", m_currOutputPosition);
  Log::writeToLog("BCopyOperator", 1, "  group", group_);  
  Log::writeToLog("BCopyOperator", 1, "  value", value_);
}
*/
