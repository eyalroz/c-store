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
#include "Aggregator.h"

Aggregator::Aggregator(Operator *agg_, int aggColIndex_)  
  : m_agg(agg_), m_aggColIndex(aggColIndex_) {
  // no grouping
  m_aggType = NO_GROUP;
  init();
}

Aggregator::Aggregator(Operator *agg_, int aggColIndex_, Operator *group_, int groupColIndex_)
  : m_agg(agg_), m_aggColIndex(aggColIndex_), m_group(group_), m_groupColIndex(groupColIndex_) {
  
  // two cases: grouping along the same column as agg_ or grouping along a different column
  if (agg_ == group_ && aggColIndex_ == groupColIndex_) {
    m_aggType = GROUP_BY_SELF;
  } else {
    m_aggType = GROUP_BY_OTHER;
  }
  init();
}

void Aggregator::init() {
  // set both blocks to consumed; both blocks need to be generated
  m_isAggBlockConsumed = m_isGroupBlockConsumed = true;

  m_areBlocksInitialized = false;

  m_currOutputPosition = 1;

  m_aggBlockOut = NULL;
  m_groupBlockOut = NULL;
  m_aggBlockIn = m_groupBlockIn = NULL;  // to be initialized later
  bi=NULL;
}

void Aggregator::initInputBlocks() {
  assert(!m_areBlocksInitialized);

  Log::writeToLog("Aggregator", 2, "initializing input blocks");
  if (m_aggType == NO_GROUP) {
    // only one input column
    m_aggBlockIn = m_agg->getNextValBlock(m_aggColIndex);
  } else {
    // initialize input blocks
    m_groupBlockIn = m_group->getNextValBlock(m_groupColIndex);

    if (m_aggType == GROUP_BY_SELF) {
      // agg column is same as group by column
      m_aggBlockIn = m_groupBlockIn;
    } else {
      m_aggBlockIn = m_agg->getNextValBlock(m_aggColIndex);
    }

    // m_groupBlockIn and m_aggBlockIn should either both be null or both non-null
    assert(!(m_groupBlockIn == NULL && m_aggBlockIn != NULL));
    assert(!(m_aggBlockIn == NULL && m_groupBlockIn != NULL));
  }
}

void Aggregator::initOutputBlocks() {
  Log::writeToLog("Aggregator", 2, "initializing output blocks");

  assert(!m_areBlocksInitialized);

  // initialize the output blocks
  // properties of output blocks should depend on input block properties
  // for now, they're just basic blocks
  if (m_aggBlockOut == NULL) {
    //m_aggBlockOut = new LongBasicBlock(true, true, true);
    m_aggBlockOut = new BasicBlock(true, true, true, ValPos::DOUBLETYPE);
  }
  if (m_groupBlockOut == NULL) {
    if ((m_groupBlockIn) &&  (m_groupBlockIn->getStartPair()))
      m_groupBlockOut = new BasicBlock(true, true, true, m_groupBlockIn->getStartPair()->clone());
    else
      m_groupBlockOut = NULL;
  }
}

Aggregator::~Aggregator() {
  if (m_aggBlockOut != NULL) {
    delete m_aggBlockOut;
  }
  if (m_groupBlockOut != NULL) {
    delete m_groupBlockOut;
  }
  delete bi;
}

Block* Aggregator::getNextValBlock(int colIndex_) {
  Log::writeToLog("Aggregator", 1, "getNextValBlock with colIndex", colIndex_);

  // note: if NO_GROUP, then the colIndex_ will be 0, which is GROUP_COL_INDEX in the code
  assert(!(m_aggType == NO_GROUP && colIndex_ != 0));

  // check if we have initialized input and output blocks yet
  if (!m_areBlocksInitialized) {
    initInputBlocks();
    // if no input agg blocks, the aggregator has no output
    if (m_aggBlockIn == NULL) {
      return NULL;
    }
    initOutputBlocks();

    m_areBlocksInitialized = true;
  }

  // check if we need to generate new output blocks
  if (m_isAggBlockConsumed && m_isGroupBlockConsumed) {
    m_isAggBlockConsumed = m_isGroupBlockConsumed = false;
    
    generateBlocks();
  }

  if (m_aggType == NO_GROUP) {
    m_isAggBlockConsumed = m_isGroupBlockConsumed = true;
    return m_aggBlockOut;
  } else if (colIndex_ == AGG_COL_INDEX && !m_isAggBlockConsumed) {

    m_isAggBlockConsumed = true;
    Log::writeToLog("Aggregator", 1, "Returning Aggregate Block");
    return m_aggBlockOut;

  } else if (colIndex_ == GROUP_COL_INDEX && !m_isGroupBlockConsumed) {

    m_isGroupBlockConsumed = true;
    Log::writeToLog("Aggregator", 1, "Returning Group By Block");
    return m_groupBlockOut;

  } else {
    // operator above is reading a block without having read other block
    if (m_isAggBlockConsumed) {

      throw new UnexpectedException("Aggregator: already read AggBlock but haven't read GroupBlock");

    } else {

      throw new UnexpectedException("Aggregator: already read GroupBlock but haven't read AggBlock");

    }
  }
}

PosBlock* Aggregator::getNextPosBlock(int colIndex_) {
  throw new UnexpectedException("Aggregator does not generate pos blocks");
}

PosBlock* Aggregator::getNextPosBlock(int colIndex_, int val_) {
  throw new UnexpectedException("Aggregator does not generate pos blocks");
}

int Aggregator::getLastPosition() {
  throw new UnexpectedException("Aggregator::getLastPosition() not implemented");
}

void Aggregator::logOutputBlock(long long value_) {
  // cout << "Value: " << value_ << endl;
  Log::writeToLog("Aggregator", 1, "Output Position", m_currOutputPosition);
  Log::writeToLog("Aggregator", 1, "  value", value_);
}

void Aggregator::logOutputBlock(int group_, long long value_) {
  // cout << group_ << "," << value_ << endl;
  Log::writeToLog("Aggregator", 1, "Output Position", m_currOutputPosition);
  Log::writeToLog("Aggregator", 1, "  group", group_);  
  Log::writeToLog("Aggregator", 1, "  value", value_);
}


// ASSUMPTIONS:
//  positions always contiguous
//  positions across both block streams line up in rows
//  cardinality of input columns equal

void Aggregator::processAggBlockGroupBySelf(AggValue* aggValue_, 
					    ValPos* groupValue_,
					    int groupStart_, 
					    int groupEnd_) {
  update(aggValue_, groupValue_, groupEnd_ - groupStart_ + 1);
}

// updates aggValue_ with aggregate values from position groupStart_ to position groupEnd_
void Aggregator::processAggBlockGroupByOtherOneValued(AggValue* aggValue_, 
							int groupStart_, 
							int groupEnd_) {
  int groupLength = groupEnd_ - groupStart_ + 1;
  int valuesProcessed = 0;

  // this method should exit with unused values in the current block
  assert(m_aggBlockIn->hasNext());

  while (valuesProcessed < groupLength) {
    // ex: size = 10, currLoc = 1, then aggLength is 9 b/c 9 more values to process
    int aggLength = m_aggBlockIn->getSize() - m_aggBlockIn->getCurrLoc();
    assert(aggLength >= 0);

    ValPos* aggPair = m_aggBlockIn->getNext();
    //assume int type for now ... fix later
    //assert(aggPair->type == ValPos::INTTYPE);
 //   int aggStart = aggPair->position; commented out as unused

    // are there sufficient values in the current agg block
    //DNA: changed below >= to > because if equal then we want to get the next blcok ready for assertion at beginning of this method
    if (valuesProcessed + aggLength > groupLength) { // yes -- agg is longer than group
      int VPThisTime = groupLength - valuesProcessed;
      update(aggValue_, aggPair, VPThisTime);
      valuesProcessed = groupLength;
      // ex: aggStart is at loc 1, getCurrLoc = 2, groupLength = 10, valuesProcessed = 6
      // goal to be at loc 5 for next call, so set to 4
      //DNA: think this is a bug ... valuesProcessed could be large if processeda previous block as well ... commenting out and replacing with valuesProcessed in current iteration:
      //m_aggBlockIn->getPairAtLoc(m_aggBlockIn->getCurrLoc() + valuesProcessed - 2);
      m_aggBlockIn->getPairAtLoc(m_aggBlockIn->getCurrLoc() + VPThisTime - 2);

    } else { // no -- agg is shorter than group block
      update(aggValue_, aggPair, aggLength);
      valuesProcessed += aggLength;
      m_aggBlockIn = m_agg->getNextValBlock(m_aggColIndex);
    }
  }
}

void Aggregator::processAggBlockGroupByOtherMultiValued(AggValue* aggValue_, int groupStart_, int groupEnd_) {
  int groupLength = groupEnd_ - groupStart_ + 1;
  int valuesProcessed = 0;
  //Pair* group;
  if (m_aggBlockIn->getStartPair()->type == ValPos::INTTYPE) {
    //ValPos* vp = m_aggBlockIn->getStartPair();
    while (valuesProcessed < groupLength) {
      if (!bi) {
	if (m_aggBlockIn->getBlock() == m_aggBlockIn) { // m_aggBlockIn is a normal block
	  bi = m_aggBlockIn->getIterator();
	} else {				  // m_aggBlockIn is an iterator
	  bi = static_cast<BlockIter*>(m_aggBlockIn);
	}
	iterarray = (int*) bi->asArray();
	itersize = bi->getSize();
	iterindex=0;
      }
      if ((itersize-iterindex)+valuesProcessed < groupLength) {
	valuesProcessed+=(itersize-iterindex);
	for (int i= iterindex; i< itersize; i++) { 
	  //group = m_aggBlockIn->getNext();
	  //update(aggValue_, group->value, 1);
	  //assert(group->value == iterarray[iterindex]);
	  //vp->set((byte*)(iterarray+iterindex));
	  update(aggValue_, iterarray[iterindex], 1);
	  iterindex++;
	}
	//assert(!m_aggBlockIn->hasNext());
	m_aggBlockIn = m_agg->getNextValBlock(m_aggColIndex);
	if (m_aggBlockIn) {
	  if (m_aggBlockIn->getBlock() == m_aggBlockIn) { // m_aggBlockIn is a normal block
	    bi = m_aggBlockIn->getIterator();
	  } else {				  // m_aggBlockIn is an iterator
	    bi = static_cast<BlockIter*>(m_aggBlockIn);
	  }
	  iterarray = (int*) bi->asArray();
	  itersize = bi->getSize();
	  iterindex=0;
	}
      }
      else {
	for (int i= 0; i< (groupLength-valuesProcessed); i++) {
	  //group = m_aggBlockIn->getNext();
	  //update(aggValue_, group->value, 1);
	  //assert(group->value == iterarray[iterindex]);
	  //vp->set((byte*)(iterarray+iterindex));
	  update(aggValue_, iterarray[iterindex], 1);
	  iterindex++;
	}
	valuesProcessed+=(groupLength-valuesProcessed);
      }
    }
    assert (valuesProcessed = groupLength);
    /*while (valuesProcessed < groupLength) {
      if (m_aggBlockIn->hasNext()) {
      group = m_aggBlockIn->getNext();
      update(aggValue_, group->value, 1);
      valuesProcessed++;
      } else {
      m_aggBlockIn = m_agg->getNextValBlock(m_aggColIndex);
      }
      }*/
  }
  else {
    byte* realArray = (byte*)iterarray;
    ValPos* vp = m_aggBlockIn->getStartPair()->clone();
    int valsize = vp->getSize();
    while (valuesProcessed < groupLength) {
      if (!bi) {
	if (m_aggBlockIn->getBlock() == m_aggBlockIn) { // m_aggBlockIn is a normal block
	  bi = m_aggBlockIn->getIterator();
	} else {				  // m_aggBlockIn is an iterator
	  bi = static_cast<BlockIter*>(m_aggBlockIn);
	}
	iterarray = (int*) bi->asArray();
	realArray = (byte*)iterarray;
	itersize = bi->getSize();
	iterindex=0;
      }
      if ((itersize-iterindex)+valuesProcessed < groupLength) {
	valuesProcessed+=(itersize-iterindex);
	for (int i= iterindex; i< itersize; i++) { 
	  //group = m_aggBlockIn->getNext();
	  //update(aggValue_, group->value, 1);
	  //assert(group->value == iterarray[iterindex]);
	  vp->set(realArray+(valsize*iterindex));
	  update(aggValue_, vp, 1);
	  iterindex++;
	}
	//assert(!m_aggBlockIn->hasNext());
	m_aggBlockIn = m_agg->getNextValBlock(m_aggColIndex);
	if (m_aggBlockIn) {
	  if (m_aggBlockIn->getBlock() == m_aggBlockIn) { // m_aggBlockIn is a normal block
	    bi = m_aggBlockIn->getIterator();
	  } else {				  // m_aggBlockIn is an iterator
	    bi = static_cast<BlockIter*>(m_aggBlockIn);
	  }
	  iterarray = (int*) bi->asArray();
	  realArray = (byte*)iterarray;
	  itersize = bi->getSize();
	  iterindex=0;
	}
      }
      else {
	for (int i= 0; i< (groupLength-valuesProcessed); i++) {
	  //group = m_aggBlockIn->getNext();
	  //update(aggValue_, group->value, 1);
	  //assert(group->value == iterarray[iterindex]);
	  vp->set(realArray+(valsize*iterindex));
	  update(aggValue_, vp, 1);
	  iterindex++;
	}
	valuesProcessed+=(groupLength-valuesProcessed);
      }
    }
    assert (valuesProcessed = groupLength);
    /*while (valuesProcessed < groupLength) {
      if (m_aggBlockIn->hasNext()) {
      group = m_aggBlockIn->getNext();
      update(aggValue_, group->value, 1);
      valuesProcessed++;
      } else {
      m_aggBlockIn = m_agg->getNextValBlock(m_aggColIndex);
      }
      }*/
    delete vp;
  }
}

void Aggregator::processAggBlock(AggValue* aggValue_, ValPos* groupValue_, int groupStart_, int groupEnd_) {
  //Log::writeToLog("Aggregator", 0, "groupStart_", groupStart_);
  //Log::writeToLog("Aggregator", 0, "groupEnd_", groupEnd_);

  // AggValue* value = (AggValue*)aggValue_; // comented out as unused warnign

  if (m_aggType == GROUP_BY_SELF) {
    // agg column is same as group by column -- do not change any cursor
    //Log::writeToLog("Aggregator", 0, "group by self");
    processAggBlockGroupBySelf(aggValue_, groupValue_, groupStart_, groupEnd_);

  } else if (m_aggType == GROUP_BY_OTHER) {
    if (m_aggBlockIn->isOneValue()) {
      //Log::writeToLog("Aggregator", 0, "group by other, one valued");
      processAggBlockGroupByOtherOneValued(aggValue_, groupStart_, groupEnd_);
    } else {
      // MIGHT NOT BE TESTED
      //Log::writeToLog("Aggregator", 0, "group by other, multi-valued");
      processAggBlockGroupByOtherMultiValued(aggValue_, groupStart_, groupEnd_);
    }

  } else if (m_aggType == NO_GROUP) {
    throw new UnexpectedException("Aggregator: NO_GROUP should not enter this code");
  } else {
    throw new UnexpectedException("Aggregator: Unknown agg type");
  }
}

void Aggregator::processAggBlocksAsOneGroup(AggValue* aggValue_) {
  //Log::writeToLog("Aggregator", 3, "processAggBlocksAsOneGroup");
  ValPos* aggPair;
  
  do {

    if (m_aggBlockIn->isOneValue()) {
      aggPair = m_aggBlockIn->getNext();
      //assume int type for now ... fix later
      //assert(aggPair->type == ValPos::INTTYPE);
      //int value = *(int*)aggPair->value;
      int nOccurrences = m_aggBlockIn->getSize();
      update(aggValue_, aggPair, nOccurrences);
    } else {
      while (m_aggBlockIn->hasNext()) {
	aggPair = m_aggBlockIn->getNext();
	//assume int type for now ... fix later
	//assert(aggPair->type == ValPos::INTTYPE);
	//int value = *(int*)aggPair->value;
	//int value = m_aggBlockIn->getNext()->value;
	update(aggValue_, aggPair, 1);
      }
    }
    m_aggBlockIn = m_agg->getNextValBlock(m_aggColIndex);
  } while (m_aggBlockIn != NULL);
}
