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
#include "MultipleColumnsTest.h"

MultipleColumnsTest::MultipleColumnsTest()
{
}

MultipleColumnsTest::~MultipleColumnsTest()
{
}

bool MultipleColumnsTest::extractAndLoad(Globals* g, char* colName_, char* colPath_) {
	string path=colPath_;
	ColumnExtracter* ce = new ColumnExtracter(path, 1, g->force_rebuild);
	RLEEncoder* encoder=new RLEEncoder((Operator*) ce, 0, 8*PAGE_SIZE, (byte) INT_VALUE_TYPE, (short) 16, (unsigned int) 27, (unsigned int) 16);
	RLEDecoder* decoder=new RLEDecoder(true);
	PagePlacer* pagePlacer=new PagePlacer(encoder, decoder, 2, false);
	pagePlacer->placeColumn(colName_, false, true);
	delete pagePlacer;

	return true;
}

bool MultipleColumnsTest::run(Globals* g, const vector<string>& args) {
	bool success = true;
   	
   	Log::writeToLog("MultipleColumnsTest", 10, "Loading Column1...");
  	success&=extractAndLoad(g,"MCT_C1", SHIPDATE_MINI);
    Log::writeToLog("MultipleColumnsTest", 10, "Loading Column2...");
   	success&=extractAndLoad(g,"MCT_C2", SHIPDATE_MINI);
	//Log::writeToLog("MultipleColumnsTest", 10, "Loading Column3...");
   	//success&=extractAndLoad(g,"MCT_C3", LINEITEM_TABLE_PATH);

  	ROSAM* am1 = new ROSAM( "MCT_C1" , 2);
  	RLEDataSource* ds1=new RLEDataSource(am1,true);
  	ds1->printColumn();
	
	ROSAM* am2 = new ROSAM( "MCT_C2" , 2);
  	RLEDataSource* ds2=new RLEDataSource(am2,true);
  	ds2->printColumn();
  	
  	//ROSAM* am3 = new ROSAM( "MCT_C3" , 2);
	//RLEDataSource* ds3=new RLEDataSource(am3,true);
  	//ds3->printColumn();
  	
 	
  	delete ds1;
  	delete am1;
  	delete ds2;
  	delete am2;  	
  	//delete ds3;
  	//delete am3;
  	
 	system ("rm MCT_C*");
	return success;
}
