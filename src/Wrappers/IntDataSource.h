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
// Uncompressed Int Datasource
// Problems: migfer@mit.edu

#ifndef _INTDATASOURCE_H_
#define _INTDATASOURCE_H_

#include "../common/DataSource.h"
#include "Decoder/IntDecoder.h"
#include "BasicBlock.h"
#include "../common/BlockWithPos.h"
#include "PosBasicBlock.h"
#include "PosMultiBlock.h"
#include "PosType2Block.h"
#include "RLETriple.h"
#include "../common/UnimplementedException.h"
#include "PosRLEBlock.h"
#include <vector>
// IF sorted:
// Primary index is on value
// Secondary index on position
// ELSE:
// Primary Index on Position
class IntDataSource : public DataSource
{
public:
	// Construct a datasource that :
	//		-access data through am_
	//		-writes blocks using writer_
	IntDataSource(AM* am_, bool valSorted_, bool isROS);
	
	// Copy constructor
	IntDataSource(const IntDataSource&);		
	
	// Destructor
	virtual ~IntDataSource();
	
	// Gets the next value block from the operator 
	virtual Block* getNextValBlock(int colIndex_);
	
	// Gets the value block from the operator that contains position pos 
	virtual Block* getNextValBlockAtPos(int colIndex_, unsigned int pos);

	// Skips to the first block with this value
	// Returns NULL if the value is outside the range of the column
	virtual Block* skipToValBlock(int colIndex_, int val_);
	
	
	// Gets the next position block (bitstring of positions) from the operator
	virtual PosBlock* getNextPosBlock(int colIndex_);
	
	// Skips to the first block with this position
	// Returns NULL if the value is outside the range of the column
	virtual PosBlock* skipToPosBlock(int colIndex_, int pos_);

	//printColumn: print to string instead of stdout
	//used for testing purpose.
	virtual void printColumn(stringstream&);

	virtual Block* getDecodedBlock(Decoder* decoder);

	virtual void setSingles(bool gs) { getSingles=gs;}	

protected:
	AM* posAm;
	virtual Block* getNextValBlock(AM* am_);
	// checks the current out block to see if it matches pred and filters
	bool checkOutBlock(bool& done_);
	bool init;	
	bool valSorted;
	bool getSingles;
	bool useWP;
	
	byte* currPage;
	Decoder* decoder;
	MultiBlock* currBlock;
	BlockWithPos* currBlockWP;
	RLEBlock* theRLEBlock;
	//MultiBlock* valOutBlock;
	PosBlock* posOutBlock;
	RLETriple* posOutTripleOnPred;
	//MultiBlock* tempBlock;

	PosBlockIter* pba;
	Type2Block* matchedPred;
	PosType2Block* matchedPredPos;
	//PosBlock** realpba;
	
private:	
	void filterCurrBlockByPred();

};

#endif //_INTDATASOURCE_H_
