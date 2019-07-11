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
 * BAndQueryTest:
 *
 * SELECT shipdate
 * FROM lineitem
 * WHERE orderdate = 8066 AND shipdate > 8060
 *
 * Files:
 * ORDERDATE_SHIPDATE: 
 * D2.data (o_orderdate, l_shipdate, l_suppkey | o_orderdate, l_suppkey)
 *
 * Author: cstore-dev
 */
#include "BAndQueryTest.h"

BAndQueryTest::BAndQueryTest() {
}

BAndQueryTest::~BAndQueryTest() {
}

/* args isn't used here */
bool BAndQueryTest::run(Globals* g, const vector<string>& args) {
  /*if (g->build_tables) {
    system("rm RLE_ORDERDATE");
    system("rm RLE_SHIPDATE");
    }*/

  system ("rm BAndQueryTest.out");
  Log::writeToLog("BAndQueryTest", 10, "BAndQueryTest starting...");
  bool success = true;
  
	string path1 = SHIPDATE_MINI;
	string path2 = SHIPDATE_MINI;

	if (g->build_tables) {
    Log::writeToLog("BAndQueryTest", 10, "Loading column orderdate...");
    ColumnExtracter* ce = new ColumnExtracter(path1,              // filename
					      1,                 // column index
					      g->force_rebuild); // force rebuild flag

    RLEEncoder* encoder = new RLEEncoder((Operator*) ce,         // data source
					 0,                      // column index
					 8*PAGE_SIZE,            // buffer size in bits
					 (byte) INT_VALUE_TYPE,  // value type
					 (short) 16,             // value size
					 (unsigned int) 27,      // start pos size
					 (unsigned int) 16);     // reps size
    RLEDecoder* decoder = new RLEDecoder(true);      // value sorted
    PagePlacer* pagePlacer = new PagePlacer(encoder,
					    decoder, 
					    2,       // num indexes
					    false);  // position primary ???
    pagePlacer->placeColumn("D2_RLE_ORDERDATE", // name
			    false,          // split on value
			    true);          // value sorted

    delete pagePlacer;
    delete ce;
    delete encoder;
    delete decoder;
  }
  
  if (g->build_tables) {
    Log::writeToLog("BAndQueryTest", 10, "Loading column shipdate...");
    string path = LINEITEM_TABLE_PATH;
    ColumnExtracter* ce = new ColumnExtracter(path2,              // filename
                                              1,                 // column index
                                              g->force_rebuild); // force rebuild flag

    RLEEncoder* encoder = new RLEEncoder((Operator*) ce,         // data source
                                          0,                      // column index
                                          8*PAGE_SIZE,            // buffer size in bits
                                          (byte) INT_VALUE_TYPE,  // value type
                                          (short) 16,             // value size
                                          (unsigned int) 27,      // start pos size
                                          (unsigned int) 16);     // reps size
    RLEDecoder* decoder = new RLEDecoder(true);      // value sorted
    PagePlacer* pagePlacer = new PagePlacer(encoder,
                                            decoder, 
                                            2,       // num indexes
                                            false);  // position primary
    pagePlacer->placeColumn("D1_RLE_SHIPDATE", // name
                            false,          // split on value
                            true);          // value sorted

    delete pagePlacer;
    delete ce;
    delete encoder;
    delete decoder;
  }

  if (g->build_tables) {
    system("mv D2_RLE_ORDERDATE* " RUNTIME_DATA);
    system("mv D1_RLE_SHIPDATE* " RUNTIME_DATA);
  }

  Log::writeToLog("BAndQueryTest", 10, "Opening column orderdate and shipdate...");
  StopWatch stopWatch;
  stopWatch.start();

  ROSAM* am1 = new ROSAM("D2_RLE_ORDERDATE", 2);
  ROSAM* am2 = new ROSAM("D1_RLE_SHIPDATE", 2);

  Predicate* pred = new Predicate(Predicate::OP_LESS_THAN);
  RLEDataSource* ds1 = new RLEDataSource(am1, true); // value sorted
  ds1->setPredicate(pred);
  ds1->changeRHSBinding(8064);

  Predicate* pred2 = new Predicate(Predicate::OP_GREATER_THAN);
  RLEDataSource* ds2 = new RLEDataSource(am2, true); // value sorted
  ds2->setPredicate(pred2);
  ds2->changeRHSBinding(8064);

	BAndOperator* bandop = new BAndOperator(ds1, 0, ds2, 0);	
	//BOrOperator* bandop = new BOrOperator(ds1, 0, ds2, 0);	
	
	// debug - output postion of PosBlock	
	bool success_ = true;
	PosBlock* block = bandop->getNextPosBlock();
	unsigned int blockIndex = 0;
	while (block != NULL) {
		cout << "Block: " << blockIndex << endl;
		while (block->hasNext()) {
			unsigned int currPos=block->getNext();			
			cout << "postion: " << currPos << endl;
			if (!success_) {
				cout << "\t \t BAnd Operation failed" << endl;
				return success_;
			}
		}
		blockIndex++;
		block=bandop->getNextPosBlock();
	}
	
/*	
  ROSAM* am3 = new ROSAM("RLE_ORDERDATE", 2);
  RLEDataSource* ds3 = new RLEDataSource(am3, true); // value sorted
  ds3->setPositionFilter(bandop, 0);
  
  Operator* srcs[1]={ds3};
  int numCols[1]={1};
  
  BlockPrinter* bPrint = new BlockPrinter(srcs,         // data sources
					  numCols,      // num columns per source
					  1,            // num sources
					  "BAndQueryTest.out");// output filename
  bPrint->printColumns();
*/
	  
  cout << "BAndQueryTest took: " << stopWatch.stop() << " ms" <<  endl;
	
  return success;	
}
