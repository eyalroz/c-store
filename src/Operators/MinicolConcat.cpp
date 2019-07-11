#include "MinicolConcat.h"

MinicolConcat::MinicolConcat(Operator* inputs, int* input_colidxs)
{
		this->inputs = inputs;
		this->input_colidxs = input_colidxs;
}

MinicolConcat::~MinicolConcat()
{
}

MinicolBlock* MinicolConcat::getNextMinicolBlock() {
	return NULL;
}

Block* MinicolConcat::getNextValBlock(int colidx) {
	return NULL;
}

PosBlock* MinicolConcat::getNextPosBlock(int colidx) {
	return NULL;
}

int MinicolConcat::getLastPosition() {
	throw new UnexpectedException("not supported");
}

