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
#ifndef _POSBLOCKARRAY_H_
#define _POSBLOCKARRAY_H_

#include "PosBlock.h"
#include "../Wrappers/CodingException.h"
#include <iostream>

// Array of Position Blocks
class PosBlockArray : public PosBlock
{
public:

	PosBlockArray(PosBlock** pba, int numEntries);
	PosBlockArray(PosBlock** pba, int numEntries, unsigned int startPos, unsigned int endPos);
	PosBlockArray(const PosBlockArray&);	
	virtual ~PosBlockArray();

	virtual PosBlock* clone(PosBlock&);
	
	// Iterator access to block
	//virtual bool hasNext() { return ourIter.hasNext(); }
	//virtual unsigned int getNext() { return ourIter.getNext(); }
	//virtual unsigned int peekNext() { return ourIter.peekNext(); }
	// Zero indexed, gets the pair at this loc and points that 
	// Cursor to the following pair
	//virtual unsigned int getCurrLoc() { return ourIter.getCurrLoc(); }
	
	virtual bool hasNext();
	virtual unsigned int getNext();
	virtual unsigned int peekNext();
	// Zero indexed, gets the pair at this loc and points that 
	// Cursor to the following pair
	virtual unsigned int getCurrLoc();
	
	virtual unsigned int getPosAtLoc(unsigned int loc_);
	
	virtual int getSize();
	virtual int getSizeInBits();
	virtual unsigned int getStartPos();
	virtual unsigned int getEndPos();
	virtual void resetBlock();	
	virtual PosBlock* copySubset(int fromPos, int untilPos);
	virtual unsigned int initBitStringIterator(unsigned int pos);
	virtual int getNextBitString();	
	virtual PosBlockIter* getIterator() const;
	virtual PosBlockIter* getIterator(unsigned int startPos_, unsigned int endPos_) const;
	
	// Stream properties
	virtual bool isPosSorted();
	
	// Block properties
	virtual bool isPosContiguous();
	virtual bool isBlockPosSorted();
	virtual bool isSparse();



	class PosBlockArrayIter : public PosBlockIter {
	  PosBlockArray* pba;
	  PosBlockIter* curBlockIter;
	  int currindex;
	  unsigned int startPos, endPos;
	  bool startEndPosValid;
	  
	public:
	  PosBlockArrayIter() : pba(NULL), curBlockIter(NULL), startPos(0), endPos(0), startEndPosValid(false) {;}
	  PosBlockArrayIter(const PosBlockArray* pba_, bool startEndPosValid = false, unsigned int startPos_ = 0, unsigned int endPos = 0);
	  PosBlockArrayIter(const PosBlockArrayIter& pbai);
	  PosBlockArrayIter& operator=(const PosBlockArrayIter& pbai);
	  virtual ~PosBlockArrayIter();
	  
	  virtual bool hasNext() const {
	  	return (currindex < pba->size - 1 || curBlockIter->hasNext());
	  }
	  
//	  inline bool hasNextNV() const {
//	  	return (currindex < pba->size - 1 || curBlockIter->hasNext());
//	  }
	  
	  virtual unsigned int getNext() {
	  	assert(hasNext());
		if (!curBlockIter->hasNext()) {
			delete curBlockIter;
			assert (currindex < pba->size - 1);
			assert (curBlockIter->hasNext());
			if (startEndPosValid) { 
				curBlockIter = pba->posBlockArray[++currindex]->getIterator(startPos, endPos);
			} else {
				curBlockIter = pba->posBlockArray[++currindex]->getIterator();
			}
		}
  		return curBlockIter->getNext();
	  }
	  
	  virtual unsigned int peekNext() const;
	  
	  virtual PosBlockIter* clone() const { /*return new PosBlockArrayIter(*this);*/ return NULL; }
	  virtual bool skipToPos(unsigned int pos) { throw UnexpectedException();}
	  virtual void resetIterator(); 
	  virtual unsigned int getCurrLoc() const { throw UnexpectedException();}
	  
	  virtual int getSize();
	  virtual unsigned int getStartPos();
	  virtual unsigned int getEndPos();
	  virtual unsigned int initBitStringIterator(unsigned int pos);
	  virtual int getNextBitString();
	  virtual void posAnd(PosBlockIter* other, PosBlock* toWrite);
	  virtual bool isPosSorted();
	  virtual bool isPosContiguous();
	  virtual bool isBlockPosSorted();
	  virtual bool isSparse();

	}; // End declaration of PosBlockArrayIter
	
	protected:
		PosBlock** posBlockArray;
		int size;
		int* sizes;
		int numPositions;
		int currindex;
		int first;
		int last;
		bool posSorted;
		bool posContig;
		bool blockPosSorted;
		bool sparse;
		unsigned int startPos, endPos;
		bool startEndPosValid;
		unsigned int* startPoses;
		unsigned int* adjStartPoses;
		unsigned int* endPoses;
		unsigned int currBSPos;
		int currBSIndex;
		//PosBlockArrayIter ourIter;
};

#endif //_POSBLOCKARRAY_H_
