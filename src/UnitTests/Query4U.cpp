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
 * SELECT o_orderdate, MAX(l_shipdate)
 * FROM lineitem, orders
 * WHERE l_orderkey = o_orderkey AND o_orderdate > D
 * GROUP BY o_orderdate
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
 */
#include "Query4U.h"

Query4U::Query4U() {
  MAX_ORDERDATE = 10500;
}

Query4U::~Query4U() {
}

/* args isn't used here */
bool Query4U::run(Globals* g, const vector<string>& args) {
  /*if (g->build_tables) {
    system("rm D2_UNCOMPRESSED_ORDERDATE");
    system("rm D2_UNCOMPRESSED_SHIPDATE");
    }*/

  system ("rm Query4U.out");
  Log::writeToLog("Query4U", 10, "Query 4 starting...");
  bool success = true;
  
  if (g->build_tables) {
    Log::writeToLog("Query4U", 10, "Loading column orderdate...");
    string path = ORDERDATE_SHIPDATE_FULL;
    ColumnExtracter* ce = new ColumnExtracter(path,              // filename
					      1,                 // column index
					      g->force_rebuild); // force rebuild flag

    IntEncoder* encoder = new IntEncoder((Operator*) ce, // data source
					 0,              // column index
					 8*PAGE_SIZE);   // buffer size in bits
    IntDecoder* decoder = new IntDecoder(false);         // value sorted
    PagePlacer* pagePlacer = new PagePlacer(encoder,
					    decoder, 
					    2,       // num indexes
					    false);  // position primary ???
    pagePlacer->placeColumn("D2_UNCOMPRESSED_ORDERDATE", // name
			    false,          // split on value
			    true);          // value sorted

    delete pagePlacer;
    delete ce;
    delete encoder;
    delete decoder;
  }
  if (g->build_tables) {
    Log::writeToLog("Query4U", 10, "Loading column shipdate...");
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
    pagePlacer->placeColumn("D2_UNCOMPRESSED_SHIPDATE", // name
			    false,                   // split on value ???
			    false);                  // value sorted

    delete pagePlacer;
    delete ce;
    delete encoder;
    delete decoder;
  }
  if (g->build_tables) {
    system("mv D2_UNCOMPRESSED_ORDERDATE " RUNTIME_DATA);
    system("mv D2_UNCOMPRESSED_SHIPDATE " RUNTIME_DATA);
  }

  Log::writeToLog("Query4U", 10, "Opening column orderdate and shipdate...");

#ifdef CSTORE_PAPI
  Misc* pMisc = new Misc();
  pMisc->init_PAPI();
  pMisc->setup_PAPI_event();
#endif

  StopWatch stopWatch;
  stopWatch.start();

#ifdef CSTORE_PAPI
  //+ starting PAPI
  pMisc->start_PAPI_event();
  long_long start_usec = pMisc->get_usec();
  long_long start_cyc = pMisc->get_cycle();
  //-
#endif

  ROSAM* am1 = new ROSAM("D2_UNCOMPRESSED_ORDERDATE" ,  // table
		   2);               // num indexes
  ROSAM* am3 = new ROSAM("D2_UNCOMPRESSED_ORDERDATE" ,  // table
		   2);               // num indexes
  ROSAM* am2 = new ROSAM("D2_UNCOMPRESSED_SHIPDATE", 
		   1);

  Predicate* pred = new Predicate(Predicate::OP_GREATER_THAN);
  IntDataSource* ds1 = new IntDataSource(am1, 
					 true, // value sorted
					 true); // isROS
  IntDataSource* ds3 = new IntDataSource(am3, 
					 true, // value sorted
					 true); // isROS
  ds1->setPredicate(pred);
  ds1->changeRHSBinding(9000);
  ds3->setPredicate(pred);
  ds3->changeRHSBinding(9000);


  IntDataSource* ds2 = new IntDataSource(am2, 
					 false, // value sorted
					 true); // isROS
  ds2->setPositionFilter(ds3,0);

  HashMax* agg = new HashMax((Operator*) ds2, 0, (Operator*) ds1, 0);
  agg->setHashFunction(new IdentityHashFunction(MAX_ORDERDATE));
  agg->setHashTableSize(MAX_ORDERDATE);

  Operator* srcs[1]={agg};
  int numCols[1]={2};
  
  BlockPrinter* bPrint = new BlockPrinter(srcs,         // data sources
					  numCols,      // num columns per source
					  1,            // num sources
#ifdef CSTORE_PAPI
            "/dev/null");// output filename
#else
					  "Query4U.out");// output filename
#endif

  bPrint->printColumns();
  
  cout << "Query 4 took: " << stopWatch.stop() << " ms" <<  endl;

#ifdef CSTORE_PAPI
        //+ output PAPI 
        pMisc->read_PAPI_event();
        pMisc->print_PAPI_event();
        long_long elapsed_usec = pMisc->get_usec() - start_usec;
        long_long elapsed_cyc = pMisc->get_cycle() - start_cyc;
        cout << "elapsed time in usec:" << elapsed_usec << endl;
        cout << "elapsed cycles:" << elapsed_cyc << endl;
        //-
#endif
	
  delete ds1;
  delete am1;
  delete ds2;
  delete am2;
  delete ds3;
  delete am3;
  return success;	
}
