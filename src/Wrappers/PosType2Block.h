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
// Single value block
// Coded by DNA


#ifndef _POSTYPE2BLOCK_H_
#define _POSTYPE2BLOCK_H_

#include "../common/PosBlock.h"
#include "../common/Block.h"
#include "Type2Block.h"
#include "CodingException.h"
#include "math.h"

class PosType2Block : public PosBlock
{
public:
	PosType2Block();
	PosType2Block(const PosType2Block&);
	virtual ~PosType2Block();
	PosBlock* clone(PosBlock&);
	virtual void setBlock(Type2Block* block_);
	virtual Type2Block* getBlock();
	//virtual void setValue(Pair* val_);
	virtual void resetBlock();
	// Iterator access to block
	virtual bool hasNext();
	//virtual bool hasNext(int value_);
	virtual unsigned int getNext();
	virtual unsigned int peekNext();
	//virtual int getValue();
	//virtual unsigned int getPosition();
	virtual unsigned int getEndPos();
	//virtual void cutOffEnd(int endPos);
	//void printBits(int bits);

	// Zero indexed, gets the pair at this pos_
	virtual unsigned int getPosAtLoc(unsigned int loc_);
	virtual unsigned int getCurrLoc();

	// return size of block		
	virtual int getSize();
	virtual int getSizeInBits();
	virtual unsigned int getStartPos();
	virtual byte* makeNewBuffer(int value_, int startPos_, int endPos_);
	virtual PosBlock* copySubset(int fromPos, int untilPos);
	virtual unsigned int initBitStringIterator(unsigned int pos);
	virtual int getNextBitString();
	
	// Stream properties
	virtual bool isPosSorted();
	virtual bool isSparse();
	
	// Block properties
	virtual bool isPosContiguous();
	virtual bool isBlockPosSorted();
	
	PosBlockIter* getIterator() const {
	  return new PosType2BlockIter(const_cast<PosType2Block*>(this));
	  //return NULL;
	}
	PosBlockIter* getIterator(unsigned int startpos, unsigned int endpos) const {
	  //return NULL;
	    return new PosType2BlockIter(const_cast<PosType2Block*>(this), startpos, endpos);
	}
	
	class PosType2BlockIter : public PosBlockIter {
	public:
	  PosType2BlockIter(PosType2Block* pType2Block_) 
	    : pType2Block(pType2Block_), blockIter(pType2Block->block->getIterator()), numOccCounter(0) 
	  {
	    startpos = pType2Block->getStartPos();
	    endpos = pType2Block->getEndPos();
	    FLAPlastPos=0;
	    FLAPlastCount=0;
	  }
	  PosType2BlockIter(PosType2Block* pType2Block_, unsigned int spos, unsigned int epos) 
	    : pType2Block(pType2Block_), blockIter(NULL), numOccCounter(0) {
	    
	    if (spos < pType2Block->getStartPos()) {
	      spos = pType2Block->getStartPos();
	    }
	    if (epos > pType2Block->getEndPos()) {
	      epos = pType2Block->getEndPos();
	    }
	    
	    assert(spos <= epos);
	    
	    if (spos > pType2Block->getEndPos() || epos < pType2Block->getStartPos()) {
	      blockIter = new EmptyBlockIter();
	    } else {				
	      blockIter = pType2Block->block->getIterator(spos,epos);
	    }
	    startpos = spos;
	    endpos = blockIter->getEndPosition();
	    FLAPlastPos=0;
	    FLAPlastCount=0;
	  }
	  virtual ~PosType2BlockIter(){delete blockIter;}
	  
	  virtual bool hasNext();
	  virtual unsigned int getNext();
	  
	  virtual unsigned int peekNext();
	  virtual unsigned int getCurrLoc();
	  virtual void resetIterator();
	  virtual bool skipToPos(unsigned int pos);
	  virtual PosBlock* clone(PosBlock& b);
	  virtual PosBlockIter* clone() const { return new PosType2BlockIter(*this); }
	  virtual int getSize();
	  virtual unsigned int getStartPos();
	  virtual unsigned int getEndPos();
	  virtual unsigned int initBitStringIterator(unsigned int pos);
	  virtual int getNextBitString();
	  virtual void posAnd(PosBlockIter* other, PosBlock* toWrite);
	  virtual void posOr(PosBlockIter* other, PosBlock* toWrite);
	  virtual int findLocAtPosAndSkip(unsigned int pos);
	  virtual bool isPosSorted();
	  virtual bool isPosContiguous();
	  virtual bool isBlockPosSorted();
	  virtual bool isSparse();
	  
	  virtual unsigned int getPosAtLoc(unsigned int loc_) {
	    //DNA: I believe the below code is wrong (remove getBlock())
		ValPos* currPair=blockIter->getBlock()->getPairAtLoc(loc_/numOccurences);
	  	if (currPair==NULL) throw new CodingException("PosType2Block: error no next position");
  		return currPair->position;	
	  }
	  
	  virtual PosBlock* getPosBlock() { return pType2Block; }
	protected:
	  PosType2Block* pType2Block;
	  BlockIter* blockIter;
	  int numOccCounter;
	  //ValPos* currPair;
	  unsigned int startpos;
	  unsigned int endpos;
	  unsigned int FLAPlastPos;
	  int FLAPlastCount;
	  //	MultiBlock::MultiBlockIter* mbIter;
	  };

	//Pair* pair;
	//Pair* utilityPair;
	ValPos* vp;
	int currPos;

protected:
	bool posContig;
	bool posSorted;
	int numOccCounter;

	/*bool pairNewed;
	byte* buffer;
	int numValues;
	int startVal;
	int startPos;
	int* currPosPtr;*/
	Type2Block* block;

};

#endif //_POSTYPE2BLOCK_H_
