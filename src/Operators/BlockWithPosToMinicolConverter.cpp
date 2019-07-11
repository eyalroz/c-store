#include "BlockWithPosToMinicolConverter.h"

BlockWithPosToMinicolConverter::BlockWithPosToMinicolConverter(DataSource* ds_, char* name) 
									: MinicolShimOperator(NULL, -1, NULL, 1, 1), ds(ds_)//, firstcall(true)
{
	this->name = new char[strlen(name) + 1];
	strcpy(this->name, name);
}

BlockWithPosToMinicolConverter::~BlockWithPosToMinicolConverter()
{
	delete [] name;
}


MinicolBlock* BlockWithPosToMinicolConverter::getNextMinicolBlock() {
//	cerr << "BWPTMC on " << name << ": getNextMinicolBlock" << endl;
	Block* b = ds->getNextValBlock(0);
//	cerr << "BWPTMC converter: update from block at " << b << " range " << b->getStartPair()->position << " to " <<
//				b->getEndPosition() << endl;
	if (b == NULL) {
		//cerr << "BWPTMC on " << name << " returning NULL." << endl;
		return NULL;
	}
		
	BlockWithPos* bwp = NULL;
	// If we need to construct a BlockWithPos
	if (b->isPosContiguous()) {
		assert(!dynamic_cast<BlockWithPos*>(b));
		assert(b->isPosSorted() && b->isPosContiguous());
		PosRLEBlock* prle = new PosRLEBlock(new RLETriple(0, b->getStartPair()->position, 
					b->getEndPosition() - b->getStartPair()->position + 1));
		
		assert(prle->getEndPos() == b->getEndPosition());
		assert(prle->getStartPos() == b->getStartPair()->position);
		assert(prle->getSize() == b->getSize());
		
	//	cerr << "BWPTMC on " << name << ": create new PosRLEBlock from " << b->getStartPair()->position << " to " <<
	//					b->getEndPosition() << " at " << prle << endl;
		bwp = new BlockWithPos();
		bwp->setBlock(b);
		bwp->setPosBlockIter(prle->getIterator());
	} else {
		assert(dynamic_cast<BlockWithPos*>(b) != NULL);
		bwp = static_cast<BlockWithPos*>(b);
	//	cerr << "BWPTMC on " << name << " direct return of " << b->getStartPair()->position << " to " <<
	//			b->getEndPosition() << endl;
	}
	
	iters[0] = bwp->getIterator();
	//cerr << "BWPTMC: set iters[0] = " << iters[0] << endl;
	assert(bwp->getPosBlockIter() != NULL);
	
	if (readblock == NULL) {
		readblock = new MinicolBlock(iters, 1, (PosBlockIter*) bwp->getPosBlockIter());
	} else {
		readblock->reinitBlock(iters, 1, static_cast<PosBlockIter*>(bwp->getPosBlockIter()));
	}
	//cerr << "BWPTMC on " << name << " finished updating; MC block " << " at " << mb << " starts " << mb->getStartPos() <<
	//	" ends: " << mb->getEndPos() << 
	//	" with position block end = " << bwp->getPosBlockIter()->getEndPos() << 
	//	 " block at " << bwp << endl;
	currblocks[0] = readblock;
	return readblock;
}
