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
#include "SelExpDM.h"

SelExpDM::SelExpDM() {
}

SelExpDM::~SelExpDM() {
}

bool SelExpDM::run(Globals* g, const vector<string>& args) {
  /*if(args.size()<1){
    cout << "at least one argument! " <<endl;
    return false;
    }*/
  _file = args[0].c_str();
  //_file = "/data/eoneil/mycstore/tpch/dbgen/scale10/final/D1.play5";
  //unsigned int fieldSize=(unsigned int)atoi(args[1].c_str());

  cout << "SelExpDM: Making file " << _file << endl;
  int i=0;
  for (; i< 100; i++)
    if (strncmp(_file+i, "sparsetest",10) == 0) {
      break;
    }

  char* temp = new char[30];
  sprintf(temp, "SEInt%s", (_file+i+11));
  char* temp2 = new char[30];
  sprintf(temp2, "SESparse%s", (_file+i+11));
  char* temp3 = new char[30];
  sprintf(temp3, "SEBSparse%s", (_file+i+11));
  char* temp4 = new char[30];
  sprintf(temp4, "SERSparse%s", (_file+i+11));
  /*char* temp3 = new char[30];
  sprintf(temp3, "test1Type2%s", (_file+i+8));
  char* temp4 = new char[30];
  sprintf(temp4, "test1RLE%s", (_file+i+8));
  char* temp5 = new char[30];
  sprintf(temp5, "test1first%s", (_file+i+8));
  char* temp7 = new char[30];
  sprintf(temp7, "test1DictC%sTable", (_file+i+8));*/


  bool success=true;
  
  ColumnExtracter* ce ;
  PagePlacer* pagePlacer;
  /*ce = new ColumnExtracter(_file, 1, g->force_rebuild);
  RLEEncoder* encoder2=new RLEEncoder((Operator*) ce, 0, 8*PAGE_SIZE, (byte) INT_VALUE_TYPE, (short) 24, (unsigned int) 32, (unsigned int) 32);
  RLEDecoder* decoder2=new RLEDecoder(true);
  pagePlacer=new PagePlacer(encoder2, decoder2, 2, true);
  pagePlacer->placeColumn(temp5, false, true);*/

  cout << "Int column" << endl;

  /*ce = new ColumnExtracter(_file, 1, g->force_rebuild);
  IntEncoder* encoder=new IntEncoder((Operator*) ce, 0, 8*PAGE_SIZE);
  IntDecoder* decoder=new IntDecoder(false);
  pagePlacer=new PagePlacer(encoder, decoder, 1, true);
  pagePlacer->placeColumn(temp, false, false);
  delete pagePlacer;

  cout << "Sparse column" << endl;

  ce = new ColumnExtracter(_file, 1, g->force_rebuild);
  SparseEncoder* encoder2=new SparseEncoder((Operator*) ce, 0, sizeof(int), 8*PAGE_SIZE);
  SparseDecoder* decoder2=new SparseDecoder(false, ValPos::INTTYPE, ssize);
  pagePlacer=new PagePlacer(encoder2, decoder2, 1, true);
  pagePlacer->placeColumn(temp2, false, false);
  delete pagePlacer;

  cout << "BitSparse column" << endl;
  ce = new ColumnExtracter(_file, 1, g->force_rebuild);
  BitSparseEncoder* encoder3=new BitSparseEncoder((Operator*) ce, 0, sizeof(int), 8*PAGE_SIZE);
  BitSparseDecoder* decoder3=new BitSparseDecoder(false, ValPos::INTTYPE, sizeof(int));
  pagePlacer=new PagePlacer(encoder3, decoder3, 1, true);
  pagePlacer->placeColumn(temp3, false, false);
  delete pagePlacer;*/

  cout << "RangeSparse column" << endl;
  ce = new ColumnExtracter(_file, 1, g->force_rebuild);
  RangeSparseEncoder* encoder4=new RangeSparseEncoder((Operator*) ce, 0, sizeof(int), 8*PAGE_SIZE);
  RangeSparseDecoder* decoder4=new RangeSparseDecoder(false, ValPos::INTTYPE, sizeof(int));
  pagePlacer=new PagePlacer(encoder4, decoder4, 1, true);
  pagePlacer->placeColumn(temp4, false, false);
  delete pagePlacer;

  /*
  cout << "RLE column" << endl;

  ce = new ColumnExtracter(_file, 2, g->force_rebuild);
  RLEEncoder* encoder3=new RLEEncoder((Operator*) ce, 0, 8*PAGE_SIZE, (byte) INT_VALUE_TYPE, (short) 8, (unsigned int) 32, (unsigned int) 32);
  RLEDecoder* decoder3=new RLEDecoder(false);
  pagePlacer=new PagePlacer(encoder3, decoder3, 1, true);
  pagePlacer->placeColumn(temp4, false, false);
  delete pagePlacer;

  cout << "Type2 column" << endl;

  ce = new ColumnExtracter(_file, 2, g->force_rebuild);
  Type2Encoder* encoder4=new Type2Encoder((Operator*) ce, 0, 6);
  Type2Decoder* decoder4=new Type2Decoder(false);
  pagePlacer=new PagePlacer(encoder4, decoder4, 1, false);
  pagePlacer->placeColumn(temp3, true, false);
  delete pagePlacer;

  cout << "DictC column" << endl;

  ce = new ColumnExtracter(_file, 2, g->force_rebuild);
  DictByteEncoder* dictEncoder=new DictByteEncoder((Operator*) ce,0, fieldSize);
  BitDecoder* bitDecoder=new BitDecoder(false);
  pagePlacer=new PagePlacer(dictEncoder, bitDecoder, 1, true);
  pagePlacer->placeColumn(temp2, false, false);

  IntEncoder* encoder5=new IntEncoder(dictEncoder->getTableDataSource(),
										dictEncoder->getTableDataSourceColIndex(), 
										8*PAGE_SIZE);										
	IntDecoder* decoder5=new IntDecoder(false);	

	PagePlacer* pagePlacer2=new PagePlacer(encoder5, decoder5, 2, false);
	pagePlacer2->placeColumn(temp7, false, true);
  */
  /*  int i=0;
  for (; i< 60; i++)
    if (strncmp(_file+i, "dnatest",7) == 0) {
      break;
      }*/

  char* temp6 = new char[30];
    //sprintf(temp, "joinexpInt%s", (_file+i+8));
    //pagePlacer->placeColumn(temp, false, false);
    //sprintf(temp, "mv joinexp* ../" RUNTIME_DATA, (_file+i+8));
    /*sprintf(temp6, "mv SEInt??.* " RUNTIME_DATA);
      system(temp6);
    sprintf(temp6, "mv SESparse??.* " RUNTIME_DATA);
    system(temp6);
    sprintf(temp6, "mv SEBSparse??.* " RUNTIME_DATA);
    system(temp6);*/
    sprintf(temp6, "mv SERSparse??.* " RUNTIME_DATA);
    system(temp6);
    //delete pagePlacer;
    //delete decoder;

  return success;	
}
