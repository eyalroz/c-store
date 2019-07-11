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
#include<iostream>

#include "Plans.h"

using namespace std;

Plans::Plans()
{
}

Plans::~Plans()
{
}

bool query1(Globals* g, const vector<string>& args);
bool query11(Globals* g, const vector<string>& args);

/* args isn't used here */
bool Plans::run(Globals* g, const vector<string>& args)
{
	// this call is simply to instantiate the catalog
	cout << "Instatiating catalog. Please wait until you are asked to enter a query" << endl << endl;
	
	CatalogInstance::getCatalog();
	
  bool interactive = true;
  string s;
  cout << "Do you wish to enter SQL statements interactively? (Y/N) ";
  cin >> s;
  if (!s.empty()) {
    if (((s.find("N", 0)) != string::npos) || (s.find("n", 0) != string::npos))  interactive = false;
  }
  if (interactive)
    cout << "OK - Plans will run in interactive mode !" << endl;
  else
    cout << "OK - Plans will run in batch mode !" << endl;
  cout << endl;
      
	while(1)
	{
		Parser* p = new Parser();
		
		// Initialize un empty list of Node*
		BaseNode::initialize();
		
		// Initialize un empty list of PObject*
		PObject::initialize();
    
    if (interactive) {		
		  cout << "Enter a query or q to quit: " << endl;		
		
    } else {
      string sqlScriptName;
      cout << "Enter file name for your sql script: ";
      cin >> sqlScriptName;
      if (!yy_set_yyin(sqlScriptName.c_str())) {
        cout << "file not foud !" << endl;
        exit(1);
      };
    }
			
		
		// Wait for a query and parse it
		int result = p->doParse();
		if( result == 0 ) 
		{
			cout << "The query parsed properly!\n" ;
				
			// run the query (= make plan + run the plan)
			system ("rm QueryX.out");
			p->runQuery();
		} else {
			cout << "Error: Program did not parse!\n";
		}
		
		
		
		// Delete allocated base nodes (plan)
		BaseNode::uninitialize();
		
		// Delete allocated PObjects (parser)
		PObject::uninitialize();
		
		delete p;
    if (!interactive) yy_reset_yyin();
	}
} 


bool query1(Globals* g, const vector<string>& args)
{
	
cout << "Line 0 " << endl;

  system ("rm Query1.out");
  Log::writeToLog("Query1", 10, "Query 1 starting...");
  bool success=true;
/*
  if (g->build_tables) 
    system ("rm Query1DAT*");


cout << "Line 1 " << endl;
  
  if (g->build_tables) {
    Log::writeToLog("Query1", 10, "Loading Column1...");
    string path=LINEITEM_TABLE_PATH;  // 
	
	cout << "Line 2 " << endl;
	
    //string path=RLE_EXTRACT_LOAD_AND_ACCESS_DATA_PATH;
    ColumnExtracter* ce = new ColumnExtracter(path, 1, g->force_rebuild);
	
	cout << "Line 3 " << endl;
	
    RLEEncoder* encoder=new RLEEncoder((Operator*) ce, 0, 8*PAGE_SIZE, (byte) INT_VALUE_TYPE, (short) 16, (unsigned int) 27, (unsigned int) 16);
 
	cout << "Line 4 " << endl;
	
	RLEDecoder* decoder=new RLEDecoder(true);
	
	cout << "Line 5 " << endl;
	
    PagePlacer* pagePlacer=new PagePlacer(encoder, decoder, 2, false);
	
	cout << "Line 6 " << endl;
	
    pagePlacer->placeColumn("Query1DATA", false, true);
	
	cout << "Line 7 " << endl;
	
    delete pagePlacer;
    delete ce;
    delete encoder;
    delete decoder;
	
	cout << "Line 8 " << endl;
  }

*/
  Log::writeToLog("Query1", 10, "Opening Column1...");
  StopWatch stopWatch;
  stopWatch.start();

cout << "Line 9" << endl;
	
  ROSAM* am1 = new ROSAM( "Query1DATA" , 2);
Predicate* pred=new Predicate(Predicate::OP_GREATER_THAN);

cout << "Line 10" << endl;

  RLEDataSource* ds1=new RLEDataSource(am1,true);
  ds1->setPredicate(pred);
  ds1->changeRHSBinding(9000);
  // ds1->printColumn();
  
cout << "Line 11" << endl;

  Count* agg = new Count((Operator*) ds1, 0, (Operator*) ds1, 0);

  Operator* srcs[1]={agg};
  int numCols[1]={2};
 
 cout << "Line 12" << endl;
  
  BlockPrinter* bPrint=new BlockPrinter(srcs, numCols, 1, "Query1.out");
  
  cout << "Line 13" << endl;
  
  bPrint->printColumns();
  
  cout << "Query 1 took: " << stopWatch.stop() << " ms" <<  endl;
        
  //delete ds1;
  //delete am1;
  //if (BUILD_TABLES) 
  //  system ("rm Query1DAT*");
  return success;
}

bool query11(Globals* g, const vector<string>& args)
{
	
cout << "Line 0 " << endl;

  system ("rm Query11.out");
  Log::writeToLog("Query11", 10, "Query 1 starting...");
  bool success=true;

  Log::writeToLog("Query11", 10, "Opening Column1...");
  StopWatch stopWatch;
  stopWatch.start();


	
  ROSAM* am1 = new ROSAM( "Query1DATA" , 2);
Predicate* pred=new Predicate(Predicate::OP_GREATER_THAN);

  RLEDataSource* ds1=new RLEDataSource(am1,true);
  ds1->setPredicate(pred);
  ds1->changeRHSBinding(9000);
  // ds1->printColumn();
  
//  Count* agg = new Count((Operator*) ds1, 0, (Operator*) ds1, 0);

 // Operator* srcs[1]={agg};
  //int numCols[1]={2};
 
  
   Operator* srcs[1]={ds1};
  int numCols[1]={1};
  
  BlockPrinter* bPrint=new BlockPrinter(srcs, numCols, 1, "Query11.out");
  bPrint->printColumns();
  
  cout << "Query 11 took: " << stopWatch.stop() << " ms" <<  endl;
        
  //delete ds1;
  //delete am1;
  //if (BUILD_TABLES) 
  //  system ("rm Query1DAT*");
  return success;
}
