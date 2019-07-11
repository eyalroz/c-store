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
#include "Union.h"

Union::Union(Operator *rs_, Operator *ws_, int nCols_) :
  Merge(rs_, ws_, nCols_) {
  m_isInitialized = false;
  m_RSblockSize = 1;
  m_RSIsPosContiguous = false;
  m_RSIsValueSorted = false;
  m_pRLEBlock = NULL;
  m_pRLETriple = NULL;
  }

Union::~Union() {
  if (m_pRLEBlock != NULL) delete m_pRLEBlock;
}

void Union::init() {

}

void Union::initOutputBlocks() {
  //  m_outpuBlocks = new Block*[m_nCols];
}

void Union::generateBlocks() {
  Log::writeToLog("Union", 2, "generateBlocks");
  
  // no more input blocks
  if (m_wsInputBlocks[0] == NULL && m_rsInputBlocks[0] == NULL) {
    for (int i = 0; i < m_nCols; i++) {
      m_outputBlocks[i] = NULL;
    }
    return;
  }
      
  //ROS stream attributes
  if (!m_isInitialized) {
    if (m_rsInputBlocks[0] != NULL) {
      m_RSblockSize = m_rsInputBlocks[0]->getSize();      
      m_RSIsPosContiguous = m_rsInputBlocks[0]->isPosContiguous();
      m_RSIsValueSorted = m_rsInputBlocks[0]->isValueSorted();
      m_pRLEBlock = new RLEBlock(m_RSIsValueSorted);
      m_pRLETriple = new RLETriple();
    }
    m_isInitialized = true;
  }
  else if (m_rsInputBlocks[0] != NULL)
    for (int i = 0; i < m_nCols; i++)
      m_rsInputBlocks[i] = m_rs->getNextValBlock(i);
  else
    for (int i = 0; i < m_nCols; i++)
      m_wsInputBlocks[i] = m_ws->getNextValBlock(i);



  // write out ROS blocks, then WOS blocks
  if (m_rsInputBlocks[0] != NULL) {
    for (int i = 0; i < m_nCols; i++) {
      m_outputBlocks[i] = m_rsInputBlocks[i];
    }
  } else {
    for (int i = 0; i < m_nCols; i++) {
      /*
	COMMENTING OUT CODE THAT CONVERTS WOS INTO RLE SINCE IT DOESN'T WORK
	if (m_RSblockSize > 1  &&  m_RSIsPosContiguous) {
        if (m_wsInputBlocks[i] != NULL) {
          Pair* pair = m_wsInputBlocks[i]->getNext();
          m_pRLETriple->setTriple(pair->value, pair->position, 1);
          m_pRLEBlock->setTriple(m_pRLETriple);
          m_outputBlocks[i] = m_pRLEBlock;
        } else {
          m_outputBlocks[i] = NULL;
        }
	} else {*/
        m_outputBlocks[i] = m_wsInputBlocks[i];
	//}
    }
  }  
}
