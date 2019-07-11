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
#include "TestDataSrc.h"

TestDataSrc::TestDataSrc( int nCols_ ) : Operator()
{
  m_nCols = nCols_;
  list<Block*> blah, bleh;
  bleh = blah;
	lastPosition = 0;

  blockLists = new list<Block*>[nCols_];
  posBlockLists = new list<PosBlock*>[nCols_];

  /*
  for (int i = 0; i < nCols_; i++) {
    blockLists[i] = new (list<Block*>);
    posBlockLists[i] = new list<PosBlock*>;
  }
  */
}

TestDataSrc::~TestDataSrc()
{
}
void TestDataSrc::addBlock(Block* block, int colIndex_) {
  assert(colIndex_ < m_nCols);
  blockLists[colIndex_].push_back(block);
}
void TestDataSrc::addBlock(PosBlock* block, int colIndex_) {
  assert(colIndex_ < m_nCols);
  posBlockLists[colIndex_].push_back(block);
}
	
Block* TestDataSrc::getNextValBlock(int colIndex_) {
  if (colIndex_ >= m_nCols) return NULL;
  
  list<Block*> *blockList = &(blockLists[colIndex_]);

  if (blockList->size()>0) {	
    Block* block=blockList->front();
    blockList->pop_front();
    return block;
  }
  else 
    return NULL;
}
PosBlock* TestDataSrc::getNextPosBlock(int colIndex_) {
  if (colIndex_ >= m_nCols) return NULL;
  
  list<PosBlock*> *posBlockList = &(posBlockLists[colIndex_]);

  if (posBlockList->size()>0) {	
    PosBlock* block=posBlockList->front();
    posBlockList->pop_front();
    return block;
  }
  else 
    return NULL;

}
PosBlock* TestDataSrc::getNextPosBlock(int colIndex_, int val_) {
	if (colIndex_!=0) return NULL;
	return NULL;
}

void TestDataSrc::setLastPosition(int lastPosition_) {
	lastPosition = lastPosition_;
}

int TestDataSrc::getLastPosition() {
	return lastPosition;
}

