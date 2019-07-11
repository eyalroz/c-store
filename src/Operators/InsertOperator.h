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
#ifndef INSERTOPERATOR_H
#define INSERTOPERATOR_H

#include <cassert>
#include "../common/Interfaces.h"
#include "../AM/WOSManager.h"
#include "../AM/WOSAM.h"

/**
 *
 * InsertOperator high-level pseudocode:
 *  
 */

class InsertOperator : public Operator {

	public:
 
	InsertOperator(WOSManager* amToTable_);

  virtual ~InsertOperator();

  int initTuple();      //return storageKey for subsequent call(s) to populateField()
  void populateField(int fieldNum_, int storageKey_, int val_);     //populate input value into tuple
  void appendTuple(int storageKey_);      //append buffer(tuple) to WOS
  void appendColumns(Operator* dataSource_[], int numColumns_, int columnIndex_[]);
    
  // Operator interface
  inline virtual Block* getNextValBlock(int val_) { throw UnimplementedException("Not applicable to InsertOperator");};
  inline virtual PosBlock* getNextPosBlock() { throw UnimplementedException("Not applicable to InsertOperator");};
  inline virtual PosBlock* getNextPosBlock(int val_) { throw UnimplementedException("Not applicable to InsertOperator");};
  inline virtual int getLastPosition() { throw UnimplementedException("Not applicable to InsertOperator"); };

  protected:    

  // INPUT STATE
  WOSManager* wmToTable;    //target table/projection
  Operator* inOp;           //Operator with input value(s) to be inserted to the target column
  
  private:
    
  ValPos* getNextPairFromBlock(Operator* dataSource_, Block* blk);    
      
  int storageKey;           //storage key for the tuple to be inserted to WOS
  char* tuple;              //buffer to hold value list of insert
  int numColumnsInTable;    //number of columns in the table/projection
      
};

#endif
