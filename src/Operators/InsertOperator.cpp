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
#include "InsertOperator.h"

InsertOperator::InsertOperator(WOSManager* wmToTable_) : wmToTable(wmToTable_) {
  numColumnsInTable = wmToTable->getNumColumns();
  assert(numColumnsInTable > 0);
  tuple = new char[(numColumnsInTable+1)*sizeof(int)];
}

InsertOperator::~InsertOperator() {
  if (tuple != NULL) delete [] tuple;
}

int InsertOperator::initTuple()
{
  assert(tuple != NULL);
  // This next statement appeared in the original CStore 0.2 code:
  // 
  //   memset(tuple, 0, sizeof(tuple));
  //
  // It's quite suspect. sizeof(tuple) is the size of the _pointer_ to the
  // first character pointed to by `tuple`. It doesn't make sense that _that_
  // should be the number of characters which should be set to 0. Instead,
  // we'll use the number of characters allocated in the constructor.
  std::size_t tupleBufferSize = (numColumnsInTable+1)*sizeof(int); 
  memset(tuple, 0, tupleBufferSize);
  //storageKey = wmToTable->rwseg->getLastStorageKey();
  storageKey = wmToTable->getLastStorageKey( false );
  if (storageKey <= 0) {
    storageKey = 1;
  } else {
    ++storageKey;  
  }
  memcpy(tuple, &(storageKey), sizeof(int));    // storage key, first 4 bytes  
  return storageKey;
}

void InsertOperator::populateField(int fieldNum_, int storageKey_, int val_)
{
  assert(fieldNum_ > 0  &&  storageKey == storageKey_);
  memcpy( tuple + (fieldNum_*sizeof(int)), &(val_), sizeof(int) );          
}

void InsertOperator::appendTuple(int storageKey_)
{
  assert(tuple != NULL  &&  storageKey == storageKey_);
  wmToTable->appendTuple(tuple);
  //wmToTable->rwseg->appendTuple(tuple);
}

void InsertOperator::appendColumns(Operator* dataSource_[], int numColumnsInDataSource_, int columnIndex_[])
{
  assert(numColumnsInDataSource_ > 0);
  Block* blks[numColumnsInDataSource_];
  unsigned int rowCount[numColumnsInDataSource_];
  memset(blks, 0, sizeof(blks));
  
  bool done = false;
  
  for (int i = 0; i < numColumnsInDataSource_; i++) {
    rowCount[i] = 0;
    blks[i] = NULL;
    blks[i] = dataSource_[i]->getNextValBlock(0);     //single-column
    if (blks[i] == NULL) {
      done = true;
      break;
    }
  }
  while (!done) {
    int storageKey = initTuple();
    //cout << rowCount[0] << "\t";
    for (int i = 0; i < numColumnsInDataSource_; ++i) {
      ValPos* p = this->getNextPairFromBlock(dataSource_[i], blks[i]);
      //assume int type for now ... fix later
      assert(p->type==ValPos::INTTYPE);
      if (p == NULL) {
        done = true;
        break; 
      } else {
        //cout << " column:" << i+1;
        populateField(columnIndex_[i]+1, storageKey, *(int*)p->value);
        rowCount[i]++;
        //cout << "=" << p->value;
      }
    }
    //cout << endl;
    if (!done) appendTuple(storageKey);
  }
}

//get a pair from the block and advance to next block if necessary
ValPos* InsertOperator::getNextPairFromBlock(Operator* dataSource_, Block* blk)
{
    ValPos* pair = NULL;
    while (blk != NULL) {
      if (blk->hasNext()) pair = blk->getNext();
      if (pair != NULL) break;
      blk = dataSource_->getNextValBlock(0);    //assume single-column data source
    }
    return pair;
}
