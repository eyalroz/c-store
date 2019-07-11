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
#ifndef __ROSAM_H
#define __ROSAM_H

#include <db_cxx.h>
#include <string>
#include <vector>
#include <map>
#include "AccessException.h"
#include "../common/Constants.h"
#include "../common/Util.h"
#include "AM.h"
#include "PageWriter.h"

/**
 * Access BDB columns
 * Basically IntegerColumn with some modifications
 */
using namespace std;
class ROSAM : public AM
{
  protected:
    Db *_db, *_secondary_db;
    Dbc *_iterator, *_secondary_iterator;
    string _tableName;

    vector<Db*> _db_arr;
    vector<Dbc*> _iter_arr;
    vector<Dbc*> _cursor_arr;
    vector<int> values;

    map<int, Db*> t2dbs;
    map<int, Dbc*> t2cursors;

    map<int, bool> aligned;
    int _offset;
    int _numindicies;
    int _numCursors;
    int currcursor;
    
    string table, _colName;
    bool isType2;

    char* _internalBuffer;
    char* _bulkBuffer;
    unsigned int _bulkBuffer_size;
    Dbt bulk_key, bulk_data;
    DbMultipleKeyDataIterator* bulk_iter;
    bool bulk_empty;
    bool use_bulk_read;

    int primkeysize;
    int seckeysize;
    int primkeytype;
    int seckeytype;

  // Methods

    char* acquireInternalBuffer();
  public:
    

    /**
     * Constructs an column of integers from a given table name
     */
    ROSAM(string table, int numindicies, int primkeysize_=sizeof(int), int seckeysize_=sizeof(int), short primkeytype_=ValPos::INTTYPE, short seckeytype_=ValPos::INTTYPE );

    /**
     * Destructs this IntegerColumn.
     */
    virtual ~ROSAM();

    void loadTable( string table );
    const void* getNextPage();
    const void* getNextPage( char *buf );
    const void* getNextPage( char* key, Dbc *iter,u_int32_t flags, bool primary, char* buffer );  
    const void* getNextPageSingle( char* key, Dbc *iter,u_int32_t flags, bool primary, char* buffer );  
    const void* getNextPageMulti( char* key, Dbc *iter,u_int32_t flags, bool primary, char* buffer );
    static int getSecondaryKey( Db *dbp,const Dbt *pkey,const Dbt *pdata, Dbt *skey );
    static int compare_int(Db *dbp, const Dbt *a, const Dbt *b);
    static int compare_key(Db *dbp, const Dbt *a, const Dbt *b);
    static int compare_double(Db *dbp, const Dbt *a, const Dbt *b);

    // Initializes the cursors (primary/secondary) for this table.
    virtual void initCursors();
    void initPrimaryCursor();
    void initSecondaryCursor();
	
    // Gets the next page along particular index
    virtual const void* getNextPagePrimary();
    virtual const void* getNextPageSecondary();
    virtual const void* skipToPagePrimary(char* key);
    virtual const void* skipToPageSecondary(char* key);
	
    virtual const void* getNextPagePrimaryDup();
    virtual const void* getNextPagePrimaryDup(int);
    virtual const void* getCursorIndex(char*, int&);
    virtual const void* resetCursorIndex(char*, int);
    virtual void setNumCursors(int);
    
    
    virtual char* getLastIndexValuePrimary();
    virtual char* getLastIndexValueSecondary();
    
    vector<int>* getSecondaryIndexValues(){  return &values; }
    virtual string toString();
    virtual string getTableName();
    virtual string getColumnName(){ return _colName; }
    virtual int getNumIndices() { return _numindicies; }

    static const unsigned int BULK_BUFFER_SIZE = 4 * 1024 * 1024;

};

#endif // __ROSAM_H
