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
 * Query 4:
 *
 * SELECT l_shipdate, Max(o_orderdate)
 * FROM lineitem, orders
 * WHERE l_orderkey = o_orderkey AND o_orderdate > D
 * GROUP BY l_shipdate
 *
 * Projections
 *   L1  =  (o_orderdate, l_shipdate | o_orderdate)
 *
 * Strategy:
 * 1. Use predicate "o_orderdate > D" to filter L1
 * 2. Use positions of filtered orderdate column to mask shipdate 
 *      while doing hash-based aggregation
 *
 * Files:
 * ORDERDATE_SHIPDATE: 
 * D2.data (o_orderdate, l_shipdate, l_suppkey | o_orderdate, l_suppkey)
 *
 * Author: Edmond Lau <edmond@mit.edu>
 * Modified Q4 by Miguel Ferreira
 */
#include "Query4Prime.h"

Query4Prime::Query4Prime() {
}

Query4Prime::~Query4Prime() {
}

/* args isn't used here */
bool Query4Prime::run(Globals* g, const vector<string>& args) {
  if (g->build_tables) {
    system("rm RLE_ORDERDATE");
    system("rm UNCOMPRESSED_SHIPDATE");
  }

  system ("rm Query4Prime.out");
  Log::writeToLog("Query4Prime", 10, "Query 4 Prime starting...");
  bool success = true;
  
  if (g->build_tables) {
    Log::writeToLog("Query4Prime", 10, "Loading column orderdate...");
    string path = ORDERDATE_SHIPDATE_FULL;
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
    pagePlacer->placeColumn("RLE_ORDERDATE", // name
			    false,          // split on value
			    true);          // value sorted

    delete pagePlacer;
    delete ce;
    delete encoder;
    delete decoder;
  }
  if (g->build_tables) {
    Log::writeToLog("Query4Prime", 10, "Loading column shipdate...");
    string path = ORDERDATE_SHIPDATE_FULL;

    ColumnExtracter* ce = new ColumnExtracter(path,              // filename
					      2,                 // column index
					      g->force_rebuild); // force rebuild flag
    IntEncoder* encoder = new IntEncoder((Operator*) ce, // data source
					 0,              // column index
					 8*PAGE_SIZE);   // buffer size in bits
    IntDecoder* decoder = new IntDecoder(false);         // value sorted
    PagePlacer* pagePlacer = new PagePlacer(encoder,
					    decoder, 
					    1,       // num indexes
					    true);  // position primary ???
    pagePlacer->placeColumn("UNCOMPRESSED_SHIPDATE", // name
			    false,                   // split on value ???
			    false);                  // value sorted

    delete pagePlacer;
    delete ce;
    delete encoder;
    delete decoder;
  }

  Log::writeToLog("Query4Prime", 10, "Opening column orderdate and shipdate...");
  StopWatch stopWatch;
  stopWatch.start();

  ROSAM* am1 = new ROSAM("RLE_ORDERDATE" ,  // table
		   2);               // num indexes
  ROSAM* am2 = new ROSAM("UNCOMPRESSED_SHIPDATE", 
		   1);

  Predicate* pred = new Predicate(Predicate::OP_GREATER_THAN);
  RLEDataSource* ds1 = new RLEDataSource(am1, 
					 true); // value sorted
  ds1->setPredicate(pred);
  ds1->changeRHSBinding(9000);


  IntDataSource* ds2 = new IntDataSource(am2, 
					 false, // value sorted
					 true); // isROS
  ds2->setPositionFilter(ds1,0);

  Max* agg = new Max((Operator*) ds1, 0, (Operator*) ds2, 0);

  Operator* srcs[1]={agg};
  int numCols[1]={2};
  
  BlockPrinter* bPrint = new BlockPrinter(srcs,         // data sources
					  numCols,      // num columns per source
					  1,            // num sources
					  "Query4Prime.out");// output filename
  bPrint->printColumns();
  
  cout << "Query 4 Prime took: " << stopWatch.stop() << " ms" <<  endl;
	
  return success;	
}
