#include "ValBlockToPosBlockConverter.h"

ValBlockToPosBlockConverter::ValBlockToPosBlockConverter(Operator* tupleSource)
{
	this->tupleSource = tupleSource;
	this->outPosBlock = new PosArrayBlock(OUTBLOCK_CAPACITY);
	curInBlock = NULL;
}

ValBlockToPosBlockConverter::~ValBlockToPosBlockConverter()
{
	delete outPosBlock;
}

// Gets the next position block (bitstring of positions) from the operator
PosBlock* ValBlockToPosBlockConverter::getNextPosBlock(int colIndex_) {
  //DNA: This will need some speed optimizations eventually ...

	outPosBlock->resetBlock();
	int i = 0;
	
	// Read anything left in a block from a previous call
	if (curInBlock != NULL && curInBlock->hasNext()) {
		while(curInBlock->hasNext() && i < OUTBLOCK_CAPACITY) {
			outPosBlock->setPosAtLoc(i, curInBlock->getNext()->position);
			i++;
		}
	}
	// Get a new one
	if (i < OUTBLOCK_CAPACITY) { // must have exhausted curInBlock
		curInBlock = tupleSource->getNextValBlock(colIndex_);
	}
	
	// Read as much as popssible of it
	if (curInBlock != NULL) { 
		while(curInBlock->hasNext() && i < OUTBLOCK_CAPACITY) {
			outPosBlock->setPosAtLoc(i, curInBlock->getNext()->position);
			i++;
		}
	} 
	
	// Empty block
	if (i == 0) {
		return NULL;
	}
	
	outPosBlock->setNumElements(i);
	//cerr << "ValToPosConverter returning block of " << i << " positons" << endl;
	return outPosBlock;
}
