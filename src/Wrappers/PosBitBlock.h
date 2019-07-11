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
#ifndef _POSBITBLOCK_H_
#define _POSBITBLOCK_H_

// Position Bit Blocks
// Problems: migfer@mit.edu


#include "../common/PosBlock.h"
#include "BitBlock.h"

class PosBitBlock : public PosBlock
{
public:
	PosBitBlock();
	PosBitBlock(const PosBitBlock&);	
	virtual ~PosBitBlock();

	PosBlock* clone(PosBlock&);
	
	virtual void setBuffer(byte* bfr, int bfrSize_);
	virtual byte* getBuffer();
	// Iterator access to block
	virtual bool hasNext();
	virtual PosPair* getNext();

	// Zero indexed, gets the pair at this loc
	virtual unsigned int getPosAtLoc(unsigned int loc_);
	virtual unsigned int getCurrLoc();

	// return size of block		
	virtual int getSize();
	virtual int getSizeInBits();
	virtual unsigned int getStartPos();
	virtual unsigned int getEndPos();
	virtual void resetBlock();	
	virtual bool isPosContiguous();
	//virtual bool isSparse();
	PosBlockIter* getIterator() const {
		cerr << "new posbitblock iter" << endl;
		throw UnexpectedException();
	}
	PosBlockIter* getIterator(unsigned int startpos, unsigned int endpos) const {
		cerr << "New PosBitBlock s/e iter" << endl;
		throw UnexpectedException();
	}
protected:
	BitBlock* block;
	bool isPosAvail;
	unsigned int posAvail;
	unsigned int numOccurences;
};

#endif //_POSBITBLOCK_H_
