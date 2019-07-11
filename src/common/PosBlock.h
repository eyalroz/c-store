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
#ifndef _POSBLOCK_H_
#define _POSBLOCK_H_

#include "PosPair.h"
#include "UnexpectedException.h"
#include <iostream>

class PosBlockIter;

// Position Blocks Inteface

class PosBlock
{
//typedef unsigned int position;
public:
	PosBlock();
	PosBlock(const PosBlock&);	
	virtual ~PosBlock();

	virtual PosBlock* clone(PosBlock&) = 0;
	
	// Iterator access to block
	virtual bool hasNext()=0;
	virtual unsigned int getNext()=0;
	virtual unsigned int peekNext()=0;
	// Zero indexed, gets the pair at this loc
	virtual unsigned int getPosAtLoc(unsigned int loc_)=0;
	virtual unsigned int getCurrLoc()=0;

	// return size of block		
	virtual int getNumOcurrences() { return numOccurences; }
	virtual void setNumOcurrences(int numOccurences_) { numOccurences=numOccurences_; }
	
	virtual int getSize()=0;
	virtual int getSizeInBits()=0;
	virtual unsigned int getStartPos()=0;
	virtual unsigned int getEndPos()=0;
	virtual void resetBlock()=0;

	virtual bool skipToPos(unsigned int pos);
	virtual PosBlock* copySubset(int fromPos, int untilPos)=0;
	virtual unsigned int initBitStringIterator(unsigned int pos)=0;
	virtual int getNextBitString()=0;

	
	// Stream properties
	virtual bool isPosSorted()=0;
	
	// Block properties
	virtual bool isPosContiguous()=0;
	virtual bool isBlockPosSorted()=0;
	virtual bool isSparse()=0;
	
	virtual PosBlockIter* getIterator() const = 0; //{
	virtual PosBlockIter* getIterator(unsigned int startpos, unsigned int endpos) const = 0;
	//	throw UnexpectedException("unsupported");
	//}
	virtual PosBlock* getPosBlock() { return this; }
	virtual PosBlock* clone() { return clone(*this); }

protected:
	int numOccurences;

	bool bitStringInitialized;
	int* bitStringPtr;
};

class PosBlockIter : public PosBlock {
public:
  virtual ~PosBlockIter(){;}
  /*
  virtual bool hasNext() const = 0;
  virtual unsigned int getNext() = 0;
  virtual unsigned int peekNext() const = 0;
  virtual unsigned int getCurrLoc() const = 0;
  */
  virtual void resetIterator() = 0;
  virtual PosBlock* getPosBlock() = 0;
  /*
  virtual bool skipToPos(unsigned int pos) = 0;
  virtual PosBlockIter* clone() const = 0;
  virtual int getSize()=0;
  virtual unsigned int getStartPos()=0;
  virtual unsigned int getEndPos()=0;
  */
  virtual unsigned int initBitStringIterator(unsigned int pos)=0;
  virtual int getNextBitString()=0;
  virtual void posAnd(PosBlockIter* other, PosBlock* toWrite)=0;
  virtual void posOr(PosBlockIter* other, PosBlock* toWrite) {
	throw UnexpectedException();
	}
  virtual int findLocAtPosAndSkip(unsigned int pos){ throw UnexpectedException("unsupported");} 
  virtual PosBlock* copySubset(int spos, int epos) { throw UnexpectedException("unsupported");}
  virtual int getSizeInBits() { throw UnexpectedException("unsupported"); }
  virtual void resetBlock() { resetIterator(); }
  virtual PosBlockIter* getIterator() const { 
  //	throw UnexpectedException("refuse to make an iterator on an iterator");
  		//PosBlockIter* pbi = (PosBlockIter*) clone(*this);
  		//pbi->resetIterator();
 	 	//return pbi;
 	 	return ((PosBlockIter*) this)->getPosBlock()->getIterator();
  }
  virtual PosBlockIter* getIterator(unsigned int spos, unsigned int epos) const {
  	//throw UnexpectedException("refuse to make an iterator on an iterator");
  	return ((PosBlockIter*) this)->getPosBlock()->getIterator(spos, epos);
  }
  /*
  virtual bool isPosSorted()=0;
  virtual bool isPosContiguous()=0;
  virtual bool isBlockPosSorted()=0;
  virtual bool isSparse()=0;
  */
};

/* Acually, I'm going to wait until bit-string iteration is stable in PosBlocks first before making them iterators like above*/
class PosBlockBSIter {
 public:
  virtual ~PosBlockBSIter(){;}
};


#endif //_POSBLOCK_H_
