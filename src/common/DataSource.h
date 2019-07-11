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
#ifndef DATASOURCE_H
#define DATASOURCE_H

class Decoder;

#include "Util.h"
#include "Constants.h"
#include "Interfaces.h"
#include "../Wrappers/PosType2Block.h"
#include "../Wrappers/Decoder/Decoder.h"
#include "../Wrappers/PosRLEBlock.h"
#include "BlockWithPos.h"
#include "../AM/DVAM.h"
#include "../AM/WOSAM.h"
#include "../AM/CatalogInstance.h"


#include <map>
#include <sstream>
// Abstract class that:
//	-Gets data from the AM
//	-Holds data in memory
// 	-Provides filtering of a column on a bitstring
class DataSource : public Operator {
public:
	// Construct a datasource that :
	//		-access data through am_
	//		-writes blocks using writer_
	DataSource(AM* am_);
	DataSource(AM* am_, bool isROS_);
	DataSource(AM* am_, bool isROS_, bool valSorted_, Decoder* decoder_);
	
	// Copy constructor
	DataSource(const DataSource&);		
	
	// Destructor
	virtual ~DataSource();
	
	// Gets the next value block from the operator 
	virtual Block* getNextValBlock(int colIndex_);
	
	// Skips to the first block with this value
	// Returns NULL if the value is outside the range of the column
	virtual Block* skipToValBlock(int colIndex_, int val_) {throw new UnimplementedException("Not done yet");return NULL;}
	
	
	// Gets the next position block (bitstring of positions) from the operator
	virtual PosBlock* getNextPosBlock(int colIndex_);
	
	// Skips to the first block with this position
	// Returns NULL if the value is outside the range of the column
	virtual PosBlock* skipToPosBlock(int colIndex_, int pos_) {throw new UnimplementedException("Not done yet");return NULL;}
		
	// Changes the RHS binding for this datasource
	virtual void changeRHSBinding(ValPos* rhs_);

	// Changes the RHS binding for this datasource - keep around so old code will still work
	virtual void changeRHSBinding(int rhsint);
	
	// sets a new predicate for this datasource
	virtual void setPredicate(Predicate* pred_);
	
	// Sets a filter for values
	virtual void setValueFilter(Operator* bitstringDataSource_, int colIndx_);
	
	// Sets a filter for postions
	virtual void setPositionFilter(Operator* bitstringDataSource_, int colIndx_);

	//printColumn, but print to string instead of stdout
	//used for testing purpose.
	virtual void printColumn(stringstream& ss){ss << "DATASOURCE PRINT" << endl;};
	
	// Gets the maximum position in the data source
	virtual int getLastPosition();

	//gets the next page along value index	
	virtual const void* getNextPageValue();

	//gets the next page along position index
	virtual const void* getNextPagePosition();

	//skips to page along the value index
	virtual const void* skipToPageValue(char* key);

	//skips to page along the position index
	virtual const void* skipToPagePosition(int key);

	//gets the next page along value index	
	virtual const void* getNextPageValue(AM* am);

	//gets the next page along position index
	virtual const void* getNextPagePosition(AM* am);

	//skips to page along the value index
	virtual const void* skipToPageValue(char* key, AM* am);

	//skips to page along the position index
	virtual const void* skipToPagePosition(int key, AM* am);

	virtual ValPos** createValueIndex();

protected:
	// Gets a page from am_ on the primary index (usually value)
	virtual byte* getValSortedGetPage(AM* am_);	

	// Main method to get a page given the predicates and filters
	virtual byte* getRightPage(AM* am_, Decoder* decoder_, bool valSorted_, bool posIndexPrimary_);
	virtual bool skipToRightPosOnPage(Decoder* decoder_);

	// Utility Method: Given an AM, and a Decoder it iterates through pages returning
	// block by block
	Block* getBlocks(AM* am_, Decoder* decoder);
	bool checkOutBlock(bool& done_);
	void init(AM* am_, bool isROS_);
	Block* getDecodedBlock(Decoder* decoder);
	Block* getNextValBlock(AM* am_);
	
	AM* getBlocksCurrAM;	
	Decoder* getBlocksCurrDecoder;
	byte* getBlocksCurrPage;


	// Filtering methods
	bool getPosFilterStartEnd(unsigned int& posFilterStart_, unsigned int& posFilterEnd_, bool getNextFilterRange_);

	// Access method used to get values
	AM* am;
	// Access method used to get positions
	bool posPrimaryIndex;

	// Have we altered filters or predicates?
	bool valFilterChanged;
	bool posFilterChanged;
	bool predChanged;
	bool predLookup;
	
	// Filter streams
	Operator* valFilter;
	unsigned int valColIndex;
	Operator* posFilter;
	unsigned int posColIndex;
	
	Predicate* pred;
	
	bool currValueFilterChanged;	
	bool posFilterInit;

	// Position Filter flags
	bool posFilterPosSorted;
	bool posFilterCurrBlockPosSorted;
	bool posFilterCurrBlockPosContig;

	// Current Position Block
	PosBlock* currPosBlock;
	bool havePosBlock;
	// Current range for position filtering
	unsigned int posFilterStart;
	unsigned int posFilterEnd;
	unsigned int posSkippedTo; // last position looked up in position index
	
	bool isROS;
	bool getNextNextTime;
	bool RLEAlways;

	bool valSorted;
	bool useWP;
	Decoder* decoder;
	Block* currBlock;
	BlockWithPos* currBlockWP;
	RLEBlock* theRLEBlock;
	RLETriple* currRLETriple;
	//MultiBlock* valOutBlock;
	PosBlock* posOutBlock;
	RLETriple* posOutTripleOnPred;
	PosBlockIter* pba;
	Type2Block* matchedPred;
	PosType2Block* matchedPredPos;
	
private:
	virtual byte* getPage(AM* am_, Decoder* decoder_);
	virtual byte* getPageOnPred(AM* am_, Decoder* decoder_, bool valSorted_);
	virtual byte* getPageOnPos(AM* am_, Decoder* decoder_, bool posIndexPrimary_);
	virtual byte* getPageOnPosAndPred(AM* am_, Decoder* decoder_, bool posIndexPrimary_);
	virtual byte* getPageOnVal(AM* am_, Decoder* decoder_, bool valSorted_);
	virtual byte* getPageOnValAndPos(AM* am_, Decoder* decoder_, bool valSorted_);		
	
	// used by getPosFilterStartEnd()
	void getNextPosFilterBlock();
};

#endif // DATASOURCE_H
