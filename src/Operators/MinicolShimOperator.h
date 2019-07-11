#ifndef MINICOLSHIMOPERATOR_H_
#define MINICOLSHIMOPERATOR_H_

#include "Operator.h"
#include "../common/MinicolBlock.h"
//#include "BlockWithPosToMinicolConverter.h"

class MinicolBlock;

class MinicolShimOperator : public Operator
{
public:
	MinicolShimOperator(Operator* op_, int op_pos_col_, MinicolShimOperator** children_, int nchildren_, int outwidth_);
	virtual ~MinicolShimOperator();
	virtual	Block* getNextValBlock(int colIndex_);
	virtual PosBlock* getNextPosBlock(int colIndex_);
	virtual	SBlock* getNextSValBlock(int colIndex_){ throw UnexpectedException("unsupported"); }
	virtual int getLastPosition() { throw UnexpectedException("unsupported"); }
	
	virtual MinicolBlock* getNextMinicolBlock();
protected:
	Operator* op;
	int op_pos_col;
	MinicolShimOperator** children;
	int nchildren;
	MinicolBlock** currblocks;
	BlockIter** iters;
	MinicolBlock* readblock;
	int outwidth;
	int ncurrblocks;
	//virtual void updateMinicolBlock();
	char* name;
	bool doConcat;
private:
	MinicolShimOperator(const MinicolShimOperator&) {throw UnexpectedException();}
};

#endif /*MINICOLSHIMOPERATOR_H_*/
