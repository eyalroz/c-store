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
#include "BlockPrinter.h"

BlockPrinter::BlockPrinter(Operator* dataSrc_[], int numCols_[], int numSrcs_, const char* fileName_)
{
	using namespace std;
	dataSrc=dataSrc_;
	numCols=numCols_;
	numSrcs=numSrcs_;
	nprinted = 0;
	blks = NULL;

	if (fileName_==NULL) {
		stdOut=true;					
	}
	else {
		outstream.open(fileName_);  
		outstream.precision(15);

		stdOut=false;
	}
}

BlockPrinter::~BlockPrinter()
{
	outstream.close();	
}

void BlockPrinter::printColumns(bool skip_output) {
  int totalNumCols=0;
  int currCol;
  bool success=true;
  for (int i=0; i<numSrcs; i++) {
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
		success &= printEntryForColumn(dataSrc[i], j, currCol, currCol==totalNumCols-1, skip_output); 
		currCol++;
      }
    }
	++nprinted;
  }
  delete[] blks;
}

void BlockPrinter::printColumnsWithPosition() {
  for (int i=0; i<numSrcs; i++) {
    printColumnsWithPositionForSrc(dataSrc[i], numCols[i]); 
  }  
}

 bool BlockPrinter::printEntryForColumn(Operator* dataSrc_, int currCol, int currTotalCol, bool isLast, bool skip_output) {
  using namespace std;
  
  if (dataSrc_==NULL) return false;
  if ((blks[currTotalCol] == NULL) || (!blks[currTotalCol]->hasNext())) {
    blks[currTotalCol]=dataSrc_->getNextValBlock(currCol);
    if (blks[currTotalCol] == NULL)
      return false;
  }
  ValPos* pair=blks[currTotalCol]->getNext();
// DSM disable printing
	if (!skip_output) {
	  if (stdOut) {
	    pair->printVal(&cout);
	    if (isLast) 
	      cout << endl;
	    else
	      cout << ",";
	    }
	  else 
	    {
	    pair->printVal(&outstream);
	    if (isLast) 
	      outstream << endl;
	    else
	      outstream << ",";
	    }
	}

  //BUG FIX: hasNext might be false now, but true next time this is called since the data blks[currTotalCol] is pointing to can be updated. However, we want to force getNextValBlock to be called to update the pointer, so put NULL in blks[currTotalCol] if !hasNext
  
  if (!blks[currTotalCol]->hasNext()) 
    blks[currTotalCol] = NULL;
  return true;

 }
void BlockPrinter::printColumnsForSrc(Operator* dataSrc_, int numCols_) {
  using namespace std;
  
  if (dataSrc_==NULL) return;
  Block* blks[numCols_];
  for (int i=0; i<numCols_; i++) {
    blks[i]=dataSrc_->getNextValBlock(i);
  }
  while (blks[0]!=NULL) {
    for (int j=0; j<blks[0]->getSize(); j++) {
      for (int col=0; col<numCols_-1; col++) {
	ValPos* pair=blks[col]->getNext();

	if (stdOut) {
	  pair->printVal(&cout);
	  cout << ",";
	}
	else 
	  {
	    //int before = StopWatch::ticks();
	    pair->printVal(&outstream);
	    outstream << ",";
	    //int after = StopWatch::ticks();
	    //totalPrint+= (after-before)*StopWatch::secondsPerTick();
	    //cout << (after-before ) << " -------> " << (after-before ) << " seconds per tick? " << StopWatch::secondsPerTick() << endl;				  
	  }
      }	
      ValPos* pair=blks[numCols_-1]->getNext();
      if (stdOut) {
	pair->printVal(&cout);
	cout << endl;
      }
      else 
	{
	  pair->printVal(&outstream);
	  //int before = StopWatch::ticks();
	  outstream << endl;
	  //int after = StopWatch::ticks();
	  
	  //totalPrint+= (after-before)*StopWatch::secondsPerTick();
	  //cout << (after-before ) << " -------> " << (after-before ) << endl;
	}
    }
    for (int i=0; i<numCols_; i++) {
      blks[i]=dataSrc_->getNextValBlock(i);
    }	
  }
  
  //cout << " Found " << totalPrint << " Seconds for printing itself " << endl << " VS COUNT " << count << endl;
}

void BlockPrinter::printColumnsWithPositionForSrc(Operator* dataSrc_, int numCols_) {
  using namespace std;

  if (dataSrc_==NULL) return;
  Block* blks[numCols_];
  for (int i=0; i<numCols_; i++) {
    blks[i]=dataSrc_->getNextValBlock(i);
  }
  while (blks[0]!=NULL) {
    for (int j=0; j<blks[0]->getSize(); j++) {
      for (int col=0; col<numCols_-1; col++) {
        ValPos* pair=blks[col]->getNext();
        if (stdOut) {
	  cout << "(";
	  pair->printVal(&cout);
          cout << "," <<  pair->position << ")"<< "\t\t";
	}
        else 
        {
          outstream << "(";
	  pair->printVal(&outstream);
	  cout << "," <<  pair->position << ")"<< "\t\t";
        }
      } 
      ValPos* pair=blks[numCols_-1]->getNext();
      if (stdOut) {
	cout << "(";
	pair->printVal(&cout);
	cout << "," <<  pair->position << ")"<< "\t\t" << endl;
      }
      else 
      {
        outstream << "(";
	pair->printVal(&outstream);
	cout << "," <<  pair->position << ")"<< "\t\t" << endl;
      }
    }
    for (int i=0; i<numCols_; i++) {
      blks[i]=dataSrc_->getNextValBlock(i);
    } 
  }
}
