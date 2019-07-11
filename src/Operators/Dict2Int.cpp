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
#include "Dict2Int.h"

Dict2Int::Dict2Int(Operator *inOp_, int inColIndex_, AM* tableAM) : m_input(inOp_), m_ColIndex(inColIndex_)
{
  bb1 = NULL;
  bb2 = NULL;
  /*rb = new RLEBlock(false);
  pageToDecode = new int[4];
  pageToDecode[0]=fs;
  currpos=1;
  pageToDecode[2]=2;//(8*sizeof(int))/fs;
  dictDecoder = new DictMultiDecoder(tableAM, false);*/
  //m_areInputBlocksInitialized = false;
  currpos=1;
  generateDictTable(tableAM);
  bb2 = new BasicBlock(false, true, true);
  ivp = new IntValPos();
}

Dict2Int::~Dict2Int()
{
  delete [] dict;
  delete ivp;
}
	
Block* Dict2Int::getNextValBlock(int colIndex_) {
  if (colIndex_!=0) return NULL;
  bb1 = (BasicBlock*)m_input->getNextValBlock(m_ColIndex);
  //bb2 = (BasicBlock*)m_input2->getNextValBlock(m_ColIndex2);
  if (bb1) {
    //pageToDecode[1] = currpos;
    ValPos* vp = bb1->getStartPair();
    //assume int type for now ... fix later
    assert(vp->type == ValPos::INTTYPE);
    int temp = *(int*)vp->value;
    ivp->set(currpos, (byte*)&dict[temp]);
    //bb2->setValue(dict[bb1->getStartPair()->value], currpos);
    bb2->setValue(ivp);
    //dictDecoder->setBuffer((byte*)pageToDecode);
    currpos++;//((8*sizeof(int))/fieldSize);
    //mb3 = dictDecoder->getNextBlock();
  }
  else return NULL;
  //Pair* p = mb3->getNext();
  //cout << "Writing triple: " << p->value << " " <<  p->position << " " << bb2->getStartPair()->value << endl;
  //rb->setTriple(new RLETriple(p->value, p->position, bb2->getStartPair()->value));
  //if (!mb3->hasNext()) {
  //bb1=NULL;
  //bb2=NULL;
  //}
  return bb2;
}
	
PosBlock* Dict2Int::getNextPosBlock(int colIndex_) {
  throw new UnimplementedException("Not done yet");
  if (colIndex_!=0) return NULL;
  return NULL;	
}

void Dict2Int::generateDictTable(AM* dictTableAM) {
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
      curpos = (vp->position)-1;
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
