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
#include "JoinTest.h"

/*
SELECT l.shipdate, s.name
FROM lineitem as l, supplier as s
WHERE l.suppkey=s.suppkey
*/

JoinTest::JoinTest() {}

JoinTest::~JoinTest() {
  //delete m_ws;
  //delete m_rs;
}

bool JoinTest::run(Globals* g, const vector<string>&) {
  system("rm JoinTest.out");
  /*if (g->build_tables) {
    system("rm D8_UNCOMPRESSED_ORDERDATE");
    system("rm D8_UNCOMPRESSED_CUSTKEY");
    system("rm D10_UNCOMPRESSED_CUSTKEY");
    system("rm D10_UNCOMPRESSED_NATIOKEY");
    }*/

  string path = D8_DATA_ROS;
  Log::writeToLog("JoinTest", 10, "JoinTest starting...");
 if (g->build_tables) {
    Log::writeToLog("JoinTest", 10, "Loading column d8.orderdate...");
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
    pagePlacer->placeColumn("D8_orderdate.ros", // name
			    false,          // split on value
			    true);          // value sorted

    delete pagePlacer;
    delete ce;
    delete encoder;
    delete decoder;
  }
 if (g->build_tables) {
    Log::writeToLog("JoinTest", 10, "Loading column d8.custkey...");
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
					    false);  // position primary ???
    pagePlacer->placeColumn("D8_custkey.ros", // name
			    false,          // split on value
			    false);          // value sorted

    delete pagePlacer;
    delete ce;
    delete encoder;
    delete decoder;
  }

 path = D10_DATA_ROS;
 if (g->build_tables) {
    Log::writeToLog("JoinTest", 10, "Loading column d10.custkey...");
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
    pagePlacer->placeColumn("D10_custkey.ros", // name
			    false,          // split on value
			    true);          // value sorted

    delete pagePlacer;
    delete ce;
    delete encoder;
    delete decoder;
  }
 if (g->build_tables) {
    Log::writeToLog("JoinTest", 10, "Loading column d10.nationkey...");
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
					    false);  // position primary ???
    pagePlacer->placeColumn("D10_nationkey.ros", // name
			    false,          // split on value
			    false);          // value sorted

    delete pagePlacer;
    delete ce;
    delete encoder;
    delete decoder;
  }

 system( "mv D8_orderdate.ros " RUNTIME_DATA );
 system( "mv D8_orderdate.ros.secondary " RUNTIME_DATA );
 system( "mv D8_custkey.ros " RUNTIME_DATA );
  
 system( "mv D10_custkey.ros " RUNTIME_DATA );
 system( "mv D10_custkey.ros.secondary " RUNTIME_DATA );
 system( "mv D10_nationkey.ros " RUNTIME_DATA );

 // unionTest();

  return true;
}

void JoinTest::unionTest() {
  //createDataSources();
  
  ROSAM* am1 = CatalogInstance::getCatalog()->getROSAM("D8_orderdate");
  ROSAM* am2 = CatalogInstance::getCatalog()->getROSAM("D10_nationkey");
  ROSAM* am3 = CatalogInstance::getCatalog()->getROSAM("D8_custkey");
  ROSAM* am4 = CatalogInstance::getCatalog()->getROSAM("D10_custkey");
  WOSAM* am5 = CatalogInstance::getCatalog()->getWOSAM("D8_projcetion", "D8_orderdate");
  WOSAM* am6 = CatalogInstance::getCatalog()->getWOSAM("D10_projection", "D10_nationkey");
  WOSAM* am7 = CatalogInstance::getCatalog()->getWOSAM("D8_projection", "D8_custkey");
  WOSAM* am8 = CatalogInstance::getCatalog()->getWOSAM("D10_projection", "D10_custkey");
  ROSAM* am9 = CatalogInstance::getCatalog()->getROSAM("D8_orderdate");
  ROSAM* am10 = CatalogInstance::getCatalog()->getROSAM("D10_nationkey");
  WOSAM* am11 = CatalogInstance::getCatalog()->getWOSAM("D8_projcetion", "D8_orderdate");
  WOSAM* am12 = CatalogInstance::getCatalog()->getWOSAM("D10_projection", "D10_nationkey");
  ROSAM* am13 = CatalogInstance::getCatalog()->getROSAM("D8_custkey");
  ROSAM* am14 = CatalogInstance::getCatalog()->getROSAM("D10_custkey");
  WOSAM* am15 = CatalogInstance::getCatalog()->getWOSAM("D8_projection", "D8_custkey");
  WOSAM* am16 = CatalogInstance::getCatalog()->getWOSAM("D10_projection", "D10_custkey");

  IntDataSource* l_suppkey_rs1 = new IntDataSource(am3, false, true);
  l_suppkey_rs1->setSingles(true);
  IntDataSource* s_suppkey_rs1 = new IntDataSource(am4, false, true);
  s_suppkey_rs1->setSingles(true);
  IntDataSource* l_suppkey_ws1 = new IntDataSource(am7, false, false);
  IntDataSource* s_suppkey_ws1 = new IntDataSource(am8, false, false);
  IntDataSource* l_suppkey_rs2 = new IntDataSource(am13, false, true);
  l_suppkey_rs2->setSingles(true);
  IntDataSource* s_suppkey_rs2 = new IntDataSource(am14, false, true);
  s_suppkey_rs2->setSingles(true);
  IntDataSource* l_suppkey_ws2 = new IntDataSource(am15, false, false);
  IntDataSource* s_suppkey_ws2 = new IntDataSource(am16, false, false);

  NLJoin* join1 = new NLJoin((Operator*)l_suppkey_ws1, 0, (Operator*) s_suppkey_ws1, 0);
  NLJoin* join2 = new NLJoin((Operator*)l_suppkey_rs1, 0, (Operator*) s_suppkey_ws2, 0);
  NLJoin* join3 = new NLJoin((Operator*)l_suppkey_ws2, 0, (Operator*) s_suppkey_rs1, 0);
  NLJoin* join4 = new NLJoin((Operator*)l_suppkey_rs2, 0, (Operator*) s_suppkey_rs2, 0);

  IntDataSource* shipdate_rs1 = new IntDataSource(am1, false, true);
  shipdate_rs1->setSingles(true);
  IntDataSource* sname_rs1 = new IntDataSource(am2, false, true);
  sname_rs1->setSingles(true);
  IntDataSource* shipdate_ws1 = new IntDataSource(am5, false, false);
  IntDataSource* sname_ws1 = new IntDataSource(am6, false, false);
  IntDataSource* shipdate_rs2 = new IntDataSource(am9, false, true);
  shipdate_rs2->setSingles(true);
  IntDataSource* sname_rs2 = new IntDataSource(am10, false, true);
  sname_rs2->setSingles(true);
  IntDataSource* shipdate_ws2 = new IntDataSource(am11, false, false);
  IntDataSource* sname_ws2 = new IntDataSource(am12, false, false);

  shipdate_rs1->setPositionFilter(join2, 0);
  shipdate_rs2->setPositionFilter(join4, 0);
  shipdate_ws1->setPositionFilter(join1, 0);
  shipdate_ws2->setPositionFilter(join3, 0);
  sname_rs1->setPositionFilter(join3, 1);
  sname_rs2->setPositionFilter(join4, 1);
  sname_ws1->setPositionFilter(join1, 1);
  sname_ws2->setPositionFilter(join2, 1);
  Union *merge1 = new Union(shipdate_rs1, shipdate_rs2, 1);
  Union *merge2 = new Union(shipdate_ws1, shipdate_ws2, 1);
  Union *merge3 = new Union(sname_rs1, sname_rs2, 1);
  Union *merge4 = new Union(sname_ws1, sname_ws2, 1);
  Union *merge5 = new Union(merge1, merge2, 1);
  Union *merge6 = new Union(merge3, merge4, 1);



  Operator *srcs[2] = {merge5, merge6};
  int nCols[2] = {1, 1};

  BlockPrinter *bPrint = new BlockPrinter(srcs,
					  nCols,
					  2,
					  "Union.out");
  bPrint->printColumns();
}

void JoinTest::createDataSources() {
  /*shipdate_ws = new TestDataSrc(2);
  shipdate_rs = new TestDataSrc(2);
  sname_ws = new TestDataSrc(2);
  sname_rs = new TestDataSrc(2);
  l_suppkey_ws = new TestDataSrc(2);
  l_suppkey_rs = new TestDataSrc(2);
  s_suppkey_ws = new TestDataSrc(2);
  s_suppkey_rs = new TestDataSrc(2);*/


  // WOS
  /*for (int i = 0; i < 10; i++) {
    BasicBlock *groupBlock = new BasicBlock(true, true, true);
    groupBlock->setValue(i, i);

    BasicBlock *valBlock = new BasicBlock(true, true, true);
    valBlock->setValue(3, i);

    m_ws->addBlock(groupBlock, 0);
    m_ws->addBlock(valBlock, 1);
  }

  for (int i = 10; i < 20; i++) {
    BasicBlock *groupBlock = new BasicBlock(true, true, true);
    groupBlock->setValue(i, i);
    
    BasicBlock *valBlock = new BasicBlock(true, true, true);
    valBlock->setValue(5, i);

    m_ws->addBlock(groupBlock, 0);
    m_ws->addBlock(valBlock, 1);
  }

  // ROS
  for (int i = 0; i < 10; i++) {
    BasicBlock *groupBlock = new BasicBlock(true, true, true);
    groupBlock->setValue(i, i);

    BasicBlock *valBlock = new BasicBlock(true, true, true);
    valBlock->setValue(7, i);

    m_rs->addBlock(groupBlock, 0);
    m_rs->addBlock(valBlock, 1);
  }

  for (int i = 20; i < 30; i++) {
    BasicBlock *groupBlock = new BasicBlock(true, true, true);
    groupBlock->setValue(i, i);
    
    BasicBlock *valBlock = new BasicBlock(true, true, true);
    valBlock->setValue(11, i);

    m_rs->addBlock(groupBlock, 0);
    m_rs->addBlock(valBlock, 1);
    }*/
}
