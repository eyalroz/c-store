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
// RLE DataSource
// Problems: migfer@mit.edu


#ifndef RLEDATASOURCE_H
#define RLEDATASOURCE_H

#include "../common/Constants.h"
#include "../common/Interfaces.h"
#include "../common/DataSource.h"
#include "../common/BlockWithPos.h"
#include "Decoder/RLEDecoder.h"
#include "RLEBlock.h"
#include "PosRLEBlock.h"
#include "PosType2Block.h"
#include "PosBasicBlock.h"
#include "CodingException.h"
#include "../Writers/PosRLEWriter.h"
#include <iostream>
#include <sstream>
#include <list>

// Implementation of the DataSource for RLE encoding
class RLEDataSource : public DataSource {
public:
	// Construct an rle data source that :
	//		-access data through am_
	//		-writes blocks using writer_
	RLEDataSource(AM* am_, bool valSorted_, RLESuperDecoder* decoder_=NULL);
	
	// Copy constructor
	RLEDataSource(const RLEDataSource&);		
	
	// Destructor
	virtual ~RLEDataSource();
	
	
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
	
	// Method used to debug, prints all triples in the column, 
	// no regard to predicates or filters
	virtual void printColumn();

	//another version of printColumn, but print to string instead of stdout
	//used for testing purpose.
	virtual void printColumn(stringstream&);
	
	virtual void setSingles(bool gs) { getSingles=gs;}

protected:
	
	bool checkOutBlock(bool& done_);
	bool checkTripleOnPos(bool& done_);
	RLETriple* getNextTripleFromDisk(AM* am_);	
	RLETriple* getNextTriple(AM* am_);
	
	RLETriple* getTriple(int skipValue_);
	bool getSingles;
	bool valSorted;
	byte* pageBuffer;	
	
	RLESuperDecoder* decoder;
	RLEBlock* outBlock;
	//Type2Block* outT2Block;
	
	bool getNewPosBlock;
	RLETriple* currTriple;
	RLETriple* nextFilteredTriple;

	PosRLEBlock* posBlock;
	
	// Position blocks on non sorted columns
	bool posInit;
	bool posDone;
	bool posIsTripleAvail;
	RLETriple* posOutTriple;
	RLETriple* posTripleAvail;
	
	BlockIter* currBlockIter;
	PosBlockIter* pba;
	BlockWithPos* currBlockWP;
	bool useWP;
	unsigned int currPos;
	bool createdOwnDecoder;

	//int currFilterStart;
	//int currFilterEnd;
};

#endif // RLEDATASOURCE_H
