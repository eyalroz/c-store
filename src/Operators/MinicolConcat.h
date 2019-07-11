#ifndef MINICOLCONCAT_H_
#define MINICOLCONCAT_H_

#include "Operator.h"
#include "../common/MinicolBlock.h"
#include "../common/UnexpectedException.h"
#include <stack>
#include <list.h>

class MinicolConcat : public Operator
{
public:
	MinicolConcat(Operator* inputs, int* input_colidxs);
	virtual ~MinicolConcat();
	MinicolBlock* getNextMinicolBlock();
	
protected:
	virtual Block* getNextValBlock(int colidx);
	virtual PosBlock* getNextPosBlock(int colidx);
	virtual int getLastPosition();
//	virtual void updateMinicolBlock();

	Operator* inputs;
	int* input_colidxs;

	Block* curblocks;
	Block** input_block_buffer;
};

#endif /*MINICOLCONCAT_H_*/
