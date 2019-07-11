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
#include "SelExpType2DataRunner.h"

SelExpType2DataRunner::SelExpType2DataRunner() {
}


SelExpType2DataRunner::~SelExpType2DataRunner() {
}

bool SelExpType2DataRunner::run(Globals* g, const vector<string>& args) {
  if(args.size()<2){
    cout << "at least two arguments! " <<endl;
    return false;
  }
  _file = args[0].c_str();
  int testnum = atoi(args[1].c_str());

  int i=0;
  for (; i< 100; i++)
    if (strncmp(_file+i, "D1.data",7) == 0) {
      break;
    }

  char* temp = new char[30];
  sprintf(temp, "test1Int%s", (_file+i+8));
  char* temp2 = new char[30];
  sprintf(temp2, "test1DictC%sEntries", (_file+i+8));
  char* temp3 = new char[30];
  sprintf(temp3, "test1Type2%sVals", (_file+i+8));
  char* temp4 = new char[30];
  sprintf(temp4, "test1RLE%s", (_file+i+8));
  char* temp5 = new char[30];
  sprintf(temp5, "test1first%s", (_file+i+8));
  char* temp7 = new char[30];
  sprintf(temp7, "test1DictC%sTable", (_file+i+8));

  cout << "SelExpType2DataRunner: Running file " << _file << endl;
    	bool success=true;
  
  StopWatch stopWatch;
  stopWatch.start();

  ROSAM* am4 = new ROSAM(temp5, 2);
    RLEDataSource* lreturn = new RLEDataSource(am4, true);


    /*ROSAM* am3 = new ROSAM(temp, 1);
      IntDataSource* suppvar=new IntDataSource(am3, false, true);*/

  /*ROSAM* am3 = new ROSAM(temp4, 1);
    RLEDataSource* suppvar=new RLEDataSource(am3, false);*/

    ROSAM* am3 = new ROSAM(temp3, 1);
    UncompressedDecoder* dec = new UncompressedDecoder(false, ValPos::INTTYPE);
    Type2DataSource* suppvar=new Type2DataSource(am3, false, dec);



  Predicate* pred2 = new Predicate(Predicate::OP_EQUAL);
  lreturn->setPredicate(pred2);
  lreturn->changeRHSBinding(65);
  suppvar->setPositionFilter(lreturn, 0);

  /*  Predicate* pred2 = new Predicate(Predicate::OP_EQUAL);
  suppvar->setPredicate(pred2);
  suppvar->changeRHSBinding(1);
  lreturn->setPositionFilter(suppvar, 0);*/



  HashCount* hashAgg = new HashCount((Operator*) suppvar, 0, (Operator*) suppvar, 0);
  hashAgg->setHashFunction(new IdentityHashFunction(100001));
  hashAgg->setHashTableSize(100001);
  Operator* srcs[1]={hashAgg};
  //Operator* srcs[2]={suppvar2,suppkey2};
  int numCols[1]={2};
  //int numCols[2]={1,1};
  
  char* temp6 = new char[30];
  sprintf(temp6, "test1Type2");
  char* temp8 = new char[30];
  sprintf(temp8, "%s.out", temp6);
  BlockPrinter* bPrint=new BlockPrinter(srcs, numCols, 1, temp8);
  bPrint->printColumns();
  
  
  sprintf(temp8, "test%dType2.dat", testnum);
  ofstream flStream;
  flStream.open(temp8, ios::out | ios::app);
  flStream.seekp(ios::end);
  flStream << "xval: " << (_file+i+8) << endl;
  stopWatch.stopToFile(temp8);
  flStream.close();
  
  cout << "Query took: " << stopWatch.stop() << " ms" <<  endl;
  
  return success;	
}
