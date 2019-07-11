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
 * DVTest:
 *    present examples of how to use delete vector
 */
#include "DVTest.h"
#include "stdlib.h"
    
const string DVTEST_PROJECTION_NAME = "Q2_projection";
const string DVTEST_COLUMN_NAME = "Q2_l_shipdate";
//const string DVTEST_PROJECTION_DATA = "Data/D1.shipdate.mini";
const string DVTEST_PROJECTION_DATA = D7_DATA_ROS;
    
DVTest::DVTest() {
}

DVTest::~DVTest() {
}

/* args is not used here */
bool DVTest::run(Globals* g, const vector<string>& args) 
{
  bool success = true;
  makeData(g, args);
  success &= runTestCase1(g, args);
  success &= runTestCase4(g, args);
  //success &= runTestCase2(g, args);
  //  success &= runTestCase3(g, args);
  return success;
}  
  
/* args is not used here */
bool DVTest::runTestCase1(Globals* g, const vector<string>& args)
{    
  bool success = true;
  
  // delete some rows
  
  WOSManager* wm = new WOSManager(DVTEST_PROJECTION_NAME,  // table name
				 2,                              // num indices
				 1 );                             // num columns
  

  //WOSManager* wm  = CatalogInstance::getCatalog()->getWOSManager(DVTEST_PROJECTION_NAME);

  wm->deleteTuple( 5, false );
  wm->deleteTuple( 4, false );
  wm->deleteTuple( 100, false );
  wm->deleteTuple( 99, false );
  wm->deleteTuple( 88, false );
  wm->deleteTuple( 12, false );
  wm->deleteTuple( 50, false );
  
  /*
  for ( int i = 101; i < 200; i ++ )
    wm->deleteTuple(i , true );
  */
  
  //construct delete vector
  IntDataSource *ds1 = new IntDataSource( wm->getDVAM( false ),
					 true,   // value sorted
					 false); // isROS
  
  //now ds1 has the position list of deleted rows for the projection  
  Operator* srcs[1]={ ds1 };
  int numCols[1]={1};
  
  system ("rm DVTest_WOS.out");
  
  //print out the delete vector
  BlockPrinter* bPrint = new BlockPrinter(srcs,          // data sources (array)
					  numCols,       // num columns per source (array)
					  1,             // num sources
					  "DVTest_WOS.out"); // filename
  bPrint->printColumnsWithPosition();
  delete bPrint;
  delete wm;
  return success;	
}

/* args is not used here */
bool DVTest::runTestCase2(Globals* g, const vector<string>& args) 
{
  bool success = true;
  
  // delete some rows
  WOSManager* wm = new WOSManager(DVTEST_PROJECTION_NAME,         // table name
                                  2,                              // num indices
                                  1 );                            // num columns
  wm->deleteTuple( 5, true );
  wm->deleteTuple( 4, true );
  wm->deleteTuple( 100, true );
  wm->deleteTuple( 99, true );
  wm->deleteTuple( 88, true );
  wm->deleteTuple( 12, true );
  wm->deleteTuple( 50, true );
  
  /*
  for ( int i = 101; i < 200; i ++ )
    wm->deleteTuple(i , true );
  */
  
  //construct delete vector
  IntDataSource *ds1 = new IntDataSource( wm->getDVAM( true ),
                                          true,   // value sorted
                                          false); // isROS
  
  BNotOperator* bnotOp = new BNotOperator(ds1, 0);

  ROSAM* am1 = CatalogInstance::getCatalog()->getROSAM(DVTEST_COLUMN_NAME);
  RLEDataSource* ds2 = new RLEDataSource(am1, true);
  ds2->setPositionFilter(bnotOp, 0);
  
  Operator* ds[1]={ds2};
  int nc[1]={1};
  
  system ("rm DVTest_Filtered.out");

  BlockPrinter* bPrint2 = new BlockPrinter(ds,          // data sources (array)
                                           nc,           // num columns per source (array)
                                           1,            // num sources
                                           "DVTest_Filtered.out"); // filename 
  bPrint2->printColumnsWithPosition();
  
  //delete bPrint;    
  delete bPrint2;    
  delete wm;
  return success; 
}

/* args is not used here */
bool DVTest::runTestCase3(Globals* g, const vector<string>& args) 
{
  bool success = true;
    
  // delete some rows
  WOSManager* wm = new WOSManager(DVTEST_PROJECTION_NAME,         // table name
                                  2,                              // num indices
                                  1 );                            // num columns
  
  wm->deleteTuple( 5, true );
  wm->deleteTuple( 4, true );
  wm->deleteTuple( 100, true );
  wm->deleteTuple( 99, true );
  wm->deleteTuple( 88, true );
  wm->deleteTuple( 12, true );
  wm->deleteTuple( 50, true );
  
  DeleteOperator* deleteOp = new DeleteOperator(wm);
  deleteOp->deleteProjection();
  
  //construct delete vector
  IntDataSource *dsWOS = new IntDataSource( wm->getDVAM( true ),
                                          true,   // value sorted
                                          false); // isROS
  
  Operator* ds[1]={dsWOS};
  int nc[1]={1};
  
  system ("rm DVTest_Deleted.out");

  BlockPrinter* bPrint = new BlockPrinter(ds,          // data sources (array)
                                           nc,           // num columns per source (array)
                                           1,            // num sources
                                           "DVTest_Deleted.out"); // filename 
  bPrint->printColumnsWithPosition();
    
  //delete bPrint;    
  //delete bPrint;    
  delete wm;
  return success; 
}

void DVTest::makeData(Globals* g, const vector<string>& args) 
{  
  if (g->build_tables) {
    string s = "rm " + DVTEST_COLUMN_NAME + "*";
    system (s.c_str());
    cout << "\tDVTest making data..." << endl;  
    ColumnExtracter* ce = new ColumnExtracter(DVTEST_PROJECTION_DATA, 1, g->force_rebuild);
    RLEEncoder* encoder=new RLEEncoder((Operator*) ce, 0, 8*PAGE_SIZE, (byte) INT_VALUE_TYPE, (short) 16, (unsigned int) 27, (unsigned int) 16);
    RLEDecoder* decoder=new RLEDecoder(true);
    PagePlacer* pagePlacer=new PagePlacer(encoder, decoder, 2, false);
    pagePlacer->placeColumn(DVTEST_COLUMN_NAME, false, true);
    delete pagePlacer;
    delete ce;
    delete encoder;
    delete decoder;
    
    string colName = DVTEST_COLUMN_NAME + "_full.ros";
    ColumnExtracter* ce2 = new ColumnExtracter(DVTEST_PROJECTION_DATA, 1, g->force_rebuild);
    RLEEncoder* encoder2 = new RLEEncoder((Operator*) ce2, 0, 8*PAGE_SIZE, (byte) INT_VALUE_TYPE, (short) 16, (unsigned int) 27, (unsigned int) 16);
    RLEDecoder* decoder2 = new RLEDecoder(true);
    PagePlacer* pagePlacer2 = new PagePlacer(encoder2, decoder2, 2, false);
    pagePlacer2->placeColumn(colName, false, true);
    delete pagePlacer2;
    delete ce2;
    delete encoder2;
    delete decoder2;
  }
}
bool DVTest::runTestCase4(Globals* g, const vector<string>& args)
{    
  bool success = true;
  
  // delete some rows
  
  WOSManager* wm = new WOSManager(DVTEST_PROJECTION_NAME,  // table name
				 2,                              // num indices
				 1 );                             // num columns
  

  //WOSManager* wm  = CatalogInstance::getCatalog()->getWOSManager(DVTEST_PROJECTION_NAME);

  wm->deleteTuple( 5, true );
  wm->deleteTuple( 4, true );
  wm->deleteTuple( 100, true );
  wm->deleteTuple( 99, true );
  wm->deleteTuple( 88, true );
  wm->deleteTuple( 12, true );
  wm->deleteTuple( 50, true );
  
  /*
  for ( int i = 101; i < 200; i ++ )
    wm->deleteTuple(i , true );
  */
  
  //construct delete vector
  IntDataSource *ds1 = new IntDataSource( wm->getDVAM( true ),
					 true,   // value sorted
					 false); // isROS
  
  //now ds1 has the position list of deleted rows for the projection  
  Operator* srcs[1]={ ds1 };
  int numCols[1]={1};
  
  system ("rm DVTest_ROS.out");
  
  //print out the delete vector
  BlockPrinter* bPrint = new BlockPrinter(srcs,          // data sources (array)
					  numCols,       // num columns per source (array)
					  1,             // num sources
					  "DVTest_ROS.out"); // filename
  bPrint->printColumnsWithPosition();
  delete bPrint;
  delete wm;
  return success;	
}
