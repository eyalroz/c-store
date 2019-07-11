#include "Type2BlockToMinicolConverter.h"

Type2BlockToMinicolConverter::Type2BlockToMinicolConverter(DataSource* ds_, char* name) : 
										MinicolShimOperator(NULL, -1, NULL, 1, 1), ds(ds_)
{
	this->name = new char[strlen(name) + 1];
	strcpy(this->name, name);
}

Type2BlockToMinicolConverter::~Type2BlockToMinicolConverter()
{
	delete [] name;
}

MinicolBlock* Type2BlockToMinicolConverter::getNextMinicolBlock() {
	BlockWithPos* b = static_cast<BlockWithPos*>(ds->getNextValBlock(0));
	assert(b == NULL || dynamic_cast<BlockWithPos*>(b) != NULL);
	if (b == NULL) {
		cerr << "T2BTMC on " << name << " returning NULL." << endl;
		return NULL;
	}
	//cerr << "Read block b on " << b->getStartPair()->position << " to " << b->getEndPosition() << endl;
	
	iters[0] = b->getIterator();
	assert(iters[0] != NULL);
	assert(b->getPosBlockIter() != NULL);
	//MinicolBlock* mb = new MinicolBlock(iters, 1, static_cast<PosBlockIter*>(b->getPosBlockIter()));
	if (readblock == NULL) {
		readblock = new MinicolBlock(iters, 1, static_cast<PosBlockIter*>(b->getPosBlockIter()));
	} else {
		readblock->reinitBlock(iters, 1, static_cast<PosBlockIter*>(b->getPosBlockIter()));
	}
	
	currblocks[0] = readblock;
	return readblock;
}
 
