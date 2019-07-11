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
#ifndef BNOTOPERATOR_H
#define BNOTOPERATOR_H

#include <cassert>
#include "../common/Interfaces.h"
#include "../Wrappers/PosRLEBlock.h"
#include "../Wrappers/PosBasicBlock.h"
#include "../Wrappers/PosDeltaBlock.h"

/**
 *
 * BNotOperator high-level pseudocode:
 * Read in blocks using getNextBlock
 */

class BNotOperator : public Operator {

	public:
  
		BNotOperator(Operator *inputOp_, int inputColIndex_);

  	virtual ~BNotOperator();
  
		// Operator interface
		virtual Block* getNextValBlock(int val_);
		virtual PosBlock* getNextPosBlock();
		virtual PosBlock* getNextPosBlock(int val_);
		inline virtual int getLastPosition() { throw UnimplementedException("Not applicable to BNotOperator"); };
  
	protected:

		// INPUT STATE
		Operator* inputOp;
		int inputColIndex; 
		PosBlock* inputBlock;
		
		// OUTPUT STATE
		PosBlock* m_BlockOut;		// output block
	
  private:
	
		//operator/datasource type
    enum OP_TYPE { RLE, DELTA, BASIC, UNKNOWN};
    OP_TYPE inputOpType;

		bool m_areInputBlocksInitialized;
		PosRLEBlock* m_prb;
		PosBasicBlock* m_pbb;
		unsigned int prevInputBlockStartPos, prevInputBlockEndPos;
		unsigned int lastInputBlockPos;
    unsigned int prevOutputBlockPos;
		// initializes the BNotOperator
		void init();
		
		// initializes the input blocks to the BNotOperator
		bool initInputBlocks();
	
		//generate output block
		void generateBlocks();
    void makeOutputBlock(OP_TYPE outputType, 
                         const unsigned int startPos,
                         const unsigned int endPos);

		//helper functions for generateBlocks()		
		void processPosRLEBlock();
		void processPosBasicOrDeltaBlock();
		
		//determine input stream type: RLE, Delta or Basic
		void setInputOpType();
		
		//helper functions
		void updateInputRange(unsigned int& inputStartPos, unsigned int& inputEndPos);		
		void updatePrevInputRange(unsigned int& startPos, unsigned int& endPos);
    void findGap(unsigned int& outputBlockPos);
    unsigned int getNextPosition();   //return 0 if stream exausted
  
};

#endif
