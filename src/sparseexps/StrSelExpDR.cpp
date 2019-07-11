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
#include "StrSelExpDR.h"

StrSelExpDR::StrSelExpDR() {
}


StrSelExpDR::~StrSelExpDR() {
}

bool StrSelExpDR::run(Globals* g, const vector<string>& args) {
  if(args.size()<2){
    cout << "at least two arguments! " <<endl;
    return false;
  }
  _file = args[0].c_str();
  //int testnum = atoi(args[1].c_str());
  int whichtest = atoi(args[1].c_str());

  cout << "StrSelExpDR: Running file " << _file << endl;
  int i=0;
  for (; i< 100; i++)
    if (strncmp(_file+i, "sparsetest",10) == 0) {
      break;
    }

  int ssize=12;

  char* temp = new char[30];
  sprintf(temp, "SEString%s", (_file+i+11));
  char* temp2 = new char[30];
  sprintf(temp2, "SESSparse%s", (_file+i+11));
  char* temp3 = new char[30];
  sprintf(temp3, "SESBSparse%s", (_file+i+11));
  char* temp4 = new char[30];
  sprintf(temp4, "SESRSparse%s", (_file+i+11));
  /*char* temp3 = new char[30];
  sprintf(temp3, "test1Type2%s", (_file+i+8));
  char* temp4 = new char[30];
  sprintf(temp4, "test1RLE%s", (_file+i+8));
  char* temp5 = new char[30];
  sprintf(temp5, "test1first%s", (_file+i+8));
  char* temp7 = new char[30];
  sprintf(temp7, "test1DictC%sTable", (_file+i+8));*/

  bool success=true;
	
  StopWatch stopWatch;
  stopWatch.start();

  ROSAM* am4 = new ROSAM("test1RLE2", 1);
  RLEDataSource* lreturn = new RLEDataSource(am4, false);


  ROSAM* am3 = new ROSAM(temp, 1);
  ROSAM* am5 = new ROSAM(temp2, 1);
  ROSAM* am6 = new ROSAM(temp3, 1);
  ROSAM* am7 = new ROSAM(temp4, 1);

  DataSource* col1;

  Decoder* decoder;

  if (whichtest==0) {
    decoder=new UncompressedDecoder(false, ValPos::STRINGTYPE);
    col1=new DataSource(am3, true, false, decoder);
  }
  else if (whichtest==1) {
    decoder=new SparseDecoder(false, ValPos::STRINGTYPE, ssize);
    col1=new DataSource(am5, true, false, decoder);
  }
  else if (whichtest==2) {
    decoder=new BitSparseDecoder(false, ValPos::STRINGTYPE, ssize);
    col1=new DataSource(am6, true, false, decoder);
  }
  else if (whichtest==3) {
    decoder=new RangeSparseDecoder(false, ValPos::STRINGTYPE, ssize);
    col1=new DataSource(am7, true, false, decoder);
    }
  else {
    cout << "Illegal test." << endl;
    exit(1);
  }

  /*ROSAM* am3 = new ROSAM(temp4, 1);
    RLEDataSource* suppvar=new RLEDataSource(am3, false);*/

    /*ROSAM* am3 = new ROSAM( "selexpquantvarVals", 1);
      Type2DataSource* suppvar=new Type2DataSource(am3, false);*/



  Predicate* pred2 = new Predicate(Predicate::OP_EQUAL);
  col1->setPredicate(pred2);
  char* x = new char[ssize];
  memset(x,0,ssize);
  memcpy(x,"49997",5);
  //int rhsint = 49996;
  StringValPos* vp = new StringValPos(0, ssize);
  vp->set((byte*)x);
  col1->changeRHSBinding(vp);

  lreturn->setPositionFilter(col1, 0);
  /*suppvar->setPositionFilter(lreturn, 0);*/

  /*Predicate* pred2 = new Predicate(Predicate::OP_EQUAL);
  suppvar->setPredicate(pred2);
  suppvar->changeRHSBinding(1);
  lreturn->setPositionFilter(suppvar, 0);*/



  /*HashCount* hashAgg = new HashCount((Operator*) suppvar, 0, (Operator*) suppvar, 0);
  hashAgg->setHashFunction(new IdentityHashFunction(100001));
  hashAgg->setHashTableSize(100001);*/

  //Operator* srcs[1]={hashAgg};
  Operator* srcs[1]={lreturn};
  //Operator* srcs[2]={suppvar2,suppkey2};
  //int numCols[1]={2};
  int numCols[1]={1};
  //int numCols[2]={1,1};
  
  char* temp6 = new char[30];
  sprintf(temp6, "SE");
  char* temp8 = new char[30];
  if (whichtest==0)
    sprintf(temp8, "%sString.out", temp6);
  else if (whichtest==1)
    sprintf(temp8, "%sSSparse.out", temp6);
  else if (whichtest==2)
    sprintf(temp8, "%sSBSparse.out", temp6);
  else if (whichtest==3)
    sprintf(temp8, "%sSRSparse.out", temp6);
  else
    assert(false);
  

  BlockPrinter* bPrint=new BlockPrinter(srcs, numCols, 1, temp8);
  bPrint->printColumns();
  
  
  if (whichtest==0)
    sprintf(temp8, "SEString.dat");
  else if (whichtest==1)
    sprintf(temp8, "SESSparse.dat");
  else if (whichtest==2)
    sprintf(temp8, "SESBSparse.dat");
  else if (whichtest==3)
    sprintf(temp8, "SESRSparse.dat");
  else
    assert(false);
  ofstream flStream;
  flStream.open(temp8, ios::out | ios::app);
  flStream.seekp(ios::end);
  flStream << "xval: " << atoi(_file+i+11) << endl;
  stopWatch.stopToFile(temp8);
  flStream.close();
  
  //cout << "Query took: " << stopWatch.stop() << " ms" <<  endl;
  delete[] temp;
  delete[] temp2;
  delete[] temp3;
  delete[] temp4;
  delete[] temp6;
  delete[] temp8;
  delete bPrint;
  delete lreturn;
  delete col1;
  delete decoder;
  delete pred2;
  delete am3;
  delete am4;
  delete am5;
  delete am6;
  delete am7;
  return success;	
}
