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
#ifndef EMHASHAGGREGATOR_H
#define EMHASHAGGREGATOR_H

#include "../common/Interfaces.h"
#include "../Util/HashMap.h"
#include "../Util/HashFunction.h"
#include "Aggregator.h"
#include "AggValue.h"
#include "../Wrappers/SMultiBlock.h"


class EMHashAggregator : public Operator {
  
 public:
  virtual ~EMHashAggregator();

  void setHashFunction(HashFunction *function_);
  void setHashTableSize(int size_);

  // Operator interface
  virtual Block* getNextValBlock(int colIndex_);
  virtual SBlock* getNextSValBlock(int colIndex_);	
  virtual PosBlock* getNextPosBlock(int colIndex_);
  virtual PosBlock* getNextPosBlock(int colIndex_, int val_);
  virtual int getLastPosition();

 protected:
  // aggregates over agg_ column with GROUP BY over group_ column
  EMHashAggregator(Operator *agg_, int aggColIndex_, int gcAttNum_, int aggAttNum_);

  // HOOK METHODS -- read Aggregator.h for comments
  virtual void generateBlocks();
  virtual AggValue* createTemplateAggValue() = 0;
  virtual void update(AggValue* aggValue_, ValPos* newValue_, int numOccurrences_) = 0;

 private:
  // HASH TABLE STATE
  HashMap *m_hashTable;         // hash table: group -> value
  int m_hashTableSize;          // number of buckets in the hash table
  HashFunction *m_hashFunction; // hash function used for hash-based aggregation
  bool m_isHashTableComputed;   // indicates whether the hash table is initialized and 
                                // has all <group, value> pairs computed and stored

  // initializer called by constructors
  void init();
  // loads the first input agg block and group block
  void initInputBlocks();
  // initializes the output blocks for the Aggreator
  void initOutputBlocks();

  void computeHashAggregation();
  SBlock* blockin;
  SMultiBlock* blockout;
  Operator *m_agg;       // input aggregation column
  int m_aggColIndex;     // index of input aggregation column
  int m_gcAttNum;
  int m_aggAttNum;
  bool m_areBlocksInitialized; 
  int m_currOutputPosition; // position value of next <value, position> pair 
  
};

#endif
