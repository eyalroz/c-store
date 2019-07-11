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
#ifndef __NOBDBAM_H
#define __NOBDBAM_H

#include <string>
#include <vector>
#include <map>
#include "AccessException.h"
#include "../common/Constants.h"
#include "../common/Util.h"
#include "AM.h"

/**
 * Access BDB columns
 * Basically IntegerColumn with some modifications
 */
using namespace std;
class NOBDBAM : public AM
{
  protected:
    byte *_iterator, *_secondary_iterator;
    string _tableName;

    vector<byte*> _iter_arr;
    vector<int> values;

    int _offset;
    int _numindicies;
    
    string table, _colName;

    char* _internalBuffer;

  // Methods

    char* acquireInternalBuffer();
    
    ifstream Afile;


  public:
    

    /**
     * Constructs an column of integers from a given table name
     */
    NOBDBAM(string table, int numindicies );

    /**
     * Destructs this IntegerColumn.
     */
    virtual ~NOBDBAM();

    void loadTable( string table );
    const void* getNextPage();
    const void* getNextPage( char *buf );
    //const void* getNextPage( int key, Dbc *iter,u_int32_t flags, bool primary, char* buffer );	
    //static int getSecondaryKey( Db *dbp,const Dbt *pkey,const Dbt *pdata, Dbt *skey );
    //static int compare_int(Db *dbp, const Dbt *a, const Dbt *b);

    // Initializes the cursors (primary/secondary) for this table.
    virtual void initCursors();
	
    // Gets the next page along particular index
    virtual const void* getNextPagePrimary();
    virtual const void* getNextPageSecondary();
    virtual const void* skipToPagePrimary(char* key);
    virtual const void* skipToPageSecondary(char* key);
	
    
    virtual char* getLastIndexValuePrimary();
    virtual char* getLastIndexValueSecondary();
    
    vector<int>* getSecondaryIndexValues(){  return &values; }
    virtual string toString();
    virtual string getTableName();
    virtual string getColumnName(){ return _colName; }
    virtual int getNumIndices() { return _numindicies; }
};

#endif // __NOBDBAM_H
