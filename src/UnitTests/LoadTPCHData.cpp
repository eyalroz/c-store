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
#include "LoadTPCHData.h"

LoadTPCHData::LoadTPCHData()
{
}

LoadTPCHData::~LoadTPCHData()
{
}


bool LoadTPCHData::run(Globals* g, const vector<string>& args) {
  if(args.size() < 1){
    cout << "LoadTPCHData::run: Improper argument" <<endl;
    return false;
  }
  //be careful: if args[0] isn't an integer string
  //atoi will return 0. So recommend having col_ >=1
  //as validation check. 
 
  int col_ = atoi(args[0].c_str());

	string path;
	string name;
	ColumnExtracter* ce;
	PagePlacer* pagePlacer;
	int numIndexes;
	bool posPrimary;
	bool valSorted;
	bool spiltOnValue;
	Encoder* encoder;
	Decoder* decoder;
	if (col_==0) {
		cout << "#### Shipdate ####" << endl;
		path=LINEITEM_TABLE_PATH;
	

		cout << " Making RLE" << endl;
	
		name="RleShipdate.BDB";
		numIndexes=2;
		posPrimary=false;
		valSorted=true;
		spiltOnValue=false;
		ce = new ColumnExtracter(path, 1, g->force_rebuild);
		encoder=new RLEEncoder((Operator*) ce, 0, 8*PAGE_SIZE, (byte) INT_VALUE_TYPE, (short) 15, (unsigned int) 26, (unsigned int) 15);
		decoder=new RLEDecoder(valSorted);
		pagePlacer=new PagePlacer(encoder, decoder, numIndexes, posPrimary);
		pagePlacer->placeColumn(name.c_str(), spiltOnValue, valSorted);

		delete pagePlacer;
	
		cout << " Making RLEII" << endl;
		name="RleIIShipdate.BDB";
		numIndexes=2;
		posPrimary=false;
		valSorted=true;
		spiltOnValue=false;
		ce = new ColumnExtracter(path, 1, g->force_rebuild);
		encoder=new RLEEncoderII((Operator*) ce, 0, 8*PAGE_SIZE, (byte) INT_VALUE_TYPE, (short) 15, (unsigned int) 15);
		decoder=new RLEDecoderII(valSorted);
		 pagePlacer=new PagePlacer(encoder, decoder, numIndexes, posPrimary);
		pagePlacer->placeColumn(name.c_str(), spiltOnValue, valSorted);

		delete pagePlacer;
	
		cout << " Making NS" << endl;
		name="NSShipdate.BDB";
		numIndexes=2;
		posPrimary=false;
		valSorted=true;
		spiltOnValue=false;
		ce = new ColumnExtracter(path, 1, g->force_rebuild);
		encoder=new NSEncoder((Operator*) ce, 0);
		decoder=new NSDecoder(valSorted);
		pagePlacer=new PagePlacer(encoder, decoder, numIndexes, posPrimary);
		pagePlacer->placeColumn(name.c_str(), spiltOnValue, valSorted);

		delete pagePlacer;
	
		cout << " Making LZ" << endl;
		name="LZShipdate.BDB";
		numIndexes=2;
		posPrimary=false;
		valSorted=true;
		spiltOnValue=false;
		ce = new ColumnExtracter(path, 1, g->force_rebuild);
		encoder=new LZEncoder((Operator*) ce, 0, 59*PAGE_SIZE);
		decoder=new LZDecoder(valSorted);
		pagePlacer=new PagePlacer(encoder, decoder, numIndexes, posPrimary);
		pagePlacer->placeColumn(name.c_str(), spiltOnValue, valSorted);

		delete pagePlacer;
	

		cout << " Making Dict" << endl;
		name="DictShipdate.BDB";
		numIndexes=2;
		posPrimary=false;
		valSorted=true;
		spiltOnValue=false;
		int fieldSize=12;
		ce = new ColumnExtracter(path, 1, g->force_rebuild);
		DictEncoder* dictEncoder=new DictEncoder((Operator*) ce,0, fieldSize);
		BitDecoder* bitDecoder=new BitDecoder(false);
		pagePlacer=new PagePlacer(dictEncoder, bitDecoder, numIndexes, posPrimary);
		pagePlacer->placeColumn(name.c_str(), spiltOnValue, valSorted);	

		IntEncoder* tableEncoder=new IntEncoder(dictEncoder->getTableDataSource(),
											dictEncoder->getTableDataSourceColIndex(), 
											8*PAGE_SIZE);										
		IntDecoder* tableDecoder=new IntDecoder(false);	
	
		PagePlacer* pagePlacer2=new PagePlacer(tableEncoder, tableDecoder, 2, false);
		pagePlacer2->placeColumn((name+"Table").c_str(), false, true);
		delete pagePlacer;
		delete pagePlacer2;

		cout << " Making Int" << endl;
		path=LINEITEM_TABLE_PATH;
		name="IntShipdate.BDB";
		numIndexes=2;
		posPrimary=true;
		valSorted=false;
		spiltOnValue=false;
		ce = new ColumnExtracter(path, 1, g->force_rebuild);
		encoder=new IntEncoder((Operator*) ce, 0, 8*PAGE_SIZE);
		decoder=new IntDecoder(valSorted);
		pagePlacer=new PagePlacer(encoder, decoder, numIndexes, posPrimary);
		pagePlacer->placeColumn(name.c_str(), spiltOnValue, valSorted);

		delete pagePlacer;


	}
	else {
		cout << "#### Suppkey ####" << endl;
	

	
		cout << " Making Int" << endl;
		path=SUPPKEY_PATH;
		name="IntSuppkey.BDB";
		numIndexes=2;
		posPrimary=false;
		valSorted=true;
		spiltOnValue=false;
		ce = new ColumnExtracter(path, 1, g->force_rebuild);
		encoder=new IntEncoder((Operator*) ce, 0, 8*PAGE_SIZE);
		decoder=new IntDecoder(valSorted);
		pagePlacer=new PagePlacer(encoder, decoder, numIndexes, posPrimary);
		pagePlacer->placeColumn(name.c_str(), spiltOnValue, valSorted);

		delete pagePlacer;

		cout << " Making NS" << endl;
		name="NSSuppkey.BDB";
		numIndexes=2;
		posPrimary=false;
		valSorted=true;
		spiltOnValue=false;
		ce = new ColumnExtracter(path, 1, g->force_rebuild);
		encoder=new NSEncoder((Operator*) ce, 0);
		decoder=new NSDecoder(valSorted);
		pagePlacer=new PagePlacer(encoder, decoder, numIndexes, posPrimary);
		pagePlacer->placeColumn(name.c_str(), spiltOnValue, valSorted);

		delete pagePlacer;

		cout << " Making LZ" << endl;
		name="LZSuppkey.BDB";
		numIndexes=2;
		posPrimary=false;
		valSorted=true;
		spiltOnValue=false;
		ce = new ColumnExtracter(path, 1, g->force_rebuild);
		encoder=new LZEncoder((Operator*) ce, 0, 60*PAGE_SIZE);
		decoder=new LZDecoder(valSorted);
		 pagePlacer=new PagePlacer(encoder, decoder, numIndexes, posPrimary);
		pagePlacer->placeColumn(name.c_str(), spiltOnValue, valSorted);
	
		delete pagePlacer;
	}
	return true;
}
