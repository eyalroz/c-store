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
 * DummyTest:
 *
 */
#include "DummyTest.h"
#include "stdlib.h"
    

DummyTest::DummyTest() {
}

DummyTest::~DummyTest() {
}

/* args is not used here */
bool DummyTest::run(Globals* g, const vector<string>& args) 
{
  bool success = true;


//ROS OPERATORS:
//PROJECT:
//SELECT:
	// This should probably be taking the projection, but I don't
	//seem to need it now that column names are unique
	//ROSAM* lp_AM3 = CatalogInstance::getCatalog()->getROSAM("Q3_l_shipdate");
	//Operator* lp_ROS3 = new RLEDataSource(lp_AM3, true);

//WOS OPERATORS:
//PROJECT:
//SELECT:
	// in ideal world, the first paramter would be projection name.
	// for now catalog will recognize the column name instead.
	WOSAM* lp_WOSAM3 = CatalogInstance::getCatalog()->getWOSAM("Q3_l_shipdate", "Q3_l_shipdate");
	Operator* lp_WOS3 = new IntDataSource(lp_WOSAM3, true, false);

//MERGE ROS AND WOS:
//UNION:
	//Operator* lp_Merge3 = new Union(lp_ROS3, lp_WOS3, 1); 

//ROS OPERATORS:
//PROJECT:
//SELECT:
	// This should probably be taking the projection, but I don't
	//seem to need it now that column names are unique
	//ROSAM* lp_AM5 = CatalogInstance::getCatalog()->getROSAM("Q3_l_suppkey");
	//Operator* lp_ROS5 = new IntDataSource(lp_AM5, false, true);

//WOS OPERATORS:
//PROJECT:
//SELECT:
	// in ideal world, the first paramter would be projection name.
	// for now catalog will recognize the column name instead.
	WOSAM* lp_WOSAM5 = CatalogInstance::getCatalog()->getWOSAM("Q3_l_suppkey", "Q3_l_suppkey");
	Operator* lp_WOS5 = new IntDataSource(lp_WOSAM5, true, false);

//MERGE ROS AND WOS:
//UNION:
	//Operator* lp_Merge5 = new Union(lp_ROS5, lp_WOS5, 1); 

//OUPUT THE RESULTS:
	Operator* srcs[2] = {lp_WOS3, lp_WOS5};
	int numColumns[2] = {1, 1};
	BlockPrinter* bPrint = new BlockPrinter(srcs, numColumns, 2, "QueryX.out");
	bPrint->printColumns();

  return success;	
}
