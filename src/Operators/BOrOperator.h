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
#ifndef BOROPERATOR_H
#define BOROPERATOR_H

#include <cassert>
#include "../common/Interfaces.h"
#include "../Wrappers/PosRLEBlock.h"
#include "../Wrappers/PosBasicBlock.h"

/**
 *
 * BOrOperator high-level pseudocode:
 * Read in blocks using getNextBlock
 */

class BOrOperator : public Operator {

 public:
  
	BOrOperator(Operator *leftOp_, int leftColIndex_, Operator *rightOp_, int rightColIndex_);

  virtual ~BOrOperator();
  
  // Operator interface
  virtual Block* getNextValBlock(int val_);
  virtual PosBlock* getNextPosBlock();
  virtual PosBlock* getNextPosBlock(int val_);
	inline virtual int getLastPosition() { throw UnimplementedException("Not applicable to BOrOperator"); };
  
 protected:

  // INPUT STATE
  Operator* leftOp;
  int leftColIndex; 
  Operator* rightOp;
  int rightColIndex;   
  PosBlock* inputBlockL;
  PosBlock* inputBlockR;
  
  // OUTPUT STATE
  PosBlock* m_BlockOut;		// output block

 private:

  bool m_areInputBlocksInitialized;
  PosRLEBlock* m_prb;
  PosBasicBlock* m_pbb;

  //postition holders
  unsigned int prevPosL, curPosL;
  unsigned int prevPosR, curPosR;
  
  void updateRightRange(int& rightStartPos, int& rightEndPos);
  void updateLeftRange(int& leftStartPos, int& leftEndPos);

  //helper function to get next position. return 0 if no more postion in the stream
  unsigned int getNextPosition(Operator* pOp, int& colIndex, PosBlock*& pBlock);
    
  // initializes the BOrOperator
  void init();
  
	// initializes the input blocks to the BOrOperator
  bool initInputBlocks();

  void generateBlocks();
  
};

#endif
