#ifndef Type2TwoColDataSource_H_
#define Type2TwoColDataSource_H_

#include "../common/DataSource.h"
#include "IntDataSource.h"

using namespace std;

class Type2TwoColDataSource : public DataSource
{
public:
	Type2TwoColDataSource(AM* am_, bool valSorted_, const string& valfile_);
	virtual ~Type2TwoColDataSource();
	
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
	ROSAM* valfileam;
	IntDataSource* valfileds;
	AM* am;
	bool valSorted;
	int nvalues;
	int* cursor_to_val_map;
	unsigned int startpos;
	unsigned int consensus_ep;
	Type2Block** currblocks;
	PosType2Block** currPosBlocks;
	PosType2Block::PosType2BlockIter** iters;
	
	Type2Block outPosBlockBackingBlock;
	PosType2Block outPosBlock;
	MultiBlock outDataBlock;
	BlockWithPos outBlock;
	int* mb_buffer;
	unsigned int mb_bufsize;
	bool firstcall;
	int nseen;
	
	string valfile;
	PosBlock* curPosBlock;
	unsigned int curpos_end;
	Block* curValBlock;
	
};

#endif /*Type2TwoColDataSource_H_*/
