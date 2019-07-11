#include "TupleGenerator.h"

TupleGenerator::TupleGenerator(Operator** sources_, int* ncols_, int nsources_, int total_ncols_) 
	: Operator(), sources(sources_), ncols(ncols_), nsources(nsources_), total_ncols(total_ncols_),
		outblock_capacity(PAGE_SIZE / total_ncols*sizeof(int)),
		outBlock(outblock_capacity, total_ncols*sizeof(int)), done(false)
{
	out_block_buffer = (int*) outBlock.getBuffer();
	curarrays = new int*[total_ncols];
	curarray_sizes = new int[total_ncols];
	curindexes = new int[total_ncols];
	curiters = new BlockIter*[total_ncols];
	for (int i = 0; i < total_ncols; ++i) {
		curarray_sizes[i] = curindexes[i] = 0;
		curiters[i] = NULL;
		curarrays[i] = NULL;
	}
}

TupleGenerator::~TupleGenerator()
{
	delete [] curindexes;
	delete [] curarrays;
	delete [] curarray_sizes;
}

SBlock* TupleGenerator::getNextSValBlock(int colIndex_) {
	assert(colIndex_ == 0);
	if (done) {
		return NULL;
	}
	
	int nwritten = 0;
	int ncols_written = 0;
	int k = 0;
	Block* b;
	int nseen = 0;
	
	while (nwritten < outblock_capacity) {
		k = 0;
		ncols_written = 0;
		for (int i = 0; i < nsources; ++i) {
			for (int j = 0; j < ncols[i]; ++j) {
				// If we've exhausted the array for this particular column
				if (curindexes[k] == curarray_sizes[k]) {
					delete curiters[k];
					
					b = sources[i]->getNextValBlock(j);
					if (b == NULL) {
						outBlock.resetBlock();
						outBlock.setNElements(nwritten);
						done = true;
						//cerr << "Wrote: " << nwritten << " saw " << nseen << endl;
						return &outBlock;
					} else {
						if (b->getBlock() == b) { // B is a normal block
							curiters[k] = b->getIterator();
						} else {				  // B is an iterator
							curiters[k] = static_cast<BlockIter*>(b);
						}
						curarrays[k] = (int*) curiters[k]->asArray();
						curarray_sizes[k] = curiters[k]->getSize();
						if (k == 1) {
							//cerr << "saw size " << curarray_sizes[k] << endl;
							nseen += curarray_sizes[k];
						}
						curindexes[k] = 0;
					}
				}
				out_block_buffer[nwritten*total_ncols + ncols_written] = curarrays[k][curindexes[k]];
				curindexes[k] = curindexes[k] + 1;
				++ncols_written;
				++k;
			}
		}
		++nwritten;
	}
	assert(nwritten == outblock_capacity);
	outBlock.resetBlock();
	outBlock.setNElements(nwritten);
	return &outBlock;
}
