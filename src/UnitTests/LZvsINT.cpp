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
#include "LZvsINT.h"

LZvsINT::LZvsINT()
{
}

LZvsINT::~LZvsINT()
{
}
bool LZvsINT::run(Globals* g, const vector<string>& args) {    
    cout << "Building Int" << endl;
    string path=SHIPDATE_TINY;  // 
    //string path=LINEITEM_TABLE_PATH;  // note will run out of memory most likely
	ColumnExtracter* ce = new ColumnExtracter(path, 1, g->force_rebuild);
	IntEncoder* encoder=new IntEncoder((Operator*) ce,0, 8*PAGE_SIZE);
	IntDecoder* decoder=new IntDecoder(false);	
	PagePlacer* pagePlacer=new PagePlacer(encoder, decoder, 2, true);
	pagePlacer->placeColumn("INT_LINEITEM_TEST", false, true);
	delete pagePlacer;
	
	cout << "Outputting Int" << endl;
	
	ROSAM* am1 = new ROSAM( "INT_LINEITEM_TEST" , 2);
	IntDataSource* ds1=new IntDataSource(am1,false,true);
  	Operator* srcs[1]={ds1};
  	int numCols[1]={1};
   	BlockPrinter* bPrint=new BlockPrinter(srcs, numCols, 1, "INT.out");
   	bPrint->printColumns();
	
	

    cout << "Building LZ" << endl;
	ColumnExtracter* ce2 = new ColumnExtracter(path, 1, g->force_rebuild);
	LZEncoder* encoder2=new LZEncoder((Operator*) ce2,0, 8*PAGE_SIZE);
	LZDecoder* decoder2=new LZDecoder(false);
	PagePlacer* pagePlacer2=new PagePlacer(encoder2, decoder2, 2, true);
	pagePlacer2->placeColumn("LZ_LINEITEM_TEST", false, true);
	delete pagePlacer2;
    cout << "Printing LZ" << endl;
	ROSAM* am2 = new ROSAM( "LZ_LINEITEM_TEST" , 2);
	LZDataSource* ds2=new LZDataSource(am2,false);	
  	Operator* srcs2[1]={ds2};
  	int numCols2[1]={1};
   	BlockPrinter* bPrint2=new BlockPrinter(srcs2, numCols2, 1, "LZ.out");
   	bPrint2->printColumns();


    cout << "Building RLE" << endl;
	ColumnExtracter* ce3 = new ColumnExtracter(path, 1, g->force_rebuild);
	LZEncoder* encoder3=new LZEncoder((Operator*) ce3,0, 8*PAGE_SIZE);
	LZDecoder* decoder3=new LZDecoder(false);
	PagePlacer* pagePlacer3=new PagePlacer(encoder3, decoder3, 2, true);
	pagePlacer3->placeColumn("RLE_LINEITEM_TEST", false, true);
	delete pagePlacer3;
    cout << "Printing RLE" << endl;
	ROSAM* am3 = new ROSAM( "RLE_LINEITEM_TEST" , 2);
	RLEDataSource* ds3=new RLEDataSource(am3,false);	
  	Operator* srcs3[1]={ds3};
  	int numCols3[1]={1};
   	BlockPrinter* bPrint3=new BlockPrinter(srcs3, numCols3, 1, "RLE.out");
   	bPrint3->printColumns();

	int ret=system("diff LZ.out RLE.out");
	ret+=system("diff LZ.out INT.out");


	system ("rm INT.out");
	system ("rm RLE.out");
	system ("rm LZ.out");
	
	system ("rm RLE_*");
	system ("rm INT_*");
	system ("rm LZ_*");
	
	if (ret==0) {
		cout << "No difference between encoding schemes" << endl;
		return true;
	}
	else {
		cout << "Differences between encoding schemes" << endl;
		return false;
	}

}
