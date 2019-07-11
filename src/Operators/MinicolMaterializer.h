#ifndef MINICOLMATERIALIZER_H_
#define MINICOLMATERIALIZER_H_

#include "Operator.h"
#include "BCopyOperator.h"
#include "ValBlockToPosBlockConverter.h"
#include "../common/UnexpectedException.h"
#include "../common/MinicolBlock.h"
#include "../common/PosBlock.h"
#include "../Wrappers/PosRLEBlock.h"
#include "../Wrappers/RLETriple.h"
#include "../common/PosBlockArray.h"


class MinicolMaterializer : public Operator
{
public:
	MinicolMaterializer(Operator* valSource, DataSource* toMaterialize);
	virtual ~MinicolMaterializer();
	virtual Block* getNextValBlock(int colidx);
	virtual PosBlock* getNextPosBlock(int colidx);
	virtual void updateMinicolBlock();
	virtual int getLastPosition() {
		throw new UnexpectedException("not supported");
	}
	
	
//	virtual void updatePosBlock(Block* b1, Block* b2);
protected:
	Operator* valSource;
	DataSource* toMaterialize;
	BCopyOperator* bCopy;
	ValBlockToPosBlockConverter* valBlockConverter;
	MinicolBlock* minicolBlock;
	Block* curValBlock;
	Block* curDSBlock;
	
	// Buffers into which we assemble the MinicolBlock
	Block** valOutputBuffer;
	int cur_val_bufidx;
	Block** dsOutputBuffer;
	int cur_ds_bufidx;
	
	// Remnants of oversize blocks that we had to split
	Block* deferredValBlock;
	Block* deferredDSBlock;
	
	int cur_block_size;
	int nval_pos;
	int nds_pos;
	
	bool inputs_exhausted;
//	PosBlock* curPosBlock;

	Block* getNextDSBlock();
	Block* getNextVBlock();
	bool canAdd(Block* b) const;
};

#endif /*MINICOLMATERIALIZER_H_*/
