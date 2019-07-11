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
#ifndef HASHJOIN_H
#define HASHJOIN_H

#include "../common/Interfaces.h"
#include "../Wrappers/RLETriple.h"
#include "../Wrappers/RLEBlock.h"
#include "../Wrappers/DeltaPosBlock.h"
#include "../Wrappers/PosBasicBlock.h"
#include "../Wrappers/PosRLEBlock.h"
#include "../Wrappers/PosArrayBlock.h"
#include "../Wrappers/PosType2Block.h"
#include "../Wrappers/Type2Block.h"
#include "../Wrappers/SMultiBlock.h"
#include "../Wrappers/MultiBlock.h"
#include "../Util/HashMap.h"
#include <math.h>
#include <hash_map.h>

class PBObject {
 public:
  PBObject() {
    currCapacity = 100;
    pba = new PosBlock*[currCapacity];
    pba[0] = NULL;
    currPBAsize = 0;
    currPBAindex = 0;
  }
  virtual ~PBObject() {
    delete[] pba;
  }
  void addPosBlock(PosBlock* pb) {
    if (currPBAsize >= currCapacity) {
      currCapacity = currCapacity*2;
      PosBlock** pba2 = new PosBlock*[currCapacity];
      for (int i = 0; i < currPBAsize; i++)
	pba2[i] = pba[i];
      delete[] pba;
      pba = pba2;
    }
    pba[currPBAsize] = pb;
    currPBAsize++;
  }
  bool hasNext() {
    return currPBAindex < currPBAsize;
  }
  void resetIterator() {
    currPBAindex=0;
  }
  PosBlock* getNext() {
    PosBlock* retval = pba[currPBAindex];
    currPBAindex++;
    return retval;
  }
  int getSize() {
    return currPBAsize;
  }
 private:
  PosBlock** pba;
  int currPBAindex;
  int currCapacity;
  int currPBAsize;  
};
/*
class BObject {
 public:
  BObject() {
    currCapacity = 100;
    ba = new Block*[currCapacity];
    ba[0] = NULL;
    currBAsize = 0;
    currBAindex = 0;
  }
  virtual ~BObject() {
    delete[] ba;
  }
  void addBlock(Block* b) {
    if (currBAsize >= currCapacity) {
      currCapacity = currCapacity*2;
      Block** ba2 = new Block*[currCapacity];
      for (int i = 0; i < currBAsize; i++)
	ba2[i] = ba[i];
      delete[] ba;
      ba = ba2;
    }
    ba[currBAsize] = b;
    currBAsize++;
  }
  bool hasNext() {
    return currBAindex < currBAsize;
  }
  Block* getNext() {
    Block* retval = ba[currBAindex];
    currBAindex++;
    return retval;
  }
  int getSize() {
    return currBAsize;
  }
 private:
  Block** ba;
  int currBAindex;
  int currCapacity;
  int currBAsize;  
};
*/
class HashJoin : public Operator {

 public:
  
// CONSTRUCTORS
  // takes in the column to be aggregated and the columns to group by
  HashJoin(Operator **left_, int *predColIndex1_, Operator **right_, int *predColIndex2_, int larraysize_, int rarraysize_, bool outputKey_);

  virtual ~HashJoin();
  
  // Operator interface
  virtual Block* getNextValBlock(int colIndex_);	
  virtual PosBlock* getNextPosBlock(int colIndex_);
  virtual PosBlock* getNextPosBlock(int colIndex_, int val_);
  inline virtual int getLastPosition() { throw UnimplementedException("Not applicable for Count Operator"); };
  
  enum { L_COL_INDEX = 0, R_COL_INDEX = 1};

 protected:

  Operator **l_src;
  int* l_predcol;
  Operator **r_src;
  int* r_predcol;
  int larraysize;
  int rarraysize;
  Block **lBlockIn;
  Block **rBlockIn;
  
  // output state
  bool m_isLBlockConsumed; // dirty means that the block has been read
  bool m_isRBlockConsumed; 
  PosBlock *lBlockOut;
  PosBlock *rBlockOut;
  int m_currOutputPosition;
  
 private:
  void init();
  void generateBlocks();
  void generatePosBlocks();
  void prepareRightHashTablePos();
  void prepareRightHashTable();
  ValPos* getNextRVP(int index);
  ValPos* getNextLVP(int index);
  void updateCurrVPs();
  PosBlock* posify(Block* b);
  ValPos* getCurrLPair();
  ValPos* getNextLPair();

  bool firstCall;
  bool dontcheck;
  Block** blocks;

  ValPos* currLPair;

  PBObject* rpbo;
  bool isRightOneValue;
  bool isLeftOneValue;
  enum {
    UNINIT = 0, 
    POSMODE = 1, 
    VALMODE = 2, 
  };
  short mode;
  HashMap* rMap;
  IdentityHashFunction* hf;
  PosBasicBlock* playpbb;
  Block** rValBlockOut;
  Block** lValBlockOut;
  bool* m_iskBlockConsumed;   // whether the kth block has been consumed by Operator above
  byte** bufferset;
  ValPos** currVPs;
  RLETriple** triples;
  bool outputKey;
  int numOutputColumns;
};

#endif
