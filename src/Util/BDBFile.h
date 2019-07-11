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
 * BDBFile.h
 * 
 *  provide read, write, delete, update function for a Berkeley DB file
 *  (Note: a internal cursor will be created and opened upon in constructor, a readRec or readNextRec with key call
 *  will point the cursor to the record being read. readNextRec() will read the next record of the current cursor.)
 *  
 */

#ifndef __BDBFile_H
#define __BDBFile_H

#include <db_cxx.h>
#include <string>
#include <vector>
#include <map>
#include "../common/UnexpectedException.h"

//typedef int (*BDB2ndDBKeyCallBack) (Db*, const Dbt*, const Dbt*, Dbt*);

/**
 * Utility class for Berkeley DB
 */
using namespace std;

class BDBFile
{
  public:
    BDBFile(string bdbFileName_);
    BDBFile(string bdbFileName_, bool isPrimary_);
    ~BDBFile();
        
    int open();
    int close();
    
    //Flush a database to stable storage
    int flush();
    
    //record operation, assume no duplicate key 
    void* readRec(void* key_, unsigned int keySize_);
    void* readNextRec();    //read next the record via dbCursor                     
    void* readNextRec(void* key_, unsigned int keySize_);
    void* readFirstRec(void* key, unsigned int keySize_);
    void* readLastRec(void* key, unsigned int keySize_);
    int writeRec(void* key_, unsigned int keySize_, void* data_, unsigned int dataSize_, bool allowOverwrite_);
    int deleteRec(void* key_, unsigned int keySize_);
    int deleteRec();  //delete the record being pointed by dbCursor
    int updateRec(void* key_, unsigned int keySize_, void* newData_, unsigned int newDataSize_);
    int updateRec(void* newData_, unsigned int newDataSize_); //delete the record being pointed by dbCursor
                
    u_long getRecordCount();
    void showStats();
    void dumpDB();
      
    //secondary index
    //int createIndex(string indexName_, BDBFile* indexFileHandle_, 
    //                unsigned int altKeyStartPos_, unsigned int altKeySize_);
    
    Db* dbHandle;
  protected:
    
    void init(string bdbFileName_, bool isPrimary_);
    
    //cursor operation
    void openCursor();
    void closeCursor();
    int cursorRead(Dbt& key, Dbt& data, u_int32_t readFlag);
    
    //index related
    //nupdateIndexCallBack(Db* primaryDb_, const Dbt* pKey, const Dbt* pData, Dbt* sKey);
    unsigned int altKeyStartPos;
    unsigned int altKeySize;
        
  private:
    
    u_int32_t dbFlags;
    int errNum, searchKeySize;
    string bdbFileName;
    Dbc* dbCursor;          //use as a record pointer
    Dbt searchKey;
    bool isPrimary;
    
    //index related
    BDBFile* indexFileHandle;
    string indexName;
};

/*
int updateIndexCallBack(Db* primaryDb_, const Dbt* pKey, const Dbt* pData, Dbt* sKey)
{
  memset(sKey, 0, sizeof(Dbt));
  char* src = (char*)pData->get_data();
  src += primaryDb_->altKeyStartPos;
  sKey->set_data((void *)src);
  sKey->set_size(primaryDb_->altKeySize);
  return (0);
}
*/


#endif // __BDBFile_H
