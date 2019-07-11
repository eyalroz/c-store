#include "MinicolBlock.h"

// Creator's resp. to ensure iters' blocks already use positions as their PosBlockArray
//MinicolBlock::MinicolBlock(BlockIter** iters_, int ncols_, PosBlockIter* positions_) : positions(positions_), 
//							iters(new BlockIter*[ncols_]), ncols(ncols_), posblock_fetched(false),
//							valblock_fetched(new bool[ncols_])
MinicolBlock::MinicolBlock(BlockIter** iters_, int ncols_, PosBlockIter* positions_, Block** cloned_blocks_) : positions(positions_), 
							iters(NULL), ncols(ncols_), posblock_fetched(false),
							valblock_fetched(new bool[ncols]), cloned_blocks(cloned_blocks_)
{
	reinitBlock(iters_, ncols_, positions_);
}
void MinicolBlock::reinitBlock(BlockIter** iters_, int ncols_, PosBlockIter* positions_) {
	
	bool isCtorCall = (iters == NULL);
	assert(isCtorCall || ncols == ncols_); // Either is constructor or reiniting w/same # cols
	
	iters = iters_;
	ncols = ncols_;
	positions = positions_;
	posblock_fetched = false;

	assert(positions_ != NULL);
	
	for (int i = 0; i < ncols; i++) {
		//iters[i] = iters_[i]; 
		//cerr << "MinicolBlock: store iter: " << iters[i] << endl;
		valblock_fetched[i] = false;
		
		//Block* b = ((BlockWithPos::BlockWithPosIter*)iters[i])->getBlock();
		//assert(b->getStartPair()->position <= positions->getStartPos());
		//assert(b->getEndPosition() >= positions->getEndPos());
		//((BlockWithPos*)((BlockWithPos::BlockWithPosIter*)iters[i])->getBlock())->setPosBlockIter((PosBlockIter*) positions->clone());
	}
}

MinicolBlock::~MinicolBlock()
{
	delete [] iters;
	delete [] cloned_blocks;
}


PosBlock* MinicolBlock::getNextPosBlock() {
	assert(positions != NULL);
	if (posblock_fetched) {
		return NULL;
	}
	posblock_fetched = true;
//	cerr << "MCB: return posblock from " << positions->getStartPos() << " to " << positions->getEndPos() << endl;
	return positions;
}

Block* MinicolBlock::getNextValBlock(int colIdx_) {
	if (valblock_fetched[colIdx_]) {
		return NULL;
	}
	valblock_fetched[colIdx_] = true;
	return iters[colIdx_];
}

/*
// Update the PosBlockArray and change all blocks to use it as their position array
void MinicolBlock::setPosBlockArray(PosBlockArray* pab) {
	positions = pab;
	for (int i = 0; i < ncols; ++i) {
		BlockWithPos* bwp = (BlockWithPos*) iters[i]->getBlock();
		bwp->setPosBlockArray(pab);
	
	}
}

PosBlockArray* MinicolBlock::getPosBlockArray() {
	return positions;
}*/

unsigned int MinicolBlock::getStartPos() const {
	return positions->getStartPos();
}
unsigned int MinicolBlock::getEndPos() const {
//	cerr << "MCB at " << this << " getting endpos from " << positions << endl;
	return positions->getEndPos();
}

MinicolBlock* MinicolBlock::clone() const {
	Block** cloned_blocks = new Block*[ncols];
	BlockIter** newiters = new BlockIter*[ncols];
	
	// Clone the blocks and iterators
	for (int i = 0; i < ncols; ++i) {
		cloned_blocks[i] = iters[i]->getBlock()->clone(*(iters[i]->getBlock()));
		newiters[i] = cloned_blocks[i]->getIterator(iters[i]->getStartPair()->position, iters[i]->getEndPosition());
	}
	// Clone the positions
	PosBlockIter* newpositions = positions->getPosBlock()->getIterator(positions->getStartPos(), positions->getEndPos());
	return new MinicolBlock(newiters, ncols, newpositions, cloned_blocks);
}
