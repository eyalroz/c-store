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
#ifndef FILTER_H_
#define FILTER_H_

#include "../common/Block.h"
#include "../common/SBlock.h"
#include "../Wrappers/RLEBlock.h"
#include "../common/PosBlock.h"
#include <cassert>
#include "../common/Interfaces.h"
//#include "../Wrappers/LongBasicBlock.h"
#include "../Operators/Operator.h"
#include "../Wrappers/SMultiBlock.h"

class Filter: public Operator
{
public:
	Filter(Operator* inputs[], int ninputs, Predicate* preds[], int npreds, int* predindexes);
	//Filter(Operator* input); // For use as a pos/val -> tuple converter
	virtual ~Filter();
	
	// Gets the next value block from the operator 
	virtual	Block* getNextValBlock(int colIndex_);

	// Gets the next string value block from the operator
	virtual	SBlock* getNextSValBlock(int colIndex_);	

	// Gets the next string value block from the operator when pred!=NULL
	virtual	SBlock* getNextSValBlockPredDumb(int colIndex_);		

	// Gets the next string value block from the operator when pred!=NULL
	virtual	SBlock* getNextSValBlockPredNotDumb(int colIndex_);	

	// Gets the next string value block from the operator when pred==NULL
	virtual	SBlock* getNextSValBlockNoPredicate(int colIndex_);

	// Gets the next string value block from the operator using parallel EM technique
	virtual	SBlock* getNextSValBlockParallel(int colIndex_);		
	//virtual	SBlock* getNextSValBlockNoPred(int colIndex_);	
	
	// Gets the next position block (bitstring of positions) from the operator
	virtual PosBlock* getNextPosBlock(int colIndex_);
	
	// Gets the maximum position in the data source
	virtual int getLastPosition();
	
	void updateInputBlocks();
	void makeSmart();
	void setDoParallel();
	void setSpecialPred(Predicate* p);
	
protected:
	Operator** inputs;
	Predicate** preds;
	Block** inputBlocks;
	BlockIter** iterArray;
	/** Number of inputs available */
	int ninputs;
	/** How many predicates */
	int npreds; 
	/** How do predicates match up to inputs indices*/
	int* predindexes; 
	/** Number of tuples per SMultiBlock */
	static const int BLOCKSIZE = 1000;
	SMultiBlock* sb1;
	SMultiBlock* sb2;
	int curpos;
	bool* exhausted;
	unsigned int* endposes;
	bool done;
	bool dumbEM;
	bool doParallel;
	Predicate* specialPred;
};

#endif /*FILTER_H_*/
