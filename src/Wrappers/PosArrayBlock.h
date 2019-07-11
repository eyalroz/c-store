#ifndef POSARRAYBLOCK_H_
#define POSARRAYBLOCK_H_

#include "../common/PosBlock.h"
#include "../common/UnexpectedException.h"
#include "../common/UnimplementedException.h"
#include <iostream.h>

class PosArrayBlock : public PosBlock
{
public:
	PosArrayBlock(int capacity);
	virtual ~PosArrayBlock();
	PosArrayBlock(const PosArrayBlock& block_);

	PosBlock* clone(PosBlock&);
	
	unsigned int getPosition();
	
	//void setPosition(unsigned int pos_);

	// Iterator access to block
	virtual bool hasNext();
	virtual unsigned int getNext();
	virtual unsigned int peekNext();
	// Zero indexed, gets the pair at this loc
	virtual unsigned int getPosAtLoc(unsigned int loc_);
	virtual unsigned int getCurrLoc();
	virtual void setPosAtLoc(int loc, int pos);
	virtual void setPosVarCap(unsigned int pos);
	//virtual void setRepeats(int repeats_) {repeats=repeats_;}
	//virtual int getRepeats() {return repeats;}
	
	// Return size of block
	//virtual int getNumOcurrences();
	//virtual void setNumOcurrences(int numOccurences_);
	
	virtual int getSize();
	virtual int getSizeInBits();
	virtual unsigned int getStartPos();
	virtual unsigned int getEndPos();
	virtual void resetBlock();

	virtual unsigned int getThisOrNextPos(unsigned int pos, int* loc);
	virtual unsigned int getThisOrPrevPos(unsigned int pos, int* loc);
	
	
	virtual PosBlock* copySubset(int fromPos, int untilPos);
	virtual unsigned int initBitStringIterator(unsigned int pos);
	virtual int getNextBitString();

	// Stream properties
	virtual bool isPosSorted();
	
	// Block properties
	virtual bool isPosContiguous();
	virtual bool isBlockPosSorted();
	virtual bool isSparse();
	
	void setNumElements(int n);
	void setBuffer(int* inBuffer, int numValues);
	PosBlockIter* getIterator() const {
	  return new PosArrayBlockIter(const_cast<PosArrayBlock*>(this));
	  //return NULL;
	}
	PosBlockIter* getIterator(unsigned int startpos, unsigned int endpos) const {
	  //return NULL;
	    return new PosArrayBlockIter(const_cast<PosArrayBlock*>(this), startpos, endpos);
	}
	
	class PosArrayBlockIter : public PosBlockIter {
	public:
	  PosArrayBlockIter(PosArrayBlock* pArrayBlock_) : pArrayBlock(pArrayBlock_) 
	  {
	  	startpos = pArrayBlock->getStartPos();
	  	endpos = pArrayBlock->getEndPos();
		endLoc = pArrayBlock->getSize()-1;
		startLoc=0;
		resetIterator();
	  }
	  PosArrayBlockIter(PosArrayBlock* pArrayBlock_, unsigned int spos, unsigned int epos);
	  virtual ~PosArrayBlockIter(){;}
	  
	  virtual bool hasNext();
	  virtual unsigned int getNext();
	  
	  virtual unsigned int peekNext();
	  virtual unsigned int getCurrLoc();
	  virtual void resetIterator();
	  virtual bool skipToPos(unsigned int pos);
	  virtual PosBlock* clone(PosBlock& b);
	  virtual PosBlockIter* clone() const { return new PosArrayBlockIter(*this); }
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
	  
	  virtual unsigned int getPosAtLoc(unsigned int loc_);
	  
	  virtual PosBlock* getPosBlock() { return pArrayBlock; }
	protected:
	  PosArrayBlock* pArrayBlock;
	  //BlockIter* blockIter;
	  //int numOccCounter;
	  //Pair* currPair;
	  unsigned int startpos;
	  unsigned int endpos;
	  int startLoc;
	  int endLoc;
	  int nextPos;
	  //	MultiBlock::MultiBlockIter* mbIter;
	};

protected:
	unsigned int* positions;
	int capacity;
	int nelements;
	int nextpos;
	int currloc;
	//int repeats;
};

#endif /*POSARRAYBLOCK_H_*/
