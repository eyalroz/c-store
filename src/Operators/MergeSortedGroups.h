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
#ifndef MERGESORTEDGROUPS_H
#define MERGESORTEDGROUPS_H

#include "../common/Interfaces.h"
#include "Merge.h"
#include "MergeOp.h"
#include <cassert>

class MergeSortedGroups : public Merge {
  
 public:
  MergeSortedGroups(Operator *rs_, Operator *ws_, int nCols_, int mergeColIndex_);
  ~MergeSortedGroups();

  void setMergeOp(MergeOp *mergeOp_);
	inline virtual int getLastPosition() { throw UnimplementedException("Not applicable for MergeSortedGroups"); };


 protected:
  virtual void generateBlocks();
  virtual void init();
  virtual void initOutputBlocks();

 private:
  //  int m_currGroup;
  int m_wsGroup;
  int m_rsGroup;
  int m_mergeColIndex;

  MergeOp* m_mergeOp;

  bool hasMoreWSBlocks();
  bool hasMoreRSBlocks();
  void assignRSGroup(Block *rsBlock);
  void assignWSGroup(Block *wsBlock);

  DoubleValPos* lvp;
  
};

#endif
