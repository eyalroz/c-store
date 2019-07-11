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
#include "Merge.h"

Merge::Merge(Operator *rs_, Operator *ws_, int nCols_) :
  m_rs(rs_), m_ws(ws_), m_nCols(nCols_)
{
  assert(rs_ != ws_);
  
  m_isInitialized = false;
  
  m_rsInputBlocks = new Block*[nCols_];
  m_wsInputBlocks = new Block*[nCols_];
  m_outputBlocks = new Block*[nCols_];
  m_currOutputPosition = 0;
  m_isConsumed = new bool[nCols_];
  for (int i = 0; i < m_nCols; i++) {
    m_isConsumed[i] = true;
    m_outputBlocks[i] = NULL;
  }

}

Merge::~Merge() {
  delete[] m_isConsumed;
  delete[] m_rsInputBlocks;
  delete[] m_wsInputBlocks;
  for (int i = 0; i < m_nCols; i++) {
    if (m_outputBlocks[i] != NULL) delete m_outputBlocks[i];
  }
  delete[] m_outputBlocks;
}

void Merge::loadWSInputBlocks() {
  Log::writeToLog("Merge", 2, "loading WS input blocks");

  // init WS input blocks 
  for (int i = 0; i < m_nCols; i++) {
    m_wsInputBlocks[i] = m_ws->getNextValBlock(i);
  }
}

void Merge::loadRSInputBlocks() {
  Log::writeToLog("Merge", 2, "loading RS input blocks");

  // init RS input blocks 
  for (int i = 0; i < m_nCols; i++) {
    m_rsInputBlocks[i] = m_rs->getNextValBlock(i);
  }
}

void Merge::initOutputBlocks() {
  Log::writeToLog("Merge", 2, "initializing output blocks");

  // TODO: update to real output block types based on input block types
  for (int i = 0; i < m_nCols; i++) {
    if (m_RSblockSize > 1  &&  m_RSIsPositionContiguous) {
      m_outputBlocks[i] = new RLEBlock(m_RSIsValueSorted);
    } else {
      m_outputBlocks[i] = new BasicBlock(true, true, true);
    }
  }
}

Block* Merge::getNextValBlock(int colIndex_) {
  assert(colIndex_ < m_nCols);

  Log::writeToLog("Merge", 2, "getNextValBlock", colIndex_);

  if (!m_isInitialized) {
    loadRSInputBlocks();
    loadWSInputBlocks();
    initOutputBlocks();
    init();
    m_isInitialized = true;
    
    //ROS stream attributes
    if (m_rsInputBlocks[0] != NULL) {
      m_RSblockSize = m_rsInputBlocks[0]->getSize();    
      // im pretty sure this is a typo; it should be isPosContiguous() instead of getSize()
      //m_RSIsPositionContiguous = m_rsInputBlocks[0]->getSize();
      m_RSIsPositionContiguous = m_rsInputBlocks[0]->isPosContiguous();
      m_RSIsValueSorted = m_rsInputBlocks[0]->isValueSorted();
    }
    
    // if both rs and ws are empty, return null
    if (m_rsInputBlocks[0] == NULL && m_wsInputBlocks[0] == NULL) {
      return NULL;
    }
  }

  // if requested column is consumed, regenerate blocks
  if (m_isConsumed[colIndex_]) {
    // as a safety check, make sure all other columns have been consumed already
    for (int i = 0; i < m_nCols; i++) {
      if (i == colIndex_ && !m_isConsumed[i]) {
	throw new UnexpectedException("Non-parallel consumption of blocks");
      }
    }
    generateBlocks();
    for (int i = 0; i < m_nCols; i++) {
      m_isConsumed[i] = false;
    }
  }

  m_isConsumed[colIndex_] = true;
  
  return m_outputBlocks[colIndex_];
}

PosBlock* Merge::getNextPosBlock(int colIndex_) {
  throw new UnexpectedException("Not Implemented");
}

PosBlock* Merge::getNextPosBlock(int colIndex_, int val_) {
  throw new UnexpectedException("Not Implemented");
}
