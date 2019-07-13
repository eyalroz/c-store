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
 /**
 */
#include "DeltaPosDataRunner.h"

DeltaPosDataRunner::DeltaPosDataRunner() {
}


DeltaPosDataRunner::~DeltaPosDataRunner() {
}

bool DeltaPosDataRunner::run(Globals* g, const vector<string>& args) {
  if(args.size()<1){
    cout << "at least one argument! " <<endl;
    return false;
  }
  _file = args[0].c_str();

  cout << "DeltaPosDataRunner: Making file " << _file << endl;
    	bool success=true;
  

  int i=0;
  for (; i< 30; i++)
    if (strncmp(_file+i, "dnatest",7) == 0) {
      break;
    }

  char* temp = new char[30];
  sprintf(temp, "test1DeltaPos%sVals", (_file+i+8));

  StopWatch stopWatch;
  stopWatch.start();
  
  ROSAM* am = new ROSAM( temp, 1);
  DeltaPosDataSource* ds=new DeltaPosDataSource(am, false);
  Count* agg = new Count((Operator*) ds, 1, (Operator*) ds, 1);
  Operator* srcs[1]={agg};
  int numCols[1]={2};
  
  strcpy(temp + strlen(temp), ".out");
  BlockPrinter* bPrint=new BlockPrinter(srcs, numCols, 1, temp);
  bPrint->printColumns();
    
  ofstream flStream;
  flStream.open("test1DeltaPos.dat");
  
  flStream << "\t"<<(_file+i+8) << "\t" << stopWatch.stop() <<  endl;
  flStream.close();
  //cout << "Query took: " << stopWatch.stop() << " ms" <<  endl;

  return success;	
}
