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
/*
 *  q.cpp
 *  cstore
 *
 *  Created by Nga Tran on 5/3/05.
 *  Copyright 2005 __MyCompanyName__. All rights reserved.
 *
 */

#include "QueryTest.h"

QueryTest::QueryTest() {
}

QueryTest::~QueryTest() {
}


/* args isn't used here */
bool QueryTest::run(Globals* g, const vector<string>& args) {
	bool success=true;
/*	
	cout << "am_l_shipdate = new AM( l_shipdate_mini.dat , 2);" << endl;
	AM* am_l_shipdate = new AM( "l_shipdate_mini.dat" , 2);		
	Operator* ds_l_shipdate  = new RLEDataSource(am_l_shipdate, true); // value sorted
	
	cout << "am_l_suppkey = new AM( l_suppkey_mini.dat , 2);" << endl;
	AM* am_l_suppkey = new AM( "l_suppkey_mini.dat" , 2);		
	Operator* ds_l_suppkey = new IntDataSource(am_l_suppkey, false, true); // value unsorted			
	
	cout << "am_o_orderdate = new AM( o_orderdate_mini.dat , 2);" << endl;
	ROSAM* am_o_orderdate = new ROSAM( "o_orderdate_mini.dat" , 2);		
	Operator* ds_o_orderdate  = new RLEDataSource(am_o_orderdate, true); // value sorted
*/	
	/*
	SELECT l_suppkey, COUNT (l_shipdate)
	FROM lineitem
	GROUP BY l_suppkey;
	*/
	/* // WORK
	Operator* lpOperator = new Count(ds_l_shipdate, 0, ds_l_suppkey, 0);

	Operator* srcs[1] = {lpOperator};
	int numColumns[1] = {2};
	BlockPrinter* bPrint = new BlockPrinter(srcs, numColumns, 1, "QueryTest.out");
	bPrint->printColumns();
	*/
	
	
	
	/*
	SELECT l_suppkey, AVG (l_shipdate)
	FROM lineitem
	GROUP BY l_suppkey;
	*/
	
	/* // FAIL
	Operator* lpOperator = new Average(ds_l_shipdate, 0, ds_l_suppkey, 0);

	Operator* srcs[1] = {lpOperator};
	int numColumns[1] = {2};
	BlockPrinter* bPrint = new BlockPrinter(srcs, numColumns, 1, "QueryTest.out");
	bPrint->printColumns();
	*/
	
	
	
	/*
	SELECT COUNT(l_shipdate)
	FROM lineitem;
	*/
	/* // FAIL
	Operator* lpOperator = new Count(ds_l_shipdate, 0);

	Operator* srcs[1] = {lpOperator};
	int numColumns[1] = {1};
	BlockPrinter* bPrint = new BlockPrinter(srcs, numColumns, 1, "QueryTest.out");
	bPrint->printColumns();
`	*/


	/*
	SELECT COUNT(l_suppkey)
	FROM lineitem;
	*/
	/*
	// FAIL
	Operator* lpOperator = new Count(ds_l_suppkey, 0);

	Operator* srcs[1] = {lpOperator};
	int numColumns[1] = {1};
	BlockPrinter* bPrint = new BlockPrinter(srcs, numColumns, 1, "QueryTest.out");
	bPrint->printColumns();
	*/
	
	
	Log::writeToLog("Query4", 10, "Opening column orderdate and shipdate...");
  StopWatch stopWatch;
  stopWatch.start();

  ROSAM* am1 = new ROSAM("Q4_o_orderdate_mini.dat" ,  // table
                   2);               // num indexes
  ROSAM* am2 = new ROSAM("Q4_l_shipdate_mini.dat", 
                   1);

  Predicate* pred = new Predicate(Predicate::OP_GREATER_THAN);
  RLEDataSource* ds1 = new RLEDataSource(am1, 
                                         true); // value sorted
  ds1->setPredicate(pred);
  ds1->changeRHSBinding(100);
  
  //
  BCopyOperator* lpCopy = new BCopyOperator(ds1, 0, 2);
  //
  
  IntDataSource* ds2 = new IntDataSource(am2, 
                                         false, // value sorted
					 true); // isROS
  //ds2->setPositionFilter(ds1,0);
  ds2->setPositionFilter(lpCopy,0);
  
  /*
  //
  RLEDataSource* ds3 = new RLEDataSource(am1, 
                                         true); // value sorted
  ds3->setPositionFilter(lpCopy,1);
  //
  */

  Max* agg = new Max((Operator*) ds2, 0, (Operator*) ds1, 0);
  //Max* agg = new Max((Operator*) ds2, 0, (Operator*) ds3, 0);

  Operator* srcs[1]={agg};
  int numCols[1]={2};
  
  BlockPrinter* bPrint = new BlockPrinter(srcs,         // data sources
                                          numCols,      // num columns per source
                                          1,            // num sources
                                          "XXX.out");// output filename
  bPrint->printColumns();
  
  cout << "Query 4 took: " << stopWatch.stop() << " ms" <<  endl;
	
	return success;	
}

