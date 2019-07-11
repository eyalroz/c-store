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
#include "NS2DataRunner.h"

NS2DataRunner::NS2DataRunner() {
}


NS2DataRunner::~NS2DataRunner() {
}

bool NS2DataRunner::run(Globals* g, const vector<string>& args) {
  if(args.size()<2){
    cout << "at least two arguments! " <<endl;
    return false;
  }
  _file = args[0].c_str();
  int testnum = atoi(args[1].c_str());

  cout << "NS2DataRunner: Running file " << _file << endl;
    	bool success=true;
  

  int i=0;
  for (; i< 30; i++)
    if (strncmp(_file+i, "dnatest",7) == 0) {
      break;
    }

  char* temp = new char[30];
  sprintf(temp, "test%dNS%s", testnum, (_file+i+8));
  char* temp2 = new char[30];
  sprintf(temp2, "tsst%dNS%s", testnum, (_file+i+8));
  char* fn1 = new char[30];
  sprintf(fn1, "test2NS%s", (_file+i+8));
  char* fn2 = new char[30];
  sprintf(fn2, "tsst2NS%s", (_file+i+8));

  StopWatch stopWatch;
  stopWatch.start();
  
  NOBDBAM* am = new NOBDBAM( fn1, 1);
  //NOBDBAM* am2 = new NOBDBAM( fn2, 1);
  NSDataSource* ds=new NSDataSource(am, false);
  //NSDataSource* ds2=new NSDataSource(am2, false);
  HashSum* hashAgg = new HashSum((Operator*) ds, 0, (Operator*) ds, 0);
  hashAgg->setHashFunction(new IdentityHashFunction(200));
  hashAgg->setHashTableSize(200);
  Operator* srcs[1]={hashAgg};
  int numCols[1]={2};
  
  char* temp3 = new char[30];
  sprintf(temp3, "%s.out", temp);
  BlockPrinter* bPrint=new BlockPrinter(srcs, numCols, 1, temp3);
  bPrint->printColumns();
  
  sprintf(temp3, "test%dNS.dat", testnum);
  ofstream flStream;
  flStream.open(temp3, ios::out | ios::app);
  flStream.seekp(ios::end);
  flStream << "xval: " << (_file+i+8) << endl;
  stopWatch.stopToFile(temp3);
  flStream.close();
  //cout << "Query took: " << stopWatch.stop() << " ms" <<  endl;

  return success;	
}
