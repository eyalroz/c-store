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
#include "DeleteOperator.h"

DeleteOperator::DeleteOperator(WOSManager* wosMgr_)
{
  assert(wosMgr_ != NULL);
  wosMgr = wosMgr_;
}

DeleteOperator::~DeleteOperator() {}

void DeleteOperator::deleteTuple(int position, bool isROS)
{
  wosMgr->deleteTuple(position, isROS);
}

void DeleteOperator::deleteTuple(Operator* inOp, bool isROS)
{
  Block* inBlock = inOp->getNextValBlock(0);
  while (inBlock != NULL) {
    for (int i = 0; i < inBlock->getSize(); ++i) {
      ValPos* pair = inBlock->getNext();
      wosMgr->deleteTuple(pair->position, isROS);
    } 
    inBlock = inOp->getNextValBlock(0);
  }
}

void DeleteOperator::deleteProjection()
{
  //construct delete vector for WOS
  IntDataSource* dsWOS = new IntDataSource( wosMgr->getDVAM( true ),
                                          true,   // value sorted
                                          false); // WOS
  
  BNotOperator* opWOS = new BNotOperator(dsWOS, 0);
  deleteByPositionList(opWOS, false);
  
  //construct delete vector for ROS
  IntDataSource* dsROS = new IntDataSource( wosMgr->getDVAM( true ),
                                          true,   // value sorted
                                          true); // ROS

  BNotOperator* opROS = new BNotOperator(dsROS, 0);
  deleteByPositionList(opROS, true);
  
  if (opROS != NULL) delete opROS;
  if (dsROS != NULL) delete dsROS;
  if (opWOS != NULL) delete opWOS;
  if (dsWOS != NULL) delete dsWOS;  
}

void DeleteOperator::deleteByPositionList(Operator* pList, bool isROS)
{
  PosBlock* block = pList->getNextPosBlock(0);    
  while (block != NULL) {
    while (block->hasNext()) {
      unsigned int thisPosition = block->getNext();
      wosMgr->deleteTuple(thisPosition, isROS);
    }
    block = pList->getNextPosBlock(0);
  }
}
