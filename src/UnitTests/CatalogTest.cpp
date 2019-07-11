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
 * CatalogTest:
 *
 */
#include "CatalogTest.h"
#include "CatalogTestBDB.h"
#include "stdlib.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
CatalogTest::CatalogTest() 
{
}

CatalogTest::~CatalogTest() 
{
}

/* args is not used here */
bool CatalogTest::run(Globals* g, const vector<string>& args)
{
  Log::writeToLog("CatalogTest", 0, "CatalogTest is starting...");
  bool success=true;

  if (args.size() > 0) {
    switch (atoi(args[0].c_str()))
    {
      case 1:
        success &= runTestCase1(g, args);     //basic file operation open/close read/write
        break;
      case 2:                                 
        success &= runTestCase2(g, args);     //test duplicate key  
        break;
      case 3:                                 //loading catalog from text files
        if (args.size() != 3) {
          cout << "\tmissing argument: two input files are needed !" << endl;
          success = false;        
        } else {
          
          
          
          success &= runTestCase3(g, args);
        }
        break;
      default:
        cout << "\tinvalid input argument: must be 1, 2 or 3" << endl;
        break;            
    }     
  } else {
    success &= runTestCase(g, args);   //catalog function
  }
  
  return success; 
}

bool CatalogTest::runTestCase(Globals* g, const vector<string>& args) 
{
  //Log::writeToLog("CatalogTest", 0, "CatalogTest is starting...");
  bool success=true;

  CatalogInstance *ci = CatalogInstance::getCatalog();

  ci->getWOSManager("D6.data")->getROSWOSSegment();

  if ( ci->isSorted( "lineitem", "l_orderkey" ) )
    cout << " l_orderkey is Sorted! " << endl << endl;
  else
    cout << " l_orderkey is not Sorted! " << endl << endl;

  if ( ci->isSorted( "orders", "o_orderkey" ) )
    cout << " o_orderkey is Sorted! " << endl << endl;
  else
    cout << " o_orderkey is not Sorted! " << endl << endl;

  cout << " Compression type of l_orderkey is " << ci->getEncodingType( "lineitem", "l_orderkey" ) << endl;
  cout << " Compression type of o_orderkey is " << ci->getEncodingType( "orders", "o_orderkey" ) << endl;

  cout << " Primary column in lineitem is " << ci->getPrimaryColumnName( "lineitem" ) << endl;
  
  //cout << ci->toString();

  vector<string> *names = ci->getColumnNames( "lineitem" );

  for ( vector<string>::iterator iter = names->begin(); iter != names->end(); iter++ )
    cout << " Got column name " << (*iter) << endl;
  cout << endl;

  names = ci->getSortOrderColumns( "lineitem" );

  for ( vector<string>::iterator iter = names->begin(); iter != names->end(); iter++ )
    cout << " ** SORT ORDER ** for lineitem Got column name " << (*iter) << endl;
  cout << endl;

  names = ci->getSortOrderColumns( "D6.data" );

  for ( vector<string>::iterator iter = names->begin(); iter != names->end(); iter++ )
    cout << " ** SORT ORDER ** for D6.data Got column name " << (*iter) << endl;
  cout << endl;

  cout << " Testing if projection NOlineitem exists " << ci->isValidProjection( "NOlineitem" ) << endl;
  cout << " Testing if projection lineitem exists " << ci->isValidProjection( "lineitem" ) << endl;

  cout << " Testing if NOlineitem has l_orderkey: " << ci->isColumnOf( "NOlineitem", "l_orderkey" ) << endl;
  cout << " Testing if lineitem has NOl_orderkey: " << ci->isColumnOf( "lineitem", "NOl_orderkey" ) << endl;
  cout << " Testing if lineitem has l_orderkey: " << ci->isColumnOf( "lineitem", "l_orderkey" ) << endl;

  delete ci;
    
  return success;	
}

bool CatalogTest::runTestCase1(Globals* g, const vector<string>& args) 
{
  Log::writeToLog("CatalogTest", 10, "runTestCase1 is starting...");
  bool success=true;
  
  BDBFile* f2 = new BDBFile("CstoreSysTableOld.db");
  int err = f2->open();
  if (err != 0) {
    cout << "open error#" << err << endl;
    exit(1);    
  }
  f2->close();
  delete f2;
  system ("rm CstoreSysTableOld.db");
  system ("rm CstoreSysTable.db");
  
  BDBFile* f1 = new BDBFile("CstoreSysTable.db");
  err = f1->open();
  if (err != 0) {
    cout << "open error#" << err << endl;
    exit(1);    
  }
  
  string PROJECTIONS[ /*19*/ ]
      = { "lineitem", "orders", "customers", "typetest", 
    "Q1_projection", "Q2_projection", "Q3_projection", 
    "Q4_projection", "Q5_projection", "Q6_projection", "Q7_projection",
    "D1.data", "D1.shipdate", "D1.suppkey", "D2.data", "D3.data",
    "D3.custkey", "D4.data", "D5.data" };
    
  int numRec = sizeof( PROJECTIONS )/sizeof( string );
  cout << "\n\nwriteRec" << endl;
  for (int i = 0; i < numRec; ++i)
  {
    char* s = (char*) PROJECTIONS[i].c_str();
    int sz = PROJECTIONS[i].length()+1;
    char* pI = new char[sizeof(int)];
    memcpy(pI, &i, sizeof(int));
    int rtrnCode = f1->writeRec(s, sz, pI, sizeof(int), false);
    if (rtrnCode != 0) {
      cout << "error#" << rtrnCode << endl;
      exit(1);    
    }
    cout << "key=" << s << "\tdata=" << i << endl;
    f1->flush();
  }

  f1->showStats();
      
  cout << "\n\nreadRec" << endl;
  for (int i = 0; i < numRec; ++i)
  {
    char* s = (char*) PROJECTIONS[i].c_str();
    int sz = PROJECTIONS[i].length()+1;
    void* v = f1->readRec(s, sz);
    if (v != NULL) {
      int k;
      memcpy(&k, v, sizeof(int));
      cout << "key=" << s << "\tdata=" << k << endl;
    } else {
      cout << "data=NULL" << endl;
    }  
  }
  
  cout << "\n\nupdateRec" << endl;
  for (int i = 0; i < numRec; ++i)
  {
    char* s = (char*) PROJECTIONS[i].c_str();
    int sz = PROJECTIONS[i].length()+1;
    cout << "updating key=" << s << endl;
    int ii = i * 100 + 2;
    int* pI = &ii;
    int rtrnCode = f1->updateRec(s, sz, pI, sizeof(int));
    if (rtrnCode != 0) {
      cout << "error#" << rtrnCode << endl;
    }
  }
  
  f1->dumpDB();
  
  cout << "\n\ndeleteRec" << endl;
  for (int i = 0; i < numRec; i+=3)
  {
    char* s = (char*) PROJECTIONS[i].c_str();
    int sz = PROJECTIONS[i].length()+1;
    cout << "deleting key=" << s << endl;
    int rtrnCode = f1->deleteRec(s, sz);
    if (rtrnCode != 0) {
      cout << "error#" << rtrnCode << endl;
    }
  }
    
  f1->showStats();
  
  delete f1;  
  return success;
}

bool CatalogTest::runTestCase2(Globals* g, const vector<string>& args) 
{
  Log::writeToLog("CatalogTest", 10, "runTestCase2 is starting...");
  bool success=true;
    
  system ("rm CstoreSysTable.db");
  BDBFile* f1 = new BDBFile("CstoreSysTable.db");
  int err = f1->open();
  if (err != 0) {
    cout << "open error#" << err << endl;
    exit(1);    
  }
  
  string PROJECTIONS[ /*19*/ ]
      = { "lineitem", "orders", "customers", "typetest", 
    "Q1_projection", "Q2_projection", "Q3_projection", 
    "Q4_projection", "Q5_projection", "Q6_projection", "Q7_projection",
    "D1.data", "D1.shipdate", "D1.suppkey", "D2.data", "D3.data",
    "D3.custkey", "D4.data", "D5.data",
      "lineitem", "orders", "customers", "typetest", 
        "Q1_projection", "Q2_projection", "Q3_projection", 
        "Q4_projection", "Q5_projection", "Q6_projection", "Q7_projection",
        "D1.data", "D1.shipdate", "D1.suppkey", "D2.data", "D3.data",
        "D3.custkey", "D4.data", "D5.data" };
    
    int numRec = sizeof( PROJECTIONS )/sizeof( string );
    cout << "\n\nwriteRec" << endl;
    for (int i = 0; i < numRec; ++i)
    {
      char* s = (char*) PROJECTIONS[i].c_str();
      int sz = PROJECTIONS[i].length()+1;
      char* pI = new char[sizeof(int)];
      memcpy(pI, &i, sizeof(int));
      int rtrnCode = f1->writeRec(s, sz, pI, sizeof(int), false);
      if (rtrnCode != 0) {
        cout << "error#" << rtrnCode << endl;
        exit(1);    
      }
      cout << "key=" << s << "\tdata=" << i << endl;
      f1->flush();
    }
    
    cout << "\n\nreadRec then update data value to 100" << endl;
    char* s = (char*) PROJECTIONS[9].c_str();
    int sz = PROJECTIONS[9].length()+1;    
    void* v = f1->readRec(s, sz);
    while (v != NULL) {
      int iVal;
      memcpy(&iVal, v, sizeof(int));
      cout << "key=" << s << "\tdata=" << iVal << endl;
      iVal = 100;
      int* ip = &iVal;
      int rtrnCode = f1->updateRec(ip, sizeof(int));
      if (rtrnCode != 0) {
        cout << "update error#" << rtrnCode << endl;
        v = NULL;
      } else {
        v = f1->readNextRec(s, sz);
      }
    }
    
    cout << "\n\nreadRec then update data value to 200" << endl;
    s = (char*) PROJECTIONS[4].c_str();
    sz = PROJECTIONS[4].length()+1;    
    v = f1->readRec(s, sz);
    while (v != NULL) {
      int iVal;
      memcpy(&iVal, v, sizeof(int));
      cout << "key=" << s << "\tdata=" << iVal << endl;
      iVal = 200;
      int* ip = &iVal;
      int rtrnCode = f1->updateRec(ip, sizeof(int));
      if (rtrnCode != 0) {
        cout << "update error#" << rtrnCode << endl;
        v = NULL;
      } else {
        v = f1->readNextRec(s, sz);
      }
    }
    
    cout << "\n\nreadRec then update data value to 300" << endl;
    s = (char*) PROJECTIONS[1].c_str();
    sz = PROJECTIONS[1].length()+1;    
    v = f1->readRec(s, sz);
    while (v != NULL) {
      int iVal;
      memcpy(&iVal, v, sizeof(int));
      cout << "key=" << s << "\tdata=" << iVal << endl;
      iVal = 300;
      int* ip = &iVal;
      int rtrnCode = f1->updateRec(ip, sizeof(int));
      if (rtrnCode != 0) {
        cout << "update error#" << rtrnCode << endl;
        v = NULL;
      } else {
        v = f1->readNextRec(s, sz);
      }
    }
    
    
    f1->showStats();
    f1->dumpDB();
  
    delete f1;  
    return success;
}

bool CatalogTest::runTestCase3(Globals* g, const vector<string>& args) 
{
  Log::writeToLog("CatalogTest", 10, "runTestCase3 is starting...");
  bool success=true;
  void* v = NULL;

  string projectionFile, columnFile;
      
  if (!args[1].empty()) {
    projectionFile = RUNTIME_DATA+args[1];  
    cout << "\tinput projection file: " + projectionFile << endl;
  } else {
    projectionFile = RUNTIME_DATA "CstoreSysTblProjection.txt";  
    cout << "\tdefault projection file: " + projectionFile + " assumed !" << endl;
  }
  
  if (!args[2].empty()) {
    columnFile = RUNTIME_DATA+args[2];  
    cout << "\tinput column file: " + columnFile << endl;
  } else {
    columnFile = RUNTIME_DATA "CstoreSysTblColumn.txt";
    cout << "\tdefault column file: " + columnFile + " assumed !" << endl;
  }
        
  string dbName = RUNTIME_DATA+CSTORE_SYSTABLE_PROJECTION;
  struct stat statinfo;
  if (stat(dbName.c_str(), &statinfo) == 0) {
    string dbNameOld = dbName + "_old";
    cout << dbName + " already exists - will be renamed to " + dbNameOld << endl;
    system(("mv -f " + dbName + " " + dbNameOld).c_str());
  } else if (errno == ENOENT) {
    cout << dbName + " does NOT exist " << " - creating new file..." << endl;
  }
  cout << endl;
  
  BDBFile* f1 = new BDBFile(dbName);
  int err = f1->open();
  if (err != 0) {
    cout << "open error#" << err << endl;
    exit(1);    
  }
  
  string stringBuf;
    
  PROJECTION_REC projRec;
  std::ifstream inFile(projectionFile.c_str(), std::ios::in);
  
  if ( !inFile )
  {
    std::cerr << "CANNOT open file: " << projectionFile << std::endl;
    throw std::exception();
  }

  int loadCount = 0;
  cout << "\tloading projection catalog";
  while (!inFile.eof())
  {
    memset(&projRec, 0, sizeof(PROJECTION_REC));
    string stringBuf, substring;
    getline(inFile, stringBuf);

    //parse the line
    if (!stringBuf.empty())
    {
      sscanf(stringBuf.c_str(),
             "%128[^#]#%d#%d#%d[^\n]",
             projRec.projectionName,
             &projRec.projectionID,
             &projRec.tableID,
             &projRec.primaryColumnID);
      int rtrnCode = f1->writeRec(&projRec.projectionID, sizeof(projRec.projectionID),
                      &projRec, sizeof(PROJECTION_REC), false);
      if (rtrnCode != 0) {
        cout << "f1->writeRec error:" << rtrnCode << endl;
        exit(1);      
      }
      cout << '.';
      loadCount++;
    }
  }
  cout << "\n\ntotal " << loadCount << " projection(s) loaded" << endl << endl;
  inFile.close();
  //f1->showStats();
  
  //f1->dumpDB();
  
  cout << "===============  PROJECTION in BDB  ===============" << endl;  
  v = f1->readFirstRec(&projRec.projectionID, sizeof(projRec.projectionID));
  while (v != NULL) {
    memcpy(&projRec, v, sizeof(PROJECTION_REC));
    cout.fill(' ');
    cout.width(25);
    cout.setf(ios_base::left  , ios_base::adjustfield);
    cout.setf(ios_base::dec, ios_base::basefield);
    cout  << projRec.projectionName
          << projRec.projectionID
          << projRec.tableID
          << projRec.primaryColumnID << endl;
    v = f1->readNextRec();
  }
  cout << endl;  
  
  int totProjection = f1->getRecordCount();

  /* debug print
  int idKey = 4;
  int* pIDKey = &idKey;
  cout << "\t\t getting projection name for projectionID = " << idKey;  
  v = f1->readRec(pIDKey, sizeof(int));
  if (v != NULL) {
    memcpy(&projRec, v, sizeof(PROJECTION_REC));
    v = f1->readNextRec(pIDKey, sizeof(int));
    cout << "\t\t\tthe name is " << projRec.projectionName << endl;  
  } else {
    cout << "\t\t\trecord not Found!" << endl;  
  } 
  */ 
   
  //----- load columns -----------------------------------------------------------------------------
  COLUMN_REC colRec;
  string dbName2 = RUNTIME_DATA+CSTORE_SYSTABLE_COLUMN;
  if (stat(dbName2.c_str(), &statinfo) == 0) {
    string dbName2Old = dbName2 + "_old";
    cout << dbName2 + " already exists - will be renamed to " + dbName2Old << endl;
    system(("mv -f " + dbName2 + " " + dbName2Old).c_str());
  } else if (errno == ENOENT) {
    cout << dbName2 + " does NOT exist " << " - creating new file..." << endl;
  }
  BDBFile* f2 = new BDBFile(dbName2, true);
  
  //secondary index
  string dbName3 = RUNTIME_DATA+CSTORE_SYSTABLE_COLUMN_AK;
  if (stat(dbName3.c_str(), &statinfo) == 0) {
    string dbName3Old = dbName3 + "_old";
    cout << dbName3 + " already exists - will be renamed to " + dbName3Old << endl;
    system(("mv -f " + dbName3 + " " + dbName3Old).c_str());
  } else if (errno == ENOENT) {
    cout << dbName3 + " NOT exists " << " - creating new file..." << endl;
  }
  cout << endl;
  
  BDBFile* f3 = new BDBFile(dbName3, false);
    
  err = f2->dbHandle->associate(NULL, f3->dbHandle, altKeyCallBack, 0);
  if (err != 0) {
    cout << "associate error#" << err << endl;
    exit(1);    
  }
  
  err = f2->open();
  if (err != 0) {
    cout << "open error#" << err << endl;
    exit(1);    
  }
  
  err = f3->open();
  if (err != 0) {
    cout << "open error#" << err << endl;
    exit(1);
  }
        
  ifstream inFile2(columnFile.c_str(), std::ios::in);
  
  if ( !inFile2 )
  {
    std::cerr << "CANNOT open file: " << columnFile << std::endl;
    throw std::exception();
  }

  loadCount = 0;
  cout << "\tloading column catalog";
  while (!inFile2.eof())
  {
    memset(&colRec, 0, sizeof(COLUMN_REC));
    string stringBuf, substring;
    getline(inFile2, stringBuf);
    
    //parse the line
    if (!stringBuf.empty())
    {
      sscanf(stringBuf.c_str(),
             "%128[^#]#%d#%d#%d#%d#%d[^\n]",
              colRec.columnName,
              &colRec.columnID,          
              &colRec.projectionID,
              &colRec.dataType,
              &colRec.compressionType,
              &colRec.isStorageKey);
      int colKey = ((colRec.projectionID << 16) | colRec.columnID);
      //cout<< colRec.projectionID << ':' << colRec.columnID << '=' << colKey <<endl;
      int rtrnCode = f2->writeRec(&colKey, 4, &colRec, sizeof(COLUMN_REC), false);
      if (rtrnCode != 0) {
        cout << "f2->writeRec error:" << rtrnCode << endl;
        exit(1);      
      }
      cout << '.';
      loadCount++;
    }
  }
  inFile2.close();
  cout << "\n\ntotal " << loadCount << " column(s) loaded" << endl << endl;
  //f2->showStats();
  
  //f2->dumpDB();
  /* debug print
  v = f2->readFirstRec(colRec.columnName, sizeof(colRec.columnName));
  while (v != NULL) {
    memcpy(&colRec, v, sizeof(COLUMN_REC));
    cout  << colRec.columnName
        <<  colRec.columnID
        << colRec.projectionID
        << colRec.dataType
        << colRec.compressionType 
        << colRec.isStorageKey << endl;
    v = f2->readNextRec();
  }
  */  
  cout << "===============  COLUMN in BDB  ===============" << endl;  
  for (int i = 1; i <= totProjection; ++i)
  {
    cout << "\n\nread column with projectionID " << i << endl;
    int* pI = &i;
    v = f3->readRec(pI, sizeof(int));
    while (v != NULL) {
      memcpy(&colRec, v, sizeof(COLUMN_REC));
      cout.fill(' ');
      cout.width(25);
      cout.setf(ios_base::left, ios_base::adjustfield);
      cout.setf(ios_base::dec, ios_base::basefield);
      cout << colRec.columnName;
      cout <<  colRec.columnID;
      cout << colRec.projectionID;
      cout << colRec.dataType;
      cout << colRec.compressionType; 
      cout << colRec.isStorageKey << endl;    
      v = f3->readNextRec(pI, sizeof(int));
    }
  }
  
  f3->close();
  f2->close();
  f1->close();
  return success;   
}

// Used to locate next delimiter in the input string and extract the token
int CatalogTest::nextToken(std::string &theString, std::string &substring)
{
  int pos = theString.find("#");
  substring.assign(theString, 0, pos);
  theString.assign(theString, pos + 1, theString.size());
  return (pos);
}
