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
 * Query 1:
 * SELECT l_shipdate, COUNT(*)
 * FROM lineitem
 * WHERE l_shipdate > D
 * GROUP BY l_shipdate
 *
 * Projections: (l_shipdate | l_shipdate)
 *
 * Strategy:
 * 1 - Compute bitmap corresponding to l_shipdate over L
 * 2 - Materialize over L and do one-pass aggregation
 *
 * Author Miguel Ferriera
 * Modified: Edmond Lau <edmond@mit.edu>
 */

#include "Query1WSRS.h"



Query1WSRS::Query1WSRS() {
  PRED_FILTER = 9000;
  PRED_TYPE = Predicate::OP_GREATER_THAN;
  MAX_SUPPKEY = 100001;
}

Query1WSRS::~Query1WSRS() {
}

/* args isn't used here */
bool Query1WSRS::run(Globals* g, const vector<string>& args) {
  if (g->build_tables) 
    system ("rm D6_SHIPDATE");

  system ("rm Query1WSRS.out");
  Log::writeToLog("Query1WSRS", 10, "Query 1 starting...");

  
  if (g->build_tables) {
    buildRSTables(g);
  }

  CatalogInstance::getCatalog();  // just to init the catalog.
  Log::writeToLog("Query1WSRS", 10, "Opening Column1...");
  StopWatch stopWatch;
  stopWatch.start();

  Aggregator *rsAgg = processRS();
  Aggregator *wsAgg = processWS();

  //printRSAgg(rsAgg);
  //printWSAgg(wsAgg);
  
  MergeSortedGroups *merge = new MergeSortedGroups(rsAgg, 
						   wsAgg, 
						   2,      // num columns
						   0);     // merge by col index
  merge->setMergeOp(new MergeCountOp());

  Operator* srcs[1]={merge};
  int numCols[1]={2};
 
  
  BlockPrinter* bPrint=new BlockPrinter(srcs, numCols, 1, "Query1WSRS.out");
  bPrint->printColumns();
  
  cout << "Query 1 took: " << stopWatch.stop() << " ms" <<  endl;
  
  return true;	
}

Aggregator* Query1WSRS::processRS() {
  ROSAM* am1 = new ROSAM( "D6_SHIPDATE" , 2);
  Predicate* pred=new Predicate(PRED_TYPE);

  RLEDataSource* ds1=new RLEDataSource(am1,true);
  ds1->setPredicate(pred);
  ds1->changeRHSBinding(PRED_FILTER);
  // ds1->printColumn();
  
  Count* agg = new Count((Operator*) ds1, 0, (Operator*) ds1, 0);
  return agg;
}

Aggregator* Query1WSRS::processWS() {
  int shipdateColIndex = 1;
  
  WOSAM* am1 = CatalogInstance::getCatalog()->getWOSAM( D6_DATA_WOS, shipdateColIndex);
    /*new WOSAM(D6_DATA_WOS, // table name
			 2,          // num indices
			 8,          // num columns
			 shipdateColIndex);         // field num
    */
  IntDataSource *ds1 = new IntDataSource(am1,
					 true,   // value sorted
					 false); // isROS

  Predicate* pred=new Predicate(PRED_TYPE);
  ds1->setPredicate(pred);
  ds1->changeRHSBinding(PRED_FILTER);
  HashCount* hashAgg = new HashCount((Operator*) ds1, 0, (Operator*) ds1, 0);
  hashAgg->setHashFunction(new IdentityHashFunction(MAX_SUPPKEY));
  hashAgg->setHashTableSize(MAX_SUPPKEY);
  return hashAgg;

}

void Query1WSRS::printWSAgg(Aggregator *agg) {
  system("Query1WS.out");

  Operator* srcs[1]={agg};
  int numCols[1]={2};
  
  BlockPrinter* bPrint = new BlockPrinter(srcs,          // data sources (array)
					  numCols,       // num columns per source (array)
					  1,             // num sources
					  "Query1WS.out"); // filename
  bPrint->printColumns();
}

void Query1WSRS::printRSAgg(Aggregator *agg) {
  system("Query1RS.out");
  Operator* srcs[1]={agg};
  int numCols[1]={2};
  
  BlockPrinter* bPrint = new BlockPrinter(srcs,          // data sources (array)
					  numCols,       // num columns per source (array)
					  1,             // num sources
					  "Query1RS.out"); // filename
  bPrint->printColumns();
}

void Query1WSRS::buildRSTables(Globals *g) {
  Log::writeToLog("Query1WSRS", 10, "Loading Column1...");
  string path = D6_DATA_ROS;
  ColumnExtracter* ce = new ColumnExtracter(path, 1, g->force_rebuild);
  RLEEncoder* encoder=new RLEEncoder((Operator*) ce, 0, 8*PAGE_SIZE, (byte) INT_VALUE_TYPE, (short) 16, (unsigned int) 27, (unsigned int) 16);
  RLEDecoder* decoder=new RLEDecoder(true);
  PagePlacer* pagePlacer=new PagePlacer(encoder, decoder, 2, false);
  pagePlacer->placeColumn("D6_SHIPDATE", false, true);
  delete pagePlacer;
  delete ce;
  delete encoder;
  delete decoder;
}
