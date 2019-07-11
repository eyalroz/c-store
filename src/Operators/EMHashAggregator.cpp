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
#include "EMHashAggregator.h"

EMHashAggregator::EMHashAggregator(Operator *agg_, int aggColIndex_, int gcAttNum_, int aggAttNum_) : m_agg(agg_), m_aggColIndex(aggColIndex_), m_gcAttNum(gcAttNum_), m_aggAttNum(aggAttNum_) {

  init();

}

EMHashAggregator::~EMHashAggregator() {
  if (m_hashTable != NULL) {
    delete m_hashTable;
  }
}

void EMHashAggregator::init() {
  m_isHashTableComputed = false;
  m_hashTable = NULL;
  m_hashTableSize = -1;
  m_hashFunction = NULL;

  m_areBlocksInitialized = false;
  m_currOutputPosition = 1;
  blockout = NULL;
  blockin = NULL;  // to be initialized later
}

void EMHashAggregator::initInputBlocks() {
  assert(!m_areBlocksInitialized);

  Log::writeToLog("EMHashAggregator", 2, "initializing input blocks");
  blockin = m_agg->getNextSValBlock(m_aggColIndex);
  assert(blockin);
}

void EMHashAggregator::initOutputBlocks() {
  Log::writeToLog("EMHashAggregator", 2, "initializing output blocks");

  assert(!m_areBlocksInitialized);

  blockout = new SMultiBlock(1, 12);

}

void EMHashAggregator::setHashFunction(HashFunction *function_) {
  m_hashFunction = function_;
}

void EMHashAggregator::setHashTableSize(int size_) {
  m_hashTableSize = size_;
}

SBlock* EMHashAggregator::getNextSValBlock(int colIndex_) {
  Log::writeToLog("EMHashAggregator", 1, "getNextValBlock with colIndex", colIndex_);
  assert(colIndex_ == 0);

  // check if we have initialized input and output blocks yet
  if (!m_areBlocksInitialized) {
    initInputBlocks();
    initOutputBlocks();
    m_areBlocksInitialized = true;
  }

  generateBlocks();
  return blockout;

}

Block* EMHashAggregator::getNextValBlock(int colIndex_) {
  throw new UnexpectedException("EMHashAggregator::getNextValBlock() not implemented");
}

PosBlock* EMHashAggregator::getNextPosBlock(int colIndex_) {
  throw new UnexpectedException("EMHashAggregator does not generate pos blocks");
}

PosBlock* EMHashAggregator::getNextPosBlock(int colIndex_, int val_) {
  throw new UnexpectedException("EMHashAggregator does not generate pos blocks");
}

int EMHashAggregator::getLastPosition() {
  throw new UnexpectedException("EMHashAggregator::getLastPosition() not implemented");
}

void EMHashAggregator::generateBlocks() {
  /*
  int* values = new int[3];
  // hash table size and hash function must be set prior to generating blocks
  assert(m_hashTableSize != -1);
  assert(m_hashFunction != NULL);

  // hash aggregation is only for grouped
  //assert(m_aggType != NO_GROUP);

  // hash the groups and values if not performed already
  if (!m_isHashTableComputed) {
    m_hashTable = new HashMap(m_hashTableSize);
    m_hashTable->setHashFunction(m_hashFunction);

    // example:
    // m_hashTable = new HashMap(100001);
    // m_hashTable->setHashFunction(new IdentityHashFunction(100001));

    computeHashAggregation();
    
    m_isHashTableComputed = true;
  }

  // if hash table has no more values, set output to null
  if (m_hashTable->isEmpty()) {
    //m_groupBlockOut = NULL;
    //m_aggBlockOut = NULL;
    blockout=NULL;
  } else { // load the next set of output blocks
    // get the next group

    // TODO: fix this iterator interface so that only one call is necessary
    int group = m_hashTable->getKey();
    AggValue* aggValue = (AggValue*) m_hashTable->remove(group); //pair->value; 
    long long value2 = (aggValue->evaluate());
    int value = (int)value2;
    // write the group and the agg value
    //logOutputBlock(group, value);
    //m_groupBlockOut->setValue(group, m_currOutputPosition);
    //m_aggBlockOut->setValue(value, m_currOutputPosition);
    values[0]=m_currOutputPosition;
    values[1]=group;
    values[2]=value;
    blockout->resetBlock();
    blockout->setValueAtPos((char*)values, 0);
    
    m_currOutputPosition++;
  }
  */  
}

void EMHashAggregator::computeHashAggregation() {
  Log::writeToLog("Aggregator", 2, "Performing hash aggregation...");

  /*  assert(!m_isHashTableComputed);
  SPair* group;
  //int groupVal, groupStart, groupEnd, groupBlockSize;
  bool isAggDataInHashTable = true;
  AggValue *aggData;
  int* values;
  int groupVal, aggVal;
  // on each iteration, 
  // - process a contiguous sequence of same-valued groups in the same block
  // - set to next block if current block is out of values
  while (blockin != NULL) { 
    // get the group -- remember there is only 1 Pair buffer used!    
    group = blockin->getNext();
    values = (int*)group->value;
    groupVal = values[m_gcAttNum];
    aggVal = values[m_aggAttNum];

    // if group is already in the map, retrieve the associated AggValue entry
    // otherwise, create a new one
    aggData = (AggValue*) m_hashTable->get(groupVal);
    
    if (aggData == NULL) {
      aggData = createTemplateAggValue();
      isAggDataInHashTable = false;
    }

    update(aggData, aggVal, 1);

    if (!isAggDataInHashTable) {
      m_hashTable->put(groupVal, aggData, sizeof(aggData));
      isAggDataInHashTable = true;
    }
    
    // get the next group block if current block has no more values
    if (blockin && !blockin->hasNext()) {
      blockin = m_agg->getNextSValBlock(m_aggColIndex);
    } 
  }
  Log::writeToLog("EMHashAggregator", 2, "Hash aggregation completed");
  */
}
