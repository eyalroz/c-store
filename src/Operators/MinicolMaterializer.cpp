#include "MinicolMaterializer.h"

MinicolMaterializer::MinicolMaterializer(Operator* valSource, DataSource* toMaterialize) 
{
	this->valSource = valSource;
	this->toMaterialize = toMaterialize;
	// Create 2 copies of the 0th output stream of valSource. Copy 0 will be
	// used as our source of values. Copy 1 will be used to position filter
	// the toMaterialize DataSource.
	bCopy = new BCopyOperator(valSource, 0, 2); 
	
	// Set up a value to position stream converter on the bcopy, then use 
	// its #1 output stream (i.e., the bcopy's #1 stream) as a position filter.
	valBlockConverter = new ValBlockToPosBlockConverter(bCopy);
	toMaterialize->setPositionFilter(valBlockConverter, 1); 
	
	// Initialize the minicolBlock
	minicolBlock = new MinicolBlock(2); // 2 columns
	
	// Initialize buffers
	cur_block_size = 0;
	cur_val_bufidx = 0;
	cur_ds_bufidx = 0;
	nval_pos = 0;
	nds_pos = 0;
	valOutputBuffer = new Block*[minicolBlock->getCapacity()];
	dsOutputBuffer = new Block*[minicolBlock->getCapacity()];
	deferredDSBlock = NULL;
	deferredValBlock = NULL;
	//curPosBlock = NULL;
	inputs_exhausted = false;
}

MinicolMaterializer::~MinicolMaterializer()
{
	delete minicolBlock;
}

Block* MinicolMaterializer::getNextValBlock(int colidx) {
	if (minicolBlock == NULL) {
		updateMinicolBlock();
	}
	Block* b = minicolBlock->getNextValBlock(colidx);
	if (b == NULL) {
		updateMinicolBlock();
		if (minicolBlock == NULL) {
			return NULL;
		} else {
			return minicolBlock->getNextValBlock(colidx);
		}
	} else {
		return b;
	}
}

PosBlock* MinicolMaterializer::getNextPosBlock(int colidx) {
	return NULL;
}

void MinicolMaterializer::updateMinicolBlock() {
	minicolBlock->clearBlock();
	cerr << "update" << endl;
	
	while(cur_block_size < (minicolBlock->getCapacity() * 8)) { // bits-per-byte 
		Block* curblock = NULL;
		
		// Get a block from the data source having a fewest number of positions currently loaded
		// in the block and add it in.
		//cerr << "nds_pos = " << nds_pos << " and nval_pos = " << nval_pos << endl;
		if (nds_pos < nval_pos) {
			if (inputs_exhausted) {
				break;
			}
			curblock = getNextDSBlock();
			if (curblock != NULL && canAdd(curblock)) {
				//cerr << "Read from DS block of size " << curblock->getSize() << endl;
				dsOutputBuffer[cur_ds_bufidx++] = curblock;
				nds_pos += curblock->getSize();
				cur_block_size += curblock->getSizeInBits();
			} else {
				if (curblock != NULL) {
					assert(deferredDSBlock == NULL);
					deferredDSBlock = curblock;
				} else {
					inputs_exhausted = true;
				}
				break;
			}
		} else {
			if (inputs_exhausted) {
				break;
			}
			curblock = getNextVBlock();
			if (curblock != NULL && curblock->getSize() > 0 && canAdd(curblock)) {
				//cerr << "Read from val block of size " << curblock->getSize() << endl;
				valOutputBuffer[cur_val_bufidx++] = curblock;
				nval_pos += curblock->getSize();
				cur_block_size += curblock->getSizeInBits();
			} else {
				if (curblock != NULL) {
					assert(deferredValBlock == NULL);
					deferredValBlock = curblock;
				} else {
					inputs_exhausted = true;
				}
				break;
			}
		}
	}
	
	// Now we have as much data in the block as we could manage
	// If one of the columns has more positions than the other, we need to
	// cut it down to size.
	if (nval_pos > nds_pos) {
		cerr << "Need to remove " << (nval_pos - nds_pos) << " positions from last val block" << endl;
		int wantedpos = valOutputBuffer[cur_val_bufidx-1]->getSize() - (nval_pos - nds_pos);
		assert(wantedpos >= 0);
		if (wantedpos > 0) {		
			deferredValBlock = valOutputBuffer[cur_val_bufidx-1]->split(wantedpos);
		} else {
			deferredValBlock = valOutputBuffer[--cur_val_bufidx];
		}
		assert(deferredValBlock->getSize() == (nval_pos - nds_pos));
		nval_pos -= (nval_pos - nds_pos);
		cur_block_size -= deferredValBlock->getSizeInBits();
		
	} else if (nds_pos > nval_pos) {
		cerr << "Need to remove " << (nds_pos - nval_pos) << " positions from last ds block" << endl;
		int wantedpos = dsOutputBuffer[cur_ds_bufidx-1]->getSize() - (nds_pos - nval_pos);
		assert(wantedpos >= 0);
		if (wantedpos > 0) {
			deferredDSBlock = dsOutputBuffer[cur_ds_bufidx-1]->split(wantedpos);
		} else {
			deferredDSBlock = dsOutputBuffer[--cur_ds_bufidx];
		}
		nds_pos -= (nds_pos - nval_pos);
		cur_block_size -= deferredDSBlock->getSizeInBits();
	} else {
		//cerr << "No removals needed; " << nds_pos << " = " << nval_pos << endl;
	}
	
	// Check that we really did manage to get everything equalized
	assert(nds_pos == nval_pos && cur_block_size >= 0);
	
	// Null-terminate arrays
	valOutputBuffer[cur_val_bufidx] = NULL;
	dsOutputBuffer[cur_ds_bufidx] = NULL;
	
	// If we actually have data, then update the block with it
	if (cur_block_size > 0) {
		minicolBlock->setValBlocks(0, valOutputBuffer);
		minicolBlock->setValBlocks(1, dsOutputBuffer);
				
		// FIXME bogus assumed contiguous
		int minpos = valOutputBuffer[0]->peekNext()->position;
		int maxpos = valOutputBuffer[cur_val_bufidx-1]->getEndPosition();
		PosBlock** pbarr = new PosBlock*[1]; // FIXME leak
		pbarr[0] = new PosRLEBlock(new RLETriple(-1, minpos, maxpos-minpos+1));
		minicolBlock->setPosBlockArray(new PosBlockArray(pbarr, 1));
		minicolBlock->resetCursors();
		
		// Reset for the next call
		cur_block_size = 0;
		cur_val_bufidx = 0;
		cur_ds_bufidx = 0;
		nds_pos = 0;
		nval_pos = 0;
		cerr << "---" << endl;
		//curPosBlock = NULL;
	} else {
		delete minicolBlock;
		minicolBlock = NULL;
	}
}


/*
void MinicolMaterializer::updatePosBlock(Block* b1, Block* b2) {
	// RLE case
	if (b1->isPosContiguous() && b1->isBlockPosSorted() && b2->isPosContiguous() && b2->isBlockPosSorted()) {
		assert((b1->getEndPosition() == b2->getEndPosition()) &&
				b1->peekNext()->position == b2->peekNext()->position);
		if (curPosBlock == NULL) {
			curPosBlock = new PosRLEBlock(new RLETriple(-1, b1->peekNext()->position, b1->getSize()));
		} else {
			PosRLEBlock* pblock = (PosRLEBlock*) curPosBlock;
			// If we can fold this position block into the previous one, do that
			if ( pblock->getTriple()->startPos + pblock->getTriple()->reps == b1->peekNext()->position) {
				pblock->getTriple()->reps += b1->getSize();
				cerr << "Extend" << endl;
			}
			
			// If we can't, add the current position block to the minicol block and allocate a new one
			else {
				cerr << "Submit and create" << endl;
				minicolBlock->addPosBlock(curPosBlock);
				curPosBlock = new PosRLEBlock(new RLETriple(-1, b1->peekNext()->position, b1->getSize()));
			}
		}
		return;
	}
	throw new UnexpectedException("unsupported block properties");
}
*/
	
inline Block* MinicolMaterializer::getNextDSBlock() {
	Block* b = NULL;
	if (deferredDSBlock != NULL) {
		b = deferredDSBlock;
		deferredDSBlock = NULL;
	} else { 
		// If nothing on stack, then read from operator
		b = toMaterialize->getNextValBlock(0);
		if (b != NULL) {
			b = b->clone(*b); 
		} else {
			cerr << "Got null DS block from operator" << endl;
		}
	}
	return b;
}

inline Block* MinicolMaterializer::getNextVBlock() {
	Block* b;
	// Try first to fetch from stack
	if (deferredValBlock != NULL) {
		b = deferredValBlock;
		deferredValBlock = NULL;
		//cerr << "\tUse deferred val block" << endl;
	} else {
		// If nothing on stack, then read from operator.
		b = bCopy->getNextValBlock(0);
		if (b != NULL) {
			b = b->clone(*b); 
		}
		//cerr << "\tUse new val block" << endl;
	}
	//cerr << "use block at " << b << " of size: " << b->getSize() << endl;
	return b;
}

inline bool MinicolMaterializer::canAdd(Block* b) const {
//	cerr << "can add with cbs = " << cur_block_size << " and sib = " << b->getSizeInBits() << endl;
	return cur_block_size + b->getSizeInBits() < minicolBlock->getCapacity() * 8; // bits-per-byte
}
