#include "MinicolShimOperator.h"

MinicolShimOperator::MinicolShimOperator(Operator* op_, int op_pos_col_, MinicolShimOperator** children_, int nchildren_, int outwidth_) : 
									op(op_), op_pos_col(op_pos_col_), nchildren(nchildren_), currblocks(NULL), readblock(NULL), outwidth(outwidth_)
{
	if (nchildren > 0) {
		children = new MinicolShimOperator*[nchildren];
		iters = new BlockIter*[outwidth];
		ncurrblocks = 0;
		
		if (children_ != NULL) {
			memcpy(children, children_, sizeof(MinicolSource*) * nchildren);
			for (int i = 0; i < nchildren; ++i) {
				ncurrblocks += children[i]->nchildren;
			}
			currblocks = new MinicolBlock*[ncurrblocks];
			for (int i = 0; i < ncurrblocks; i++) {
				currblocks[i] = NULL;
			}
		} else {
			currblocks = new MinicolBlock*[1]; // Hack for BlockWithPosToMinicolConverter
			currblocks[0] = NULL;
		}
	}
	if (op_)
	  doConcat=false;
	else
	  doConcat=true;
	name = "MSO";
//	for (int i = 0; i < PBB_SIZE; ++i) {
//		posblock_buffer[i] = NULL;
//	}
}

MinicolShimOperator::~MinicolShimOperator()
{
	if (nchildren > 0) {
		delete [] children;
		delete [] currblocks;
		delete [] iters;
	}
}

// Get data from the current minicol block--for normal operators
Block* MinicolShimOperator::getNextValBlock(int colIndex_) {
	// We're out of minicol blocks
	if (readblock == NULL) {
		readblock = getNextMinicolBlock();
	}
	if (readblock == NULL) {
		return NULL;
	}
	assert(colIndex_ < readblock->ncols);
	
	// See if we have anything in the current MC block
	assert(colIndex_ < readblock->getNumCols());
	Block* b = readblock->getNextValBlock(colIndex_);
	if (b == NULL) {
		readblock = getNextMinicolBlock();
//		cerr << "MSO: Read NULL from readblock; using next return" << endl;
		return readblock == NULL ? NULL : readblock->getNextValBlock(colIndex_);
	} 
//	cerr << "MSO: Returning val from current read block = " << readblock << ": " << b << endl;
	return b;
}

// Get data from the current minicol block--for normal operators
PosBlock* MinicolShimOperator::getNextPosBlock(int colIndex_) {
	assert(colIndex_ == 0); // Minicol blocks only have one position stream

	// We're out of minicol blocks
	if (readblock == NULL) {
		readblock = getNextMinicolBlock();
	}
	if (readblock == NULL) {
		return NULL;
	}
	
	// See if we have anything in the current MC block
	assert(colIndex_ < readblock->getNumCols());
	PosBlock* b = readblock->getNextPosBlock();
	if (b == NULL) {
		readblock = getNextMinicolBlock();
		return readblock == NULL ? NULL : readblock->getNextPosBlock();
	} 
//	cerr << "MSO: returning pos from current read block = " << readblock << ": " << b << endl;
	return b;

}

// Get the current minicol block--used by a shim operator that has us as a child to
// fetch values to copy around the operator interposed between us. It needs to call this
// function before starting to pull values from the interposed operator, because reading values
// may cause the operator to cause the child minicol source to update its minicol block, thus losing
// the minicol block that we wanted.
MinicolBlock* MinicolShimOperator::getNextMinicolBlock() {
		
  //unsigned int old_readblock_endpos = readblock == NULL ? 0 : readblock->getEndPos();
		
	PosBlock* pb;
	//cerr << "Call op->getNextPosBlock" << endl;
	
	if (doConcat) {
	  currblocks[0] = children[0]->getNextMinicolBlock();
	  if (currblocks[0] == NULL)
	    return NULL;
	  pb = currblocks[0]->positions;
	}
	else
	  pb = op->getNextPosBlock(0);
	//cerr << "Done call op->getNextPosBlock" << endl;
	if (pb == NULL) {
		return NULL;
	}
	//cerr << "MSO: Read from op posblock at " << pb << " size " << pb->getSize() << " from " << pb->getStartPos() << " to " << pb->getEndPos() << endl;
	
//	if (currblocks[0] == NULL || old_readblock_endpos == currblocks[0]->getEndPos()) {
		//cerr << "MSO in currblocks update loop" << endl;
		int idx = 0;
		for (int i = 0; i < nchildren; ++i) {
			int nchild_mcbs = children[i]->nchildren;
			for (int j = 0; j < nchild_mcbs; ++j) {
				currblocks[idx] = children[i]->currblocks[j];
				assert(currblocks[idx] != NULL);
			//	cerr << "Update currblocks[" << idx << "] (" << children[i]->name << ") = " << currblocks[idx] << " to block starting at " <<
			//		currblocks[idx]->getStartPos() << " and ending at " << currblocks[idx]->getEndPos() << endl;
				assert(currblocks[idx] != NULL);
				//assert(idx > 0 ? (currblocks[idx]->getEndPos() == currblocks[idx-1]->getEndPos()) : true);
				
				// Current block needs to be a superset of position block
				assert(currblocks[idx]->getEndPos() >= pb->getEndPos());
				assert(currblocks[idx]->getStartPos() <= pb->getStartPos());
				++idx;
			}
		}
		assert(idx == ncurrblocks);
//	} else {
//		cerr << "Not updating; readblock endpos = " << readblock->getEndPos() << " and currblock endpos = "
//				<< currblocks[0]->getEndPos() << endl;
//	}
	

	// Make sure the position block doesn't go past the end of the current minicol block. FIXME check all blocks.
//	if (pb->getEndPos() > currblocks[0]->getEndPos()) {
//		cerr << "Fatal: pb end pos = " << pb->getEndPos() << " is greater than currblock end pos : " << currblocks[0]->getEndPos() << endl;
//		throw UnexpectedException();
//	}
	
	// Build a PosBlockArray using it
	//PosBlock** pblocks = new PosBlock*[1];
	//pblocks[0] = pb;
	//PosBlockArray* pba = new PosBlockArray(pblocks, 1);
	
	// Create an array of BlockIters from the BlockIters in currblocks
	/*int*/ idx = 0;
	for (int i = 0; i < ncurrblocks; ++i) {
		for (int j = 0; j < currblocks[i]->getNumCols(); ++j) {
			//int s = currblocks[i]->iters[j]->getBlock()->peekNext()->position;
			//int e = currblocks[i]->iters[j]->getBlock()->getEndPosition();
	//		cerr << "MSO: currblock update using positions from " << pb->getStartPos() << " to " << pb->getEndPos() << 
	//				"] and data block " << currblocks[i]->iters[j]->getBlock() << " on [" << s << "," << e << "]" << endl;
			
			// DSM don't do this anymore b/c we pass a PosBlockIterator to the getIterator method (!)
			//((BlockWithPos*)((BlockWithPos::BlockWithPosIter*)currblocks[i]->iters[j])->getBlock())->setPosBlockIter(pb->getIterator());
		  int startpos = pb->getStartPos();
		  int endpos = pb->getEndPos();
			iters[idx] = ((BlockWithPos*)currblocks[i]->iters[j]->getBlock())->getIterator(pb->getPosBlock() == pb? pb->getIterator() : (PosBlockIter*)pb->getPosBlock()->getIterator(startpos, endpos));
			
			//iters[idx] = ((BlockWithPos::BlockWithPosIter*)currblocks[i]->iters[j])->getBlock()->getIterator(pb->getStartPos(), pb->getEndPos());
			//cerr << "produce iter[" << idx << "] on " << currblocks[i]->iters[j]->getBlock() << " from " << pb->getStartPos() << " to " << pb->getEndPos() << endl;
			++idx;
		}
	}
	assert(idx == outwidth);
	
	
	//cerr << "MSO: done updating MCB; returning MCB ending at " << pb->getEndPos() << endl;
	
	// Make a minicol block out of the lot of it
	return new MinicolBlock(iters, outwidth, pb->getIterator());
}

