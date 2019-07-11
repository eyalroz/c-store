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
#include "LMDataMaker.h"

LMDataMaker::LMDataMaker() {
}


LMDataMaker::~LMDataMaker() {
}

bool LMDataMaker::run(Globals* g, const vector<string>& args) {
  bool success=true;

  _file = args[0].c_str();

  cout << "LMDataMaker: Making files " << _file << endl;

  /*ColumnExtracter* ce2 = new ColumnExtracter(_file, 2, g->force_rebuild);
  RLEEncoder* encoder2=new RLEEncoder((Operator*) ce2, 0, 8*PAGE_SIZE, (byte) INT_VALUE_TYPE, (short) 32, (unsigned int) 32, (unsigned int) 16);
  RLEDecoder* decoder2=new RLEDecoder(false);
  PagePlacer* pagePlacer2=new PagePlacer(encoder2, decoder2, 1, true);*/

  ColumnExtracter* ce2 = new ColumnExtracter(_file, 2, g->force_rebuild);
  IntEncoder* encoder2=new IntEncoder((Operator*) ce2, 0, 8*PAGE_SIZE);
  IntDecoder* decoder2=new IntDecoder(false);
  PagePlacer* pagePlacer2=new PagePlacer(encoder2, decoder2, 1, true);

  ColumnExtracter* ce = new ColumnExtracter(_file, 1, g->force_rebuild);
  IntEncoder* encoder=new IntEncoder((Operator*) ce, 0, 8*PAGE_SIZE);
  IntDecoder* decoder=new IntDecoder(false);
  PagePlacer* pagePlacer=new PagePlacer(encoder, decoder, 1, true);

  ColumnExtracter* ce3 = new ColumnExtracter(_file, 2, g->force_rebuild);
  Type2Encoder* encoder3=new Type2Encoder((Operator*) ce3, 0, 6);
  Type2Decoder* decoder3=new Type2Decoder(false, encoder3->getValueIndex());
  PagePlacer* pagePlacer3=new PagePlacer(encoder3, decoder3, 1, false);

  char* temp = new char[30];
  //sprintf(temp, "test1Int%s", (_file+i+8));
  pagePlacer2->placeColumn("LMNATCODEINT", false, false);
  sprintf(temp, "mv LMNATCODEINT* " RUNTIME_DATA);
  system(temp);

  pagePlacer->placeColumn("LMCCUSTKEYINT", false, false);
  sprintf(temp, "mv LMCCUSTKEYINT* " RUNTIME_DATA);
  system(temp);

  //pagePlacer3->placeColumn("LMLINENUMT2", true, false);
  //sprintf(temp, "mv LMLINENUMT2* " RUNTIME_DATA);
  //system(temp);

  delete pagePlacer2;
  delete decoder2;
  delete pagePlacer;
  delete decoder;
  delete pagePlacer3;
  delete decoder3;

  return success;	
}
