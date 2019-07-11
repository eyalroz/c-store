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
#include "ProjMaker.h"

ProjMaker::ProjMaker() {
}

ProjMaker::~ProjMaker() {
}

bool ProjMaker::run(Globals* g, const vector<string>& args) {
  if(args.size()<1){
    cout << "at least one argument! " <<endl;
    return false;
  }
  _file = args[0].c_str();

  bool success=true;
  char* pname = new char[256];
  memset(pname,0,256);
  char** cnames = new char*[256];
  int* ctypes = new int[256];
  for (int i = 0; i < 256; i++) {
    cnames[i] = new char[256];
    memset(cnames[i],0,256);
    ctypes[i] = 0;
  }
  cout << endl << "Welcome to the projection maker!" << endl;
  cout << "This program will take the contents of a space-delimited file" << endl;
  cout << "and convert it into a set of C-Store columns." << endl;
  cout << "You have already given me the input file: " << _file << endl;
  cout << "Right now I am assuming that every column" << endl;
  cout << "in this file are integer columns. We will release a new" << endl;
  cout << "version of C-Store that will allow columns to be any type" << endl;
  cout << "some time after the SIGMOD deadline. Currently the query" << endl;
  cout << "executer can handle any type, but we didn't have a chance" << endl;
  cout << "to update the plan generator before the release for this capability." << endl;
  cout << "Thus, for now, if you don't have integer data, you will have" << endl;
  cout << "to create the columns and query them by hand." << endl << endl;
  cout << "Please enter the projection name" << endl;
  cin >> pname;
  cout << "Projection name is: " << pname << endl;
  int numColumns=0;
  cout << "How many colunms are in this projection?" << endl;
  cin >> numColumns;
  if (numColumns < 1 || numColumns > 256) {
    cout << "Num columns must be between 1 and 256." << endl;
    return false;
  }
  for (int i = 0; i < numColumns; i++) {
    cout << "Please enter the name of column: " << (i+1) << endl;
    cin >> cnames[i];
    cout << "What is the compression type of " << cnames[i] << "?" << endl;
    cout << "No compression: 1" << endl;
    cout << "RLE compression: 2" << endl;
    cout << "Other compression types will be included in future releases." << endl;
    cin >> ctypes[i];
    if (ctypes[i] < 1 || ctypes[i] > 2) {
      cout << "Illegal type." << endl;
      return false;
    }
  }
  ifstream infile (RUNTIME_DATA "CstoreSysTblProjection.txt");
  if ( !infile.is_open() ) {
    cout << "Could not open projection file" << endl;
    return false;
  }
  char* inbuf = new char[256];
  memset(inbuf,0,256);
  int projnumber = 0;
  int lindex;
  while (infile>>inbuf) {
    int i;
    for (i = 0; i < 256; i++)
      if (inbuf[i] == '#') {
	lindex=i;
	break;
      }
    if (i == 256) {
      cout << "Bad input projection file" << endl;
      infile.close();
      return false;
    }
    for (i = (lindex+1); i < 256; i++) {
      if (inbuf[i] == '#')
	break;
    }
    if (i == 256) {
      cout << "Bad input projection file" << endl;
      infile.close();
      return false;
    }
    projnumber = atoi(inbuf+(lindex+1));
  }
  projnumber++;
  infile.close();
  ofstream outfile ( RUNTIME_DATA "CstoreSysTblProjection.txt", ios::app );
  outfile << pname << '#' << projnumber << '#' << "0#0" << endl;
  outfile.close();
  ofstream outfile2 ( RUNTIME_DATA "CstoreSysTblColumn.txt", ios::app );
  for (int i = 0; i < (numColumns); i++) {
    outfile2 << pname << '.' << cnames[i] << '#' << (i+1) << '#' << projnumber;
    outfile2 << "#1#" << ((ctypes[i] == 1)?'6':'1') <<  "#1" << endl;
  }
  outfile2.close();
  ct = new CatalogTest();
  vector<string> ctargs;
  ctargs.push_back("3");
  ctargs.push_back("CstoreSysTblProjection.txt");
  ctargs.push_back("CstoreSysTblColumn.txt");
  bool temp = ct->run(g, ctargs);
  if (!temp) {
    cout << "Failed to write catalog" << endl;
    return false;
  }
  for (int i = 0; i < (numColumns); i++) {
    ColumnExtracter* ce = new ColumnExtracter(_file, (i+1), g->force_rebuild);
    char* temp = new char[512];
    sprintf(temp, "%s.%s.ros", pname,cnames[i]);
    if (ctypes[i] == 1) {
      IntEncoder* encoder=new IntEncoder((Operator*) ce, 0, 8*PAGE_SIZE);
      IntDecoder* decoder=new IntDecoder(false);
      PagePlacer* pagePlacer=new PagePlacer(encoder, decoder, 1, true);
      pagePlacer->placeColumn(temp, false, false);
      sprintf(temp, "mv %s.%s.ros " RUNTIME_DATA, pname,cnames[i]);
      system(temp);
      delete encoder;
      delete decoder;
      delete pagePlacer;
    }
    else if (ctypes[i] ==2) {
      RLEEncoder* encoder2=new RLEEncoder((Operator*) ce, 0, 8*PAGE_SIZE, (byte) INT_VALUE_TYPE, (short) 32, (unsigned int) 32, (unsigned int) 32);
      RLEDecoder* decoder2=new RLEDecoder(false);
      PagePlacer* pagePlacer=new PagePlacer(encoder2, decoder2, 1, true);
      pagePlacer->placeColumn(temp, false, false);
      sprintf(temp, "mv %s.ros " RUNTIME_DATA, cnames[i]);
      system(temp);
    }
    else {
      cout << "Unknown columns type." << endl;
      return false;
    }
    delete ce;
    delete[] temp;
  }
  return success;	
}
