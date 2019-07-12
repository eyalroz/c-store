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
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <string.h>
#include <map>
#include <cmath>
#include <cassert>
#include <db_cxx.h>

#include "BDBFile.h"

using namespace std;

BDBFile::BDBFile(string bdbFileName_)
{
  isPrimary = true;
  this->init(bdbFileName_, true);
}

BDBFile::BDBFile(string bdbFileName_, bool isPrimary_)
{
  this->init(bdbFileName_, isPrimary_);
}

void BDBFile::init(string bdbFileName_, bool isPrimary_)
{
  isPrimary = isPrimary_;
  bdbFileName = bdbFileName_;
  dbHandle = new Db(NULL, 0);
  dbHandle->set_error_stream(&cout);
  if (!isPrimary) dbHandle->set_flags(DB_DUP);
}

BDBFile::~BDBFile()
{
	delete dbHandle;
}

int BDBFile::open() {
  assert(dbHandle != NULL);
  try {
    dbHandle->open(
        NULL,                 //txnid
        bdbFileName.c_str(),  //file 
        NULL,                 //database
        DB_BTREE,             //type
        DB_CREATE,            //flags
        0664                  //mode       
    );
    this->openCursor();
  } catch (DbException &e) {
    return e.get_errno();
  } catch (std::exception &e) {
    throw UnexpectedException("BDBFile.cpp: Failed on open");    
  }
  return 0;
}

int BDBFile::close() {
  assert(dbHandle != NULL);
  this->closeCursor();
  return dbHandle->close(0);
}

/* 
int BDBFile::remove() {
  return dbHandle->remove(bdbFileName.c_str(), NULL, 0);
}

int BDBFile::rename(string newBdbFileName_) {
  assert(dbHandle != NULL);
  bdbFileName = newBdbFileName_;
  return dbHandle->rename(bdbFileName.c_str(), NULL, newBdbFileName_.c_str(), 0);  
}
*/

//Flush a database to stable storage
int BDBFile::flush() {
  assert(dbHandle != NULL);
  return dbHandle->sync(0);
}

int BDBFile::writeRec(void* key_, unsigned int keySize_, void* data_, unsigned int dataSize_, bool allowOverwrite_)
{
  assert(dbHandle != NULL);
  //int Db::put(DbTxn *txnid, Dbt *key, Dbt *data, u_int32_t flags);

  Dbt key(key_, keySize_);
  Dbt data(data_, dataSize_);
  
  int flags = 0, rtrnCode;
  if (allowOverwrite_) flags = DB_NOOVERWRITE;
  try {
    rtrnCode = dbHandle->put(NULL, &key, &data, flags);
    if (rtrnCode != 0) {
      dbHandle->err(rtrnCode, "BDBFile::writeRec");
      if (rtrnCode==DB_REP_HANDLE_DEAD) {
        cout << "The database handle has been invalidated!" << endl;
      } else if (rtrnCode==DB_SECONDARY_BAD) {
        cout << "A secondary index references a nonexistent primary key!" << endl;
      } else {
        cout << "EINVAL" << endl;
      }
    }
  } catch(DbException& e) {
    cout << "DbException " << e.what() << endl;
    throw;
  }
  return rtrnCode;
}

int BDBFile::updateRec(void* key_, unsigned int keySize_, void* newData_, unsigned int newDataSize_)
{
  assert(dbHandle != NULL);
  void* v = readRec(key_, keySize_);
  if (v != NULL) {
    return updateRec(newData_, newDataSize_);
  } else {
    return DB_NOTFOUND; 
  }      
}

int BDBFile::updateRec(void* newData_, unsigned int newDataSize_)
{
  assert(dbHandle != NULL);
  Dbt key;
  Dbc* pDbc(dbCursor);
  int rtrnCode = pDbc->del(0);
  if (rtrnCode == 0) {
    Dbt data(newData_, newDataSize_);
    return pDbc->put(&key, &data, DB_CURRENT);
  } else {
    return DB_NOTFOUND; 
  }
}

void* BDBFile::readRec(void* key_, unsigned int keySize_)    
{
  assert(dbHandle != NULL);
  Dbt key(key_, keySize_);
  Dbt data;
  int rtrnCode = dbCursor->get(&key, &data, DB_SET);
  if (rtrnCode > 0) {
    return NULL;
  } else {
    return data.get_data();
  }
}

void* BDBFile::readNextRec()
{
  assert(dbHandle != NULL);
  int rtrnCode;
  Dbt key, data;
  rtrnCode = dbCursor->get(&key, &data, DB_NEXT);  
  if (rtrnCode > 0) {
    errNum = rtrnCode;
    return NULL;
  } else {
    return data.get_data();
  }
}

void* BDBFile::readNextRec(void* key_, unsigned int keySize_)    
{
  assert(dbHandle != NULL &&  key_ != NULL  && keySize_ > 0);
  int rtrnCode;
  Dbt key, data;
  rtrnCode = dbCursor->get(&key, &data, DB_NEXT_DUP);
  if (rtrnCode > 0) {
    errNum = rtrnCode;
    return NULL;
  } else {
    if (keySize_ == key.get_size()) {
      if (memcmp(key.get_data(), key_, keySize_) == 0) return data.get_data();
    }
    return NULL;
  }
}

void* BDBFile::readFirstRec(void* key_, unsigned int keySize_)    
{
  assert(dbHandle != NULL && dbCursor != NULL);
  Dbt key;
  Dbt data;
  int rtrnCode = dbCursor->get(&key, &data, DB_FIRST);
  if (rtrnCode > 0) {
    key_ = NULL;
    keySize_ = 0;
    return NULL;
  } else {
    key_ = key.get_data();
    keySize_ = key.get_size();
    return data.get_data();
  }
}

void* BDBFile::readLastRec(void* key_, unsigned int keySize_)    
{
  assert(dbHandle != NULL);
  Dbt key;
  Dbt data;
  int rtrnCode = this->cursorRead(key, data, DB_LAST);
  if (rtrnCode > 0) {
    key_ = NULL;
    keySize_ = 0;
    return NULL;
  } else {
    key_ = key.get_data();
    keySize_ = key.get_size();
    return data.get_data();
  }
}

int BDBFile::cursorRead(Dbt& key, Dbt& data, u_int32_t readFlag)    
{
  assert(dbHandle != NULL);
  return dbCursor->get(&key, &data, readFlag);
}

int BDBFile::deleteRec(void* key_, unsigned int keySize_)
{
  assert(dbHandle != NULL);
  //int Db::get(DbTxn *txnid, Dbt *key, Dbt *data, u_int32_t flags);
  Dbt key(key_, keySize_); 
  return dbHandle->del(NULL, &key, 0);
}

int BDBFile::deleteRec()
{
  assert(dbHandle != NULL);
  return dbCursor->del(0);
}

void BDBFile::openCursor()
{
  assert(dbHandle != NULL);
  dbHandle->cursor(NULL, &dbCursor, 0);
}

void BDBFile::closeCursor()
{
  if (dbCursor != NULL) dbCursor->close();
}

// Note: The returned pointer is _owning_ and must be freed
static DB_BTREE_STAT* getBTreeStatistics(Db* dbHandle)
{
  DB_BTREE_STAT *pStat = NULL;

#if OLD_BERKELEY_DB_VERSION
  dbHandle->stat(&pStat, 0);
#else
  DbTxn* not_part_of_concurrent_data_store_group = NULL;
  dbHandle->stat(not_part_of_concurrent_data_store_group, &pStat, 0);
#endif
  return pStat;
}

void BDBFile::showStats()
{
  DB_BTREE_STAT *pStat = getBTreeStatistics(dbHandle);

  // Note: must use free, not delete.
  // This struct is allocated by C.
  //
  cout << bdbFileName << " contains "
      << pStat->bt_ndata << " records\n";
  free(pStat);  
}

u_long BDBFile::getRecordCount()
{
  DB_BTREE_STAT *pStat = getBTreeStatistics(dbHandle);

  // Note: must use free, not delete.
  // This struct is allocated by C.
  //
  u_long cnt = pStat->bt_ndata;
  free(pStat);
  return cnt;  
}

void BDBFile::dumpDB()
{
  cout << "\n\n\n===== dumping contents of " << bdbFileName << " =====\n" << endl;

  // We put a try block around this section of code
  // to ensure that our database is properly closed
  // in the event of an error.
  //
  try {
    // Acquire a cursor for the table.
    Dbc *dbcp;
    dbHandle->cursor(NULL, &dbcp, 0);

    // Walk through the table, printing the key/data pairs.
    Dbt key;
    Dbt data;
    int rtrnCode = 0;
    int count = 0;
    while ((rtrnCode = dbcp->get(&key, &data, DB_NEXT) == 0)) {
      char *key_string = (char *)key.get_data();
      //char *data_string = (char *)data.get_data();
      char* data_string = (char *)data.get_data();
      cout << key_string << " : " << *(data_string) << "\n";
      ++count;
    }
    if (rtrnCode > 0) dbHandle->err(rtrnCode, "BDBFile::dumpDB");
    dbcp->close();
    cout << "\n\n\n===== total of " << count << " record(s) =====\n" << endl;
  }
  catch (DbException &dbe) {
    cout << "BDBFile::dumpDB " << dbe.what() << "\n";
  }
}

/*
//index related
int BDBFile::createIndex(string indexName_, BDBFile* indexFileHandle_, 
                unsigned int altKeyStartPos_, unsigned int altKeySize_)
{
  assert((altKeySize_ * altKeyStartPos_) > 0);
  altKeyStartPos = altKeyStartPos_;
  altKeySize = altKeySize_;
  return dbHandle->associate(NULL, indexFileHandle_->dbHandle, updateIndexCallBack, 0);
}
*/
