#ifndef MINICOLBLOCK_H_
#define MINICOLBLOCK_H_

#include "Block.h"
#include "PosBlock.h"
#include "Constants.h" // for PAGE_SIZE
#include "BlockWithPos.h"
#include <list>
#include "../Operators/MinicolShimOperator.h"

/** An class representing a series of positions and corresponding values from one or
 * more columns (author: DSM). This class is used in the minicolumn representation of early-
 * materialized results. 
 * 
 * MinicolBlocks have a set maximum size in memory, which is currently based on PAGE_SIZE, 
 * which is currently set to 65,000 bytes. In general, however, it will not be possible to
 * size blocks exactly to this maximum size; this task is further complicated by the fact that
 * values from different columns may be represented using different encodings.  
 */
class MinicolBlock
{
	
	/* The positions */
	PosBlockIter* positions;
	
	BlockIter** iters;
	int ncols;
	bool posblock_fetched;
	bool* valblock_fetched;
	Block** cloned_blocks;
	
public:
	MinicolBlock(BlockIter** iters_, int ncols_, PosBlockIter* positions_, Block** cloned_blocks_ = NULL);
	virtual ~MinicolBlock();

	Block* getNextValBlock(int colidx);
	PosBlock* getNextPosBlock();

	//void setPosBlockArray(PosBlockArray*);
	//PosBlockArray* getPosBlockArray();
	unsigned int getStartPos() const;
	unsigned int getEndPos() const;
	void reinitBlock(BlockIter** iters_, int ncols_, PosBlockIter* positions_);
	MinicolBlock* clone() const;
	
	int getNumCols() const {
		return ncols;
	}
	friend class MinicolShimOperator;
};

class MinicolSource {
public:
  virtual ~MinicolSource(){};
  virtual MinicolBlock* getCurrentMinicolBlock() = 0;
};

#endif /*MINICOLBLOCK_H_*/
