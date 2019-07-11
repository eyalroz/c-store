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
#include "Sum.h"

// SumFunction methods

AggValue* SumFunction::createTemplateAggValue() {
  SumValue *value = new SumValue;
  return value;
}

void SumFunction::update(AggValue* aggValue_,
			 ValPos* newValue_,
			 int numOccurrences_) {
  SumValue *value = (SumValue*)aggValue_;
  value->sum += (newValue_->getDouble() * ((long long)numOccurrences_));
}

void SumFunction::update(AggValue* aggValue_,
			 int newValue_,
			 int numOccurrences_) {
  SumValue *value = (SumValue*)aggValue_;
  value->sum += (newValue_ * ((long long)numOccurrences_));
}

// Pipelined Sum methods

Sum::Sum(Operator *agg_, int aggColIndex_)
  : AbstractPipelinedAggregator(agg_, aggColIndex_) {
  m_func = new SumFunction;
}

Sum::Sum(Operator *agg_, int aggColIndex_, Operator* group_, int groupColIndex_)
  : AbstractPipelinedAggregator(agg_, aggColIndex_, group_, groupColIndex_) {
  m_func = new SumFunction;
}

// HashSum functions 

HashSum::HashSum(Operator *agg_, int aggColIndex_, Operator* group_, int groupColIndex_)
  : AbstractHashAggregator(agg_, aggColIndex_, group_, groupColIndex_) {
  m_func = new SumFunction;
}

// EMHashSum functions 

EMHashSum::EMHashSum(Operator *agg_, int aggColIndex_, int gcAttNum_, int aggAttNum_)
  : AbstractEMHashAggregator(agg_, aggColIndex_, gcAttNum_, aggAttNum_) {
  m_func = new SumFunction;
}


