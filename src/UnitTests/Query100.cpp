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
 * Query100:
 * SELECT something
 * FROM lineitem AS A, lineitem AS B
 * WHERE A.l_shipdate = B.l_shipdate
 *
 * Projections: (l_shipdate | l_shipdate)
 *
 * Strategy:
 * 1 - Compute NLJoin
 */
#include "Query100.h"

Query100::Query100() {
}

Query100::~Query100() {
}

bool Query100::run(Globals* g, const vector<string>& args) {
  	Log::writeToLog("Query100", 10, "Query 100 starting...");
    	bool success=true;
  
	/*	  	if (BUILD_TABLES) {
    	Log::writeToLog("Query1", 10, "Loading Column1...");
//    	success&=extractAndLoad("Q1_C1", LINEITEM_TABLE_PATH);

		string path=LINEITEM_TABLE_PATH;  //SHIPDATE_MINI
		ColumnExtracter* ce = new ColumnExtracter(path, 1, g->force_rebuild);
		RLEEncoder* encoder=new RLEEncoder((Operator*) ce, 0, 8*PAGE_SIZE, (byte) INT_VALUE_TYPE, (short) 16, (unsigned int) 27, (unsigned int) 16);
		RLEDecoder* decoder=new RLEDecoder(true);
		PagePlacer* pagePlacer=new PagePlacer(encoder, decoder, 2, false);
		pagePlacer->placeColumn("Q1_C1", false, true);
		delete pagePlacer;

		}*/

	/*string path=DELTAPOS_TEST_BIG2;
	ColumnExtracter* ce = new ColumnExtracter(path, 1, g->force_rebuild);
	RLEEncoder* encoder1=new RLEEncoder((Operator*) ce, 0, 8*PAGE_SIZE, (byte) INT_VALUE_TYPE, (short) 16, (unsigned int) 27, (unsigned int) 24);
	RLEDecoder* decoder1=new RLEDecoder(true);
	PagePlacer* pagePlacer=new PagePlacer(encoder1, decoder1, 2, false);
	pagePlacer->placeColumn("Q100_C1", false, true);
	delete pagePlacer;*/
	
	/*ColumnExtracter* ce = new ColumnExtracter("dna.sorted", 1, g->force_rebuild);
	IntEncoder* encoder=new IntEncoder((Operator*) ce, 0, 8*PAGE_SIZE);
	IntDecoder* decoder=new IntDecoder(true);
	PagePlacer* pagePlacer=new PagePlacer(encoder, decoder, 1, true);
	pagePlacer->placeColumn("QDNA_C2", false, false);
	delete pagePlacer;*/
	
	/*path=DELTAPOS_TEST_BIG;
	ce = new ColumnExtracter(path, 1, g->force_rebuild);
	DeltaPosEncoder* encoder=new DeltaPosEncoder((Operator*) ce, 0, 5);
	DeltaPosDecoder* decoder=new DeltaPosDecoder(false);
	pagePlacer=new PagePlacer(encoder, decoder, 1, true);
	pagePlacer->placeColumn("DeltaPosTestDATABIG", true, false);
	delete pagePlacer;
	delete decoder;*/

	/*string path=DELTAPOS_TEST_BIG;
	ColumnExtracter* ce = new ColumnExtracter(path, 1, g->force_rebuild);
	RLEEncoderII* encoder2=new RLEEncoderII((Operator*) ce, 0, 8*PAGE_SIZE, (byte) INT_VALUE_TYPE, (short) 8, (unsigned int) 27, (unsigned int) 8);
	RLEDecoderII* decoder2=new RLEDecoderII(false);
	PagePlacer* pagePlacer=new PagePlacer(encoder2, decoder2, 1, true);
	pagePlacer->placeColumn("RLETestDATABIG", false, false);
	delete pagePlacer;
	delete decoder2;*/

	/*string path=DELTAPOS_TEST_BIG;
	ColumnExtracter* ce = new ColumnExtracter(path, 1, g->force_rebuild);
	NSEncoder* encoder=new NSEncoder((Operator*) ce, 0);
	NSDecoder* decoder=new NSDecoder(false);
	PagePlacer* pagePlacer=new PagePlacer(encoder, decoder, 1, true);
	pagePlacer->placeColumn("NSTestDATABIG", false, false);
	delete pagePlacer;*/

	string path=DELTAPOS_TEST_BIG;
	ColumnExtracter* ce = new ColumnExtracter(path, 1, g->force_rebuild);
	IntEncoder* encoder=new IntEncoder((Operator*) ce, 0, 8*PAGE_SIZE);
	IntDecoder* decoder=new IntDecoder(false);
	PagePlacer* pagePlacer=new PagePlacer(encoder, decoder, 1, true);
	pagePlacer->placeColumn("IntTestDATABIG", false, false);
	delete pagePlacer;

	/*string path=DELTAPOS_TEST_BIG;
	ColumnExtracter* ce = new ColumnExtracter(path, 1, g->force_rebuild);
	
	LZEncoder* encoder=new LZEncoder(ce, 0, 2560000/sizeof(int));
	LZDecoder* decoder=new LZDecoder(false);

	PagePlacer* pagePlacer=new PagePlacer(encoder, decoder, 1, true);
	pagePlacer->placeColumn("LZTestDATABIG", false, false);
	delete pagePlacer;*/


  	StopWatch stopWatch;
  	stopWatch.start();
  	Log::writeToLog("Query100", 10, "Opening Column1...");
  	//ROSAM* am1 = new ROSAM( "Q100_C1" , 2);
	//ROSAM* am2 = new ROSAM( "QDNA_C2" , 1);
	//ROSAM* am3 = new ROSAM( "DeltaPosTestDATABIGVals", 1);
	//ROSAM* am4 = new ROSAM( "RLETestDATABIG", 1);
	ROSAM* am5 = new ROSAM( "IntTestDATABIG", 1);
	//ROSAM* am6 = new ROSAM( "LZTestDATABIG", 1);
	//ROSAM* am7 = new ROSAM( "NSTestDATABIG", 1);
  	//Predicate* pred=new Predicate(Predicate::OP_GREATER_THAN);
  	//RLEIIDataSource* ds1=new RLEIIDataSource(am1,true);
	//IntDataSource* ds2=new IntDataSource(am2, true, true);
	//DeltaPosDataSource* ds3=new DeltaPosDataSource(am3, false);
	//RLEIIDataSource* ds4=new RLEIIDataSource(am4, false);
	IntDataSource* ds5=new IntDataSource(am5, false, true);
	//LZDataSource* ds6=new LZDataSource(am6, false);
	//NSDataSource* ds7=new NSDataSource(am7, false);
  	//ds1->setPredicate(pred);
  	//ds1->changeRHSBinding(9000);
  	// ds1->printColumn();
  
	//NLJoin* join = new NLJoin((Operator*)ds2, 0, (Operator*) ds3, 0);
	//join->getNextPosBlock(0);
	//join->getNextPosBlock(1);
  	

	Count* agg = new Count((Operator*) ds5, 0, (Operator*) ds5, 0);
	Operator* srcs[1]={agg};
  	int numCols[1]={2};
 
  
   	BlockPrinter* bPrint=new BlockPrinter(srcs, numCols, 1, "Query100.out");
   	bPrint->printColumns();
  
	cout << "Query 100 took: " << stopWatch.stop() << " ms" <<  endl;
	
  

  	//delete ds1;
  	//delete am1;
	//system ("rm Q1_*");
   	return success;	
}
