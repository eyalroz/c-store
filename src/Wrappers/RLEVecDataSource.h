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
//RLEVecDataSource
#ifndef RLEVECDATASOURCE_H
#define RLEVECDATASOURCE_H

#include "../common/Constants.h"
#include "../common/Interfaces.h"
#include "../common/DataSource.h"
#include "Decoder/RLEDecoder.h"
#include "RLEBlock.h"
#include "RLETriple.h"
#include "CodingException.h"
#include <iostream>

// Implementation of the DataSource for RLE encoding
class RLEVecDataSource : public Operator {
public:
	// Construct an rle data source that :
	//		-access data through am_
	//		-writes blocks using writer_
	RLEVecDataSource(vector<RLETriple>* vec_);
	
	// Copy constructor
	RLEVecDataSource(const RLEVecDataSource& ds);		
	
	// Destructor
	~RLEVecDataSource();
	
	
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
	
	// Changes the RHS binding for this datasource
	virtual void changeRHSBinding(int rhs_);
	
	// sets a new predicate for this datasource
	virtual void setPredicate(Predicate* pred_);
	
	// Sets a filter for values
	virtual void setValueBitstring(DataSource* valueFilter_);
	
	// Sets a filter for postions
	virtual void setPositionBitstring(DataSource* posFilter_);

	// Method used to debug, prints all triples in the column, 
	// no regard to predicates or filters
	virtual void printColumn();
	
	inline virtual int getLastPosition() { throw UnimplementedException("Not applicable for RLEVecDataSource"); };
	
protected:
	bool checkPage();
	bool checkPage(int skipValue_);
	RLETriple* getNextTriple();
	RLETriple* getTriple(int skipValue_);
	
	byte* pageBuffer;

	
	bool valFilterChanged;
	bool posFilterChanged;
	bool predChanged;
	
	DataSource* valueFilter;
	DataSource* posFilter;
	Predicate* pred;
	
	RLEDecoder* decoder;
	RLEBlock* outBlock;

	vector<RLETriple>* tripVecPtr;
	int idx;
};

#endif // RLEDATASOURCE_H
