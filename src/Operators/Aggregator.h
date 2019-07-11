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
#ifndef AGGREGATOR_H
#define AGGREGATOR_H

#include "AggValue.h"
#include <cassert>
#include "../common/Interfaces.h"
//#include "../Wrappers/LongBasicBlock.h"

/**
 * An Aggregator computes an aggregate function of an input "aggregate" column 
 * subject to a list of "group" columns. Currently, the Aggregator supports either
 * 0 or 1 group columns.  The aggregation type of the Aggregator is NO_GROUP if 
 * there are 0 group columns and either GROUP_BY_SELF or GROUP_BY_OTHER depending
 * on whether the 1 group column is identical to the aggregate column.  This aspect
 * will need to be later extended to support multiple group by columns.
 * 
 * When an operator above the Aggregator calls getNextValBlock() on the Aggregator,
 * the Aggregator checks whether its current output blocks have already been 
 * consumed; if so, it loads the next set of output blocks via the generateBlocks()
 * call.  The Aggregator expects its output columns to be consumed in parallel and
 * throws an exception if the access pattern is not parallel.  The Aggregator itself
 * tries to consume its input blocks in parallel in most cases, does not make this
 * guarantee for performance reasons (i.e., Count does not need to examine the 
 * aggregate column, though this parameter can probably be removed).
 *
 * The function generateBlocks() is virtual and the implementation is deferred to
 * Aggregator subtypes.  For instance, a HashAggregator will tabulate the aggregates
 * for all groups, and simply extract each group one by one from the table to return
 * to the client.  On the other hand, a PipelinedAggregator processes each sequence
 * of contiguous group values that it sees and returns the group when the end of the
 * sequence has been determined.
 *
 * The use of a HashAggregator versus a PipelinedAggregator is determined by the
 * C-Store Query Optimizer.
 *
 * To support arbitrary aggregation functions, the Aggregator uses the following
 * pattern.  Each Aggregator function (i.e. Count, Sum, Min, etc.) implements a 
 * createTempateAggValue() interface that returns an AggValue* data structure.
 * It then defines an update(AggValue, int newValue_, int numOccurrences_) method 
 * that updates the data structure with the specified number of occurrences of the 
 * new value.  At any point, calling evaluate() on the data structure returns the
 * current value (i.e. count, sum, min, etc.) of the data structure based on all
 * the updates that it has seen.  This interface will later need to be refined
 * to support evaluations that return multiple values; for example, calling 
 * evaluate() on an Average function may need to return both the sum and the
 * total number in order to support partial aggregate computations.
 *
 * The process*() methods are wrappers around the above update() function that
 * implement the logic of taking in a group extent (start and end position) and
 * then making the appropriate update() call.  The current interface for the 
 * process*() methods assume contiguous positions, which is not true for the WOS.
 * WOS processing still works for now because of the following two reasons:
 *
 * 1) there is only ever one value per block and
 * 2) we use hash-based aggregation for the WOS.
 *
 * These assumptions will not hold in a true WOS implementation, and the interface
 * will need to be fixed.
 *
 * @author  Edmond Lau <edmond@mit.edu>
 */

// TODO: test multi-valued block methods in Aggregator and PipelinedAggregator

// remember only 1 Pair buffer used

class Aggregator : public Operator {

 public:
  virtual ~Aggregator();
  
  // Operator interface
  virtual Block* getNextValBlock(int colIndex_);	
  virtual PosBlock* getNextPosBlock(int colIndex_);
  virtual PosBlock* getNextPosBlock(int colIndex_, int val_);
  virtual int getLastPosition();
  
  // enumeration names to refer to the indexes
  enum { GROUP_COL_INDEX = 0, AGG_COL_INDEX = 1};

 protected:
  // types of aggregation:
  // NO_GROUP: aggregation without a GROUP BY clause
  // GROUP_BY_SELF: the GROUP BY column is the same as the aggregation column
  // GROUP_BY_OTHER: the GROUP BY column is some other column than the aggregation column
  enum AGG_TYPE { NO_GROUP, GROUP_BY_SELF, GROUP_BY_OTHER };
    
  // properties
  AGG_TYPE m_aggType;

  // INPUT STATE
  Operator *m_agg;       // input aggregation column
  int m_aggColIndex;     // index of input aggregation column
  Operator *m_group;     // input group by column
  int m_groupColIndex;   // index of group by column
  Block *m_aggBlockIn;   // input agg block
  Block *m_groupBlockIn; // input group by block
  
  // OUTPUT STATE
  bool m_isAggBlockConsumed;   // whether the agg block has been consumed by Operator above
  bool m_isGroupBlockConsumed; // whether the group block has been consumed by Operator above
  //LongBasicBlock *m_aggBlockOut;        // output agg block
  BasicBlock *m_aggBlockOut;        // output agg block
  BasicBlock *m_groupBlockOut;      // output group by block
  int m_currOutputPosition;    // position value of next <value, position> pair

  // CONSTRUCTORS

  // aggregates over agg_ column with no GROUP BY column
  Aggregator(Operator *agg_, int aggColIndex_);
  // aggregates over agg_ column with GROUP BY over group_ column
  Aggregator(Operator *agg_, int aggColIndex_, Operator *group_, int groupColIndex_);

  // HOOK METHODS
  // Generates/loads the next set of output blocks
  // - setting of consumption flags is taken care of by Aggregator
  virtual void generateBlocks() = 0;

  // Creates an AggValue data structure to maintain current state of a group's aggregate value
  virtual AggValue* createTemplateAggValue() = 0;
  // Updates the AggValue data structure with the specified new value and number of occurrences
  virtual void update(AggValue* aggValue_, ValPos* newValue_, int numOccurrences_) = 0;
  virtual void update(AggValue* aggValue_, int newValue_, int numOccurrences_) =0;

  // METHODS CALLED BY SUBCLASSES

  // Processes the agg values in the block stream spanning the positions 
  //   groupStart_ to groupEnd_ and updates aggValue_ with the new value. 
  //   The two positions denote the extent of the current group. The manner 
  //   in which aggValue_ is updated depends on the type of the aggregation
  //   Operator.
  //
  // pre: cursor in m_aggBlockIn is pointing at groupStart_.  
  //      calling m_aggBlockIn will not return NULL.
  // post: cursor in m_aggBlockIn is pointing at block with groupEnd_ + 1
  //       cursor in m_groupBlockIn does not change
  virtual void processAggBlock(AggValue* aggValue_, ValPos* groupValue_, int groupStart_, int groupEnd_);

  // Processes all the agg blocks as if they belonged to the same group.
  //   aggValue_ is updated with the new value after processing the 
  //   entire agg column.
  virtual void processAggBlocksAsOneGroup(AggValue* aggValue_);

  // UTILITY METHODS to log values/groups
  void logOutputBlock(long long value_);
  void logOutputBlock(int group_, long long value_);

 private:

  bool m_areBlocksInitialized;  

  // initializes the Aggregator
  void init();
  // loads the first input agg block and group block
  void initInputBlocks();
  // initializes the output blocks for the Aggreator
  void initOutputBlocks();

  
  void processAggBlockGroupBySelf(AggValue* aggValue_, ValPos* groupValue_, int groupStart_, int groupEnd_);
  void processAggBlockGroupByOtherOneValued(AggValue* aggValue_, int groupStart_, int groupEnd_);
  void processAggBlockGroupByOtherMultiValued(AggValue* aggValue_, int groupStart_, int groupEnd_);

  BlockIter* bi;
  int* iterarray;
  int itersize;
  int iterindex;
};

#endif
