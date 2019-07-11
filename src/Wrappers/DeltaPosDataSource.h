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
// Delta on Position DataSource
// Problems: migfer@mit.edu


#ifndef _DELTAPOSDATASOURCE_H_
#define _DELTAPOSDATASOURCE_H_

#include "../common/Interfaces.h"
#include "DeltaPosBlock.h"
#include "PosDeltaBlock.h"
#include "IntDataSource.h"
#include "Decoder/DeltaPosDecoder.h"
#include "../common/DataSource.h"
#include <map>
#include <sstream>

using namespace std;

typedef map<const int,ROSAM*> AMMap;
typedef map<const int,DeltaPosBlock*> BlockMap;

typedef BlockMap::iterator BlockMapIterator; 
typedef AMMap::iterator AMMapIterator;


class DeltaPosDataSource : public DataSource
{
public:
	// Construct a datasource that :
	//		-access data through am_
	//		-writes blocks using writer_
	DeltaPosDataSource(AM* am_, bool valSorted_);
	DeltaPosDataSource(const DeltaPosDataSource&);
	virtual ~DeltaPosDataSource();	
	
	// Gets the next value block from the operator 
	virtual Block* getNextValBlock(int colIndex_);
	
	// Skips to the first block with this value
	// Returns NULL if the value is outside the range of the column
	virtual Block* skipToValBlock(int colIndex_, int val_);
	
	
	// Gets the next position block (bitstring of positions) from the operator
	virtual PosBlock* getNextPosBlock(int colIndex_);
	
	// Skips to the first block with this position
	// Returns NULL if the value is outside the range of the column
	virtual PosBlock* skipToPosBlock(int colIndex_, int pos_);

	//printColumn, but print to string instead of stdout
	//used for testing purpose.
	virtual void printColumn(stringstream&);

protected:
	bool valSorted;
	enum Mode { UNINIT, NORMAL, POS_CONTIG};
	Mode mode;

	Block* getNextValBlockNormal();
	Block* getNextValBlockPosContig();
	
	// Initilizes an AM per value
	void initAMsPosContig();
	void populateBlocksMap();
	void populateBlockMaps(BlockMapIterator startIter,	BlockMapIterator endIter);
	void loadPairs();
	void loadPairs(BlockMapIterator startIter,	BlockMapIterator endIter);
	
	// Just opens an AM for first value
	void initAMsNormal();
	void changeValueNormal(int val_);
	bool getNextValNormal(int& value_);
	
	// sets the mode of operation given the first call to getBlock
	// calls to initialize
	void setMode(bool isPosContig_);
	
	// For PosContig mode
	int posContigNumValues;
	BasicBlock* posContigOutBlock;
	DeltaPosDecoder* posContigDecoder;
	AMMap valueAMMap;
	BlockMap blockMap;
	map<int, int> posValueMap;
	map<int, int> valPosMap;	
	// For normal mode
	IntDataSource* valDataSource;
	ROSAM* currColumnAM;
	DeltaPosDecoder* currColumnDecoder;

	DeltaPosBlock* outBlock;
	PosDeltaBlock* posOutBlock;
};

#endif //_DELTAPOSDATASOURCE_H_
