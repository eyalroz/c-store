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
#include "DictCPUDataRunner.h"

DictCPUDataRunner::DictCPUDataRunner() {
}


DictCPUDataRunner::~DictCPUDataRunner() {
}

bool DictCPUDataRunner::run(Globals* g, const vector<string>& args) {
  if(args.size()<2){
    cout << "at least two arguments! " <<endl;
    return false;
  }
  _file = args[0].c_str();
  int testnum = atoi(args[1].c_str());

  cout << "DictCPUDataRunner: Making file " << _file << endl;
    	bool success=true;
  

  int i=0;
  for (; i< 30; i++)
    if (strncmp(_file+i, "dnatest",7) == 0) {
      break;
    }

  /*char* temp1 = new char[30];
  sprintf(temp1, "test%dDict%sEntries", testnum, (_file+i+8));
  char* temp2 = new char[30];
  sprintf(temp2, "test%dDict%sTable", testnum, (_file+i+8));*/
  char* temp3 = new char[30];
  sprintf(temp3, "test%dDictC%s", testnum, (_file+i+8));
  /*char* temp4 = new char[30];
  sprintf(temp4, "tsst1Dict%sEntries", (_file+i+8));
  char* temp5 = new char[30];
  sprintf(temp5, "tsst1Dict%sTable", (_file+i+8));*/
  char* fn1 = new char[30];
  sprintf(fn1, "test1DictC%sEntries", (_file+i+8));
  char* fn2 = new char[30];
  sprintf(fn2, "test1DictC%sTable", (_file+i+8));
  /*char* fn4 = new char[30];
  sprintf(fn4, "tsst1Dict%sEntries", (_file+i+8));
  char* fn5 = new char[30];
  sprintf(fn5, "tsst1Dict%sTable", (_file+i+8));*/

  StopWatch stopWatch;
  stopWatch.start();
  
  ROSAM* am1 = new ROSAM( fn1, 1);
  ROSAM* am2 = new ROSAM( fn2, 1);
  //ROSAM* am4 = new ROSAM( fn4, 1);
  //ROSAM* am5 = new ROSAM( fn5, 1);
  DictCPUDataSource* ds=new DictCPUDataSource(am2, am1, false);
  ds->setDecodingType(1);
  //DictDataSource* ds2=new DictDataSource(am5, am4, false);
  HashCount* hashAgg = new HashCount((Operator*) ds, 0, (Operator*) ds, 0);
  hashAgg->setHashFunction(new IdentityHashFunction(atoi(_file+i+8) + 1));
  hashAgg->setHashTableSize(atoi(_file+i+8) + 1);
  Dict2RLE* di = new Dict2RLE((Operator*) hashAgg, 0, (Operator*) hashAgg, 1, am2);
  HashSum* hashAgg2 = new HashSum((Operator*) di, 0, (Operator*) di, 0);
  hashAgg2->setHashFunction(new IdentityHashFunction(atoi(_file+i+8) + 1));
  hashAgg2->setHashTableSize(atoi(_file+i+8) + 1);
  
  //Ident* id = new Ident((Operator*) hashAgg, 1);
  Operator* srcs[1]={hashAgg2};
  int numCols[1]={2};
  
  char* temp6 = new char[30];
  sprintf(temp6, "%s.out", temp3);
  BlockPrinter* bPrint=new BlockPrinter(srcs, numCols, 1, temp6);
  bPrint->printColumns();
  
  
  sprintf(temp6, "test%dDictC.dat", testnum);
  ofstream flStream;
  flStream.open(temp6, ios::out | ios::app);
  flStream.seekp(ios::end);
  flStream << "xval: " << (_file+i+8) << endl;
  stopWatch.stopToFile(temp6);
  flStream.close();

  return success;	
}
