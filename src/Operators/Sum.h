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
#ifndef SUM_H
#define SUM_H

#include "AbstractAggregator.h"
#include "AggValue.h"
#include "Operator.h"

class SumFunction : public AggregatorFunction {
 public:
  virtual ~SumFunction(){};
  virtual AggValue* createTemplateAggValue();
  virtual void update(AggValue* aggValue_, ValPos* newValue_, int numOccurrences_);
  virtual void update(AggValue* aggValue_, int newValue_, int numOccurrences_);
};


class Sum : public AbstractPipelinedAggregator {

 public:
  Sum(Operator *agg_, int aggColIndex_);
  Sum(Operator *agg_, int aggColIndex_, Operator* group_, int groupColIndex_);
  virtual ~Sum(){};

};

class HashSum : public AbstractHashAggregator {

 public:
  HashSum(Operator *agg_, int aggColIndex_, Operator* group_, int groupColIndex_);
  virtual ~HashSum(){};

};

class EMHashSum : public AbstractEMHashAggregator {

 public:
  EMHashSum(Operator *agg_, int aggColIndex_, int gcAttNum_, int aggAttNum_);
  virtual ~EMHashSum(){};

};

#endif

