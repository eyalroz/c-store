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
 *  PlanDataFullScale.cpp
 *  cstore
 *
 *  Created by alexr on 4/26/05.
 *  
 *	Generate data for full scale tests.
 */

#include "PlanData.h"

PlanDataFullScale::PlanDataFullScale() {
}

PlanDataFullScale::~PlanDataFullScale() {
}


/* args isn't used here */
bool PlanDataFullScale::run(Globals* g, const vector<string>& args) {

	  bool success=true;
	  g->build_tables=true;
	  
	  cout << " generate generate_D1_l_shipdate " << endl << endl;
	  generate_D1_l_shipdate(g);
	  cout << " DONE generate generate_D1_l_shipdate " << endl;
	  
	  cout << " generate generate_D1_l_suppkey(g) "<< endl << endl;
	  generate_D1_l_suppkey(g);	
	  cout << " DONE generate generate_D1_l_suppkey(g) "<< endl << endl;

	  cout << " Generate other 6 columns " << endl;
	  generate_D1_l_orderkey(g);	

	  generate_D1_l_partkey(g);	
	  generate_D1_l_linenumber(g);	
	  generate_D1_l_quantity(g);	
	  generate_D1_l_extendedprice(g);	
	  generate_D1_l_returnflag(g);	

	  cout << " generate generate_D2_o_orderdate(g);" << endl << endl;
	  generate_D2_o_orderdate(g);
	  cout << " DONE generate generate_D2_o_orderdate(g);" << endl << endl;
	  
	  cout << " generate generate_D2_l_shipdate(g); " << endl << endl;
	  generate_D2_l_shipdate(g);
	  cout << " DONE generate generate_D2_l_shipdate(g); " << endl << endl;
	  
	  cout << " generate generate_D2_l_suppkey(g); " << endl << endl;
	  generate_D2_l_suppkey(g);	
	  cout << " DONE generate generate_D2_l_suppkey(g); " << endl << endl;


	  generate_D4_l_extendedprice(g);
	  cout << " DONE generate generate_D4_l_extendedprice(g); " << endl << endl;
	  cout << " generate generate_D4_l_returnflag(g); " << endl << endl;
	  generate_D4_l_returnflag(g);
	  cout << " DONE generate generate_D4_l_returnflag(g); " << endl << endl;
	  cout << " generate generate_D4_c_nationkey(g); " << endl << endl;
	  generate_D4_c_nationkey(g);
	  cout << " DONE generate generate_D4_c_nationkey(g); " << endl << endl;
	
/*	
//	generate_D3_o_orderkey(g);
	generate_D3_o_orderdate(g);
		
	generate_D5_c_custkey(g);
	generate_D5_c_nationkey(g);
*/	
	cout << "Copying files... " << endl;

	// Data for general queries (any)

	 system ("cp -f D1_l_shipdate.ros " RUNTIME_DATA "l_shipdate.ros");
	 system ("cp -f D1_l_shipdate.ros.secondary " RUNTIME_DATA "l_shipdate.ros.secondary");
	 
	 system ("cp -f D1_l_suppkey.ros " RUNTIME_DATA "l_suppkey.ros");
	 //system ("cp -f D1_l_suppkey.ros.secondary l_suppkey_full.ros.secondary");
	 // OTHER 6 columns from D6:
	 system ("mv -f D1_l_orderkey.ros " RUNTIME_DATA "l_orderkey.ros");
	 system ("mv -f D1_l_partkey.ros " RUNTIME_DATA "l_partkey.ros");
	 system ("mv -f D1_l_linenumber.ros " RUNTIME_DATA "l_linenumber.ros");
	 system ("mv -f D1_l_quantity.ros " RUNTIME_DATA "l_quantity.ros");
	 system ("mv -f D1_l_extendedprice.ros " RUNTIME_DATA "l_extendedprice.ros");
	 system ("mv -f D1_l_returnflag.ros " RUNTIME_DATA "l_returnflag.ros");

	 system ("ln " RUNTIME_DATA "l_orderkey.ros " RUNTIME_DATA "Q1_l_orderkey.ros");
	 system ("ln " RUNTIME_DATA "l_orderkey.ros " RUNTIME_DATA "Q2_l_orderkey.ros");
	 system ("ln " RUNTIME_DATA "l_orderkey.ros " RUNTIME_DATA "Q3_l_orderkey.ros");
	 system ("ln " RUNTIME_DATA "l_partkey.ros " RUNTIME_DATA "Q1_l_partkey.ros");
	 system ("ln " RUNTIME_DATA "l_partkey.ros " RUNTIME_DATA "Q2_l_partkey.ros");
	 system ("ln " RUNTIME_DATA "l_partkey.ros " RUNTIME_DATA "Q3_l_partkey.ros");
	 system ("ln " RUNTIME_DATA "l_linenumber.ros " RUNTIME_DATA "Q1_l_linenumber.ros");
	 system ("ln " RUNTIME_DATA "l_linenumber.ros " RUNTIME_DATA "Q2_l_linenumber.ros");
	 system ("ln " RUNTIME_DATA "l_linenumber.ros " RUNTIME_DATA "Q3_l_linenumber.ros");
	 system ("ln " RUNTIME_DATA "l_quantity.ros " RUNTIME_DATA "Q1_l_quantity.ros");
	 system ("ln " RUNTIME_DATA "l_quantity.ros " RUNTIME_DATA "Q2_l_quantity.ros");
	 system ("ln " RUNTIME_DATA "l_quantity.ros " RUNTIME_DATA "Q3_l_quantity.ros");
	 system ("ln " RUNTIME_DATA "l_extendedprice.ros " RUNTIME_DATA "Q1_l_extendedprice.ros");
	 system ("ln " RUNTIME_DATA "l_extendedprice.ros " RUNTIME_DATA "Q2_l_extendedprice.ros");
	 system ("ln " RUNTIME_DATA "l_extendedprice.ros " RUNTIME_DATA "Q3_l_extendedprice.ros");
	 system ("ln " RUNTIME_DATA "l_returnflag.ros " RUNTIME_DATA "Q1_l_returnflag.ros");
	 system ("ln " RUNTIME_DATA "l_returnflag.ros " RUNTIME_DATA "Q2_l_returnflag.ros");
	 system ("ln " RUNTIME_DATA "l_returnflag.ros " RUNTIME_DATA "Q3_l_returnflag.ros");

	 
	 system ("cp -f D2_o_orderdate.ros " RUNTIME_DATA "o_orderdate.ros");
	 system ("cp -f D2_o_orderdate.ros.secondary " RUNTIME_DATA "o_orderdate.ros.secondary");
	 
	
	 // Data for VLDB 2005 queries 1-7 
	 
	 // l_shipdate for queries 1-3, 2 indexes
	 //system ("cp -f D1_l_shipdate.ros " RUNTIME_DATA "Q1_l_shipdate.ros");
	 //system ("cp -f D1_l_shipdate.ros.secondary " RUNTIME_DATA "Q1_l_shipdate.ros.secondary");
	 system ("ln " RUNTIME_DATA "l_shipdate.ros " RUNTIME_DATA "Q1_l_shipdate.ros");
	 system ("ln " RUNTIME_DATA "l_shipdate.ros.secondary " RUNTIME_DATA "Q1_l_shipdate.ros.secondary");
	 
	 //system ("cp -f D1_l_shipdate.ros " RUNTIME_DATA "Q2_l_shipdate.ros");
	 //system ("cp -f D1_l_shipdate.ros.secondary " RUNTIME_DATA "Q2_l_shipdate.ros.secondary");
	 system ("ln " RUNTIME_DATA "l_shipdate.ros " RUNTIME_DATA "Q2_l_shipdate.ros");
	 system ("ln " RUNTIME_DATA "l_shipdate.ros.secondary " RUNTIME_DATA "Q2_l_shipdate.ros.secondary");
	 
	 //	 system ("mv -f D1_l_shipdate.ros " RUNTIME_DATA "Q3_l_shipdate.ros");
	 //system ("mv -f D1_l_shipdate.ros.secondary " RUNTIME_DATA "Q3_l_shipdate.ros.secondary");
	 system ("ln " RUNTIME_DATA "l_shipdate.ros " RUNTIME_DATA "Q3_l_shipdate.ros");
	 system ("ln " RUNTIME_DATA "l_shipdate.ros.secondary " RUNTIME_DATA "Q3_l_shipdate.ros.secondary");
	 
	 //	 l_suppkey for queries 2-3, 2 indexes
	 system ("cp -f D1_l_suppkey.ros " RUNTIME_DATA "Q2_l_suppkey.ros");
 
	 //system ("mv -f D1_l_suppkey.ros " RUNTIME_DATA "Q3_l_suppkey.ros");
	 system ("ln " RUNTIME_DATA "Q2_l_suppkey.ros " RUNTIME_DATA "Q3_l_suppkey.ros ");
	 system ("ln " RUNTIME_DATA "Q2_l_suppkey.ros " RUNTIME_DATA "Q1_l_suppkey.ros ");
	 

	 system ("ln " RUNTIME_DATA "Q3_l_shipdate.ros " RUNTIME_DATA "D6_l_shipdate.ros ");
	 system ("ln " RUNTIME_DATA "Q3_l_shipdate.ros.secondary " RUNTIME_DATA "D6_l_shipdate.ros.secondary ");

	 system ("ln " RUNTIME_DATA "Q2_l_suppkey.ros " RUNTIME_DATA "D6_l_suppkey.ros ");
	 system ("ln " RUNTIME_DATA "l_orderkey.ros " RUNTIME_DATA "D6_l_orderkey.ros ");
	 system ("ln " RUNTIME_DATA "l_partkey.ros " RUNTIME_DATA "D6_l_partkey.ros ");
	 system ("ln " RUNTIME_DATA "l_linenumber.ros " RUNTIME_DATA "D6_l_linenumber.ros ");
	 system ("ln " RUNTIME_DATA "l_quantity.ros " RUNTIME_DATA "D6_l_quantity.ros ");	 
	 system ("ln " RUNTIME_DATA "l_extendedprice.ros " RUNTIME_DATA "D6_l_extendedprice.ros ");	 
	 system ("ln " RUNTIME_DATA "l_returnflag.ros " RUNTIME_DATA "D6_l_returnflag.ros ");


	 //	 l_suppkey for queries 4-6, 2 indexes
	 system ("cp -f D2_o_orderdate.ros " RUNTIME_DATA "Q4_o_orderdate.ros");
	 system ("cp -f D2_o_orderdate.ros.secondary " RUNTIME_DATA "Q4_o_orderdate.ros.secondary");
	 
	 //system ("cp -f D2_o_orderdate.ros " RUNTIME_DATA "Q5_o_orderdate.ros");
	 //system ("cp -f D2_o_orderdate.ros.secondary " RUNTIME_DATA "Q5_o_orderdate.ros.secondary");
	 system ("ln " RUNTIME_DATA "Q4_o_orderdate.ros  " RUNTIME_DATA "Q5_o_orderdate.ros" );
	 system ("ln " RUNTIME_DATA "Q4_o_orderdate.ros.secondary  " RUNTIME_DATA "Q5_o_orderdate.ros.secondary" );
	 
	 system ("ln " RUNTIME_DATA "Q4_o_orderdate.ros  " RUNTIME_DATA "D7_o_orderdate.ros" );
	 system ("ln " RUNTIME_DATA "Q4_o_orderdate.ros.secondary  " RUNTIME_DATA "D7_o_orderdate.ros.secondary" );
	 //system ("mv -f D2_o_orderdate.ros " RUNTIME_DATA "Q6_o_orderdate.ros");
	 //system ("mv -f D2_o_orderdate.ros.secondary " RUNTIME_DATA "Q6_o_orderdate.ros.secondary");
	 system ("ln " RUNTIME_DATA "Q4_o_orderdate.ros  " RUNTIME_DATA "Q6_o_orderdate.ros" );
	 system ("ln " RUNTIME_DATA "Q4_o_orderdate.ros.secondary  " RUNTIME_DATA "Q6_o_orderdate.ros.secondary" );
	 
	 //	 l_shipdate for queries 4-6, 1 index
	 system ("cp -f D2_l_shipdate.ros " RUNTIME_DATA "Q4_l_shipdate.ros");
	 
	 //system ("cp -f D2_l_shipdate.ros " RUNTIME_DATA "Q5_l_shipdate.ros");
	 system ("ln " RUNTIME_DATA "Q4_l_shipdate.ros " RUNTIME_DATA "Q5_l_shipdate.ros" );
	 //system ("mv -f D2_l_shipdate.ros " RUNTIME_DATA "Q6_l_shipdate.ros");
	 system ("ln " RUNTIME_DATA "Q4_l_shipdate.ros " RUNTIME_DATA "Q6_l_shipdate.ros" );
	 system ("ln " RUNTIME_DATA "Q4_l_shipdate.ros " RUNTIME_DATA "D7_l_shipdate.ros" );
		 
	 //	 l_suppkey for queries 5-6, 1 index
	 system ("cp -f D2_l_suppkey.ros " RUNTIME_DATA "Q5_l_suppkey.ros");
	 
	 //system ("mv  D2_l_suppkey.ros " RUNTIME_DATA "Q6_l_suppkey.ros");
	 system ("ln " RUNTIME_DATA "Q5_l_suppkey.ros " RUNTIME_DATA "Q6_l_suppkey.ros");
	 system ("ln " RUNTIME_DATA "Q5_l_suppkey.ros " RUNTIME_DATA "D7_l_suppkey.ros");

	 // l_extendedprice for query 7
	 system ("mv -f D4_l_extendedprice.ros " RUNTIME_DATA "Q7_l_extendedprice.ros");
	 // l_returnflag for query 7
	 system ("mv -f D4_l_returnflag.ros " RUNTIME_DATA "Q7_l_returnflag.ros");
	 system ("mv -f D4_l_returnflag.ros.secondary " RUNTIME_DATA "Q7_l_returnflag.ros.secondary");

	 // c_nationkey for query 7
	 system ("mv -f D4_c_nationkey.ros " RUNTIME_DATA "Q7_c_nationkey.ros");

	 // move WOS stuff  -- Don't need to, generation in ../RuntimeData now.
	 /*system( "mv D6.data.wos.TUPLES " RUNTIME_DATA "D6.data.wos.TUPLES" );
	 system( "mv D7.data.wos.TUPLES " RUNTIME_DATA "D7.data.wos.TUPLES" );
	 
	 system( "mv Q1_l_shipdate.wos.TUPLES " RUNTIME_DATA "Q1_l_shipdate.wos.TUPLE" );
	 system( "mv Q2_o_orderdate.wos.TUPLES " RUNTIME_DATA "Q2_o_orderdate.wos.TUPLES" );
	 */
/*	 
//	 system ("cp -f D3_o_orderdate.ros o_orderdate.ros");
//	system ("cp -f D3_o_orderdate.ros.secondary o_orderdate.ros.secondary");
	 
	 system ("cp -f D5_c_custkey.ros c_custkey.ros");
	 system ("cp -f D5_c_custkey.ros.secondary c_custkey.ros.secondary");
	 
//	 system ("cp -f D5_c_nationkey.ros c_nationkey.ros");
//	 system ("cp -f D5_c_nationkey.ros.secondary c_nationkey.ros.secondary");	   
*/
	 cout << " returning success... " << endl;
	return success;	
}



// D1.shipdate:
// (l_shipdate | l_shipdate, l_suppkey)

// Used for Query1, Query2, Query3
bool PlanDataFullScale::generate_D1_l_shipdate(Globals* g) {

  if (g->build_tables) 
    system ("rm -f D1_l_shipdate.ros");
	
  Log::writeToLog("generate_D1_l_shipdate_", 10, "generate_D1_l_shipdate_ starting...");
  bool success=true;
  
  if (g->build_tables) {
    Log::writeToLog("generate_D1_l_shipdate_", 10, "Loading Column...");
	string path=string(D6_FULL)+"ros";  
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
    pagePlacer->placeColumn("D1_l_shipdate.ros", // name
                            false,          // split on value
                            true);          // value sorted


    WOSManager* wosMgr = new WOSManager(string(D6_FULL)+"wos",        // table name
                                        2,                            // num indices
                                        8 );                          // num columns
    
    delete wosMgr;
    
    //WOSAM* wosam = new WOSAM( string(D6_FULL)+"wos", 2, 8, 1 ); //, "D1_l_shipdate_full.wos" );

    //delete wosam;

  string s = string(D6_FULL), str;
  int loc;
  loc = s.rfind("/", string::npos);
  if(s.rfind("/", 0) != string::npos) {
    str=s.substr(loc+1); 
  } else {
    str=s;    
  }

    system ((" cp -f " + ( str+"wos.storage_key " ) + "Q1_l_shipdate.wos.storage_key" ).c_str() );
    system ((" cp -f " + ( str+"wos.sort_key " ) + "Q1_l_shipdate.wos.sort_key" ).c_str() );
    system ((" cp -f " + ( str+"wos.TUPLES " ) + "Q1_l_shipdate.wos.TUPLES" ).c_str() );

    delete pagePlacer;
    delete ce;
    delete encoder;
    delete decoder;
  }
  
  return success;	
}


// D1.suppkey:
// (l_suppkey | l_shipdate, l_suppkey)

// Used for Query2, Query3
bool PlanDataFullScale::generate_D1_l_suppkey(Globals* g) {

  if (g->build_tables) 
    system ("rm -f D1_l_suppkey.ros");
	
  Log::writeToLog("generate_D1_l_suppkey", 10, "generate_D1_l_suppkey starting...");
  bool success=true;
  
  if (g->build_tables) {
    Log::writeToLog("generate_D1_l_suppkey", 10, "Loading Column...");
    string path=string(D6_FULL)+"ros";  // 

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
    pagePlacer->placeColumn("D1_l_suppkey.ros", // name
                            false,                   // split on value ???
                            false);                  // value sorted							

    delete pagePlacer;
    delete ce;
    delete encoder;
    delete decoder;										 

  }
  
  return success;	
}

// D2.rosa:
// (o_orderdate, l_shipdate, l_suppkey | o_orderdate, l_suppkey)

// Used for Query4, Query5, Query6
bool PlanDataFullScale::generate_D2_o_orderdate(Globals* g)
{
  if (g->build_tables) 
    system ("rm -f D2_o_orderdate.ros");
	
  Log::writeToLog("generate_D2_o_orderdate", 10, "generate_D2_o_orderdate starting...");
  bool success=true;
  
  if (g->build_tables) {
    Log::writeToLog("generate_D2_o_orderdate", 10, "Loading Column...");
    string path=string(D7_FULL)+"ros";  // 
    
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
    pagePlacer->placeColumn("D2_o_orderdate.ros", // name
                            false,          // split on value
                            true);          // value sorted
							
    WOSManager* wosMgr = new WOSManager(string(D7_FULL)+"wos",        // table name
                                        2,                            // num indices
                                        3 );                          // num columns
    
    delete wosMgr;

    //WOSAM* wosam = new WOSAM( string(D7_FULL)+"wos", 2, 3, 1 ); //, "D2_o_orderdate_full.wos" );

    //delete wosam;

  string s = string(D7_FULL), str;
  int loc;
  loc = s.rfind("/", string::npos);
  if(s.rfind("/", 0) != string::npos) {
    str=s.substr(loc+1); 
  } else {
    str=s;    
  }

    system ((" cp -f " + ( str+"wos.storage_key " ) + "Q2_o_orderdate.wos.storage_key"  ).c_str());
    system ((" cp -f " + ( str+"wos.sort_key " ) + "Q2_o_orderdate.wos.sort_key" ).c_str() );
    system ((" cp -f " + ( str+"wos.TUPLES " ) + "Q2_o_orderdate.wos.TUPLES" ).c_str() );

	delete pagePlacer;
    delete ce;
    delete encoder;
    delete decoder;
	
  }
  
  return success;
}


// D2.rosa:
// (o_orderdate, l_shipdate, l_suppkey | o_orderdate, l_suppkey)

// Used for Query4, Query5, Query6
bool PlanDataFullScale::generate_D2_l_shipdate(Globals* g)
{
	  if (g->build_tables) 
    system ("rm -f D2_l_shipdate.ros");
	
  Log::writeToLog("generate_D2_l_shipdate", 10, "generate_D2_l_shipdate starting...");
  bool success=true;
  
  if (g->build_tables) {
    Log::writeToLog("generate_D2_l_shipdate", 10, "Loading Column...");
    string path=string(D7_FULL)+"ros";  // 

    // Alexr -- changed this, because column seems to be col2!
    // maybe I am reading the wrong data file?
    ColumnExtracter* ce = new ColumnExtracter(path,              // filename
                                              3,                 // column index
                                              g->force_rebuild); // force rebuild flag
    IntEncoder* encoder = new IntEncoder((Operator*) ce, // data source
                                         0,              // column index
                                         8*PAGE_SIZE);   // buffer size in bits
    IntDecoder* decoder = new IntDecoder(false);         // value unsorted
    PagePlacer* pagePlacer = new PagePlacer(encoder,
                                            decoder, 
                                            1,       // num indexes
                                            true);  // position primary ???
    pagePlacer->placeColumn("D2_l_shipdate.ros", // name
                            false,                   // split on value ???
                            false);                  // value sorted

    delete pagePlacer;
    delete ce;
    delete encoder;
    delete decoder;
  }
		  
  return success;
}

// D2.rosa:
// (o_orderdate, l_shipdate, l_suppkey | o_orderdate, l_suppkey)

// Used for Query5, Query6
bool PlanDataFullScale::generate_D2_l_suppkey(Globals* g)
{
	  if (g->build_tables) 
    system ("rm -f D2_l_suppkey.ros");
	
  Log::writeToLog("generate_D2_l_suppkey", 10, "generate_D2_l_suppkey starting...");
  bool success=true;
  
  if (g->build_tables) {
    Log::writeToLog("generate_D2_l_suppkey", 10, "Loading Column...");
    string path=string(D7_FULL)+"ros";  //
    // --Alexr this data indexes were off! D7 has changed
    // column numbering vs D2, even though it shouldn't have.
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
    pagePlacer->placeColumn("D2_l_suppkey.ros", // name
                            false,                   // split on value ???
                            false);                  // value sorted

    delete pagePlacer;
    delete ce;
    delete encoder;
    delete decoder;

  }
  
  return success;
}

/*	
bool PlanDataFullScale::generate_D3_o_orderkey(Globals* g)
{
	  if (g->build_tables) 
    system ("rm -f D3_o_orderkey.ros");
	
  Log::writeToLog("generate_D3_o_orderkey", 10, "generate_D3_o_orderkey starting...");
  bool success=true;
  
  if (g->build_tables) {
  Log::writeToLog("generate_D3_o_orderkey", 10, "Loading Column...");
  string path=string(D3_DATA_FULL)+"ros";  // 
    //string path=RLE_EXTRACT_LOAD_AND_ACCESS_DATA_PATH;
    ColumnExtracter* ce = new ColumnExtracter(path, 1, g->force_rebuild);
    RLEEncoder* encoder=new RLEEncoder((Operator*) ce, 0, 8*PAGE_SIZE, (byte) INT_VALUE_TYPE, (short) 16, (unsigned int) 27, (unsigned int) 16);
    RLEDecoder* decoder=new RLEDecoder(true);
    PagePlacer* pagePlacer=new PagePlacer(encoder, decoder, 2, false);
    pagePlacer->placeColumn("D3_o_orderkey.ros", false, true);
    delete pagePlacer;
    delete ce;
    delete encoder;
    delete decoder;
  }
  Log::writeToLog("D3_o_orderkey.ros", 10, "Opening Column...");
  StopWatch stopWatch;
  
  return success;
}

bool PlanDataFullScale::generate_D3_o_orderdate(Globals* g)
{
	  if (g->build_tables) 
    system ("rm -f D3_o_orderdate.ros");
	
  Log::writeToLog("generate_D3_o_orderdate", 10, "generate_D3_o_orderdate starting...");
  bool success=true;
  
  if (g->build_tables) {
    Log::writeToLog("generate_D3_o_orderdate", 10, "Loading Column...");
    string path=string(D3_DATA_FULL)+"ros";  // 
    //string path=RLE_EXTRACT_LOAD_AND_ACCESS_DATA_PATH;
    ColumnExtracter* ce = new ColumnExtracter(path, 2, g->force_rebuild);
    RLEEncoder* encoder=new RLEEncoder((Operator*) ce, 0, 8*PAGE_SIZE, (byte) INT_VALUE_TYPE, (short) 16, (unsigned int) 27, (unsigned int) 16);
    RLEDecoder* decoder=new RLEDecoder(true);
    PagePlacer* pagePlacer=new PagePlacer(encoder, decoder, 2, false);
    pagePlacer->placeColumn("D3_o_orderdate.ros", false, true);
    delete pagePlacer;
    delete ce;
    delete encoder;
    delete decoder;
  }
  Log::writeToLog("D3_o_orderdate.ros", 10, "Opening Column...");
  StopWatch stopWatch;
  
  return success;
}

bool PlanDataFullScale::generate_D3_o_custkey(Globals* g)
{
	  if (g->build_tables) 
    system ("rm -f D3_o_custkey.ros");
	
  Log::writeToLog("generate_D3_o_custkey", 10, "generate_D3_o_orderdate starting...");
  bool success=true;
  
  if (g->build_tables) {
    Log::writeToLog("generate_D3_o_custkey", 10, "Loading Column...");
    string path=string(D3_CUSTKEY_FULL)+"ros";  // 
    //string path=RLE_EXTRACT_LOAD_AND_ACCESS_DATA_PATH;
    ColumnExtracter* ce = new ColumnExtracter(path, 1, g->force_rebuild);
    RLEEncoder* encoder=new RLEEncoder((Operator*) ce, 0, 8*PAGE_SIZE, (byte) INT_VALUE_TYPE, (short) 16, (unsigned int) 27, (unsigned int) 16);
    RLEDecoder* decoder=new RLEDecoder(true);
    PagePlacer* pagePlacer=new PagePlacer(encoder, decoder, 2, false);
    pagePlacer->placeColumn("D3_o_custkey.ros", false, true);
    delete pagePlacer;
    delete ce;
    delete encoder;
    delete decoder;
  }
  Log::writeToLog("D3_o_custkey.ros", 10, "Opening Column...");
  StopWatch stopWatch;
  
  return success;
}
*/
	
// D4.rosa:
// (l_extendedprice, l_returnflag, c_nationkey | l_returnflag)

// Used for Query7
bool PlanDataFullScale::generate_D4_l_extendedprice(Globals* g)
{  
	if (g->build_tables) 
    system ("rm -f D4_l_returnflag.ros");
	
	bool success=true;
	
	if (g->build_tables) {
    Log::writeToLog("generate_D4_l_extendedprice", 10, "Loading Column...");
	string path=string(D9_FULL)+"ros";  
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
    pagePlacer->placeColumn("D4_l_extendedprice.ros", // name
                            false,                   // split on value ???
                            false);                  // value sorted

    delete pagePlacer;
    delete ce;
    delete encoder;
    delete decoder;	
  }
  
  return success;
}


// D4.rosa:
// (l_extendedprice, l_returnflag, c_nationkey | l_returnflag)

// Used for Query7
bool PlanDataFullScale::generate_D4_l_returnflag(Globals* g)
{					
	if (g->build_tables) 
    system ("rm -f D4_l_returnflag.ros");
	
  Log::writeToLog("generate_D4_l_returnflag", 10, "generate_D4_l_returnflag starting...");
  bool success=true;
  
  if (g->build_tables) {
    Log::writeToLog("generate_D4_l_returnflag", 10, "Loading Column...");
	string path=string(D9_FULL)+"ros";  
	ColumnExtracter* ce = new ColumnExtracter(path,              // filename
                                              1,                 // column index
                                              g->force_rebuild); // force rebuild flag
    
	RLEEncoder* encoder = new RLEEncoder((Operator*) ce,         // data source
                                         0,                      // column index
                                         8*PAGE_SIZE,            // buffer size in bits
                                         (byte) INT_VALUE_TYPE,  // value type
                                         (short) 8,             // value size
                                         (unsigned int) 27,      // start pos size
                                         (unsigned int) 32);     // reps size
										 
	RLEDecoder* decoder = new RLEDecoder(true);      // value sorted
    PagePlacer* pagePlacer = new PagePlacer(encoder,
                                            decoder, 
                                            2,       // num indexes
                                            false);  // position primary ???
    pagePlacer->placeColumn("D4_l_returnflag.ros", // name
                            false,          // split on value
                            true);          // value sorted

    WOSManager* wosMgr = new WOSManager(string(D9_FULL)+"wos",        // table name
                                        2,                            // num indices
                                        3 );                          // num columns
    
    delete wosMgr;
    
    //WOSAM* wosam = new WOSAM( string(D9_FULL)+"wos", 2, 3, 1 ); //, "D1_l_returnflag_full.wos" );

    //delete wosam;

  string s = string(D7_FULL), str;
  int loc;
  loc = s.rfind("/", string::npos);
  if(s.rfind("/", 0) != string::npos) {
    str=s.substr(loc+1); 
  } else {
    str=s;    
  }

    system ((" cp -f " + ( str+"wos.storage_key " ) + "Q1_l_returnflag.wos.storage_key").c_str() );
    system ((" cp -f " + ( str+"wos.sort_key " ) + "Q1_l_returnflag.wos.sort_key" ).c_str());
    system ((" cp -f " + ( str+"wos.TUPLES " ) + "Q1_l_returnflag.wos.TUPLES" ).c_str() );

    delete pagePlacer;
    delete ce;
    delete encoder;
    delete decoder;
	}
	
	return success;

}


// D4.rosa:
// (l_extendedprice, l_returnflag, c_nationkey | l_returnflag)

// Used for Query7
bool PlanDataFullScale::generate_D4_c_nationkey(Globals* g)
{
	if (g->build_tables) 
    system ("rm -f D4_c_nationkey.ros");
	
  Log::writeToLog("generate_D4_c_nationkey", 10, "generate_D4_c_nationkey starting...");
  bool success=true;
  
	if (g->build_tables) {
    Log::writeToLog("generate_D4_c_nationkey", 10, "Loading Column...");
	
	
	string path=string(D9_FULL)+"ros";  
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
    pagePlacer->placeColumn("D4_c_nationkey.ros", // name
                            false,                   // split on value ???
                            false);                  // value sorted

    delete pagePlacer;
    delete ce;
    delete encoder;
    delete decoder;
  }
  
  return success;
}

/*	
bool PlanDataFullScale::generate_D5_c_custkey(Globals* g)
{
	  if (g->build_tables) 
    system ("rm -f D5_c_custkey.ros");
	
  Log::writeToLog("generate_D5_c_custkey", 10, "generate_D5_c_custkey starting...");
  bool success=true;
  
  if (g->build_tables) {
    Log::writeToLog("generate_D5_c_custkey", 10, "Loading Column...");
    string path=string(D5_DATA_FULL)+"ros";  // 
    //string path=RLE_EXTRACT_LOAD_AND_ACCESS_DATA_PATH;
    ColumnExtracter* ce = new ColumnExtracter(path, 1, g->force_rebuild);
    RLEEncoder* encoder=new RLEEncoder((Operator*) ce, 0, 8*PAGE_SIZE, (byte) INT_VALUE_TYPE, (short) 16, (unsigned int) 27, (unsigned int) 16);
    RLEDecoder* decoder=new RLEDecoder(true);
    PagePlacer* pagePlacer=new PagePlacer(encoder, decoder, 2, false);
    pagePlacer->placeColumn("D5_c_custkey.ros", false, true);
    delete pagePlacer;
    delete ce;
    delete encoder;
    delete decoder;
  }
  Log::writeToLog("D5_c_custkey.ros", 10, "Opening Column...");
  StopWatch stopWatch;
  
  return success;
}

bool PlanDataFullScale::generate_D5_c_nationkey(Globals* g)
{
	  if (g->build_tables) 
    system ("rm -f D5_c_nationkey.ros");
	
  Log::writeToLog("generate_D5_c_nationkey", 10, "generate_D5_c_nationkey starting...");
  bool success=true;
  
  if (g->build_tables) {
    Log::writeToLog("generate_D5_c_nationkey", 10, "Loading Column...");
    string path=string(D10_FULL)+"ros";  // 
    //string path=RLE_EXTRACT_LOAD_AND_ACCESS_DATA_PATH;
    ColumnExtracter* ce = new ColumnExtracter(path, 2, g->force_rebuild);
    RLEEncoder* encoder=new RLEEncoder((Operator*) ce, 0, 8*PAGE_SIZE, (byte) INT_VALUE_TYPE, (short) 16, (unsigned int) 27, (unsigned int) 16);
    RLEDecoder* decoder=new RLEDecoder(true);
    PagePlacer* pagePlacer=new PagePlacer(encoder, decoder, 2, false);
    pagePlacer->placeColumn("D5_c_nationkey.ros", false, true);
    delete pagePlacer;
    delete ce;
    delete encoder;
    delete decoder;
  }
  Log::writeToLog("D5_c_nationkey.ros", 10, "Opening Column...");
  StopWatch stopWatch;
  
  return success;
}
*/


// SIX other columns for D6.data for Chen:

bool PlanDataFullScale::generate_D1_l_orderkey(Globals* g) 
{
  if (g->build_tables) 
    system ("rm -f D1_l_orderkey.ros");
  
  Log::writeToLog("generate_D1_l_orderkey", 10, "generate_D1_l_orderkey starting...");
  bool success=true;
  
  if (g->build_tables) {
    Log::writeToLog("generate_D1_l_orderkey", 10, "Loading Column...");
    string path=string(D6_FULL)+"ros";  // 
    
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
    pagePlacer->placeColumn("D1_l_orderkey.ros", // name
                            false,                   // split on value ???
                            false);                  // value sorted							
    
    delete pagePlacer;
    delete ce;
    delete encoder;
    delete decoder;										   }
  
  return success;	
}

bool PlanDataFullScale::generate_D1_l_partkey(Globals* g) 
{
  if (g->build_tables) 
    system ("rm -f D1_l_partkey.ros");
  
  Log::writeToLog("generate_D1_l_partkey", 10, "generate_D1_l_partkey starting...");
  bool success=true;
  
  if (g->build_tables) {
    Log::writeToLog("generate_D1_l_partkey", 10, "Loading Column...");
    string path=string(D6_FULL)+"ros";  // 
    
    ColumnExtracter* ce = new ColumnExtracter(path,              // filename
                                              4,                 // column index
                                              g->force_rebuild); // force rebuild flag
    
    IntEncoder* encoder = new IntEncoder((Operator*) ce, // data source
                                         0,              // column index
                                         8*PAGE_SIZE);   // buffer size in bits
    
    IntDecoder* decoder = new IntDecoder(false);         // value sorted
    PagePlacer* pagePlacer = new PagePlacer(encoder,
                                            decoder, 
                                            1,       // num indexes
                                            true);  // position primary ???
    pagePlacer->placeColumn("D1_l_partkey.ros", // name
                            false,                   // split on value ???
                            false);                  // value sorted							
    
    delete pagePlacer;
    delete ce;
    delete encoder;
    delete decoder;										   }
  
  return success;	
}

bool PlanDataFullScale::generate_D1_l_linenumber(Globals* g) 
{
  if (g->build_tables) 
    system ("rm -f D1_l_linenumber.ros");
  
  Log::writeToLog("generate_D1_l_linenumber", 10, "generate_D1_l_linenumber starting...");
  bool success=true;
  
  if (g->build_tables) {
    Log::writeToLog("generate_D1_l_linenumber", 10, "Loading Column...");
    string path=string(D6_FULL)+"ros";  // 
    
    ColumnExtracter* ce = new ColumnExtracter(path,              // filename
                                              5,                 // column index
                                              g->force_rebuild); // force rebuild flag
    
    IntEncoder* encoder = new IntEncoder((Operator*) ce, // data source
                                         0,              // column index
                                         8*PAGE_SIZE);   // buffer size in bits
    
    IntDecoder* decoder = new IntDecoder(false);         // value sorted
    PagePlacer* pagePlacer = new PagePlacer(encoder,
                                            decoder, 
                                            1,       // num indexes
                                            true);  // position primary ???
    pagePlacer->placeColumn("D1_l_linenumber.ros", // name
                            false,                   // split on value ???
                            false);                  // value sorted							
    
    delete pagePlacer;
    delete ce;
    delete encoder;
    delete decoder;										   }
  
  return success;	
}

bool PlanDataFullScale::generate_D1_l_quantity(Globals* g) 
{
  if (g->build_tables) 
    system ("rm -f D1_l_quantity.ros");
  
  Log::writeToLog("generate_D1_l_quantity", 10, "generate_D1_l_quantity starting...");
  bool success=true;
  
  if (g->build_tables) {
    Log::writeToLog("generate_D1_l_quantity", 10, "Loading Column...");
    string path=string(D6_FULL)+"ros";  // 
    
    ColumnExtracter* ce = new ColumnExtracter(path,              // filename
                                              6,                 // column index
                                              g->force_rebuild); // force rebuild flag
    
    IntEncoder* encoder = new IntEncoder((Operator*) ce, // data source
                                         0,              // column index
                                         8*PAGE_SIZE);   // buffer size in bits
    
    IntDecoder* decoder = new IntDecoder(false);         // value sorted
    PagePlacer* pagePlacer = new PagePlacer(encoder,
                                            decoder, 
                                            1,       // num indexes
                                            true);  // position primary ???
    pagePlacer->placeColumn("D1_l_quantity.ros", // name
                            false,                   // split on value ???
                            false);                  // value sorted							
    
    delete pagePlacer;
    delete ce;
    delete encoder;
    delete decoder;										   }
  
  return success;	
}

bool PlanDataFullScale::generate_D1_l_extendedprice(Globals* g) 
{
  if (g->build_tables) 
    system ("rm -f D1_l_extendedprice.ros");
  
  Log::writeToLog("generate_D1_l_extendedprice", 10, "generate_D1_l_extendedprice starting...");
  bool success=true;
  
  if (g->build_tables) {
    Log::writeToLog("generate_D1_l_extendedprice", 10, "Loading Column...");
    string path=string(D6_FULL)+"ros";  // 
    
    ColumnExtracter* ce = new ColumnExtracter(path,              // filename
                                              7,                 // column index
                                              g->force_rebuild); // force rebuild flag
    
    IntEncoder* encoder = new IntEncoder((Operator*) ce, // data source
                                         0,              // column index
                                         8*PAGE_SIZE);   // buffer size in bits
    
    IntDecoder* decoder = new IntDecoder(false);         // value sorted
    PagePlacer* pagePlacer = new PagePlacer(encoder,
                                            decoder, 
                                            1,       // num indexes
                                            true);  // position primary ???
    pagePlacer->placeColumn("D1_l_extendedprice.ros", // name
                            false,                   // split on value ???
                            false);                  // value sorted							
    
    delete pagePlacer;
    delete ce;
    delete encoder;
    delete decoder;										   }
  
  return success;	
}


bool PlanDataFullScale::generate_D1_l_returnflag(Globals* g) 
{
  if (g->build_tables) 
    system ("rm -f D1_l_returnflag.ros");
  
  Log::writeToLog("generate_D1_l_returnflag", 10, "generate_D1_l_returnflag starting...");
  bool success=true;
  
  if (g->build_tables) {
    Log::writeToLog("generate_D1_l_returnflag", 10, "Loading Column...");
    string path=string(D6_FULL)+"ros";  // 
    
    ColumnExtracter* ce = new ColumnExtracter(path,              // filename
                                              8,                 // column index
                                              g->force_rebuild); // force rebuild flag
    
    IntEncoder* encoder = new IntEncoder((Operator*) ce, // data source
                                         0,              // column index
                                         8*PAGE_SIZE);   // buffer size in bits
    
    IntDecoder* decoder = new IntDecoder(false);         // value sorted
    PagePlacer* pagePlacer = new PagePlacer(encoder,
                                            decoder, 
                                            1,       // num indexes
                                            true);  // position primary ???
    pagePlacer->placeColumn("D1_l_returnflag.ros", // name
                            false,                   // split on value ???
                            false);                  // value sorted							
    
    delete pagePlacer;
    delete ce;
    delete encoder;
    delete decoder;										   }
  
  return success;	
}


