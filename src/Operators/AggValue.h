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
#ifndef AGGVALUE_H
#define AGGVALUE_H

struct AggValue {
  virtual ~AggValue(){};
  virtual double evaluate() = 0;
  virtual void reset() = 0;
};

struct CountValue : public AggValue {
  int count;
  virtual ~CountValue(){};
  CountValue() : count(0) {}
  virtual double evaluate() {
    return count;
  }

  virtual void reset() {
    count = 0;
  }
};

struct SumValue : public AggValue {
  double sum;
  virtual ~SumValue(){};
  
  SumValue() : sum(0) {}

  virtual double evaluate() {
    return sum;
  }
  virtual void reset() {
    sum = 0;
  }
};

struct MaxValue : public AggValue {
  double max;
  virtual ~MaxValue(){};
   
  MaxValue() : max(0) {}

  virtual double evaluate() {
    return max;
  }
  virtual void reset() {
    max = 0;
  }
};

struct MinValue : public AggValue {
  double min;
  virtual ~MinValue(){};
  
  // use -1 to represent largest value
  MinValue() : min(-1) {}

  virtual double evaluate() {
    if (min == -1) {
      return 0;
    } else {
      return min;
    }
  }

  virtual void reset() {
    min = 0;
  }
};

struct AverageValue : public AggValue {
  double sum;
  int total;
  virtual ~AverageValue(){};

  AverageValue() : sum(0), total(0) {}

  virtual double evaluate() {
    if (total == 0) {
      return 0;
    }

    return sum/(double)total;
  }

  virtual void reset() {
    sum = total = 0;
  }

};

#endif
