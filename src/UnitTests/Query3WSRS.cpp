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
 * 1. Use the predicate "l_shipdate > D" to filter L1
 * 2. Use positions of filtered L1 to mask L2, while doing hash-based aggregation
 *
 * Will operator structure know that the column is sorted?
 *
 * Author: Edmond Lau <edmond@mit.edu>
 */
#include "Query3WSRS.h"

Query3WSRS::Query3WSRS() {
  MAX_SUPPKEY = 100001;
  PRED_FILTER = 9000;
  PRED_TYPE = Predicate::OP_GREATER_THAN;
}

Query3WSRS::~Query3WSRS() {
}

/* args isn't used here */
bool Query3WSRS::run(Globals* g, const vector<string>& args) {
  //testPredicate();
  
  if (g->build_tables) {
    system("rm D6_SHIPDATE");
    system("rm D6_SUPPKEY");
    system("rm D6*wos*");
  }

  system("rm Query3WSRS.out");
  Log::writeToLog("Query3WSRS", 10, "Query 3 starting...");

  // THIS is a rebuild WOS table call!
  // so paramters for num indices and columns do not matter
  //wmanager = new WOSManager(D6_DATA_WOS,  // table name
  //			    2,            // num indices
  //			    8 );          // num columns
  CatalogInstance::getCatalog();
  cout << " CREATED CATALOG " << endl;

  if (g->build_tables) 
    {
      
      Log::writeToLog("Query3WSRS", 10, "Building tables...");
      buildRSTables(g);

      Log::writeToLog("Query3WSRS", 10, "Done building RS");

      //wmanager->writeTuplesToDisk();

      Log::writeToLog("Query3WSRS", 10, "Done building tables");
      delete lam1;
    }

  StopWatch stopWatch;

  int shipdateColIndex = 1;
  int suppkeyColIndex = 2;

  //int before = StopWatch::ticks();

  lam1 = CatalogInstance::getCatalog()->getWOSAM( D6_DATA_WOS, shipdateColIndex );
//wmanager->getWOSAM( shipdateColIndex );

    /*new WOSAM(D6_DATA_WOS, // table name
		   2,          // num indices
		   8,          // num columns
		   shipdateColIndex);         // field num
    */
  //int after = StopWatch::ticks();
  //cout << " Tick time " << ( after - before ) * StopWatch::secondsPerTick() << endl;

  lam2 =  CatalogInstance::getCatalog()->getWOSAM( D6_DATA_WOS, suppkeyColIndex );
  //wmanager->getWOSAM( suppkeyColIndex );
  /*new WOSAM(D6_DATA_WOS,
		   2,
			 8,
			 suppkeyColIndex);
  */
  stopWatch.start();

  //Aggregator *wsAgg = processWS();
  //printWSAgg(wsAgg);
  
  //Aggregator *rsAgg = processRS();
  //printRSAgg(rsAgg);

    
  cout << " processing .... " << endl;
  Aggregator *rsAgg = processRS();
  Aggregator *wsAgg = processWS();
  cout << " done processing rs/ws " << endl;
  // printRSAgg(rsAgg);
  //printWSAgg(wsAgg);
  
  MergeSortedGroups *merge = new MergeSortedGroups(rsAgg, 
						   wsAgg, 
						   2,      // num columns
						   0);     // merge by col index
  merge->setMergeOp(new MergeCountOp());
 

  Operator* srcs[1]={merge};
  int numCols[1]={2};
  
  BlockPrinter* bPrint = new BlockPrinter(srcs,          // data sources (array)
					  numCols,       // num columns per source (array)
					  1,             // num sources
					  "Query3WSRS.out"); // filename
  bPrint->printColumns();
  
  
  cout << "Query 3 took: " << stopWatch.stop() << " ms" <<  endl;
  //delete lam2;

  return true;
}

void Query3WSRS::buildRSTables(Globals *g) {
  {
    Log::writeToLog("Query3WSRS", 10, "Loading column shipdate...");
    //string path = LINEITEM_TABLE_PATH;
    string path = D6_DATA_ROS;
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
					    false);  // position primary
    pagePlacer->placeColumn("D6_SHIPDATE", // name
			    false,          // split on value
			    true);          // value sorted

    delete pagePlacer;
    delete ce;
    delete encoder;
    delete decoder;
  }
  {
    Log::writeToLog("Query3WSRS", 10, "Loading column suppkey...");
    //string path = SUPPKEY_PATH;
    string path = D6_DATA_ROS;

    ColumnExtracter* ce = new ColumnExtracter(path,              // filename
					      2,                 // column index
					      g->force_rebuild); // force rebuild flag
    IntEncoder* encoder = new IntEncoder((Operator*) ce, // data source
					 0,              // column index
					 8*PAGE_SIZE);   // buffer size in bits
    IntDecoder* decoder = new IntDecoder(false);         // value sorted
    PagePlacer* pagePlacer = new PagePlacer(encoder,
					    decoder, 
					    1,       // num indexes XXX
					    true);  // position primary XXX
    pagePlacer->placeColumn("D6_SUPPKEY", // name
			    false,                   // split on value
			    false);                  // value sorted

    delete pagePlacer;
    delete ce;
    delete encoder;
    delete decoder;
  }
}

void Query3WSRS::printWSAgg(Aggregator *agg) {
  system("rm Query3WS.out");

  Operator* srcs[1]={agg};
  int numCols[1]={2};
  
  BlockPrinter* bPrint = new BlockPrinter(srcs,          // data sources (array)
					  numCols,       // num columns per source (array)
					  1,             // num sources
					  "Query3WS.out"); // filename
  bPrint->printColumns();
}

void Query3WSRS::printRSAgg(Aggregator *agg) {
  system("rm Query3RS.out");

  Operator* srcs[1]={agg};
  int numCols[1]={2};
  
  BlockPrinter* bPrint = new BlockPrinter(srcs,          // data sources (array)
					  numCols,       // num columns per source (array)
					  1,             // num sources
					  "Query3RS.out"); // filename
  bPrint->printColumns();
}

Aggregator * Query3WSRS::processRS() {
  Log::writeToLog("Query3WSRS", 10, "Opening columns shipdate and suppkey ...");

  ROSAM* am1 = new ROSAM("D6_SHIPDATE" , // table
		   2);              // num indexes
  ROSAM* am2 = new ROSAM("D6_SUPPKEY", 
		   1);


  Predicate* pred = new Predicate(PRED_TYPE);
  RLEDataSource* ds1 = new RLEDataSource(am1,
					 true); // value sorted
  ds1->setPredicate(pred);
  ds1->changeRHSBinding(PRED_FILTER);

  IntDataSource* ds2 = new IntDataSource(am2,
					 false, // value sorted
					 true); // isROS
  ds2->setPositionFilter(ds1, 0);

  HashCount* hashAgg = new HashCount((Operator*) ds2, 0, (Operator*) ds2, 0);
  hashAgg->setHashFunction(new IdentityHashFunction(MAX_SUPPKEY));
  hashAgg->setHashTableSize(MAX_SUPPKEY);

  return hashAgg;	
}

Aggregator * Query3WSRS::processWS() {
  
  //int shipdateColIndex = 1;
  //int suppkeyColIndex = 2;
  
  WOSAM *am1 = lam1, *am2 = lam2;
  /*  WOSAM* am1 = new WOSAM(D6_DATA_WOS, // table name
			 2,          // num indices
			 8,          // num columns
			 shipdateColIndex);         // field num
  WOSAM* am2 = new WOSAM(D6_DATA_WOS,
			 2,
			 8,
			 suppkeyColIndex);
  */
  Predicate *pred = new Predicate(PRED_TYPE);
  IntDataSource *ds1 = new IntDataSource(am1,
					 true,   // value sorted
					 false); // isROS

  ds1->setPredicate(pred);
  ds1->changeRHSBinding(PRED_FILTER);

  IntDataSource *ds2 = new IntDataSource(am2,
					 false,  // value sorted
					 false); // isROS

  ds2->setPositionFilter(ds1, 0);

  HashCount *hashAgg = new HashCount((Operator*) ds2, 0, (Operator*) ds2, 0);

  hashAgg->setHashFunction(new IdentityHashFunction(MAX_SUPPKEY));
  hashAgg->setHashTableSize(MAX_SUPPKEY);

  return hashAgg;
					 
}

