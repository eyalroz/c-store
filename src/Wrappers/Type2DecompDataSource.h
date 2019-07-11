#ifndef TYPE2DECOMPDATASOURCE_H_
#define TYPE2DECOMPDATASOURCE_H_

#include "../common/DataSource.h"
#include "IntDataSource.h"

using namespace std;

class Type2DecompDataSource : public DataSource
{
public:
	Type2DecompDataSource(AM* am_, bool valSorted_, Decoder* valDecoder_);
	virtual ~Type2DecompDataSource();
	
	// Gets the next value block from the operator 
	virtual Block* getNextValBlock(int colIndex_);
	
	// Skips to the first block with this value
	// Returns NULL if the value is outside the range of the column
	virtual Block* skipToValBlock(int colIndex_, int val_) {
		throw UnexpectedException("not implemented");
	}
	
	
	// Gets the next position block (bitstring of positions) from the operator
	virtual PosBlock* getNextPosBlock(int colIndex_);
	
	// Skips to the first block with this position
	// Returns NULL if the value is outside the range of the column
	virtual PosBlock* skipToPosBlock(int colIndex_, int pos_) {
		throw UnexpectedException("not implemented");
	}
	
	bool iterOK(int i);
	bool blockOK(int i);
	
protected:
	enum BlockType {VAL, POS};
	void init_cursors();
	void update_blocks();
	void* getNextXBlock(int colIndex_, BlockType type);
	
	ROSAM* valam;
	AM* am;
	bool valSorted;
	int nvalues;
	byte* cursor_to_val_map;
	unsigned int startpos;
	unsigned int consensus_ep;
	Type2Block** currblocks;
	PosType2Block** currPosBlocks;
	PosType2Block::PosType2BlockIter** iters;
	
	Type2Block outPosBlockBackingBlock;
	PosType2Block outPosBlock;
	MultiBlock* outDataBlock;
	BlockWithPos outBlock;
	byte* mb_buffer;
	unsigned int mb_bufsize;
	bool firstcall;
	int nseen;
	int valsize;
	ValPos** valindex;
	Decoder* valDecoder;
	
};

#endif /*TYPE2DECOMPDATASOURCE_H_*/
