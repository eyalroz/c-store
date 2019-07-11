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
 * Query 3:
 *
 * SELECT l_suppkey, COUNT (*)
 * FROM lineitem
 * WHERE l_shipdate > D
 * GROUP BY l_suppkey
 *
 * Projections: 
 * L1  =  (l_shipdate | l_shipdate, l_suppkey)
 * L2  =  (l_suppkey | l_shipdate, l_suppkey)
 *
 * Strategy:
 * 1. Use predicate "l_shipdate > D" to filter L1
 * 2. Use positions of filtered L1 to mask L2 while doing hash-based aggregation
 *
 * Author: Edmond Lau <edmond@mit.edu>
 */
#include "Query3.h"

Query3::Query3() {
  MAX_SUPPKEY = 131072;
}

Query3::~Query3() {
}

/* args isn't used here */
bool Query3::run(Globals* g, const vector<string>& args) {
  /*if (g->build_tables) {
    system("rm D1_RLE_SHIPDATE*");
    system("rm D1_UNCOMPRESSED_SUPPKEY");
    }*/

  system ("rm Query3.out");
  Log::writeToLog("Query3", 10, "Query 3 starting...");
  bool success = true;
  
  if (g->build_tables) {
    Log::writeToLog("Query3", 10, "Loading column shipdate...");
    string path = LINEITEM_TABLE_PATH;
    ColumnExtracter* ce = new ColumnExtracter(path,              // filename
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
    pagePlacer->placeColumn("D1_RLE_SHIPDATE", // name
			    false,          // split on value
			    true);          // value sorted

    delete pagePlacer;
    delete ce;
    delete encoder;
    delete decoder;
  }
  if (g->build_tables) {
    Log::writeToLog("Query3", 10, "Loading column suppkey...");
    string path = SUPPKEY_PATH;

    ColumnExtracter* ce = new ColumnExtracter(path,              // filename
					      1,                 // column index
					      g->force_rebuild); // force rebuild flag
    IntEncoder* encoder = new IntEncoder((Operator*) ce, // data source
					 0,              // column index
					 8*PAGE_SIZE);   // buffer size in bits
    IntDecoder* decoder = new IntDecoder(false);         // value sorted
    PagePlacer* pagePlacer = new PagePlacer(encoder,
					    decoder, 
					    1,       // num indexes
					    true);  // position primary XXX
    pagePlacer->placeColumn("D1_UNCOMPRESSED_SUPPKEY", // name
			    false,                   // split on value ???
			    false);                  // value sorted

    delete pagePlacer;
    delete ce;
    delete encoder;
    delete decoder;
  }
  if (g->build_tables) {
    system("mv D1_RLE_SHIPDATE* " RUNTIME_DATA);
    system("mv D1_UNCOMPRESSED_SUPPKEY " RUNTIME_DATA);
  }

  Log::writeToLog("Query3", 10, "Opening column shipdate and suppkey...");
  StopWatch stopWatch;
  stopWatch.start();

  ROSAM* am1 = new ROSAM("D1_RLE_SHIPDATE" ,  // table
		   2);               // num indexes
  ROSAM* am2 = new ROSAM("D1_UNCOMPRESSED_SUPPKEY", 
		   1);

  Predicate* pred = new Predicate(Predicate::OP_GREATER_THAN);
  RLEDataSource* ds1 = new RLEDataSource(am1, 
					 true); // value sorted
  ds1->setPredicate(pred);
  ds1->changeRHSBinding(9000);
  // ds1->printColumn();

  IntDataSource* ds2 = new IntDataSource(am2, 
					 false, // value sorted
					 true); // isROS
  ds2->setPositionFilter(ds1, 0);

  HashCount* hashAgg = new HashCount((Operator*) ds2, 0, (Operator*) ds2, 0);
  hashAgg->setHashFunction(new IdentityHashFunction(MAX_SUPPKEY));
  hashAgg->setHashTableSize(MAX_SUPPKEY);
		       

  Operator* srcs[1]={hashAgg};
  int numCols[1]={2};
  
  BlockPrinter* bPrint = new BlockPrinter(srcs,         // data sources
					  numCols,      // num columns per source
					  1,            // num sources
					  "Query3.out");// output filename
  bPrint->printColumns();
  
  delete ds1;
  delete am1;
  delete ds2;
  delete am2;
  cout << "Query 3 took: " << stopWatch.stop() << " ms" <<  endl;
	
  return success;	
}
