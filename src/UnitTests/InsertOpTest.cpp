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
 * InsertOpTest:
 *
 */
#include "InsertOpTest.h"
#include "stdlib.h"
    
InsertOpTest::InsertOpTest() {}

InsertOpTest::~InsertOpTest() {}

/* args is not used here */
bool InsertOpTest::run(Globals* g, const vector<string>& args) 
/* args is not used here */
{
  myG = g;
  
  StopWatch* stopWatch = new StopWatch();
  stopWatch->start();
  
  Log::writeToLog("InsertOpTest", 10, "InsertOpTest starting...");
  
  bool success = true;
  
  success &= runTestCase1();
  
  success &= runTestCase2();
    
  cout << "InsertOpTest took: " << stopWatch->stop() << " ms" <<  endl;
  delete stopWatch;
  
  return success;
}

bool InsertOpTest::runTestCase1() 
{
  /*
  INSERT Syntax 1:

  INSERT INTO table-name [(column-name [,column-name]...)] VALUES (insert-value [, insert-value]...)
  */
  
  // INSERT INTO D1.data.mini
  // VALUES (val1, val2, val3, val4, val5, val6)


  Log::writeToLog("InsertOpTest", 10, "runTestCase1 - single insert");
  bool success = true;
  
  //default test file, contains 200 rows, 6 columns
  string table=D1_DATA_MINI;
   
   //+ for degugging - delete last snapshot
  string wosSnapShot;
  string s = table;
  string::size_type loc;
  loc = s.rfind("/", string::npos);
  if(loc != string::npos) {
    wosSnapShot=s.substr(loc+1);
  } else {
    wosSnapShot=table;    
  }
  
  //comment this line will load WOS data from the previous run
  system (("rm "+wosSnapShot+".TUPLES").c_str());
  //-
  
  WOSManager* wosMgr = new WOSManager(table,  // table name
                                  2,            // num indices
                                  6 );          // num columns
  
  InsertOperator* insop = new InsertOperator(wosMgr);
  
  //this inserting 10 rows to WOS
  const int SORT_KEY_START  = 999999990; 
  const int SORT_KEY_END    = 999999999;
  int insertCount = 0;
  int storage_key = 0, sort_key = 0;
  for (sort_key=SORT_KEY_START; sort_key <= SORT_KEY_END; ++sort_key) {
    storage_key = insop->initTuple();
    cout << "inserting tuple: storage key " << storage_key << " sort key " << sort_key << endl;
    insop->populateField(1, storage_key, sort_key);
    insop->populateField(2, storage_key, 22222222);
    insop->populateField(3, storage_key, 33333333);
    insop->populateField(4, storage_key, 44444444);
    insop->populateField(5, storage_key, 55555555);
    insop->populateField(6, storage_key, 66666666);
    insop->appendTuple(storage_key);
    ++insertCount;
  }
  
  //snapshot WOS to disk for debugging
  wosMgr->rwseg->writeTuplesToDisk();

  /*  
  char *a = NULL;
  storage_key = 0, sort_key = 0;
  int insertedCount = 0;
      
  // Please not that the boolean says that the iterator should be set
  // to that value. Following the first call, all calls should specify false.
  // 
  // 0 returns the pointer to the storage key part of the tuple. The tuple
  // is stored continguously [SK][Field1][Field2]...etc.
  a = wosMgr->rwseg->getNextFieldByStorageKey( 0, 0, true );
  
  memcpy( &storage_key, a, sizeof(int ) );
  memcpy( &sort_key, a+sizeof(int), sizeof(int ) );
  // cout << " first Tuple: storage key " << storage_key << " sort key " << sort_key << endl;

  insertedCount = 0;
  while ( ( a = wosMgr->rwseg->getNextFieldByStorageKey( 0, 0, false )) != NULL )
  {
    memcpy( &storage_key, a, sizeof(int ) );
    memcpy( &sort_key, a+sizeof(int), sizeof(int ) );
    if (sort_key >= SORT_KEY_START  &&  sort_key <= SORT_KEY_END) {
      cout << "inserted tuple: storage key " << storage_key << " sort key " << sort_key << endl;
      ++insertedCount;
    }
  }
  
  
  delete a;  
  if (insertCount != insertedCount) {
    cout << "insertCount=" << insertCount << " insertedCount=" << insertedCount << endl;
    success = false;
  }
  */
  
  delete wosMgr;
  return success;
}


bool InsertOpTest::runTestCase2() 
{
  bool success = true;
  /*
  INSERT Syntax 2:
  
      INSERT INTO table-name [(column-name [,column-name]...)] VALUES (select-statement)
  */
  
  Log::writeToLog("InsertOpTest", 10, "testCase2 starting...");
  cout << endl << "\ttestCase2 - INSERT INTO table [(column...)] VALUES (select-statement)" << endl;
  
  Log::writeToLog("InsertOpTest", 10, "Loading Column...");
  string path=SHIPDATE_MINI;  // 
  ColumnExtracter* ce = new ColumnExtracter(path, 1, myG->force_rebuild);
  RLEEncoder* encoder=new RLEEncoder((Operator*) ce, 0, 8*PAGE_SIZE, (byte) INT_VALUE_TYPE, (short) 16, (unsigned int) 27, (unsigned int) 16);
  RLEDecoder* decoder=new RLEDecoder(true);
  PagePlacer* pagePlacer=new PagePlacer(encoder, decoder, 2, false);
  pagePlacer->placeColumn("SHIPDATE_MINI", false, true);
  delete pagePlacer;
  delete ce;
  delete encoder;
  delete decoder;
    
  //create 6 single-column data source
  ROSAM* am1 = new ROSAM( "SHIPDATE_MINI" , 2);
  RLEDataSource* ds1=new RLEDataSource(am1,true);
  ROSAM* am2 = new ROSAM( "SHIPDATE_MINI" , 2);
  RLEDataSource* ds2=new RLEDataSource(am2,true);
  ROSAM* am3 = new ROSAM( "SHIPDATE_MINI" , 2);
  RLEDataSource* ds3=new RLEDataSource(am3,true);
  ROSAM* am4 = new ROSAM( "SHIPDATE_MINI" , 2);
  RLEDataSource* ds4=new RLEDataSource(am4,true);
  ROSAM* am5 = new ROSAM( "SHIPDATE_MINI" , 2);
  RLEDataSource* ds5=new RLEDataSource(am5,true);
  ROSAM* am6 = new ROSAM( "SHIPDATE_MINI" , 2);
  RLEDataSource* ds6=new RLEDataSource(am6,true);

  Operator* dataSource[] = {ds1, ds2, ds3, ds4, ds5, ds6};
  int columnIndex[] = {1, 2, 3, 4, 5, 6};
    
  //default test file, contains 200 rows, 6 columns
  string table=D1_DATA_MINI;
   
   //+ for degugging - delete last snapshot
  string wosSnapShot;
  string s = table;
  string::size_type loc;
  loc = s.rfind("/", string::npos);
  if(loc != string::npos) {
    wosSnapShot=s.substr(loc+1) + "_testCase2";
  } else {
    wosSnapShot=table + "_testCase2";    
  }
  system (("rm "+wosSnapShot+".TUPLES").c_str());
  //-
  
  WOSManager* wosMgr = new WOSManager(wosSnapShot,  // table name
                                      2,            // num indices
                                      6 );          // num columns
  
  InsertOperator* insertOp = new InsertOperator(wosMgr);
    
  insertOp->appendColumns(dataSource, 6, columnIndex);  
  
  //snapshot WOS to disk for debugging
  //wosMgr->rwseg->writeTuplesToDisk();
  
  return success;
}
