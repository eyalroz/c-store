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
#include "Dict2DataMaker.h"

Dict2DataMaker::Dict2DataMaker() {
}


Dict2DataMaker::~Dict2DataMaker() {
}

bool Dict2DataMaker::run(Globals* g, const vector<string>& args) {
  if(args.size()<1){
    cout << "at least one argument! " <<endl;
    return false;
  }
  _file = args[0].c_str();
  unsigned int fieldSize=(unsigned int)atoi(args[1].c_str());

  cout << "Dict2DataMaker: Making file " << _file << endl;
    	bool success=true;
  
	//string path=DELTAPOS_TEST_BIG;
	ColumnExtracter* ce = new ColumnExtracter(_file, 1, g->force_rebuild);
	DictEncoder* dictEncoder=new DictEncoder((Operator*) ce,0, fieldSize);

  int i=0;
  for (; i< 60; i++)
    if (strncmp(_file+i, "dnatest",7) == 0) {
      break;
    }

  char* temp = new char[30];
  sprintf(temp, "test2Dict%sEntries", (_file+i+8));

  ofstream Afile;
  Afile.open(temp, ios::out | ios::app | ios::binary);
  
  byte* page=dictEncoder->getPage();
  while (page!=NULL) {
    //cout << "hi dan" << endl;
    Afile.write((char*)page, PAGE_SIZE);
    //Afile.seekp(0, ios::end);
    page=dictEncoder->getPage();
  }
  
  
  Afile.close();

  IntEncoder* encoder=new IntEncoder(dictEncoder->getTableDataSource(),
				     dictEncoder->getTableDataSourceColIndex(),
				     8*PAGE_SIZE);			       

  IntDecoder* decoder=new IntDecoder(false);

  PagePlacer* pagePlacer2=new PagePlacer(encoder, decoder, 2, false);
  sprintf(temp, "test2Dict%sTable", (_file+i+8));
  pagePlacer2->placeColumn(temp, false, true);
  sprintf(temp, "mv test2Dict%sTable* " RUNTIME_DATA, (_file+i+8));
  system(temp);
  sprintf(temp, "mv test2Dict%sEntries " RUNTIME_DATA, (_file+i+8));
  system(temp);

  delete pagePlacer2;

  //delete pagePlacer;
  //delete decoder;

  return success;	
}
