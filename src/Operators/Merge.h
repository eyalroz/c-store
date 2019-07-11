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
#ifndef MERGE_H
#define MERGE_H

#include "../common/Interfaces.h"
#include <cassert>
#include "../Wrappers/RLEBlock.h"
//#include "../Wrappers/LongBasicBlock.h"

/**
 * The Merge operator is a base class for all merge-related operations.
 */

class Merge : public Operator {
  
 public:
  Merge(Operator *rs_, Operator *ws_, int nCols_);
  ~Merge();

  // Operator interface
  virtual Block* getNextValBlock(int colIndex_);	
  virtual PosBlock* getNextPosBlock(int colIndex_);
  virtual PosBlock* getNextPosBlock(int colIndex_, int val_);
  virtual int getLastPosition() = 0;

 protected:
  Operator *m_rs;
  Operator *m_ws;
  
  int m_nCols;

  Block** m_rsInputBlocks;
  Block** m_wsInputBlocks;
  // fix this later
  Block** m_outputBlocks;

  // loads the output blocks with the new set of values
  // needs to maintain its own state regarding which input blocks have been used
  virtual void generateBlocks() = 0;

  // called during block intialization
  // does not need to load the starting RS and WS input blocks
  // does not need to create output blocks
  virtual void init() = 0;

  virtual void initOutputBlocks() = 0;

  void loadWSInputBlocks();
  void loadRSInputBlocks();

  int m_currOutputPosition;

 private:

  bool* m_isConsumed;
  bool m_isInitialized;
  int m_RSblockSize;
  bool m_RSIsPositionContiguous;
  bool m_RSIsValueSorted;
};

#endif
