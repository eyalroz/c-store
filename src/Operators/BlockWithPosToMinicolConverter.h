#ifndef BLOCKWITHPOSTOMINICOLCONVERTER_H_
#define BLOCKWITHPOSTOMINICOLCONVERTER_H_

#include "../common/MinicolBlock.h"
#include "../common/DataSource.h"
#include "../common/BlockWithPos.h"
#include "../Wrappers/PosRLEBlock.h"
#include "MinicolShimOperator.h"
#include <string.h>

class BlockWithPosToMinicolConverter : public MinicolShimOperator
{
public:
	BlockWithPosToMinicolConverter(DataSource* ds_, char* name);
	~BlockWithPosToMinicolConverter();
	
//	virtual	Block* getNextValBlock(int colIndex_);
//	virtual PosBlock* getNextPosBlock(int colIndex_);
	virtual int getLastPosition() {
		throw UnexpectedException();
	}

	virtual MinicolBlock* getNextMinicolBlock();
protected:
	void updateMinicolBlock();
	DataSource* ds;
	BlockIter* iters[1]; 
	//char* name;
	BlockWithPos outPosBlock;
};

#endif /*BLOCKWITHPOSTOMINICOLCONVERTER_H_*/
