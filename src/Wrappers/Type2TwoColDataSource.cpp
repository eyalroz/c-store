#include "Type2TwoColDataSource.h"

Type2TwoColDataSource::Type2TwoColDataSource(AM* am_, bool valSorted_, const string& valfile_) 
	: DataSource(am_), am(am_), valSorted(valSorted), startpos(1), consensus_ep(0), outPosBlockBackingBlock(valSorted_),
	outDataBlock(valSorted_, true, true), mb_buffer(NULL), mb_bufsize(0), firstcall(true), 
	valfile(valfile_), curPosBlock(NULL), curpos_end(0), curValBlock(NULL)
{
	cursor_to_val_map = NULL;
	currblocks = NULL;
	consensus_ep = 0;
	outPosBlock.setBlock(&outPosBlockBackingBlock);
	nseen = 0;
}

Type2TwoColDataSource::~Type2TwoColDataSource()
{
	delete [] cursor_to_val_map;
	for (int i = 0; i < nvalues; ++i) {
		delete currblocks[i];
		delete currPosBlocks[i];
	}
	delete [] currblocks;
	delete [] currPosBlocks;
}

void Type2TwoColDataSource::init_cursors() {
	string tableName=am->getTableName();
	ostringstream nameStream;
	nameStream << tableName << "All";
	
	// Open a DS on the file containing the list of distinct values in the column
	IntDataSource* valDataSource=new IntDataSource(am, true, true);
	valDataSource->setSingles(true);
	valam = new ROSAM(nameStream.str(), -1);
	valfileam = new ROSAM(valfile, 1);
	valfileds = new IntDataSource(valfileam, true, true);
	// Initialize all data members that depend on the number of
	// distinct values in the column.	
	nvalues = valDataSource->getLastPosition(); // Number of distinct values
	valam->setNumCursors(nvalues);
	cursor_to_val_map = new int[nvalues];
	currblocks = new Type2Block*[nvalues];
	currPosBlocks = new PosType2Block*[nvalues];
	iters = new PosType2Block::PosType2BlockIter*[nvalues];
	for (int i = 0; i < nvalues; i++) {
		cursor_to_val_map[i] = -1;
		currblocks[i] = NULL;
		currPosBlocks[i] = NULL;
		iters[i] = NULL;
	}
	
	// Read the distinct values, getting the first block for each value as we go
	Block* valBlock;
	int i = 0;
	while ( (valBlock = valDataSource->getNextValBlock(0)) ) {
		int value=valBlock->getNext()->value;
		
		// Fetch if predicate is NULL or non-NULL but evaluates to true
		bool should_fetch = true;
		if (pred != NULL) {
			pred->setLHS(value);
			if (!pred->evalPredicate()) {
				should_fetch = false;
			}
		}
		
		if (should_fetch) {
			// Read the page for this value from disk. getCursorIndex *sets* its second
			// argument to be the internal index of the cursor it's returning!
			int cursoridx = -1;
			byte* page=(byte*)valam->getCursorIndex(value, cursoridx);
			assert(cursoridx == i);
			assert(page != NULL);
			cursor_to_val_map[cursoridx] = value;
	
			// getCursorIndex also gives us back the page corresponding to the first block for
			// the given value, so we'll update the current blocks array, too.
			currblocks[cursoridx]=new Type2Block(valSorted);
			currblocks[cursoridx]->setBufferWithHeader(page);
			
			currPosBlocks[cursoridx] = new PosType2Block();
			currPosBlocks[cursoridx]->setBlock(currblocks[cursoridx]);
			//cerr << "Loaded block for value " << value << " to cursoridx = " << cursoridx << endl;
		} 
		++i;
	}
	assert(i == nvalues);
}

void Type2TwoColDataSource::update_blocks() {
	for (int i = 0; i < nvalues; ++i) {
		if (currblocks[i] != NULL && currblocks[i]->getEndPosition() == consensus_ep) {
			byte* page= (byte*) valam->getNextPagePrimaryDup(i); // i is the cursor index
			if (page != NULL) {
				currblocks[i]->setBufferWithHeader(page);	// DSM does this leak memory from previous call?
				currPosBlocks[i]->setBlock(currblocks[i]);
				iters[i] = NULL;
		//		cerr << "Loaded new block in slot " << i << " from " << currPosBlocks[i]->getStartPos() <<
			//		" to " << currPosBlocks[i]->getEndPos() << endl;
			} else {
				currblocks[i] = NULL;
				currPosBlocks[i] = NULL;
				iters[i] = NULL;
			}
		}
	}
}

void* Type2TwoColDataSource::getNextXBlock(int colIndex_, BlockType type) {
	if (firstcall) {
		init_cursors();
		firstcall = false;
	}
	update_blocks();
	
	// Calculate the consensus end position--this finds the first non-null block to which
	// we initialize consensus_ep
	int consensus_idx = -1;
	for (int i = 0; i < nvalues; ++i) {
		if (currblocks[i] != NULL) {
			consensus_ep = currblocks[i]->getEndPosition();
			consensus_idx = i;
			break;
		}
	}
	
	// All blocks were NULL
	if (consensus_idx == -1) {
//		cerr << "Total seen in DS was " << nseen << endl;
		return NULL;
	} 
	
	// Search through the rest of the blocks to see if there's a better consensus_ep
	for (int i = consensus_idx; i < nvalues; ++i) {
		if (currblocks[i] != NULL) {
			unsigned int ep = currblocks[i]->getEndPosition();
			if (ep < consensus_ep) {
				consensus_ep = ep;
				consensus_idx = i;
			}
		}
	}
	
	// Create iterators on these positions
	for (int i = 0; i < nvalues; ++i) {				
		if (currPosBlocks[i] != NULL && currPosBlocks[i]->getStartPos() <= consensus_ep) {
			iters[i] = static_cast<PosType2Block::PosType2BlockIter*>(currPosBlocks[i]->getIterator(startpos, consensus_ep));
		} 
	}
	
	int first_or_idx = -1;	
	if (pred != NULL) {
		// Or the bitstrings together. First, we find the first non-NULL block that
		// isn't the consensus block (block with the shortest end position)
		for (int i = 0; i < nvalues; ++i) {
			if (iterOK(i) && i != consensus_idx) {
				first_or_idx = i;
				break;
			}
		}
	
		// If we could find one, then we do ORing. Otherwise, we just keep the consensus block.
		if (first_or_idx != -1) {
			iters[consensus_idx]->posOr(iters[first_or_idx], &outPosBlock);
			//PosType2Block::PosType2BlockIter* outIter = static_cast<PosType2Block::PosType2BlockIter*>(outPosBlock.getIterator(startpos, consensus_ep));	
			PosType2Block::PosType2BlockIter* outIter = static_cast<PosType2Block::PosType2BlockIter*>(outPosBlock.getIterator());	

			#ifndef NDEBUG
			assert(outPosBlock.getSize() == iters[consensus_idx]->getSize() + iters[first_or_idx]->getSize());
			assert(outIter->getSize() == outPosBlock.getSize());
			int old_out_sz = outPosBlock.getSize();
			#endif
			
			for (int i = first_or_idx + 1; i < nvalues; ++i) {
				if (i != consensus_idx && blockOK(i) && iterOK(i)) {
					outIter->posOr(iters[i], &outPosBlock);
					delete outIter;
					outIter = static_cast<PosType2Block::PosType2BlockIter*>(outPosBlock.getIterator());	
			
					#ifndef NDEBUG		
					assert(old_out_sz == outPosBlock.getSize() - iters[i]->getSize());
					old_out_sz = outPosBlock.getSize();
					nseen += iters[i]->getSize();
					#endif
				}
			}
		}
	}
	
	// This is as far as we need to go for a position block
	if (type == POS) {
		if (first_or_idx != -1) {
			return (void*) outPosBlock.getIterator(startpos, consensus_ep);
		} else {
			return (void*) iters[consensus_idx];
		}
	}
	
	// If we get to here, then we know we're dealing with a value block
	assert(type == VAL);
		
	// Make sure the multiblock has a large enough buffer
	if (mb_bufsize < (consensus_ep - startpos + 1)) {
		delete [] mb_buffer;
		mb_buffer = new int[consensus_ep - startpos + 1];
		mb_bufsize = consensus_ep - startpos + 1;
	}
		
	// Now, update the multiblock with the right values given these positions
	for (int i = 0; i < nvalues; ++i) {
		if (blockOK(i) && iterOK(i)) {
			PosBlockIter* pbi = currPosBlocks[i]->getIterator(startpos, consensus_ep);
			while(pbi->hasNext()) {
				int pos = pbi->getNext();
				mb_buffer[pos - startpos] = cursor_to_val_map[i];
			}
		}
	}
	outDataBlock.setBufferDirect(startpos, consensus_ep - startpos + 1, (byte*) mb_buffer);
	
	// Put the right values into the BlockWithPos
	outBlock.setBlock(&outDataBlock);

	// If we actually did oring (had multiple blocks ready)
	if (first_or_idx != -1) {
		outBlock.setPosBlockIter(outPosBlock.getIterator(startpos, consensus_ep));
	} else { 	// We only had one block ready and didn't actually or anything
		#ifndef NDEBUG
		nseen += iters[consensus_idx]->getSize();
		#endif
		outBlock.setPosBlockIter(iters[consensus_idx]);
	}
	startpos = consensus_ep + 1;
	if (pred != NULL) {
		return (void*) &outBlock;
	} else {
		return (void*) &outDataBlock;
	}
}			

Block* Type2TwoColDataSource::getNextValBlock(int colIndex_) {
	if (curPosBlock == NULL || curpos_end == curPosBlock->getEndPos()) {
		curPosBlock = static_cast<PosBlock*>(getNextXBlock(colIndex_, POS));
	}
	
	while (curValBlock == NULL || curpos_end == curValBlock->getEndPosition() 
		|| curValBlock->getStartPair()->position < curPosBlock->getStartPos() ) {
		curValBlock = valfileds->getNextValBlock(0);
	}
	
	if (curPosBlock == NULL || curValBlock == NULL) {
		return NULL;
	}
	int consensus_ep = curPosBlock->getEndPos() <? curValBlock->getEndPosition();
	
	//BlockIter* bi = curValBlock->getIterator(curpval_end + 1, consensus_ep);
	PosBlockIter* pbi = curPosBlock->getIterator(curpos_end + 1, consensus_ep);
	outBlock.setBlock(curValBlock);
	outBlock.setPosBlockIter(pbi);
	curpos_end = consensus_ep;
	return &outBlock;
}

PosBlock* Type2TwoColDataSource::getNextPosBlock(int colIndex_) {
	//BlockWithPos* b = static_cast<BlockWithPos*>(getNextValBlock(colIndex_));
	//return b == NULL ? NULL : b->getPosBlockIter();
	throw UnexpectedException("unsupported");
//	return static_cast<PosBlock*>(getNextXBlock(colIndex_, POS));
}
	

bool Type2TwoColDataSource::iterOK(int i) {
	return iters[i] != NULL && iters[i]->getEndPos() != 0;
}

bool Type2TwoColDataSource::blockOK(int i) {
	return currPosBlocks[i] != NULL && currPosBlocks[i]->getStartPos() <= consensus_ep;
}
