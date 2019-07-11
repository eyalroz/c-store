#include "Concat.h"

Concat::Concat(Operator* inputs[], int ninputs, int numtouse)
{
	this->inputs = inputs;
	this->ninputs = ninputs;
	this->numtouse = numtouse;
	this->inputBlocks = new Block*[ninputs];
	for (int i = 0; i < ninputs; i++) {
		inputBlocks[i] = NULL;
	}
	this->sb = new SMultiBlock(BLOCKSIZE, numtouse*sizeof(int));
}

Concat::Concat(Operator* input) {
	this->inputs = new Operator*[1];
	inputs[0] = input;
	this->ninputs = 1;
	this->numtouse = 1;
	this->inputBlocks = new Block*[ninputs];
	for (int i = 0; i < ninputs; i++) {
		inputBlocks[i] = NULL;
	}
	this->sb = new SMultiBlock(BLOCKSIZE, numtouse*sizeof(int));
}
	

Concat::~Concat()
{
	delete [] inputBlocks;
}

// Gets the next value block from the operator 
Block* Concat::getNextValBlock(int colIndex_) {
	throw new UnexpectedException("concat does not handle getNextValBlock");
}

/**
 * This operator needs to return the next SBlock worth of tuples. Recall that
 * in CStore, for efficiency, iterators forward blocks of tuples, not single
 * tuples. colIndex_ is completely ignored by this function.
 * DSM FIXME: what should the blocksize be for SMultiblocks?
 */
SBlock* Concat::getNextSValBlock(int colIndex_) {
	int values[numtouse];
	int position;
	bool done = false;

	updateInputBlocks();

	// Hack to get the right position value
	if (inputBlocks[0] != NULL && inputBlocks[0]->hasNext()) {
		position = inputBlocks[0]->peekNext()->position;
	} else {
		return NULL;
	}
	
	sb->resetBlock();
	sb->setStartPosition(position);
	
	for (int j = 0; j < BLOCKSIZE && !done; ++j) {
		int pos = 0;
		for (int i = 0; i < ninputs; ++i) {
			if (inputBlocks[i] == NULL) {
				done = true;
				break;
			}
			ValPos* p = inputBlocks[i]->getNext();
			//assume int type for now ... fix later
			assert(p->type == ValPos::INTTYPE);
			if (i < numtouse) {
				//cerr << "Read pair " << p->value << endl;
				values[pos++] = *(int*)p->value;
			}
		}
		//cerr << "Setting value at: " << j << "/" << BLOCKSIZE 
		//		<< " pos " << position << endl;
		sb->setValueAtPos((char*)values, j);
		updateInputBlocks(); // DSM FIXME inefficient?
	}

	return sb;
}
	
// Gets the next position block (bitstring of positions) from the operator
PosBlock* Concat::getNextPosBlock(int colIndex_) {
	throw new UnexpectedException("concat doesn't handle getNextPosBlock");
}
	
// Gets the maximum position in the data source
int Concat::getLastPosition() 
{
	throw new UnexpectedException("concat doesn't handle getLastPosition");		
}

void Concat::updateInputBlocks() {
	for (int i = 0; i < ninputs; ++i) {
		if (inputBlocks[i] == NULL || !inputBlocks[i]->hasNext()) {
			// The zero below just assumes we want the first stream from
			// all operators DSM FIXME
			inputBlocks[i] = inputs[i]->getNextValBlock(0);
			//cerr << "Set block " << i << " to " << inputBlocks[i] << endl;
		}
	}
}
