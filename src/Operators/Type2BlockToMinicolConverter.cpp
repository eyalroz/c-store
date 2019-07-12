#include "Type2BlockToMinicolConverter.h"

Type2BlockToMinicolConverter::Type2BlockToMinicolConverter(DataSource* ds_, const char* name) :
										MinicolShimOperator(NULL, -1, NULL, 1, 1), ds(ds_)
{
	// Note: The base class also sets a value for name - but does not
	// allocate memory, so it's safe(ish) for us to overwrite the value
	// of `this->name`
	this->name = strdup(name);
}

Type2BlockToMinicolConverter::~Type2BlockToMinicolConverter()
{
	// Note: The base class doesn't free any memory, so leaving `name`
	// with the same value is ok
	free(const_cast<char*>(name));
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
 
