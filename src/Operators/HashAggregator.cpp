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
#include "HashAggregator.h"

HashAggregator::HashAggregator(Operator *agg_, int aggColIndex_, Operator *group_, int groupColIndex_) :
    Aggregator(agg_, aggColIndex_, group_, groupColIndex_) {

  init();

}

HashAggregator::~HashAggregator() {
  if (m_hashTable != NULL) {
    delete m_hashTable;
  }
}

void HashAggregator::init() {
  m_isHashTableComputed = false;
  m_hashTable = NULL;
  m_hashTableSize = -1;
  m_hashFunction = NULL;
}

void HashAggregator::setHashFunction(HashFunction *function_) {
  m_hashFunction = function_;
}

void HashAggregator::setHashTableSize(int size_) {
  m_hashTableSize = size_;
}

void HashAggregator::generateBlocks() {
  // hash table size and hash function must be set prior to generating blocks
  assert(m_hashTableSize != -1);
  assert(m_hashFunction != NULL);

  // hash aggregation is only for grouped
  assert(m_aggType != NO_GROUP);

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
    m_groupBlockOut = NULL;
    m_aggBlockOut = NULL;
  } else { // load the next set of output blocks
    // get the next group

    // TODO: fix this iterator interface so that only one call is necessary
    ValPos* group = m_hashTable->getKey()->clone();
    AggValue* aggValue;
    if (group->type == ValPos::INTTYPE)
      aggValue = (AggValue*) m_hashTable->remove(*(int*)group->value); //pair->value; 
    else
      aggValue = (AggValue*) m_hashTable->remove(group); //pair->value; 

    double value = aggValue->evaluate();
    // write the group and the agg value
    //logOutputBlock(group, value);
    group->position=m_currOutputPosition;
    m_groupBlockOut->setValue(group);
    delete group;
    DoubleValPos vp;
    vp.set(m_currOutputPosition, (byte*)&value);
    m_aggBlockOut->setValue(&vp);
    m_currOutputPosition++;
  }
  
}

void HashAggregator::computeHashAggregation() {
  Log::writeToLog("Aggregator", 2, "Performing hash aggregation...");

  assert(!m_isHashTableComputed);
  ValPos* group;
  int groupStart, groupEnd, groupBlockSize;
  bool isAggDataInHashTable = true;
  AggValue *aggData;
  bool finishedWithBlock=false;

  int* iterarray=NULL;
  int itersize=0;
  int iterindex=0;
  BlockIter* blockIter;
  if (!m_groupBlockIn->isOneValue()) {
    if (m_groupBlockIn->getBlock() == m_groupBlockIn) { // m_groupBlockIn is a normal block
      blockIter = m_groupBlockIn->getIterator();
    } else {				  // m_aggBlockIn is an iterator
      blockIter = static_cast<BlockIter*>(m_groupBlockIn);
    }
    iterarray = (int*) blockIter->asArray();
    itersize = blockIter->getSize();
    iterindex=0;
  }

  // on each iteration, 
  // - process a contiguous sequence of same-valued groups in the same block
  // - set to next block if current block is out of values
  while (m_groupBlockIn != NULL) { 

    // get the next group block if current block has no more values
    while (!m_groupBlockIn->hasNext() || finishedWithBlock) {
      finishedWithBlock=false;
      m_groupBlockIn = m_group->getNextValBlock(m_groupColIndex);
      if (!m_groupBlockIn)
	break;
      if (!m_groupBlockIn->isOneValue()) {
	if (m_groupBlockIn->getBlock() == m_groupBlockIn) { // m_groupBlockIn is a normal block
	  blockIter = m_groupBlockIn->getIterator();
	} else {				  // m_aggBlockIn is an iterator
	  blockIter = static_cast<BlockIter*>(m_groupBlockIn);
	}
	iterarray = (int*) blockIter->asArray();
	itersize = blockIter->getSize();
	iterindex=0;
      }
    } 
    if (!m_groupBlockIn)
      break;

    // get the group -- remember there is only 1 Pair buffer used!    
    group = m_groupBlockIn->getNext();
    //assume int type for now ... fix later
    //assert(group->type == ValPos::INTTYPE);
    iterindex++;
    //groupVal = *(int*)group->value;

    // if group is already in the map, retrieve the associated AggValue entry
    // otherwise, create a new one
    if(group->type == ValPos::INTTYPE)
      aggData = (AggValue*) m_hashTable->get(*(int*)group->value);
    else
      aggData = (AggValue*) m_hashTable->get(group);
    
    if (aggData == NULL) {
      aggData = createTemplateAggValue();
      isAggDataInHashTable = false;
    }

    groupStart = group->position;

    
    if (m_groupBlockIn->isOneValue()) {

      // calculate the end of current group in this block
      groupBlockSize = m_groupBlockIn->getSize();
      if (groupBlockSize > 1) {
	//if (m_groupBlockIn->hasNext(groupVal)) {
	  //if (m_groupBlockIn->isOneValue()) {
	    //Note, this could be Type2 as well, but getPairAtLoc will produce a
	    //bitcount rather than a position in the pair so this works ok ...
	    //the advantage of this is that this requires one less check
	    //in this critical performance path
	    //group = NULL;//m_groupBlockIn->getPairAtLoc(groupBlockSize - 1);
	    groupEnd = m_groupBlockIn->getSize() + groupStart-1;//m_groupBlockIn->getEndPosition();//group->position;
	    finishedWithBlock=true;
	    //m_groupBlockIn = m_group->getNextValBlock(m_groupColIndex);
	    //} 
	    //else {
	    //while (m_groupBlockIn->hasNext(groupVal)) {
	    // group = m_groupBlockIn->getNext();
	    //}
	    //groupEnd = group->position;
	    //}
	    //}
	    //else {
	    //groupEnd = groupStart;
	    //}
      }
      else
	groupEnd = groupStart;
     
      //assert(m_aggBlockIn != NULL);
      // process the aggregate column data at the group's location
      processAggBlock(aggData, group, groupStart, groupEnd);
      
      // processAggBlock modifies the data pointed to by aggData,
      //  so we need to put <group, aggData> into hash table only
      //  if aggData is not already in the table
      
      if (!isAggDataInHashTable) {
	if(group->type == ValPos::INTTYPE)
	  m_hashTable->put(*(int*)group->value, aggData, sizeof(aggData));
	else
	  m_hashTable->put(group, aggData, sizeof(aggData));
	isAggDataInHashTable = true;
      }
      
      
    }
    else {
      if (group->type == ValPos::INTTYPE) {
	int gv = *(int*)group->value;
	int blockStart=groupStart;
	while ((iterindex < itersize) && (gv == iterarray[iterindex]))
	  iterindex++;
	processAggBlock(aggData, group, groupStart, iterindex+blockStart-1);
	if (!isAggDataInHashTable) {
	  m_hashTable->put(gv, aggData, sizeof(aggData));
	  isAggDataInHashTable = true;
	}
	while(iterindex < itersize) {
	  groupStart=iterindex+blockStart;
	  gv = iterarray[iterindex];
	  group->set((byte*)&gv);
	  iterindex++;
	  aggData = (AggValue*) m_hashTable->get(gv);
	  if (aggData == NULL) {
	    aggData = createTemplateAggValue();
	    while ((iterindex < itersize) && (gv == iterarray[iterindex]))
	      iterindex++;
	    processAggBlock(aggData, group, groupStart, iterindex+blockStart-1);
	    m_hashTable->put(gv, aggData, sizeof(aggData));
	  }
	  else {
	    while ((iterindex < itersize) && (gv == iterarray[iterindex]))
	      iterindex++;
	    processAggBlock(aggData, group, groupStart, iterindex+blockStart-1);
	  }
	}
	assert (iterindex==itersize);
	isAggDataInHashTable = true;
	finishedWithBlock=true;
      }
      else {
	//int gv = *(int*)group->value;
	byte* realarray = (byte*)iterarray;
	int valsize = group->getSize();
	int blockStart=groupStart;
	while ((iterindex < itersize) && (memcmp(group->value,realarray + (iterindex*valsize), valsize) == 0))
	  iterindex++;
	processAggBlock(aggData, group, groupStart, iterindex+blockStart-1);
	if (!isAggDataInHashTable) {
	  m_hashTable->put(group, aggData, sizeof(aggData));
	  isAggDataInHashTable = true;
	}
	while(iterindex < itersize) {
	  groupStart=iterindex+blockStart;
	  group->set(realarray+(iterindex*valsize));
	  //group->set((byte*)&gv);
	  iterindex++;
	  aggData = (AggValue*) m_hashTable->get(group);
	  
	  if (aggData == NULL) {
	    aggData = createTemplateAggValue();
	    while ((iterindex < itersize) && (memcmp(group->value,realarray + (iterindex*valsize), valsize) == 0))
	      iterindex++;
	    processAggBlock(aggData, group, groupStart, iterindex+blockStart-1);
	    m_hashTable->put(group, aggData, sizeof(aggData));
	  }
	  else {
	    while ((iterindex < itersize) && (memcmp(group->value,realarray + (iterindex*valsize), valsize) == 0))
	      iterindex++;
	    processAggBlock(aggData, group, groupStart, iterindex+blockStart-1);
	  }
	}
	assert (iterindex==itersize);
	isAggDataInHashTable = true;
	finishedWithBlock=true;
      }
    }
  }
  Log::writeToLog("Aggregator", 2, "Hash aggregation completed");
}
