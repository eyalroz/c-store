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
#include "PipelinedAggregator.h"

PipelinedAggregator::PipelinedAggregator(Operator *agg_, int aggColIndex_) :
  Aggregator(agg_, aggColIndex_) { 
  
	m_aggData = NULL;
}

PipelinedAggregator::PipelinedAggregator(Operator *agg_, int aggColIndex_, Operator *group_, int groupColIndex_) :
    Aggregator(agg_, aggColIndex_, group_, groupColIndex_) {

  init();

}

PipelinedAggregator:: ~PipelinedAggregator() {
/*
  if (m_aggData != NULL) {
    delete m_aggData;
  }
  */
}

void PipelinedAggregator::init() {
  m_currGroup = NULL;
  m_aggData = NULL;
}

void PipelinedAggregator::generateBlocks() {

  // if an input source has dried out, return NULL
  if (m_aggBlockIn == NULL) {
    // group block should also be NULL; otherwise we're not grouping
    assert(m_aggType == NO_GROUP || m_groupBlockIn == NULL);
    m_aggBlockOut = NULL;
    m_groupBlockOut = NULL;
    return;
  }

  if (m_groupBlockIn == NULL && m_aggType != NO_GROUP) {
    // agg block should also be NULL
    //assert((m_aggBlockIn == NULL) || (!m_aggBlockIn->hasNext()));
    m_aggBlockOut = NULL;
    m_groupBlockOut = NULL;    
    return;
  }

  if (m_aggData == NULL) {
    m_aggData = createTemplateAggValue();
  }

  // input sources still have blocks

  if (m_aggType == NO_GROUP) {
    // TESTED
    Log::writeToLog("Aggregator", 0, "Case: NO_GROUP");
    aggregateNoGroup();

  }else { // if (m_aggType == NO_GROUP) {
    
    Log::writeToLog("Aggregator", 0, "Case: GROUP");

    if (m_groupBlockIn->isOneValue()) {
      // TESTED by Query 1 & 2
      Log::writeToLog("Aggregator", 0, "Case: value sorted, 1 value");
      aggregateOneValuedBlocks();
      
    } else { // if (m_groupBlockIn->isOneValue()) {      
      // NOT TESTED
      Log::writeToLog("Aggregator", 0, "Case: value sorted, 1+ value");
      aggregateMultiValuedBlocks();
    }
  }
}

void PipelinedAggregator::aggregateNoGroup() {
  // get the template
  AggValue *aggData = createTemplateAggValue();
  
  // tabulate the results for the entire column
  processAggBlocksAsOneGroup(aggData);
  
  // write out results
  double aggValue = aggData->evaluate();
  //logOutputBlock(aggValue);
  DoubleValPos lvp;
  lvp.set(m_currOutputPosition, (byte*)&aggValue);
  m_aggBlockOut->setValue(&lvp);
  m_currOutputPosition++;

  delete aggData;
}

void PipelinedAggregator::aggregateOneValuedBlocks() {
  // this condition should have been caught in generateBlocks
  assert(m_groupBlockIn != NULL);

  m_aggData->reset();

  // extent of group is entire block b/c of one value property
  int groupStart, groupEnd;
  ValPos *vp = m_groupBlockIn->getNext();
  do {
    //assume int type for now ... fix later
    //assert(vp->type == ValPos::INTTYPE);
    //groupVal = *(int*)vp->value;
    groupStart = vp->position;
    groupEnd = m_groupBlockIn->getPairAtLoc(m_groupBlockIn->getSize()-1)->position;

    // debugging
    //cout << " Mcurr " << m_currGroup << " val " << pair->value << " Position " << pair->position << " (not that end, eln is " << groupEnd << endl;
    if (m_currGroup && *m_currGroup > vp) {
      throw new UnexpectedException("input to PipelinedAggregator is not sorted");
    }

    delete m_currGroup;
    m_currGroup = vp->clone();
    
    // must read in only one block?
    processAggBlock(m_aggData, vp, groupStart, groupEnd);
    m_groupBlockIn = m_group->getNextValBlock(m_groupColIndex);
    if (m_aggType == GROUP_BY_SELF) {
      m_aggBlockIn = m_groupBlockIn;
    }
  } while (m_groupBlockIn != NULL && (vp = m_groupBlockIn->getNext()) && (*vp == m_currGroup));
  // keep reading in group blocks while the value hasn't changed
  if (m_groupBlockIn && vp)
    m_groupBlockIn->resetBlock();
  
  // write out the next value
  double aggValue = m_aggData->evaluate();
  //logOutputBlock(groupVal, aggValue);
  m_currGroup->position=m_currOutputPosition;
  m_groupBlockOut->setValue(m_currGroup);
  DoubleValPos lvp;
  lvp.set(m_currOutputPosition, (byte*)&aggValue);
  m_aggBlockOut->setValue(&lvp);
  m_currOutputPosition++;
}

void PipelinedAggregator::aggregateMultiValuedBlocks() {
  // update to next group block if current block has no more groups
  if (!m_groupBlockIn->hasNext()) {
    m_groupBlockIn = m_group->getNextValBlock(m_groupColIndex);
    //    if (m_groupBlockIn)
    //groupStart=m_groupBlockIn->peekNext()->position;
    //else
    if (!m_groupBlockIn) {
      m_groupBlockOut = NULL;
      m_aggBlockOut = NULL;
      return;
    }
    if (m_aggType == GROUP_BY_SELF) {
      m_aggBlockIn = m_groupBlockIn;
    }
  }
  ValPos* vp = m_groupBlockIn->getNext();
  //assume int type for now ... fix later
  //assert(vp->type == ValPos::INTTYPE);
  
  //int groupVal = *(int*)vp->value;
  int groupStart = vp->position;

  // debugging
  if (m_currGroup && *m_currGroup > vp) {
    throw new UnexpectedException("input to PipelinedAggregator is not sorted");
  }

  delete m_currGroup;
  m_currGroup = vp->clone();
  
  m_aggData->reset();
 
  // calculate extent of group, reading in blocks as necessary  
  //if (m_groupBlockIn) {
    
  while (m_groupBlockIn->hasNext() && *m_groupBlockIn->peekNext() == vp) {
    while (m_groupBlockIn->hasNext() && *m_groupBlockIn->peekNext() == vp) {
      vp = m_groupBlockIn->getNext();
      //assume int type for now ... fix later
      //assert(vp->type == ValPos::INTTYPE);
    }
    if (!m_groupBlockIn->hasNext()) {
      m_groupBlockIn = m_group->getNextValBlock(m_groupColIndex);
      if (!m_groupBlockIn) {
	break;
      }
      if (m_aggType == GROUP_BY_SELF) {
	m_aggBlockIn = m_groupBlockIn;
      }
    }
  }

    int groupEnd = vp->position;
    // processAggBlock pulls up new blocks if necessary
    processAggBlock(m_aggData, vp, groupStart, groupEnd);
    
    //}

  // write out the next value
  double aggValue = m_aggData->evaluate();
  //logOutputBlock(groupVal, aggValue);
  //m_groupBlockOut->setValue(groupVal, m_currOutputPosition);
  m_currGroup->position=m_currOutputPosition;
  m_groupBlockOut->setValue(m_currGroup);

  DoubleValPos lvp;
  lvp.set(m_currOutputPosition, (byte*)&aggValue);
  m_aggBlockOut->setValue(&lvp);
  m_currOutputPosition++;

}
