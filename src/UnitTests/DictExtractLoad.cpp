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
#include "DictExtractLoad.h"

DictExtractLoad::DictExtractLoad() : UnitTest()
{
}

DictExtractLoad::~DictExtractLoad() 
{
}


bool DictExtractLoad::run(Globals* g, const vector<string>& args) {
   	system ("rm DICT.out");	
	//system ("rm DictColumnTEST*");


	bool success=true;
    cout << "Building DictColumn" << endl;
    string path=SHIPDATE_TINY; 
	ColumnExtracter* ce = new ColumnExtracter(path, 1, g->force_rebuild);
	
	// DictEncoder(Operator* blockSrc_, int colIndex_, unsigned int fieldSize_);	
	// PAGE PLACER Signatures
	// 	PagePlacer(Encoder* src_, Decoder* decoder, int numIndexes_, bool posPrimary_);
	// 	void placeColumn(string name_, bool splitOnValue_, bool valSorted_);

	unsigned int fieldSize=7;
	DictEncoder* dictEncoder=new DictEncoder((Operator*) ce,0, fieldSize);
	BitDecoder* bitDecoder=new BitDecoder(false);
	PagePlacer* pagePlacer=new PagePlacer(dictEncoder, bitDecoder, 1, true);
	pagePlacer->placeColumn("DictColumnTESTEntries", false, false);

	IntEncoder* encoder=new IntEncoder(dictEncoder->getTableDataSource(),
					   dictEncoder->getTableDataSourceColIndex(), 
					   8*PAGE_SIZE);										
	IntDecoder* decoder=new IntDecoder(false);	

	PagePlacer* pagePlacer2=new PagePlacer(encoder, decoder, 2, false);
	pagePlacer2->placeColumn("DictColumnTESTTable", false, true);
	delete pagePlacer;
	delete pagePlacer2;
	
	system("mv DictColumnTEST* " RUNTIME_DATA);

	cout << "Outputting Column" << endl;
	
	ROSAM* amTable = new ROSAM( "DictColumnTESTTable" , 2);
	ROSAM* amEnt = new ROSAM( "DictColumnTESTEntries" , 1);

	// DictDataSource(ROSAM* dictTableAM_, ROSAM* entriesAM_, bool valSorted_);
	DictDataSource* ds1=new DictDataSource(amTable, amEnt, false);
  	Operator* srcs[1]={ds1};
  	int numCols[1]={1};
   	BlockPrinter* bPrint=new BlockPrinter(srcs, numCols, 1, "DICT.out");
   	bPrint->printColumns();	
   	delete ds1;
   	delete amTable;
   	delete amEnt;
   	string diff="diff DICT.out ";
   	diff+=path+".1";
   	success&=test("Small output test: ", 0==system(diff.c_str()), 0, 0);
  	//system ("rm DICT.out");
   	
   	

	// DictEncoder(Operator* blockSrc_, int colIndex_, unsigned int fieldSize_);	
	// PAGE PLACER Signatures
	// 	PagePlacer(Encoder* src_, Decoder* decoder, int numIndexes_, bool posPrimary_);
	// 	void placeColumn(string name_, bool splitOnValue_, bool valSorted_);
	/*   	path=SHIPDATE_MINI;
	//	path=LINEITEM_TABLE_PATH; WARNING this takes aroung 20 mins
	ColumnExtracter* ce2 = new ColumnExtracter(path, 1, g->force_rebuild);
	fieldSize=15;
	DictEncoder* dictEncoder2=new DictEncoder((Operator*) ce2,0, fieldSize);
	BitDecoder* bitDecoder2=new BitDecoder(false);
	pagePlacer=new PagePlacer(dictEncoder2, bitDecoder2, 1, true);
	pagePlacer->placeColumn("DictColumnTESTBIGEntries", false, false);

	IntEncoder* encoder2=new IntEncoder(dictEncoder2->getTableDataSource(),
										dictEncoder2->getTableDataSourceColIndex(), 
										8*PAGE_SIZE);										
	IntDecoder* decoder2=new IntDecoder(false);	

	pagePlacer2=new PagePlacer(encoder2, decoder2, 2, false);
	pagePlacer2->placeColumn("DictColumnTESTBIGTable", false, true);
	delete pagePlacer;
	delete pagePlacer2;
	
	//system("mv DictColumnTESTBIG* RuntimeData/");
	cout << "Outputting Column" << endl;
	
	amTable = new ROSAM( "DictColumnTESTBIGTable" , 2);
	amEnt = new ROSAM( "DictColumnTESTBIGEntries" , 1);
	// DictDataSource(ROSAM* dictTableAM_, ROSAM* entriesAM_, bool valSorted_);
	ds1=new DictDataSource(amTable, amEnt, false);
  	Operator* srcs2[1]={ds1};
  	int numCols2[1]={1};
   	BlockPrinter* bPrint2=new BlockPrinter(srcs2, numCols2, 1, "DICT.out");
   	bPrint2->printColumns();	
   	delete ds1;
   	delete amTable;
   	delete amEnt;
   	diff="diff DICT.out ";
   	diff+=path+".1";
 	diff+=" > bigDiff";
   	success&=test("Large output test: ", 0==system(diff.c_str()), 0, 0);
   	   	
   	system ("rm DICT.out");
	
	//system ("rm DictColumnTEST*");
	*/
   	
   	return success;
}
