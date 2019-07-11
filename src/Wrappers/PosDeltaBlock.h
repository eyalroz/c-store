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
#ifndef _POSDELTABLOCK_H_
#define _POSDELTABLOCK_H_

// Position Delta Blocks
// Problems: migfer@mit.edu


#include "../common/PosBlock.h"
#include "DeltaPosBlock.h"

class PosDeltaBlock : public PosBlock
{
public:
	PosDeltaBlock();
	PosDeltaBlock(const PosDeltaBlock& block_);
	virtual ~PosDeltaBlock();

	PosBlock* clone(PosBlock&);
	
	void setDeltaBlock(DeltaPosBlock* blk_);
	void setBuffer(byte* bfr_, unsigned int bfrSize_);
	byte* getBuffer();
	
	
	virtual bool hasNext();
	virtual unsigned int getNext();
	virtual unsigned int peekNext();

	// Zero indexed, gets the pair at this loc
	virtual unsigned int getPosAtLoc(unsigned int loc_);
	virtual unsigned int getCurrLoc();

	// return size of block		
	virtual int getSize();
	virtual int getSizeInBits();
	virtual unsigned int getStartPos();
	virtual unsigned int getEndPos();
	virtual void resetBlock();	

	virtual PosBlock* copySubset(int fromPos, int untilPos);
	virtual unsigned int initBitStringIterator(unsigned int pos);
	virtual int getNextBitString();
	
	// Stream properties
	virtual bool isPosSorted();
	
	// Block properties
	virtual bool isPosContiguous();
	virtual bool isBlockPosSorted();
	virtual bool isSparse();
	
	PosBlockIter* getIterator() const {
		cerr << "call to posdeltablock iter" << endl;
		throw UnexpectedException();
	}
	PosBlockIter* getIterator(unsigned int startpos, unsigned int endpos) const {
		cerr << "New PosDeltaBlock s/e iter" << endl;
		throw UnexpectedException();
	}
	
protected:
	DeltaPosBlock* block;	
	bool isBlockExternal;
};

#endif //_POSDELTABLOCK_H_
