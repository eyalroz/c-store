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
#include "SBlockPrinter.h"
using namespace std;

SBlockPrinter::SBlockPrinter(Operator* dataSrc_, int numSrcs_, const char* fileName_)
{
	dataSrc=dataSrc_;
	//numCols=numCols_;
	numSrcs=numSrcs_;
	nprinted = 0;

	if (fileName_==NULL) {
		stdOut=true;					
	}
	else {
		outstream.open(fileName_);
		stdOut=false;
	}
}

SBlockPrinter::~SBlockPrinter()
{
	outstream.close();	
}

void SBlockPrinter::printColumns(bool skip_output) {
  //int totalNumCols=0;
  //int currCol;
  bool success=true;
  /*for (int i=0; i<numSrcs; i++) {
    totalNumCols+=numCols[i];
  }
  blks = new Block*[totalNumCols];
  for (int i=0;i<totalNumCols;i++)
    blks[i]=NULL;
  //for (int i=0; i<numSrcs; i++) {
  //  blks[i] = new Block*[numCols[i]];
  //}
  while (success) {
    currCol=0;
    for (int i=0; i<numSrcs; i++) {
      for (int j=0; j < numCols[i]; j++) {
	success&=printEntryForColumn(dataSrc[i], j, currCol, currCol==totalNumCols-1); 
	currCol++;
      }
    }
	++nprinted;
	}*/
  while (success) {
    success=printEntryForColumn(skip_output); 
  }
  
}

/*void SBlockPrinter::printColumnsWithPosition() {
  for (int i=0; i<numSrcs; i++) {
    printColumnsWithPositionForSrc(dataSrc[i], numCols[i]); 
  }  
  }*/

bool SBlockPrinter::printEntryForColumn(bool skip_output) {
  SBlock* sb;
  SPair* p;
  int* temp;
  while ((sb = dataSrc->getNextSValBlock(0))) {
    while (sb->hasNext()) {
      p = sb->getNext();
      temp = (int*)(p->value);
	  if (!skip_output) {
	      for (int i=1; i < numSrcs-1; i++) {
				outstream << temp[i] << ",";
	      }
	      outstream << temp[numSrcs-1] << endl;
	  }
      ++nprinted;
    }
  }
  return false;
}
