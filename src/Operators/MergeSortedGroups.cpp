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
#include "MergeSortedGroups.h"

MergeSortedGroups::MergeSortedGroups(Operator *rs_, Operator *ws_, int nCols_, int mergeColIndex_) : Merge(rs_, ws_, nCols_), m_mergeColIndex(mergeColIndex_), m_mergeOp(NULL) {
  // simplify code with an assumption for now
  //assert(mergeColIndex_ == 0);
}

MergeSortedGroups::~MergeSortedGroups(){}

void MergeSortedGroups::setMergeOp(MergeOp *mergeOp_) {
  m_mergeOp = mergeOp_;
}

void MergeSortedGroups::initOutputBlocks() {
  Log::writeToLog("Merge", 2, "initializing output blocks");

  // TODO: update to real output block types based on input block types
  for (int i = 0; i < m_nCols; i++) {
    m_outputBlocks[i] = (Block*) new BasicBlock(true, true, true, ValPos::DOUBLETYPE);
  }
  lvp = new DoubleValPos();
}


void MergeSortedGroups::init() {
  Log::writeToLog("MergeSortedGroups", 2, "init");

  Block *rsBlock = m_rsInputBlocks[m_mergeColIndex];
  Block *wsBlock = m_wsInputBlocks[m_mergeColIndex];

  assignRSGroup(rsBlock);
  assignWSGroup(wsBlock);

  Log::writeToLog("MergeSortedGroups", 2, "m_rsGroup", m_rsGroup);
  Log::writeToLog("MergeSortedGroups", 2, "m_wsGroup", m_wsGroup);
}

void MergeSortedGroups::generateBlocks() {
  Log::writeToLog("MergeSortedGroups", 2, "generateBlocks");

  if (!hasMoreRSBlocks() && !hasMoreWSBlocks()) {
    // no more input blocks, clean up output blocks
    for (int i = 0; i < m_nCols; i++) {
      delete m_outputBlocks[i];
      m_outputBlocks[i] = NULL;
    }
    return;
  }
  Block *rsBlock;
  Block *wsBlock;
  if (m_mergeColIndex!=-1) {
    rsBlock = m_rsInputBlocks[m_mergeColIndex];
    wsBlock = m_wsInputBlocks[m_mergeColIndex];
  }
  else {
    rsBlock = NULL;
    wsBlock = NULL;
  }

  assert(m_mergeOp != NULL);

  // MergeSortedGroups operator only works on sorted columns
  // groups should only have one value
  if (rsBlock != NULL) {
    assert(rsBlock->isValueSorted());
    assert(rsBlock->isOneValue());
  }

  if (wsBlock != NULL) {
    assert(wsBlock->isValueSorted());
    assert(wsBlock->isOneValue());
  }

  // INVARIANT: values currently stored in m_wsGroup and m_rsGroup have
  // not been used
  
  // find the next group
  //assert(m_currGroup != m_wsGroup && m_currGroup != m_rsGroup);

  if (hasMoreWSBlocks() && 
      (!hasMoreRSBlocks() || m_wsGroup < m_rsGroup)) {
    // WS group is smaller
    // pass along all the ws blocks to the output blocks
    for (int i = 0; i < m_nCols; i++) {
      if (i == m_mergeColIndex) { // assign the merge column value
	double temp = m_wsGroup;
	lvp->set(m_currOutputPosition, (byte*)&temp);
	((BasicBlock*)m_outputBlocks[i])->setValue(lvp);
      } else { // pass on the value
	assert(m_wsInputBlocks[i]->hasNext());
	ValPos* vp = m_wsInputBlocks[i]->getNext();
	assert(vp->type == ValPos::INTTYPE || vp->type == ValPos::DOUBLETYPE);
	double value;
	if (vp->type == ValPos::INTTYPE)
	  value = *(int*)vp->value; 
	else
	  value = *(double*)vp->value; 
	lvp->set(m_currOutputPosition, (byte*)&value);
	((BasicBlock*)m_outputBlocks[i])->setValue(lvp);
      }
    }

    // load up the next set of WS blocks
    loadWSInputBlocks();
    
    // assign the next ws group
    wsBlock = m_wsInputBlocks[m_mergeColIndex];
    assignWSGroup(wsBlock);

  } else if (hasMoreRSBlocks() && 
	     (!hasMoreWSBlocks() || m_wsGroup > m_rsGroup)) {
    // RS group is smaller
    // pass along all the rs blocks to the output blocks
    for (int i = 0; i < m_nCols; i++) {
      if (i == m_mergeColIndex) { // assign the merge column value
	double temp = m_rsGroup;
	lvp->set(m_currOutputPosition, (byte*)&temp);
	((BasicBlock*)m_outputBlocks[i])->setValue(lvp);
      } else {
	assert(m_rsInputBlocks[i]->hasNext());
	ValPos* vp = m_rsInputBlocks[i]->getNext();
	assert(vp->type == ValPos::INTTYPE || vp->type == ValPos::DOUBLETYPE);
	double value;
	if (vp->type == ValPos::INTTYPE)
	  value = *(int*)vp->value; 
	else
	  value = *(double*)vp->value; 
	lvp->set(m_currOutputPosition, (byte*)&value);
	((BasicBlock*)m_outputBlocks[i])->setValue(lvp);
      }
    }

    // load up the next set of RS blocks
    loadRSInputBlocks();

    // assign the next RS group
    rsBlock = m_rsInputBlocks[m_mergeColIndex];
    assignRSGroup(rsBlock);

  } else { // more RSBlocks and WSBlocks and groups equal
    // perform the actual merge
    for (int i = 0; i < m_nCols; i++) {
      if (i == m_mergeColIndex) {
	double temp = m_rsGroup;
	lvp->set(m_currOutputPosition, (byte*)&temp);
	((BasicBlock*)m_outputBlocks[i])->setValue(lvp);
      } else {
	assert(m_rsInputBlocks[i]->hasNext());
	assert(m_wsInputBlocks[i]->hasNext());
	ValPos* vp1 = m_rsInputBlocks[i]->getNext();
	ValPos* vp2 = m_wsInputBlocks[i]->getNext();
	assert(vp1->type == ValPos::INTTYPE || vp1->type == ValPos::DOUBLETYPE);
	assert(vp2->type == ValPos::INTTYPE || vp2->type == ValPos::DOUBLETYPE);
	double val1, val2;
	if (vp1->type == ValPos::INTTYPE)
	  val1 = *(int*)vp1->value; 
	else
	  val1 = *(double*)vp1->value; 
	if (vp2->type == ValPos::INTTYPE)
	  val2 = *(int*)vp2->value; 
	else
	  val2 = *(double*)vp2->value; 
	double value = m_mergeOp->merge(val1,val2);
	lvp->set(m_currOutputPosition, (byte*)&value);
	((BasicBlock*)m_outputBlocks[i])->setValue(lvp);
      }
    }
    
    // load in new sets of WS and RS blocks
    if (hasMoreWSBlocks()) {
      loadWSInputBlocks();
      if (m_mergeColIndex!=-1) 
	assignWSGroup(m_wsInputBlocks[m_mergeColIndex]);
      else if (m_wsInputBlocks[0] == NULL)
	  m_wsGroup = -1;
   
    }
    if (hasMoreRSBlocks()) {
      loadRSInputBlocks();
      if (m_mergeColIndex!=-1) 
	assignRSGroup(m_rsInputBlocks[m_mergeColIndex]);
      else if (m_rsInputBlocks[0] == NULL)
	  m_rsGroup = -1;
    }
  }

  // update the position
  m_currOutputPosition++;


  // looping assumes that mergeColIndex is 0
  // assign other output blocks
  
}

bool MergeSortedGroups::hasMoreWSBlocks() {
  return m_wsGroup != -1;
}

bool MergeSortedGroups::hasMoreRSBlocks() {
  return m_rsGroup != -1;
}

void MergeSortedGroups::assignRSGroup(Block *rsBlock) {
  if (m_mergeColIndex == -1)
    m_rsGroup = 1;
  else if (rsBlock == NULL || !rsBlock->hasNext()) {
    m_rsGroup = -1;
  } else {
    ValPos* vp = rsBlock->getNext();
    assert(vp->type == ValPos::INTTYPE || vp->type == ValPos::DOUBLETYPE);
    m_rsGroup = *(int*)vp->value;
  }
}

void MergeSortedGroups::assignWSGroup(Block *wsBlock) {
  if (m_mergeColIndex == -1)
    m_wsGroup = 1;
  else if (wsBlock == NULL || !wsBlock->hasNext()) {
    m_wsGroup = -1;
  } else {
    ValPos* vp = wsBlock->getNext();
    assert(vp->type == ValPos::INTTYPE || vp->type == ValPos::DOUBLETYPE);
    m_wsGroup = *(int*)vp->value;
  }
}
