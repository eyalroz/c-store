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
 * Query 7:
 *
 * SELECT c_nationkey, SUM(l_extendedprice)
 * FROM lineitem, orders, customers
 * WHERE l_orderkey = o_orderkey AND o_custkey = c_custkey AND l_returnflag = 'R'
 * GROUP BY c_nationkey
 *
 * Projections
 *   L1  =  (l_returnkey | l_returnflag, c_custkey)
 *   L2  =  (c_custkey | l_returnflag, c_custkey)
 *   L3  =  (c_custkey, c_name, c_adddress, l_extendedprice, l_discount | c_custkey)
 *
 * Strategy:
 * 1. Apply predicate l_returnflag = 'R' to L1
 * 2. Materialize (1) over L2, which is sorted by c_custkey
 * 3. Merge-join (2) with L3.
 * 3. Perform hash-based aggregation on result
 *
 * Files:
 *
 * Author: Daniel Abadi <dna@mit.edu>
 */
#include "Query7U.h"

Query7U::Query7U() {
  MAX_NATIONKEY = 50;
}

Query7U::~Query7U() {
}

/* args isn't used here */
bool Query7U::run(Globals* g, const vector<string>& args) {
  /*if (g->build_tables) {
    system("rm D4_UNCOMPRESSED_EXTENDEDPRICE*");
    system("rm D4_UNCOMPRESSED_RETURNFLAG");
    system("rm D4_UNCOMPRESSED_NATIONKEY");
  }*/

  system ("rm Query7U.out");
  Log::writeToLog("Query7U", 10, "Query 7 starting...");
  bool success = true;
  
  string path = D4_DATA;

  if (g->build_tables) {
    Log::writeToLog("Query7U", 10, "Loading column returnflag...");
    ColumnExtracter* ce = new ColumnExtracter(path,              // filename
					      2,                 // column index
					      g->force_rebuild); // force rebuild flag

    IntEncoder* encoder = new IntEncoder((Operator*) ce, // data source
					 0,              // column index
					 8*PAGE_SIZE);   // buffer size in bits
    IntDecoder* decoder = new IntDecoder(true);         // value sorted

    PagePlacer* pagePlacer = new PagePlacer(encoder,
					    decoder, 
					    2,       // num indexes
					    false);  // position primary ???
    pagePlacer->placeColumn("D4_UNCOMPRESSED_RETURNFLAG", // name
			    false,          // split on value
			    true);          // value sorted

    delete pagePlacer;
    delete ce;
    delete encoder;
    delete decoder;
  }

  if (g->build_tables) {
    Log::writeToLog("Query7U", 10, "Loading column extendedprice...");

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
					    true);  // position primary ???
    pagePlacer->placeColumn("D4_UNCOMPRESSED_EXTENDEDPRICE", // name
			    false,                   // split on value ???
			    false);                  // value sorted

    delete pagePlacer;
    delete ce;
    delete encoder;
    delete decoder;
  }

  if (g->build_tables) {
    Log::writeToLog("Query7U", 10, "Loading column nationkey...");

    ColumnExtracter* ce = new ColumnExtracter(path,              // filename
					      3,                 // column index
					      g->force_rebuild); // force rebuild flag
    IntEncoder* encoder = new IntEncoder((Operator*) ce, // data source
					 0,              // column index
					 8*PAGE_SIZE);   // buffer size in bits
    IntDecoder* decoder = new IntDecoder(false);         // value sorted
    PagePlacer* pagePlacer = new PagePlacer(encoder,
					    decoder, 
					    1,       // num indexes
					    true);  // position primary ???
    pagePlacer->placeColumn("D4_UNCOMPRESSED_NATIONKEY", // name
			    false,                   // split on value ???
			    false);                  // value sorted

    delete pagePlacer;
    delete ce;
    delete encoder;
    delete decoder;
  }
  if (g->build_tables) {
    system("mv D4_UNCOMPRESSED_EXTENDEDPRICE " RUNTIME_DATA);
    system("mv D4_UNCOMPRESSED_RETURNFLAG* " RUNTIME_DATA);
    system("mv D4_UNCOMPRESSED_NATIONKEY " RUNTIME_DATA);
  }

  Log::writeToLog("Query7U", 10, "Opening D4 columns");
  StopWatch stopWatch;
  stopWatch.start();

  ROSAM* am1 = new ROSAM("D4_UNCOMPRESSED_RETURNFLAG" ,  // table
		   2);               // num indexes
  ROSAM* am2 = new ROSAM("D4_UNCOMPRESSED_EXTENDEDPRICE", 
		   1);
  ROSAM* am3 = new ROSAM("D4_UNCOMPRESSED_NATIONKEY", 
		   1);

  Predicate* pred = new Predicate(Predicate::OP_EQUAL);
  IntDataSource* ds1 = new IntDataSource(am1, 
					 true, // value sorted
					 true); // isROS
  ds1->setPredicate(pred);
  ds1->changeRHSBinding(82);
  // ds1->printColumn();

  BCopyOperator* bc = new BCopyOperator(ds1, 0, 2);

  IntDataSource* ds2 = new IntDataSource(am2, 
					 false, // value sorted
					 true); // isROS
  ds2->setPositionFilter(bc,0);
  IntDataSource* ds3 = new IntDataSource(am3, 
					 false, // value sorted
					 true); // isROS
  ds3->setPositionFilter(bc,1);

  HashSum* agg = new HashSum((Operator*) ds2, 0, (Operator*) ds3, 0);
  agg->setHashFunction(new IdentityHashFunction(MAX_NATIONKEY));
  agg->setHashTableSize(MAX_NATIONKEY);

  Operator* srcs[1]={agg};
  int numCols[1]={2};
  
  BlockPrinter* bPrint = new BlockPrinter(srcs,         // data sources
					  numCols,      // num columns per source
					  1,            // num sources
					  "Query7U.out");// output filename
  bPrint->printColumns();
  
  cout << "Query 7 took: " << stopWatch.stop() << " ms" <<  endl;
  
  delete ds1;
  delete am1;
  delete ds2;
  delete am2;
  delete ds3;
  delete am3;
  return success;
}
