#ifndef TUPLEGENERATOR_H_
#define TUPLEGENERATOR_H_

#include "Operator.h"
#include "../Wrappers/SMultiBlock.h"

class TupleGenerator : public Operator
{
public:
	TupleGenerator(Operator** sources_, int* ncols_, int nsources_, int total_ncols_); 
	virtual ~TupleGenerator();

	virtual	Block* getNextValBlock(int colIndex_) { throw UnexpectedException("unsupported"); }

	virtual	SBlock* getNextSValBlock(int colIndex_);	
	
	// Gets the next position block (bitstring of positions) from the operator
	virtual PosBlock* getNextPosBlock(int colIndex_) { throw UnexpectedException("unsupported"); }
	
	// Gets the maximum position in the data source
	virtual int getLastPosition() { throw UnexpectedException("unsupported"); }
protected:
	Operator** sources;
	int* ncols;
	int nsources;
	int total_ncols;
	int outblock_capacity;
	
	SMultiBlock outBlock;
	int** curarrays;
	int* curarray_sizes;
	int* curindexes;
	BlockIter** curiters;
	int* out_block_buffer;
	bool done;
//	static const int OUTBLOCK_SIZE = 1000; // DSM is this an OK value?
};

#endif /*TUPLEGENERATOR_H_*/
