/* Copyright (c) 2005, Regents of Massachusetts Institute of Technology, 
 * Brandeis University, Brown University, and University of Massachusetts 
 * Boston. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, 
 *     this list of conditions and the following disclaimer.
 *   - Redistributions in binary form must reproduce the above copyright 
 *     notice, this list of conditions and the following disclaimer in the 
 *     documentation and/or other materials provided with the distribution.
 *   - Neither the name of Massachusetts Institute of Technology, 
 *     Brandeis University, Brown University, or University of 
 *     Massachusetts Boston nor the names of its contributors may be used 
 *     to endorse or promote products derived from this software without 
 *     specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED 
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
// Writes Dictionary Blocks
// Problems: dna@mit.edu


#ifndef _DICTMULTIWRITER_H_
#define _DICTMULTIWRITER_H_

#include "../common/Writer.h"
#include "../Wrappers/BitBlock.h"
#include "../Wrappers/BasicBlock.h"
#include "../Wrappers/Util/ByteWriter.h"
#include "BasicBlockWriter.h"
#include <map>

class DictMultiWriter : public Writer
{
public:
	DictMultiWriter(Operator* blockSrc_, int colIndex_, unsigned int bfrSizeBits_, unsigned int fieldSize_);
	virtual ~DictMultiWriter();
	
	// Gets the next value block from the operator 
	virtual	Block* getNextValBlock(int colIndex_);	
	
	// Gets the next position block (bitstring of positions) from the operator
	virtual PosBlock* getNextPosBlock(int colIndex_);
	
	inline virtual int getLastPosition() { throw UnimplementedException("Not applicable for DictMultiWriter"); };
	
protected:
	void resetWriter();
	Block* getEntries();
	Block* getTable();
	unsigned int getEntry(int* entryarray);
	unsigned int setEntry(unsigned int entry, unsigned int index, unsigned int value);

	unsigned int fieldBytes;
	unsigned int fieldSize;
	unsigned int escapeInt;
	unsigned int numDistinctValues;
	unsigned int numEncodedValsInInt;
	unsigned int tableSize;
	BasicBlockWriter* blockWriter;
	BasicBlock* currBlock;
	BitBlock* outBlock;
	ByteWriter* byteWriter;
	
	BasicBlock* dictBlock;
	
	bool init;
	bool readyToOutDict;
	byte* buffer;
	int* currval;
	unsigned int currindex;
	int* vals;
	int curpos;
	unsigned int bfrSizeBits;
	unsigned int* startPosPtr;
	unsigned int* numValsPtr;
	
	/*struct ltfunc { bool operator()(const int* s1, const int* s2) const { return memcmp(s1, s2,16*sizeof(int)) < 0; } };

	map<int*, unsigned int, ltfunc> valueEntryMap;
	map<unsigned int, int*> entryValueMap;*/

	map<int, unsigned int> valueEntryMap;
	map<unsigned int, int> entryValueMap;

};

#endif //_DICTMULTIWRITER_H_
