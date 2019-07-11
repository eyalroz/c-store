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
#include "SColumnExtracter.h"

SColumnExtracter::SColumnExtracter(string fileName, int c_index, int stringSize_, bool FORCE_REBUILD)
{
  sbb = new SBasicBlock(true, true, true);
  p = new SPair();
  svp = new StringValPos(0, (unsigned short)stringSize_);
  bb = new BasicBlock(true, true, true, svp);
  curPos = 1; 
  stringSize=stringSize_;
  curVal=new char[stringSize];

  string sortedColumn = extractColumn( fileName, c_index, FORCE_REBUILD );
  infile = new ifstream( sortedColumn.c_str());


}

string SColumnExtracter::extractColumn( string fileName, int index, bool FORCE_REBUILD )
{
  char* buff = new char[256];
  if (FORCE_REBUILD)
    sprintf(buff, "cat %s | awk '{print $%d}' > %s.%d", fileName.c_str(), index, fileName.c_str(), index);
  else
    sprintf(buff, "./filechecker.sh %s %d %s.%d", fileName.c_str(), index, fileName.c_str(), index);

  system(buff);
  sprintf(buff, "%s.%d", fileName.c_str(), index);
  string s = buff;
  delete[] buff;
  return s;
}


SColumnExtracter::~SColumnExtracter()
{
  delete sbb;
  delete infile;
  delete p;
  delete[] curVal;
  delete svp;
  delete bb;
}
	
Block* SColumnExtracter::getNextValBlock(int colIndex_) {
  if (colIndex_!=0) return NULL;
  string line;
  int size;
  if (!(getline (*infile, line)))
    return NULL;
  size = line.size();
  memset(svp->value+size, 0, stringSize-size);
  memcpy(svp->value, line.c_str(), size );
  //p->value = curVal;
  svp->position = curPos;
  curPos++;
  bb->setValue(svp);
  return bb;

}
	
SBlock* SColumnExtracter::getNextSValBlock(int colIndex_) {
  if (colIndex_!=0) return NULL;
  string line;
  if (!(getline (*infile, line)))
    return NULL;
  memcpy(curVal, line.c_str(), stringSize );
  p->value = curVal;
  p->position = curPos;
  curPos++;
  sbb->setValue(p);
  return sbb;
}
	
PosBlock* SColumnExtracter::getNextPosBlock(int colIndex_) {
	if (colIndex_!=0) return NULL;
	return NULL;	
}
	
