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
 * SelectDV:
 *
 */
#include "SelectDV.h"

SelectDV::SelectDV() {
}

SelectDV::~SelectDV() {
}

/* args isn't used here */
bool SelectDV::run(Globals* g, const vector<string>& args) {
  StopWatch stopWatch;
  stopWatch.start();
  bool success = true;

  //Example code to show how delete vector works
  //first part of the example delete tuples from Q2_projection where Q2_l_shipdate < 8037
  //
  //test data of Q2_projeciton contains
  //
  //  total   Q2_l_shipdate
  //  -----   -------------
  //  196     8036
  //  412     8037
  //
  //
  //second part of the example showed using delete vectors as position filters.
  //SQL: select Q2_l_shipdate from Q2_l_shipdate where Q2_l_shipdate < 8038
  //
  //result file QueryX.out should only contain 8037s since 8036s have been deleted in the first
  //part of the code
  //
  //Note: there are debug output code in each step, uncomment it to see the delete vetor output
  //      but doing so the final output QueryX.out will not be correct since the data blocks
  //      would be consumed eariler in the sequence.      

  
  //ROS OPERATORS:
  //SELECT:
  ROSAM* lp_AM300 = CatalogInstance::getCatalog()->getROSAM("Q2_l_shipdate");
  Operator* lp_ROS300 = new RLEDataSource(lp_AM300, true);
  ( (DataSource*) lp_ROS300)->setPredicate(new Predicate((Predicate::PredicateType) 0));
  ( (DataSource*) lp_ROS300)->changeRHSBinding(8037);
  
  //DELETE ROS FROM PROJECTION:
  // The following 2 lines of code will be replaced by
  // WOSManager* lpWOSManager = CatalogInstance::getCatalog()->getWOSManager(m_sProjection);
  string sColumnAny100 = CatalogInstance::getCatalog()->getPrimaryColumnName("Q2_projection");
  WOSManager* lpROS_WOSManager100 = CatalogInstance::getCatalog()->getWOSManager(sColumnAny100);

  DeleteOperator* lpDeleteOp = new DeleteOperator(lpROS_WOSManager100);
  lpDeleteOp->deleteTuple(lp_ROS300, true);

//construct ROS delete vector
  IntDataSource *dvROS = new IntDataSource(
      lpROS_WOSManager100->getDVAM( true ),
  true,   // value sorted
  false); // isROS

  /* Debug - OUPUT ROS DV:
  Operator* srcs5[1] = {dvROS};
  int numColumns5[1] = {1};
  BlockPrinter* bPrint5 = new BlockPrinter(srcs5, numColumns5, 1, "QueryX_dvROS.out");
  bPrint5->printColumnsWithPosition();
  */

//WOS OPERATORS:
//SELECT:
  // in ideal world, the first paramter would be projection name.
  // for now catalog will recognize the column name instead.
  WOSAM* lp_WOSAM300 = CatalogInstance::getCatalog()->getWOSAM("Q2_l_shipdate", "Q2_l_shipdate");
  Operator* lp_WOS300 = new IntDataSource(lp_WOSAM300, true, false);
  ( (DataSource*) lp_WOS300)->setPredicate(new Predicate((Predicate::PredicateType) 0));
  ( (DataSource*) lp_WOS300)->changeRHSBinding(8037);

//DELETE WOS FROM PROJECTION:
  lpDeleteOp->deleteTuple(lp_WOS300, false);

 
//*************************************************************************************************
//Begin second part of the example:
//
//Query: SELECT Q2_l_shipdate FROM Q2_projection WHERE (Q2_l_shipdate < 8038)!
//
//
//construct WOS delete vector
  StopWatch selectTimer;
  selectTimer.start();

  IntDataSource *dvWOS = new IntDataSource(
    lpROS_WOSManager100->getDVAM( false ),
    true,   // value sorted
    false); // isROS

  /* Debug - OUPUT WOS delete vector:
  Operator* srcs4[1] = {dvWOS};
  int numColumns4[1] = {1};
  BlockPrinter* bPrint4 = new BlockPrinter(srcs4, numColumns4, 1, "QueryX_dvWOS.out");
  bPrint4->printColumnsWithPosition();  
  */
//************************************************************************************************
//ROS OPERATORS:
//SELECT:
	// This should probably be taking the projection, but I don't
	//seem to need it now that column names are unique
	ROSAM* lp_AM2 = CatalogInstance::getCatalog()->getROSAM("Q2_l_shipdate");
	Operator* lp_ROS2 = new RLEDataSource(lp_AM2, true);
        ( (DataSource*) lp_ROS2)->setPredicate(new Predicate((Predicate::PredicateType) 0));
        ( (DataSource*) lp_ROS2)->changeRHSBinding(8038);
//VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV
	//OPERATOR: BNotOperator
	BNotOperator* lp_ROS4 = new BNotOperator(dvROS, 0);

	//OPERATOR: BAndOperator
        Operator* lp_ROS5 = new BAndOperator(lp_ROS2, 0, lp_ROS4, 0);

	//data source
	ROSAM* lp_AM6 = CatalogInstance::getCatalog()->getROSAM("Q2_l_shipdate");
	Operator* lp_ROS6 = new RLEDataSource(lp_AM6, true);
	((DataSource*) lp_ROS6)->setPositionFilter(lp_ROS5, 0);

	/* debug - OUPUT filtered ROS data:

	Operator* srcs3[1] = {lp_ROS6};
	int numColumns3[1] = {1};
	BlockPrinter* bPrint3 = new BlockPrinter(srcs3, numColumns3, 1, "QueryX_lpROS6.out");
	bPrint3->printColumnsWithPosition();  

	*/

//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


//WOS OPERATORS:
//SELECT:
	// in ideal world, the first paramter would be projection name.
	// for now catalog will recognize the column name instead.
	WOSAM* lp_WOSAM2 = CatalogInstance::getCatalog()->getWOSAM("Q2_l_shipdate", "Q2_l_shipdate");
	Operator* lp_WOS2 = new IntDataSource(lp_WOSAM2, true, false);
        ( (DataSource*) lp_WOS2)->setPredicate(new Predicate((Predicate::PredicateType) 0));
        ( (DataSource*) lp_WOS2)->changeRHSBinding(8038);
//VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV
	//OPERATOR: BNotOperator
	BNotOperator* lp_WOS4 = new BNotOperator(dvWOS, 0);

	//OPERATOR: BAndOperator
        Operator* lp_WOS5 = new BAndOperator(lp_WOS2, 0, lp_WOS4, 0);

	//data source
	WOSAM* lp_WOSAM6 = CatalogInstance::getCatalog()->getWOSAM("Q2_l_shipdate", "Q2_l_shipdate");
	Operator* lp_WOS6 = new IntDataSource(lp_WOSAM6, true, false);

	((DataSource*) lp_WOS6)->setPositionFilter(lp_WOS5, 0);

//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

	/* debug - OUPUT filtered WOS data:

	Operator* srcs2[1] = {lp_WOS6};
	int numColumns2[1] = {1};
	BlockPrinter* bPrint2 = new BlockPrinter(srcs2, numColumns2, 1, "QueryX_lpWOS6.out");
	bPrint2->printColumnsWithPosition();  

	*/


//MERGE ROS AND WOS:
//UNION:
	Operator* lp_Merge2 = new Union(lp_ROS6, lp_WOS6, 1); 

//OUPUT THE RESULTS:
	Operator* srcs[1] = {lp_Merge2};
	int numColumns[1] = {1};
	BlockPrinter* bPrint = new BlockPrinter(srcs, numColumns, 1, "QueryX.out");
	bPrint->printColumns();
	cout << endl;
	cout << "Select Query took: " << selectTimer.stop() << " ms" <<  endl;
  cout << endl;
  cout << "SelectDV took: " << stopWatch.stop() << " ms" <<  endl;
  return success;	
}
