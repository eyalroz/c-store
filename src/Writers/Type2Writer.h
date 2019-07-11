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
// Writes Delta Pos Blocks
// Problems: migfer@mit.edu


#ifndef _TYPE2WRITER_H_
#define _TYPE2WRITER_H_

#include "../common/Writer.h"
#include "../Wrappers/Util/BitWriter.h"
#include "../Util/SpillMap.h"
#include "../Util/HashMap.h"
#include "DeltaPosValue.h"
#include "../Wrappers/Type2Block.h"
#include "../Wrappers/DeltaPosBlock.h"
#include "../Wrappers/Decoder/DeltaPosDecoder.h"

class Type2Writer : public Writer
{
public:

	Type2Writer(Operator* blockSrc_, int colIndex_, int bfrSizeInBits_, int deltaSize_);
	virtual ~Type2Writer();
	
	// Gets the next value block from the operator 
	virtual	Block* getNextValBlock(int colIndex_);
	
	// Gets the next position block (bitstring of positions) from the operator
	virtual PosBlock* getNextPosBlock(int colIndex_);
	
	inline virtual int getLastPosition() { throw UnimplementedException("Not applicable for Type2Writer"); };

	virtual ValPos** getValueIndex();

protected:
	ValPos* getNextPair();
	Block* inBlock;
	DeltaPosBlock* currBlockD;
	Type2Block* currBlockT2;
	ValPos* currVP;
	unsigned int deltaSize;
	unsigned int bfrSizeInBits;
	SpillMap bufferMap;
	HashMap* hashTable;
	bool emptyBuffers;
	int prevmod;
	ValPos** values;
	int* indexes;
	int valindex;

};

#endif //_TYPE2WRITER_H_
