#ifndef VALBLOCKTOPOSBLOCKCONVERTER_H_
#define VALBLOCKTOPOSBLOCKCONVERTER_H_

#include "../common/Block.h"
#include "../common/SBlock.h"
#include "../common/PosBlock.h"
#include <cassert>
#include "../common/Interfaces.h"
#include "../Wrappers/LongBasicBlock.h"
#include "../Operators/Operator.h"
#include "../Wrappers/SMultiBlock.h"
#include "../Wrappers/PosArrayBlock.h"

class ValBlockToPosBlockConverter : public Operator {
public:
	ValBlockToPosBlockConverter(Operator* tupleSource);
	virtual ~ValBlockToPosBlockConverter();
	
	// Gets the next position block (bitstring of positions) from the operator
	virtual PosBlock* getNextPosBlock(int colIndex_);
	
	// Gets the next value block from the operator 
	virtual	Block* getNextValBlock(int colIndex_) {
		throw new UnexpectedException("not supported");
	}
	
	// Gets the next string value block from the operator
	// THIS IS UGLY ...FIX LATER
	virtual	SBlock* getNextSValBlock(int colIndex_) {
		throw new UnexpectedException("not supported");
	}
	
	// Gets the maximum position in the data source
	virtual int getLastPosition() {
		throw new UnexpectedException("not supported");
	}
protected:
	static const int OUTBLOCK_CAPACITY = PAGE_SIZE / sizeof(int);
	Operator* tupleSource;
	PosArrayBlock* outPosBlock;
	Block* curInBlock;
};

#endif /*SBLOCKTOPOSBLOCKCONVERTER_H_*/
