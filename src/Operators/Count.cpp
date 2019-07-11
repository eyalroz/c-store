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
#include "Count.h"

// CountFunction

AggValue* CountFunction::createTemplateAggValue() {
  CountValue *value = new CountValue;
  value->count = 0;
  return value;
}

void CountFunction::update(AggValue* aggValue_, ValPos* newValue_, int numOccurrences_) {
  ((CountValue*)aggValue_)->count+=numOccurrences_;
}

void CountFunction::update(AggValue* aggValue_, int newValue_, int numOccurrences_) {
  ((CountValue*)aggValue_)->count+=numOccurrences_;
}

// Count, inheriting from AbstractPipelinedAggregator

Count::Count(Operator *agg_, int aggColIndex_)
  : AbstractPipelinedAggregator(agg_, aggColIndex_) {
  m_func = new CountFunction;
}

Count::Count(Operator *agg_, int aggColIndex_, Operator* group_, int groupColIndex_)
  : AbstractPipelinedAggregator(agg_, aggColIndex_, group_, groupColIndex_) {
  m_func = new CountFunction;
}

void Count::processAggBlock(AggValue* aggValue_, ValPos* groupValue_, int groupStart_, int groupEnd_) {
  update(aggValue_, groupValue_, groupEnd_ - groupStart_ + 1);
}

void Count::processAggBlocksAsOneGroup(AggValue* aggValue_) {
  CountValue* value = (CountValue*)aggValue_;
  do {
    value->count += m_aggBlockIn->getSize();
    m_aggBlockIn = m_agg->getNextValBlock(m_aggColIndex);
  } while (m_aggBlockIn != NULL);
}

// HashCount
HashCount::HashCount(Operator *agg_, int aggColIndex_, Operator* group_, int groupColIndex_)
  : AbstractHashAggregator(agg_, aggColIndex_, group_, groupColIndex_) {
  m_func = new CountFunction;
}

void HashCount::processAggBlock(AggValue* aggValue_, ValPos* groupValue_, int groupStart_, int groupEnd_) {
  //Optimization: copying the update method code here to avoid the method call. I hope this code duplication doesn't come back to bite me ...
  ((CountValue*)aggValue_)->count+=(groupEnd_ - groupStart_ + 1);
  //update(aggValue_, groupValue_, groupEnd_ - groupStart_ + 1);
}

void HashCount::processAggBlocksAsOneGroup(AggValue* aggValue_) {
  CountValue* value = (CountValue*)aggValue_;
  do {
    value->count += m_aggBlockIn->getSize();
    m_aggBlockIn = m_agg->getNextValBlock(m_aggColIndex);
  } while (m_aggBlockIn != NULL);
}
