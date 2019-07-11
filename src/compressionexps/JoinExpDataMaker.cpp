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
#include "JoinExpDataMaker.h"

JoinExpDataMaker::JoinExpDataMaker() {
}

JoinExpDataMaker::~JoinExpDataMaker() {
}

bool JoinExpDataMaker::run(Globals* g, const vector<string>& args) {
  /*if(args.size()<1){
    cout << "at least one argument! " <<endl;
    return false;
    }*/
  //_file = args[0].c_str();
  _file = SUPP_DATA;
  _file2 = D1_RETSUPP50SORT;
  unsigned int fieldSize=(unsigned int)atoi(args[1].c_str());

  cout << "JoinExpDataMaker: Making file " << _file << endl;
  bool success=true;

  ColumnExtracter* ce;
  IntEncoder* encoder;
  IntDecoder* decoder;
  PagePlacer* pagePlacer;
  RLEEncoder* encoder2;
  RLEDecoder* decoder2;

  //string path=DELTAPOS_TEST_BIG;

    cout << "supp key" << endl;

  ce = new ColumnExtracter(_file, 1, g->force_rebuild);
  encoder=new IntEncoder((Operator*) ce, 0, 8*PAGE_SIZE);
  decoder=new IntDecoder(false);
  pagePlacer=new PagePlacer(encoder, decoder, 1, true);
  pagePlacer->placeColumn("joinexpsuppkey", false, false);

  cout << "supp rank" << endl;

  ce = new ColumnExtracter(_file, 2, g->force_rebuild);
  encoder=new IntEncoder((Operator*) ce, 0, 8*PAGE_SIZE);
  decoder=new IntDecoder(false);
  pagePlacer=new PagePlacer(encoder, decoder, 1, true);
  pagePlacer->placeColumn("joinexpname", false, false);

  cout << "supp chain" << endl;

  ce = new ColumnExtracter(_file, 3, g->force_rebuild);
  encoder2=new RLEEncoder((Operator*) ce, 0, 8*PAGE_SIZE, (byte) INT_VALUE_TYPE, (short) 16, (unsigned int) 32, (unsigned int) 32);
  decoder2=new RLEDecoder(true);
  pagePlacer=new PagePlacer(encoder2, decoder2, 2, true);
  pagePlacer->placeColumn("joinexpchain", false, true);

  cout << "ret flag" << endl;

  ce = new ColumnExtracter(_file2, 1, g->force_rebuild);
  encoder2=new RLEEncoder((Operator*) ce, 0, 8*PAGE_SIZE, (byte) INT_VALUE_TYPE, (short) 16, (unsigned int) 32, (unsigned int) 32);
  decoder2=new RLEDecoder(true);
  pagePlacer=new PagePlacer(encoder2, decoder2, 2, true);
  pagePlacer->placeColumn("joinexpreturn", false, true);

  cout << "Int column" << endl;

  ce = new ColumnExtracter(_file2, 2, g->force_rebuild);
  encoder=new IntEncoder((Operator*) ce, 0, 8*PAGE_SIZE);
  decoder=new IntDecoder(false);
  pagePlacer=new PagePlacer(encoder, decoder, 1, true);
  pagePlacer->placeColumn("test1Int", false, false);

  cout << "RLE column" << endl;

  ce = new ColumnExtracter(_file2, 2, g->force_rebuild);
  encoder2=new RLEEncoder((Operator*) ce, 0, 8*PAGE_SIZE, (byte) INT_VALUE_TYPE, (short) 24, (unsigned int) 32, (unsigned int) 32);
  decoder2=new RLEDecoder(false);
  pagePlacer=new PagePlacer(encoder2, decoder2, 1, true);
  pagePlacer->placeColumn("test1RLE", false, false);

  /*cout << "Type2 column" << endl;

  ce = new ColumnExtracter(_file2, 2, g->force_rebuild);
  Type2Encoder* tencoder=new Type2Encoder((Operator*) ce, 0, 6);
  Type2Decoder* tdecoder=new Type2Decoder(false);
  pagePlacer=new PagePlacer(tencoder, tdecoder, 1, false);
  pagePlacer->placeColumn("test1Type2", true, false);*/

  cout << "DictC column" << endl;

  ce = new ColumnExtracter(_file2, 2, g->force_rebuild);
  DictByteEncoder* dictEncoder=new DictByteEncoder((Operator*) ce,0, fieldSize);
  BitDecoder* bitDecoder=new BitDecoder(false);
  pagePlacer=new PagePlacer(dictEncoder, bitDecoder, 1, true);
  pagePlacer->placeColumn("test1DictCEntries", false, false);

  IntEncoder* encoder5=new IntEncoder(dictEncoder->getTableDataSource(),
										dictEncoder->getTableDataSourceColIndex(), 
										8*PAGE_SIZE);										
	IntDecoder* decoder5=new IntDecoder(false);	

	PagePlacer* pagePlacer2=new PagePlacer(encoder5, decoder5, 2, false);
	pagePlacer2->placeColumn("test1DictCTable", false, true);

  /*  int i=0;
  for (; i< 60; i++)
    if (strncmp(_file+i, "dnatest",7) == 0) {
      break;
      }*/

    char* temp = new char[30];
    //sprintf(temp, "joinexpInt%s", (_file+i+8));
    //pagePlacer->placeColumn(temp, false, false);
    //sprintf(temp, "mv joinexp* ../../RuntimeData", (_file+i+8));
    //sprintf(temp, "mv joinexp* ../RuntimeData/");
  sprintf(temp, "mv test1* " RUNTIME_DATA);
  system(temp);
  delete pagePlacer;
  delete decoder;

  return success;	
}
