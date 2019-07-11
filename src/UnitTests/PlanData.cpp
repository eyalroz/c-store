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
 *  PlanData.cpp
 *  cstore
 *
 *  Created by Nga Tran on 4/22/05.
 *  
 *	Generate data for plan tests
 */

#include "PlanData.h"

PlanData::PlanData() {
}

PlanData::~PlanData() {
}


/* args isn't used here */
bool PlanData::run(Globals* g, const vector<string>& args) {
	  bool success=true;
	  
	generate_D1_l_shipdate(g);
	generate_D1_l_suppkey(g);	
	
	generate_D2_o_orderdate(g);
	generate_D2_l_shipdate(g);
	generate_D2_l_suppkey(g);	
	
	generate_D4_l_extendedprice(g);	// Runtime error
	generate_D4_l_returnflag(g);
	generate_D4_c_nationkey(g);

/*	
//	generate_D3_o_orderkey(g);
	generate_D3_o_orderdate(g);
		
	generate_D5_c_custkey(g);
	generate_D5_c_nationkey(g);
*/	
	
	// Data for general queries (any)
	
	 system ("cp  D1_l_shipdate_mini.dat l_shipdate_mini.dat");
	 system ("cp  D1_l_shipdate_mini.dat.secondary l_shipdate_mini.dat.secondary");
	 
	 system ("cp  D1_l_suppkey_mini.dat l_suppkey_mini.dat");
	 //system ("cp  D1_l_suppkey_mini.dat.secondary l_suppkey_mini.dat.secondary");
	 
	 system ("cp  D2_o_orderdate_mini.dat o_orderdate_mini.dat");
	 system ("cp  D2_o_orderdate_mini.dat.secondary o_orderdate_mini.dat.secondary");
	 
	
	 // Data for VLDB 2005 queries 1-7 
	 
	 // l_shipdate for queries 1-3, 2 indexes
	 system ("cp  D1_l_shipdate_mini.dat Q1_l_shipdate_mini.dat");
	 system ("cp  D1_l_shipdate_mini.dat.secondary Q1_l_shipdate_mini.dat.secondary");
	 
	 system ("cp  D1_l_shipdate_mini.dat Q2_l_shipdate_mini.dat");
	 system ("cp  D1_l_shipdate_mini.dat.secondary Q2_l_shipdate_mini.dat.secondary");
	 
	 system ("cp  D1_l_shipdate_mini.dat Q3_l_shipdate_mini.dat");
	 system ("cp  D1_l_shipdate_mini.dat.secondary Q3_l_shipdate_mini.dat.secondary");
	 
	 //	 l_suppkey for queries 2-3, 2 indexes
	 system ("cp  D1_l_suppkey_mini.dat Q2_l_suppkey_mini.dat");
	 
	 system ("cp  D1_l_suppkey_mini.dat Q3_l_suppkey_mini.dat");

	 //	 l_suppkey for queries 4-6, 2 indexes
	 system ("cp  D2_o_orderdate_mini.dat Q4_o_orderdate_mini.dat");
	 system ("cp  D2_o_orderdate_mini.dat.secondary Q4_o_orderdate_mini.dat.secondary");
	 
	 system ("cp  D2_o_orderdate_mini.dat Q5_o_orderdate_mini.dat");
	 system ("cp  D2_o_orderdate_mini.dat.secondary Q5_o_orderdate_mini.dat.secondary");
	 
	 system ("cp  D2_o_orderdate_mini.dat Q6_o_orderdate_mini.dat");
	 system ("cp  D2_o_orderdate_mini.dat.secondary Q6_o_orderdate_mini.dat.secondary");
	 
	 //	 l_shipdate for queries 4-6, 1 index
	 system ("cp  D2_l_shipdate_mini.dat Q4_l_shipdate_mini.dat");
	 
	 system ("cp  D2_l_shipdate_mini.dat Q5_l_shipdate_mini.dat");
	 
	 system ("cp  D2_l_shipdate_mini.dat Q6_l_shipdate_mini.dat");
	 
	 //	 l_suppkey for queries 5-6, 1 index
	 system ("cp  D2_l_suppkey_mini.dat Q5_l_suppkey_mini.dat");
	 
	 system ("cp  D2_l_suppkey_mini.dat Q6_l_suppkey_mini.dat");
	 
	 // l_extendedprice for query 7
	 system ("cp  D4_l_extendedprice_mini.dat Q7_l_extendedprice_mini.dat");
	 
	 // l_returnflag for query 7
	 system ("cp  D4_l_returnflag_mini.dat Q7_l_returnflag_mini.dat");
	 system ("cp  D4_l_returnflag_mini.dat.secondary Q7_l_returnflag_mini.dat.secondary");
	 
	 // c_nationkey for query 7
	 system ("cp  D4_c_nationkey_mini.dat Q7_c_nationkey_mini.dat");

/*	 
//	 system ("cp  D3_o_orderdate_mini.dat o_orderdate_mini.dat");
//	system ("cp  D3_o_orderdate_mini.dat.secondary o_orderdate_mini.dat.secondary");
	 
	 system ("cp  D5_c_custkey_mini.dat c_custkey_mini.dat");
	 system ("cp  D5_c_custkey_mini.dat.secondary c_custkey_mini.dat.secondary");
	 
//	 system ("cp  D5_c_nationkey_mini.dat c_nationkey_mini.dat");
//	 system ("cp  D5_c_nationkey_mini.dat.secondary c_nationkey_mini.dat.secondary");	   
*/
	return success;	
}



// D1.shipdate:
// (l_shipdate | l_shipdate, l_suppkey)

// Used for Query1, Query2, Query3
bool PlanData::generate_D1_l_shipdate(Globals* g) {

  if (g->build_tables) 
    system ("rm D1_l_shipdate_mini.dat");
	
  Log::writeToLog("generate_D1_l_shipdate", 10, "generate_D1_l_shipdate starting...");
  bool success=true;
  
  if (g->build_tables) {
    Log::writeToLog("generate_D1_l_shipdate", 10, "Loading Column...");
	string path=D1_SHIPDATE_MINI;  
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
    pagePlacer->placeColumn("D1_l_shipdate_mini.dat", // name
                            false,          // split on value
                            true);          // value sorted

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
bool PlanData::generate_D1_l_suppkey(Globals* g) {

  if (g->build_tables) 
    system ("rm D1_l_suppkey_mini.dat");
	
  Log::writeToLog("generate_D1_l_suppkey", 10, "generate_D1_l_suppkey starting...");
  bool success=true;
  
  if (g->build_tables) {
    Log::writeToLog("generate_D1_l_suppkey", 10, "Loading Column...");
    string path=D1_SUPPKEY_MINI;  // 

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
    pagePlacer->placeColumn("D1_l_suppkey_mini.dat", // name
                            false,                   // split on value ???
                            false);                  // value sorted							

    delete pagePlacer;
    delete ce;
    delete encoder;
    delete decoder;										 

  }
  
  return success;	
}

// D2.data:
// (o_orderdate, l_shipdate, l_suppkey | o_orderdate, l_suppkey)

// Used for Query4, Query5, Query6
bool PlanData::generate_D2_o_orderdate(Globals* g)
{
  if (g->build_tables) 
    system ("rm D2_o_orderdate_mini.dat");
	
  Log::writeToLog("generate_D2_o_orderdate", 10, "generate_D2_o_orderdate starting...");
  bool success=true;
  
  if (g->build_tables) {
    Log::writeToLog("generate_D2_o_orderdate", 10, "Loading Column...");
    string path=D2_DATA_MINI;  // 
    
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
    pagePlacer->placeColumn("D2_o_orderdate_mini.dat", // name
                            false,          // split on value
                            true);          // value sorted
							
	delete pagePlacer;
    delete ce;
    delete encoder;
    delete decoder;
	
  }
  
  return success;
}


// D2.data:
// (o_orderdate, l_shipdate, l_suppkey | o_orderdate, l_suppkey)

// Used for Query4, Query5, Query6
bool PlanData::generate_D2_l_shipdate(Globals* g)
{
	  if (g->build_tables) 
    system ("rm D2_l_shipdate_mini.dat");
	
  Log::writeToLog("generate_D2_l_shipdate", 10, "generate_D2_l_shipdate starting...");
  bool success=true;
  
  if (g->build_tables) {
    Log::writeToLog("generate_D2_l_shipdate", 10, "Loading Column...");
    string path=D2_DATA_MINI;  // 

    ColumnExtracter* ce = new ColumnExtracter(path,              // filename
                                              2,                 // column index
                                              g->force_rebuild); // force rebuild flag
    IntEncoder* encoder = new IntEncoder((Operator*) ce, // data source
                                         0,              // column index
                                         8*PAGE_SIZE);   // buffer size in bits
    IntDecoder* decoder = new IntDecoder(false);         // value unsorted
    PagePlacer* pagePlacer = new PagePlacer(encoder,
                                            decoder, 
                                            1,       // num indexes
                                            true);  // position primary ???
    pagePlacer->placeColumn("D2_l_shipdate_mini.dat", // name
                            false,                   // split on value ???
                            false);                  // value sorted

    delete pagePlacer;
    delete ce;
    delete encoder;
    delete decoder;
  }
		  
  return success;
}

// D2.data:
// (o_orderdate, l_shipdate, l_suppkey | o_orderdate, l_suppkey)

// Used for Query5, Query6
bool PlanData::generate_D2_l_suppkey(Globals* g)
{
	  if (g->build_tables) 
    system ("rm D2_l_suppkey_mini.dat");
	
  Log::writeToLog("generate_D2_l_suppkey", 10, "generate_D2_l_suppkey starting...");
  bool success=true;
  
  if (g->build_tables) {
    Log::writeToLog("generate_D2_l_suppkey", 10, "Loading Column...");
    string path=D2_DATA_MINI;  // 
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
    pagePlacer->placeColumn("D2_l_suppkey_mini.dat", // name
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
bool PlanData::generate_D3_o_orderkey(Globals* g)
{
	  if (g->build_tables) 
    system ("rm D3_o_orderkey_mini.dat");
	
  Log::writeToLog("generate_D3_o_orderkey", 10, "generate_D3_o_orderkey starting...");
  bool success=true;
  
  if (g->build_tables) {
    Log::writeToLog("generate_D3_o_orderkey", 10, "Loading Column...");
    string path=D3_DATA_MINI;  // 
    //string path=RLE_EXTRACT_LOAD_AND_ACCESS_DATA_PATH;
    ColumnExtracter* ce = new ColumnExtracter(path, 1, g->force_rebuild);
    RLEEncoder* encoder=new RLEEncoder((Operator*) ce, 0, 8*PAGE_SIZE, (byte) INT_VALUE_TYPE, (short) 16, (unsigned int) 27, (unsigned int) 16);
    RLEDecoder* decoder=new RLEDecoder(true);
    PagePlacer* pagePlacer=new PagePlacer(encoder, decoder, 2, false);
    pagePlacer->placeColumn("D3_o_orderkey_mini.dat", false, true);
    delete pagePlacer;
    delete ce;
    delete encoder;
    delete decoder;
  }
  Log::writeToLog("D3_o_orderkey_mini.dat", 10, "Opening Column...");
  StopWatch stopWatch;
  
  return success;
}

bool PlanData::generate_D3_o_orderdate(Globals* g)
{
	  if (g->build_tables) 
    system ("rm D3_o_orderdate_mini.dat");
	
  Log::writeToLog("generate_D3_o_orderdate", 10, "generate_D3_o_orderdate starting...");
  bool success=true;
  
  if (g->build_tables) {
    Log::writeToLog("generate_D3_o_orderdate", 10, "Loading Column...");
    string path=D3_DATA_MINI;  // 
    //string path=RLE_EXTRACT_LOAD_AND_ACCESS_DATA_PATH;
    ColumnExtracter* ce = new ColumnExtracter(path, 2, g->force_rebuild);
    RLEEncoder* encoder=new RLEEncoder((Operator*) ce, 0, 8*PAGE_SIZE, (byte) INT_VALUE_TYPE, (short) 16, (unsigned int) 27, (unsigned int) 16);
    RLEDecoder* decoder=new RLEDecoder(true);
    PagePlacer* pagePlacer=new PagePlacer(encoder, decoder, 2, false);
    pagePlacer->placeColumn("D3_o_orderdate_mini.dat", false, true);
    delete pagePlacer;
    delete ce;
    delete encoder;
    delete decoder;
  }
  Log::writeToLog("D3_o_orderdate_mini.dat", 10, "Opening Column...");
  StopWatch stopWatch;
  
  return success;
}

bool PlanData::generate_D3_o_custkey(Globals* g)
{
	  if (g->build_tables) 
    system ("rm D3_o_custkey_mini.dat");
	
  Log::writeToLog("generate_D3_o_custkey", 10, "generate_D3_o_orderdate starting...");
  bool success=true;
  
  if (g->build_tables) {
    Log::writeToLog("generate_D3_o_custkey", 10, "Loading Column...");
    string path=D3_CUSTKEY_MINI;  // 
    //string path=RLE_EXTRACT_LOAD_AND_ACCESS_DATA_PATH;
    ColumnExtracter* ce = new ColumnExtracter(path, 1, g->force_rebuild);
    RLEEncoder* encoder=new RLEEncoder((Operator*) ce, 0, 8*PAGE_SIZE, (byte) INT_VALUE_TYPE, (short) 16, (unsigned int) 27, (unsigned int) 16);
    RLEDecoder* decoder=new RLEDecoder(true);
    PagePlacer* pagePlacer=new PagePlacer(encoder, decoder, 2, false);
    pagePlacer->placeColumn("D3_o_custkey_mini.dat", false, true);
    delete pagePlacer;
    delete ce;
    delete encoder;
    delete decoder;
  }
  Log::writeToLog("D3_o_custkey_mini.dat", 10, "Opening Column...");
  StopWatch stopWatch;
  
  return success;
}
*/
	
// D4.data:
// (l_extendedprice, l_returnflag, c_nationkey | l_returnflag)

// Used for Query7
bool PlanData::generate_D4_l_extendedprice(Globals* g)
{  
	if (g->build_tables) 
    system ("rm D4_l_returnflag_mini.dat");
	
	bool success=true;
	
	if (g->build_tables) {
    Log::writeToLog("generate_D4_l_extendedprice", 10, "Loading Column...");
	string path=D4_DATA_MINI;  
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
    pagePlacer->placeColumn("D4_l_extendedprice_mini.dat", // name
                            false,                   // split on value ???
                            false);                  // value sorted

    delete pagePlacer;
    delete ce;
    delete encoder;
    delete decoder;	
  }
  
  return success;
}


// D4.data:
// (l_extendedprice, l_returnflag, c_nationkey | l_returnflag)

// Used for Query7
bool PlanData::generate_D4_l_returnflag(Globals* g)
{					
	if (g->build_tables) 
    system ("rm D4_l_returnflag_mini.dat");
	
  Log::writeToLog("generate_D4_l_returnflag", 10, "generate_D4_l_returnflag starting...");
  bool success=true;
  
  if (g->build_tables) {
    Log::writeToLog("generate_D4_l_returnflag", 10, "Loading Column...");
	string path=D4_DATA_MINI;  
	ColumnExtracter* ce = new ColumnExtracter(path,              // filename
                                              2,                 // column index
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
    pagePlacer->placeColumn("D4_l_returnflag_mini.dat", // name
                            false,          // split on value
                            true);          // value sorted

    delete pagePlacer;
    delete ce;
    delete encoder;
    delete decoder;
	}
	
	return success;

}


// D4.data:
// (l_extendedprice, l_returnflag, c_nationkey | l_returnflag)

// Used for Query7
bool PlanData::generate_D4_c_nationkey(Globals* g)
{
	if (g->build_tables) 
    system ("rm D4_c_nationkey_mini.dat");
	
  Log::writeToLog("generate_D4_c_nationkey", 10, "generate_D4_c_nationkey starting...");
  bool success=true;
  
	if (g->build_tables) {
    Log::writeToLog("generate_D4_c_nationkey", 10, "Loading Column...");
	
	
	string path=D4_DATA_MINI;  
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
    pagePlacer->placeColumn("D4_c_nationkey_mini.dat", // name
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
bool PlanData::generate_D5_c_custkey(Globals* g)
{
	  if (g->build_tables) 
    system ("rm D5_c_custkey_mini.dat");
	
  Log::writeToLog("generate_D5_c_custkey", 10, "generate_D5_c_custkey starting...");
  bool success=true;
  
  if (g->build_tables) {
    Log::writeToLog("generate_D5_c_custkey", 10, "Loading Column...");
    string path=D5_DATA_MINI;  // 
    //string path=RLE_EXTRACT_LOAD_AND_ACCESS_DATA_PATH;
    ColumnExtracter* ce = new ColumnExtracter(path, 1, g->force_rebuild);
    RLEEncoder* encoder=new RLEEncoder((Operator*) ce, 0, 8*PAGE_SIZE, (byte) INT_VALUE_TYPE, (short) 16, (unsigned int) 27, (unsigned int) 16);
    RLEDecoder* decoder=new RLEDecoder(true);
    PagePlacer* pagePlacer=new PagePlacer(encoder, decoder, 2, false);
    pagePlacer->placeColumn("D5_c_custkey_mini.dat", false, true);
    delete pagePlacer;
    delete ce;
    delete encoder;
    delete decoder;
  }
  Log::writeToLog("D5_c_custkey_mini.dat", 10, "Opening Column...");
  StopWatch stopWatch;
  
  return success;
}

bool PlanData::generate_D5_c_nationkey(Globals* g)
{
	  if (g->build_tables) 
    system ("rm D5_c_nationkey_mini.dat");
	
  Log::writeToLog("generate_D5_c_nationkey", 10, "generate_D5_c_nationkey starting...");
  bool success=true;
  
  if (g->build_tables) {
    Log::writeToLog("generate_D5_c_nationkey", 10, "Loading Column...");
    string path=D5_DATA_MINI;  // 
    //string path=RLE_EXTRACT_LOAD_AND_ACCESS_DATA_PATH;
    ColumnExtracter* ce = new ColumnExtracter(path, 2, g->force_rebuild);
    RLEEncoder* encoder=new RLEEncoder((Operator*) ce, 0, 8*PAGE_SIZE, (byte) INT_VALUE_TYPE, (short) 16, (unsigned int) 27, (unsigned int) 16);
    RLEDecoder* decoder=new RLEDecoder(true);
    PagePlacer* pagePlacer=new PagePlacer(encoder, decoder, 2, false);
    pagePlacer->placeColumn("D5_c_nationkey_mini.dat", false, true);
    delete pagePlacer;
    delete ce;
    delete encoder;
    delete decoder;
  }
  Log::writeToLog("D5_c_nationkey_mini.dat", 10, "Opening Column...");
  StopWatch stopWatch;
  
  return success;
}
*/


