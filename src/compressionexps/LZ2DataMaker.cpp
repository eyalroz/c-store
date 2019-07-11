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
#include "LZ2DataMaker.h"
#include <fstream.h>

LZ2DataMaker::LZ2DataMaker() {
}


LZ2DataMaker::~LZ2DataMaker() {
}

bool LZ2DataMaker::run(Globals* g, const vector<string>& args) {
  if(args.size()<2){
    cout << "at least two arguments! " <<endl;
    return false;
  }
  _file = args[0].c_str();
  /*_bsize = atoi(args[1].c_str());
  
  if(_bsize <1){
    cout << " bsize must >=1 " <<endl;
    return false;
    }*/

  bool success=true;
  ofstream Afile;
  ColumnExtracter* ce2 = new ColumnExtracter(_file, 1, g->force_rebuild);

  IntEncoder* encoder2=new IntEncoder((Operator*) ce2, 0, 8*PAGE_SIZE);
  Afile.open("LZOUT.bin", ios::out | ios::app | ios::binary);

  byte* page=encoder2->getPage();
  while (page!=NULL) {
    //cout << "hi dan" << endl;
    Afile.write((char*)page, PAGE_SIZE);
    //Afile.seekp(0, ios::end);
    page=encoder2->getPage();
  }


  Afile.close();

  system("./findBlockSize.sh");

  ifstream Bfile ("bsize.out");
  if (! Bfile.is_open()) {
    cout << "Error opening file" << endl;
    exit(1);
  }
  char buffer1[256];
  Bfile.getline(buffer1, 100);
  _bsize = atoi(buffer1);
  cout << "bsize is: " << _bsize << endl;

  cout << "LZ2DataMaker: Making file " << _file << endl;
  
	//string path=DELTAPOS_TEST_BIG;
	ColumnExtracter* ce = new ColumnExtracter(_file, 1, g->force_rebuild);

	LZEncoder* encoder=new LZEncoder(ce, 0, _bsize/sizeof(int));

  int i=0;
  for (; i< 60; i++)
    if (strncmp(_file+i, "dnatest",7) == 0) {
      break;
    }

  char* temp = new char[30];
  sprintf(temp, "test2LZ%s", (_file+i+8));
  
  ofstream Afile2;
  Afile2.open(temp, ios::out | ios::app | ios::binary);
  
  byte* page2=encoder->getPage();
  while (page2!=NULL) {
    //cout << "hi dan" << endl;
    Afile2.write((char*)page2, PAGE_SIZE);
    //Afile.seekp(0, ios::end);
    page2=encoder->getPage();
  }
  
  
  Afile2.close();

  sprintf(temp, "mv test2LZ%s " RUNTIME_DATA, (_file+i+8));
  system(temp);
    
  return success;	
}
