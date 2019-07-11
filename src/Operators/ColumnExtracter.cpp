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
#include "ColumnExtracter.h"

ColumnExtracter::ColumnExtracter(string fileName, int c_index, bool FORCE_REBUILD)
{
  bb = new BasicBlock(true, true, true);
  //assume int type for now ... fix later
  p = new IntValPos();
  curPos = 1; 


  string sortedColumn = extractColumn( fileName, c_index, FORCE_REBUILD );
  infile = new ifstream( sortedColumn.c_str());


}

string ColumnExtracter::extractColumn( string fileName, int index, bool FORCE_REBUILD )
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


ColumnExtracter::~ColumnExtracter()
{
  delete bb;
  delete infile;
  delete p;
  //delete bb;
  //delete infile;
}
	
Block* ColumnExtracter::getNextValBlock(int colIndex_) {
  if (colIndex_!=0) return NULL;
  string line;
  int curVal;
  if (!(getline (*infile, line)))
    return NULL;
  curVal = atoi( line.c_str() );
  p->set(curPos, (byte*)&curVal);
  //p->value = curVal;
  //p->position = curPos;
  curPos++;
  bb->setValue(p);
  return bb;
}
	
PosBlock* ColumnExtracter::getNextPosBlock(int colIndex_) {
	if (colIndex_!=0) return NULL;
	return NULL;	
}
	
