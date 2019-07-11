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
#ifndef NLJOIN_H
#define NLJOIN_H

//#include "AggValue.h"
#include "../common/Interfaces.h"
#include "../Wrappers/RLETriple.h"
#include "../Wrappers/RLEBlock.h"
#include "../Wrappers/DeltaPosBlock.h"
#include "../Wrappers/PosBasicBlock.h"
#include "../Wrappers/PosRLEBlock.h"
#include "../Wrappers/PosType2Block.h"
#include "../Wrappers/Type2Block.h"
#include "../Wrappers/SMultiBlock.h"
#include "../Wrappers/MultiBlock.h"
#include <cmath>
#include <unordered_map>

class NLJoin : public Operator {

 public:
  
// CONSTRUCTORS
  // takes in the column to be aggregated and the columns to group by
  NLJoin(Operator *left_, int predColIndex1_, Operator *right_, int predColIndex2_);

  virtual ~NLJoin();
  
  // Operator interface
  virtual Block* getNextValBlock(int colIndex_);	
  virtual PosBlock* getNextPosBlock(int colIndex_);
  virtual PosBlock* getNextPosBlock(int colIndex_, int val_);
  virtual SBlock* getNextSValBlock(int colIndex_);
  virtual SBlock* getNextSValBlockEM(int colIndex_);
  virtual Block* getNextValBlockMC(int colIndex_);
  //virtual PosBlock* getNextPosBlockMC(int colIndex_);
  virtual void generateBlocksMC();
  inline virtual int getLastPosition() { throw UnimplementedException("Not applicable for Count Operator"); };
  virtual void setDontCheck();
  virtual void doLM() { doEM=false;};
  
  enum { L_COL_INDEX = 0, R_COL_INDEX = 1};

 protected:
  //enum AGG_TYPE { NO_GROUP, GROUP_BY_SELF, GROUP_BY_OTHER };
    
  // properties
  //AGG_TYPE m_aggType;
  // some hash table -- to be added

  // input state
  Operator *l_src;
  int l_predcol;
  Operator *r_src;
  int r_predcol;
  Block *lBlockIn;
  Block *rBlockIn;
  
  // output state
  bool m_isLBlockConsumed; // dirty means that the block has been read
  bool m_isRBlockConsumed; 
  PosBlock *lBlockOut;
  PosBlock *rBlockOut;
  int m_currOutputPosition;
  
  // For Sval I/O
  SMultiBlock* svlBlockIn;

  SMultiBlock* svBlockOut;
  SPair* currLHSTuple;
  int curr_rhs_block_idx;
  int out_tupwidth;
  bool join_done;

  // For MC
  Block *rBlockInExtra;
  BlockIter** blocks3;
  BlockIter** blocks4;
  Block** blocks2;
  RLEBlock* outRLEBlock;
  BlockIter* outBlockIter;
  Block *lBlockInExtra;

  // hook methods
  //virtual AggValue* createTemplateAggValue() = 0;

  // Processes the agg values in the block stream spanning the positions groupStart_ to groupEnd_
  //   and updates aggValue_ with the new value. The two positions denote the extent of the 
  //   current group. The manner in which aggValue_ is updated depends on the type of aggregation
  //   Operator.  Objects that inherit from this class must implement the logic.
  //virtual void processAggBlock(AggValue* aggValue_, int groupStart_, int groupEnd_) = 0;

  // Processes all the agg blocks as if they belonged to the same group
  //   aggValue_ is updated with the new value after processing the entire agg column.
  //virtual void processAggBlocksAsOneGroup(AggValue* aggValue_) = 0;

  //Aggregator::AGG_TYPE getAggType();

 private:
  void generateBlocks();
  void init();
  void initOutputBlocks();
  void readSvalInnerTable();
  void logOutputBlock(int count);
  ValPos* getCurrLPair();
  ValPos* getNextLPair();
  ValPos* getNextLPairMC();
  //void logOutputBlock(int group, int count);

  bool firstCall;
  bool dontcheck;
  Block** blocks;
  SMultiBlock* rightFirstBlock;
  int** sblocks;
  int* sblocksizes;
  int curblocki;
  int** indicies;
  int currRightIndex;

  bool doEM;

  ValPos* currLPair;

};

#endif
