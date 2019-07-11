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
#include "JoinExpType2DataRunner.h"

JoinExpType2DataRunner::JoinExpType2DataRunner() {
}


JoinExpType2DataRunner::~JoinExpType2DataRunner() {
}

bool JoinExpType2DataRunner::run(Globals* g, const vector<string>& args) {
  /*if(args.size()<2){
    cout << "at least two arguments! " <<endl;
    return false;
  }
  _file = args[0].c_str();
  int testnum = atoi(args[1].c_str());*/

  int testnum = atoi(args[1].c_str());
  _file = SUPP_DATA;
  _file2 = D1_RETSUPPSORT;

  cout << "JoinExpType2DataRunner: Running file " << _file << endl;
  bool success=true;
  
  /*  char* temp = new char[30];
  sprintf(temp, "test1Int%s", (_file+i+8));
  char* temp2 = new char[30];
  sprintf(temp2, "test1DictC%sEntries", (_file+i+8));
  char* temp3 = new char[30];
  sprintf(temp3, "test1Type2%s", (_file+i+8));
  char* temp4 = new char[30];
  sprintf(temp4, "test1RLE%s", (_file+i+8));
  char* temp5 = new char[30];
  sprintf(temp5, "test1first%s", (_file+i+8));
  char* temp7 = new char[30];
  sprintf(temp7, "test1DictC%sTable", (_file+i+8));*/

  StopWatch stopWatch;
  stopWatch.start();

  ROSAM* am = new ROSAM( "joinexpsuppkey", 1);
  ROSAM* am2 = new ROSAM( "joinexpchain", 2);
  IntDataSource* ods=new IntDataSource(am, false, true);
  RLEDataSource* ds1=new RLEDataSource(am2, true);
  Predicate* pred = new Predicate(Predicate::OP_EQUAL);
  ds1->setPredicate(pred);
  ds1->changeRHSBinding(0);
  ods->setPositionFilter(ds1, 0);
  //ods->setSingles(true);


  ROSAM* am4 = new ROSAM( "joinexpreturn", 2);
  RLEDataSource* lreturn = new RLEDataSource(am4, true);
  Predicate* pred2 = new Predicate(Predicate::OP_EQUAL);
  lreturn->setPredicate(pred2);
  lreturn->changeRHSBinding(78);

  /*ROSAM* am3 = new ROSAM("test1Int", 1);
  IntDataSource* suppvar=new IntDataSource(am3, false, true);
  //suppvar->setSingles(true);
  suppvar->setPositionFilter(lreturn, 0);*/

  /*ROSAM* am3 = new ROSAM( "test1RLE", 1);
  RLEDataSource* suppvar=new RLEDataSource(am3, false);
  //suppvar->setSingles(true);
  suppvar->setPositionFilter(lreturn, 0);*/

  ROSAM* am3 = new ROSAM( "test1Type2Vals", 1);
  UncompressedDecoder* dec = new UncompressedDecoder(false, ValPos::INTTYPE);
  Type2DataSource* suppvar=new Type2DataSource(am3, false, dec);
  //suppvar->setSingles(true);
  suppvar->setPositionFilter(lreturn, 0);

  /*ROSAM* am3 = new ROSAM("test1DictCEntries", 1);
  //ROSAM* am6 = new ROSAM("test1DictCTable", 1);
  DictCPUDataSource* suppvar=new DictCPUDataSource(am6, am3, false);
  suppvar->setDecodingType(1);
  suppvar->setPositionFilter(lreturn, 0);*/

  NLJoin* join1 = new NLJoin(suppvar, 0, ods, 0);
  join1->setDontCheck();

  ROSAM* am5 = new ROSAM( "joinexpname", 1);
  IntDataSource* suppname = new IntDataSource(am5, false, true);
  suppname->setSingles(true);

  //suppvar2->setPositionFilter(join1, 0);
  suppname->setPositionFilter(join1, 1);

  HashCount* hashAgg = new HashCount((Operator*) suppname, 0, (Operator*) suppname, 0);
  hashAgg->setHashFunction(new IdentityHashFunction(200));
  hashAgg->setHashTableSize(200);
  Operator* srcs[1]={hashAgg};
  //Operator* srcs[2]={suppvar2,suppkey2};
  int numCols[1]={2};
  //int numCols[2]={1,1};
  
  char* temp = new char[30];
  sprintf(temp, "test1Type2");
  char* temp3 = new char[30];
  sprintf(temp3, "%s.out", temp);
  BlockPrinter* bPrint=new BlockPrinter(srcs, numCols, 1, temp3);
  bPrint->printColumns();
  
      
  sprintf(temp3, "test%dType2.dat", testnum);
  ofstream flStream;
  flStream.open(temp3, ios::out | ios::app);
  flStream.seekp(ios::end);
  flStream << "xval: " << (atoi(args[0].c_str())) << endl;
  stopWatch.stopToFile(temp3);
  flStream.close();
  
  cout << "Query took: " << stopWatch.stop() << " ms" <<  endl;
  
  return success;	
}
