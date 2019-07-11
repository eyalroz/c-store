#ifndef MINICOLCONCATTEST_H_
#define MINICOLCONCATTEST_H_

#include "../UnitTests/UnitTest.h"
#include "../AM/ROSAM.h"
#include "../Operators/Operator.h"
#include "../Operators/BAndOperator.h"
#include "../Operators/MinicolConcat.h"
#include "../Wrappers/RLEDataSource.h"
#include "../Wrappers/IntDataSource.h"
#include "../Operators/MinicolMaterializer.h"
#include "../Operators/BlockWithPosToMinicolConverter.h"
#include "../common/MinicolBlock.h"
#include "../Operators/MinicolShimOperator.h"
#include "../Operators/PosAndOperator.h"

class MinicolConcatTest : public UnitTest
{
public:
	MinicolConcatTest();
	virtual ~MinicolConcatTest();
	virtual bool run(Globals* g, const vector<string>& args);
	IntDataSource* getUncompressedSuppkey();
};

class PassthroughOperator : public Operator {
public:
	PassthroughOperator(Operator* op1_, Operator* op2_) : op1(op1_), op2(op2_) {;}
	
	// Gets the next value block from the operator 
	virtual	Block* getNextValBlock(int colIndex_) {
		op2->getNextValBlock(colIndex_);
		return op1->getNextValBlock(colIndex_);
	}

	// Gets the next position block (bitstring of positions) from the operator
	virtual PosBlock* getNextPosBlock(int colIndex_) {
		op2->getNextPosBlock(colIndex_);
		return op1->getNextPosBlock(colIndex_);
	}
	
	// Gets the maximum position in the data source
	virtual int getLastPosition() { throw UnexpectedException(); }
	
protected:
	Operator* op1;
	Operator* op2;
};
#endif /*MINICOLCONCATTEST_H_*/
