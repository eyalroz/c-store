#include "Type2DecompDataSource.h"

Type2DecompDataSource::Type2DecompDataSource(AM* am_, bool valSorted_, Decoder* valDecoder_) 
	: DataSource(am_), am(am_), valSorted(valSorted), startpos(1), consensus_ep(0), outPosBlockBackingBlock(valSorted_), mb_buffer(NULL), mb_bufsize(0), firstcall(true)
{
	cursor_to_val_map = NULL;
	currblocks = NULL;
	consensus_ep = 0;
	outPosBlock.setBlock(&outPosBlockBackingBlock);
	nseen = 0;
	valsize = 0;
	valindex = NULL;
	valDecoder = valDecoder_;
	outDataBlock = NULL;
}

Type2DecompDataSource::~Type2DecompDataSource()
{
	delete [] cursor_to_val_map;
	for (int i = 0; i < nvalues; ++i) {
		delete currblocks[i];
		delete currPosBlocks[i];
	}
	delete [] currblocks;
	delete [] currPosBlocks;
	delete outDataBlock;
}

void Type2DecompDataSource::init_cursors() {
	string tableName=am->getTableName();

	ROSAM* rosam = new ROSAM(tableName, 1);
	DataSource* tempDS = new DataSource(rosam, true, false, valDecoder);
	valindex = tempDS->createValueIndex();
	delete tempDS;
	delete rosam;
	valsize = 4;
	if (valindex)
	  valsize = valindex[0]->getSize();

	ostringstream nameStream;
	nameStream << tableName << "All";
	
	// Open a DS on the file containing the list of distinct values in the column
	DataSource* valDataSource=new DataSource(am, true, false, valDecoder);
	//valDataSource->setSingles(true);
	if (valindex && valindex[0]) {
	  valam = new ROSAM(nameStream.str(), -1, valsize, sizeof(int), valindex[0]->type, ValPos::INTTYPE);
	  outDataBlock = new MultiBlock(valSorted, true, true, valindex[0]);
	}
	else {
	  valam = new ROSAM(nameStream.str(), -1, sizeof(int), sizeof(int), ValPos::INTTYPE, ValPos::INTTYPE);
	  outDataBlock = new MultiBlock(valSorted, true, true);
	}
       

	// Initialize all data members that depend on the number of
	// distinct values in the column.	
	nvalues = valDataSource->getLastPosition(); // Number of distinct values
	valam->setNumCursors(nvalues);
	cursor_to_val_map = new byte[nvalues*valsize];
	memset(cursor_to_val_map, 0, nvalues*valsize);
	currblocks = new Type2Block*[nvalues];
	currPosBlocks = new PosType2Block*[nvalues];
	iters = new PosType2Block::PosType2BlockIter*[nvalues];
	for (int i = 0; i < nvalues; i++) {
	  //cursor_to_val_map[i] = -1;
		currblocks[i] = NULL;
		currPosBlocks[i] = NULL;
		iters[i] = NULL;
	}
	
	// Read the distinct values, getting the first block for each value as we go
	Block* valBlock = valDataSource->getNextValBlock(0);
	int i = 0;
	while ( valBlock ) {
	  if (!valBlock->hasNext()) {
	    valBlock = valDataSource->getNextValBlock(0);
	    continue;
	  } 
	  ValPos* vp = valBlock->getNext();
	  //assert(vp->type == ValPos::INTTYPE);
	  //int value=*(int*)vp->value;
		
		// Fetch if predicate is NULL or non-NULL but evaluates to true
		bool should_fetch = true;
		if (pred != NULL) {
			pred->setLHS(vp);
			if (!pred->evalPredicate()) {
				should_fetch = false;
			}
		}
		
		if (should_fetch) {
			// Read the page for this value from disk. getCursorIndex *sets* its second
			// argument to be the internal index of the cursor it's returning!
			int cursoridx = -1;
			byte* page=(byte*)valam->getCursorIndex((char*)vp->value, cursoridx);
			//assert(cursoridx == i); not true if predicate
			assert(page != NULL);
			memcpy(cursor_to_val_map+(cursoridx*valsize), vp->value, valsize);
			//cursor_to_val_map[cursoridx] = value;
	
			// getCursorIndex also gives us back the page corresponding to the first block for
			// the given value, so we'll update the current blocks array, too.
			currblocks[cursoridx]=new Type2Block(valSorted);
			currblocks[cursoridx]->setBufferWithHeader(page);
			if (valindex && valindex[0])
			  currblocks[cursoridx]->setValue(valindex);
			
			currPosBlocks[cursoridx] = new PosType2Block();
			currPosBlocks[cursoridx]->setBlock(currblocks[cursoridx]);
			//cerr << "Loaded block for value " << value << " to cursoridx = " << cursoridx << endl;
		} 
		++i;
	}
	assert(i == nvalues);
}

void Type2DecompDataSource::update_blocks() {
	for (int i = 0; i < nvalues; ++i) {
		if (currblocks[i] != NULL && currblocks[i]->getEndPosition() == consensus_ep) {
			byte* page= (byte*) valam->getNextPagePrimaryDup(i); // i is the cursor index
			if (page != NULL) {
				currblocks[i]->setBufferWithHeader(page);	// DSM does this leak memory from previous call?
				if (valindex && valindex[0])
				  currblocks[i]->setValue(valindex);
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

void* Type2DecompDataSource::getNextXBlock(int colIndex_, BlockType type) {
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
		mb_buffer = new byte[(consensus_ep - startpos + 1)*valsize];
		mb_bufsize = consensus_ep - startpos + 1;
	}
		
	// Now, update the multiblock with the right values given these positions
	for (int i = 0; i < nvalues; ++i) {
		if (blockOK(i) && iterOK(i)) {
			PosBlockIter* pbi = currPosBlocks[i]->getIterator(startpos, consensus_ep);
			while(pbi->hasNext()) {
				int pos = pbi->getNext();
				memcpy(mb_buffer+((pos-startpos)*valsize), cursor_to_val_map + (i*valsize), valsize);
				//mb_buffer[pos - startpos] = cursor_to_val_map[i];
			}
		}
	}
	outDataBlock->setBufferDirect(startpos, consensus_ep - startpos + 1, mb_buffer);
	
	// Put the right values into the BlockWithPos
	outBlock.setBlock(outDataBlock);

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
		return (void*) outDataBlock;
	}
}			

Block* Type2DecompDataSource::getNextValBlock(int colIndex_) {
	return static_cast<Block*>(getNextXBlock(colIndex_, VAL));
}

PosBlock* Type2DecompDataSource::getNextPosBlock(int colIndex_) {
	//BlockWithPos* b = static_cast<BlockWithPos*>(getNextValBlock(colIndex_));
	//return b == NULL ? NULL : b->getPosBlockIter();
	return static_cast<PosBlock*>(getNextXBlock(colIndex_, POS));
}
	

bool Type2DecompDataSource::iterOK(int i) {
	return iters[i] != NULL && iters[i]->getEndPos() != 0;
}

bool Type2DecompDataSource::blockOK(int i) {
	return currPosBlocks[i] != NULL && currPosBlocks[i]->getStartPos() <= consensus_ep;
}
