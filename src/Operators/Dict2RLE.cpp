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
#include "Dict2RLE.h"

Dict2RLE::Dict2RLE(Operator *inOp_, int inColIndex_, Operator *inOp2_, int inColIndex2_,AM* tableAM) : m_input(inOp_), m_ColIndex(inColIndex_), m_input2(inOp2_), m_ColIndex2(inColIndex2_)
{
  bb1 = NULL;
  bb2 = NULL;
  rb = new RLEBlock(false);
  currpos=1;
  generateDictTable(tableAM);

}

Dict2RLE::~Dict2RLE()
{
  delete rb;
}
	
Block* Dict2RLE::getNextValBlock(int colIndex_) {
  if (colIndex_!=0) return NULL;
  //assume int val for now ... fix later
  IntValPos* p = new IntValPos();
  bb1 = (BasicBlock*)m_input->getNextValBlock(m_ColIndex);
  bb2 = (BasicBlock*)m_input2->getNextValBlock(m_ColIndex2);
  if (bb1) {
    ValPos* vp = bb1->getStartPair();
    //assume int val for now ... fix later
    assert(vp->type == ValPos::INTTYPE);
    p->set(currpos, vp->value);
    //p->value = dict[bb1->getStartPair()->value];
    //p->position = currpos;
    currpos++;
  }
  else return NULL;
  ValPos* vp2 = bb1->getStartPair();
  //assume int val for now ... fix later
  assert(vp2->type == ValPos::INTTYPE);
  rb->setTriple(new RLETriple(p, p->position, *(int*)vp2->value));
  return rb;
}
	
PosBlock* Dict2RLE::getNextPosBlock(int colIndex_) {
  throw new UnimplementedException("Not done yet");
  if (colIndex_!=0) return NULL;
  return NULL;	
}


void Dict2RLE::generateDictTable(AM* dictTableAM) {
  IntDataSource* ds = new IntDataSource(dictTableAM, false, true);
  ds->setSingles(true);
  int reallastpos = ds->getLastPosition();
  BasicBlock* block=(BasicBlock*) ds->getNextValBlock(0);

  dict = new unsigned int[reallastpos];

  unsigned int curpos = 0;
  while (block!=NULL) {
    ValPos* vp = block->getStartPair();
    //assume int type for now ... fix later
    assert(vp->type == ValPos::INTTYPE);
      unsigned int value=*(int*)vp->value;
      curpos = (block->getStartPair()->position)-1;
        //cout << "pos is: " << curpos << endl;
      dict[curpos]=value;
      //Log::writeToLog("DictDataSource", 1, "initTable(): got Entry", curpos);					
      //Log::writeToLog("DictDataSource", 1, "initTable(): for value", value);
      
      block=(BasicBlock*) ds->getNextValBlock(0);
      //to fix strange bug where dictionary table page gets read in twice from the ROSAM (should probably fix for real at some point)
      //if (block->getStartPair()->position == 1)
      //block=NULL;
    //curpos++;
  }
}
