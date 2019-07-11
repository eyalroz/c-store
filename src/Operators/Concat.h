#ifndef CONCAT_H_
#define CONCAT_H_

#include "../common/Block.h"
#include "../common/SBlock.h"
#include "../common/PosBlock.h"
#include <cassert>
#include "../common/Interfaces.h"
#include "../Wrappers/LongBasicBlock.h"
#include "../Operators/Operator.h"
#include "../Wrappers/SMultiBlock.h"

class Concat: public Operator
{
public:
	Concat(Operator* inputs[], int ninputs, int numtouse);
	Concat(Operator* input); // For use as a pos/val -> tuple converter
	virtual ~Concat();
	
	// Gets the next value block from the operator 
	virtual	Block* getNextValBlock(int colIndex_);

	// Gets the next string value block from the operator
	// THIS IS UGLY ...FIX LATER
	virtual	SBlock* getNextSValBlock(int colIndex_);	
	
	// Gets the next position block (bitstring of positions) from the operator
	virtual PosBlock* getNextPosBlock(int colIndex_);
	
	// Gets the maximum position in the data source
	virtual int getLastPosition();
	
	void updateInputBlocks();
	
protected:
	Operator** inputs;
	Block** inputBlocks;
	/** Number of inputs available */
	int ninputs;
	/** How many of the ninputs will we actually use? */
	int numtouse; 
	/** Number of tuples per SMultiBlock */
	static const int BLOCKSIZE = 1000;
	SMultiBlock* sb;
};

#endif /*CONCAT_H_*/
