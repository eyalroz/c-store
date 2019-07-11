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
#include "ShortCutDict2RLE.h"

ShortCutDict2RLE::ShortCutDict2RLE(Operator *inOp_, int inColIndex_, Operator *inOp2_, int inColIndex2_,AM* tableAM, int fs) : m_input(inOp_), m_ColIndex(inColIndex_), m_input2(inOp2_), m_ColIndex2(inColIndex2_), fieldSize(fs)
{
  bb1 = NULL;
  bb2 = NULL;
  rb = new RLEBlock(false);
  pageToDecode = new int[4];
  pageToDecode[0]=fs;
  if (fieldSize >16 || fieldSize == 0)
    throw new UnexpectedException("fieldSize must be <= 16 bits");
  if (fieldSize <=4)
    fieldBytes = 1;
  else fieldBytes = 2;

  numvalsperint = (fieldBytes*8) / fieldSize;
  if (fieldBytes == 1)
    numvalsperint*=4;
  else
    numvalsperint*=2;
  currpos=1;
  pageToDecode[2]=numvalsperint;
  dictDecoder = new DictMultiDecoder(tableAM, false);
  //m_areInputBlocksInitialized = false;
}

ShortCutDict2RLE::~ShortCutDict2RLE()
{
  delete rb;
}
	
Block* ShortCutDict2RLE::getNextValBlock(int colIndex_) {
  if (colIndex_!=0) return NULL;
  if (!bb1) {
    bb1 = (BasicBlock*)m_input->getNextValBlock(m_ColIndex);
    bb2 = (BasicBlock*)m_input2->getNextValBlock(m_ColIndex2);
    if (bb1) {
      pageToDecode[1] = currpos;
      ValPos* vp = bb1->getStartPair();
      //assume int type for now ... fix later
      assert(vp->type == ValPos::INTTYPE);
      pageToDecode[3] = *(int*)vp->value;
      dictDecoder->setBuffer((byte*)pageToDecode);
      currpos+=numvalsperint;
      mb3 = dictDecoder->getNextBlock();
    }
    else return NULL;
  }
  ValPos* p = mb3->getNext();
  //cout << "Writing triple: " << p->value << " " <<  p->position << " " << bb2->getStartPair()->value << endl;
  ValPos* p2 = bb2->getStartPair();
  //assume int type for now ... fix later
  assert(p2->type == ValPos::INTTYPE);
  rb->setTriple(new RLETriple(p, p->position, *(int*)p2->value));
  if (!mb3->hasNext()) {
    bb1=NULL;
    bb2=NULL;
  }
  return rb;
}
	
PosBlock* ShortCutDict2RLE::getNextPosBlock(int colIndex_) {
  throw new UnimplementedException("Not done yet");
  if (colIndex_!=0) return NULL;
  return NULL;	
}
