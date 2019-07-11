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
#ifndef DELETEOPERATOR_H
#define DELETEOPERATOR_H

#include <cassert>
#include "../common/Interfaces.h"
#include "../AM/WOSAM.h"
#include "../AM/ROSAM.h"
#include "../AM/WOSManager.h"
#include "../Wrappers/IntDataSource.h"
#include "./BNotOperator.h"
#include "BlockPrinter.h"

/**
 *
 * DeleteOperator
 *  
 */

class DeleteOperator : public Operator {

  public:
 
  DeleteOperator(WOSManager* wosMgr_);

  virtual ~DeleteOperator();
  
  void deleteTuple(int position, bool isROS);
  //delet position from a data source
  void deleteTuple(Operator* inOp, bool isROS);
  void deleteProjection();
  //delet position from a position list (pList)
  void deleteByPositionList(Operator* pList, bool isROS);
    
  // Operator interface
  inline virtual Block* getNextValBlock(int val_) { throw UnimplementedException("Not applicable to DeleteOperator");};
  inline virtual PosBlock* getNextPosBlock() { throw UnimplementedException("Not applicable to DeleteOperator");};
  inline virtual PosBlock* getNextPosBlock(int val_) { throw UnimplementedException("Not applicable to DeleteOperator");};
  inline virtual int getLastPosition() { throw UnimplementedException("Not applicable to DeleteOperator"); };
  
  protected:
    

  // INPUT STATE
  WOSManager* wosMgr;
  
  private:
    
};

#endif
