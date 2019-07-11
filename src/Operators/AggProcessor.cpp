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
#include "AggProcessor.h"

AggProcessor::AggProcessor(Operator *agg_, int aggColIndex_)
  : Aggregator(agg_, aggColIndex_) {}

AggProcessor::AggProcessor(Operator *agg_, int aggColIndex_, Operator* group_, int groupColIndex_)
  : Aggregator(agg_, aggColIndex_, group_, groupColIndex_) {}

// TESTED
void AggProcessor::processAggBlockGroupBySelf(AggValue* aggValue_, 
					      int groupValue_,
					    
					      int groupStart_, 
					      int groupEnd_) {
  update(aggValue_, groupValue_, groupEnd_ - groupStart_ + 1);
}

// updates aggValue_ with aggregate values from position groupStart_ to position groupEnd_
void AggProcessor::processAggBlockGroupByOtherOneValued(AggValue* aggValue_, 
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

    Pair* aggPair = m_aggBlockIn->getNext();
 //   int aggStart = aggPair->position; commented out as unused

    // are there sufficient values in the current agg block
    //DNA: changed below >= to > because if equal then we want to get the next blcok ready for assertion at beginning of this method
    if (valuesProcessed + aggLength > groupLength) { // yes -- agg is longer than group
      int VPThisTime = groupLength - valuesProcessed;
      update(aggValue_, aggPair->value, VPThisTime);
      valuesProcessed = groupLength;
      // ex: aggStart is at loc 1, getCurrLoc = 2, groupLength = 10, valuesProcessed = 6
      // goal to be at loc 5 for next call, so set to 4
      //DNA: think this is a bug ... valuesProcessed could be large if processeda previous block as well ... commenting out and replacing with valuesProcessed in current iteration:
      //m_aggBlockIn->getPairAtLoc(m_aggBlockIn->getCurrLoc() + valuesProcessed - 2);
      m_aggBlockIn->getPairAtLoc(m_aggBlockIn->getCurrLoc() + VPThisTime - 2);

    } else { // no -- agg is shorter than group block
      update(aggValue_, aggPair->value, aggLength);
      valuesProcessed += aggLength;
      m_aggBlockIn = m_agg->getNextValBlock(m_aggColIndex);
    }
  }
}

void AggProcessor::processAggBlockGroupByOtherMultiValued(AggValue* aggValue_, int groupStart_, int groupEnd_) {
  int groupLength = groupEnd_ - groupStart_ + 1;
  int valuesProcessed = 0;
  
  while (valuesProcessed < groupLength) {
    if (m_aggBlockIn->hasNext()) {
      Pair *group = m_aggBlockIn->getNext();
      update(aggValue_, group->value, 1);
      valuesProcessed++;
    } else {
      m_aggBlockIn = m_agg->getNextValBlock(m_aggColIndex);
    }
  }
}

void AggProcessor::processAggBlock(AggValue* aggValue_, int groupValue_, int groupStart_, int groupEnd_) {
  cout << "asdf" << endl;
  //Log::writeToLog("AggProcessor", 0, "groupStart_", groupStart_);
  //Log::writeToLog("AggProcessor", 0, "groupEnd_", groupEnd_);

  // AggValue* value = (AggValue*)aggValue_; // comented out as unused warnign

  if (m_aggType == GROUP_BY_SELF) {
    // agg column is same as group by column -- do not change any cursor
    //Log::writeToLog("AggProcessor", 0, "group by self");
    processAggBlockGroupBySelf(aggValue_, groupValue_, groupStart_, groupEnd_);

  } else if (m_aggType == GROUP_BY_OTHER) {
    if (m_aggBlockIn->isOneValue()) {
      //Log::writeToLog("AggProcessor", 0, "group by other, one valued");
      processAggBlockGroupByOtherOneValued(aggValue_, groupStart_, groupEnd_);
    } else {
      //Log::writeToLog("AggProcessor", 0, "group by other, multi-valued");
      processAggBlockGroupByOtherMultiValued(aggValue_, groupStart_, groupEnd_);
    }

  } else if (m_aggType == NO_GROUP) {
    throw new UnexpectedException("AggProcessor: NO_GROUP should not enter this code");
  } else {
    throw new UnexpectedException("AggProcessor: Unknown agg type");
  }
}

void AggProcessor::processAggBlocksAsOneGroup(AggValue* aggValue_) {
  //Log::writeToLog("AggProcessor", 3, "processAggBlocksAsOneGroup");
  
  do {

    if (m_aggBlockIn->isOneValue()) {
      int value = m_aggBlockIn->getNext()->value;
      int nOccurrences = m_aggBlockIn->getSize();
      update(aggValue_, value, nOccurrences);
    } else {
      while (m_aggBlockIn->hasNext()) {
	int value = m_aggBlockIn->getNext()->value;
	update(aggValue_, value, 1);
      }
    }
    m_aggBlockIn = m_agg->getNextValBlock(m_aggColIndex);
  } while (m_aggBlockIn != NULL);
}
