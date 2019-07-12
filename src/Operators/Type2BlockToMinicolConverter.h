#ifndef TYPE2BLOCKTOMINICOLCONVERTER_H_
#define TYPE2BLOCKTOMINICOLCONVERTER_H_

#include "MinicolShimOperator.h"
#include "../Wrappers/RLEBlock.h"
#include "../Wrappers/PosType2Block.h"
#include "../common/MinicolBlock.h"
#include "../common/DataSource.h"

class Type2BlockToMinicolConverter : public MinicolShimOperator
{
public:
	Type2BlockToMinicolConverter(DataSource* ds_, const char* name);
	virtual ~Type2BlockToMinicolConverter();
protected:
	virtual int getLastPosition() {
		throw UnexpectedException();
	}

	virtual MinicolBlock* getNextMinicolBlock();
protected:
	void updateMinicolBlock();
	DataSource* ds;
	BlockIter* iters[1]; 
	//char* name;
//	PosType2Block outPosBlock;
//	RLEBlock outValBlock;
//	BlockWithPos outBlockWithPos;
};

#endif /*TYPE2BLOCKTOMINICOLCONVERTER_H_*/
