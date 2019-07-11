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
 * @(#)IntegerColumn.h
 *
 */

#ifndef __IntegerColumn_H
#define __IntegerColumn_H

#include <db_cxx.h>
#include <string>
#include <vector>
#include <map>

#include "common.h"

/**
 * Comments go here
 */
using namespace std;

class IntegerColumn
{
  protected:
    Db *_db, *_secondary_db;
    Dbc *_iterator, *_secondary_iterator;
    //int m_size;
    //int* m_integers;
    string _tableName;

    vector<Db*> _db_arr;
    vector<Dbc*> _iter_arr;
    vector<int> values;

    map<int, Db*> t2dbs;
    map<int, Dbc*> t2cursors;

    map<int, bool> aligned;
    int _offset;

  // Methods
  public:
    int m_type;

    /**
     * Constructs an column of integers from a given table name
     */
    IntegerColumn( string table, int type );

    /**
     * Constructs a column object with no data
     * (need to load the file or the table)
     */
    //IntegerColumn( Db* db );

    /**
     * Destructs this IntegerColumn.
     */
    ~IntegerColumn();

    /**
     * Loads the column 
     * specfied integer.
     */
    void loadTable( string table, int type );

    /**
     * Initializes the cursors (primary/secondary) for this table.
     *
     */
    void initCursors();

    void setType2Cursors( int offset );

    /**
     * Returns the next page blob of the preset size.
     *
     */
    // I don't think primary is needed anymore.
    const void *getNextPage( int key, Dbc *iter,u_int32_t flags, bool primary,
			     char* buffer );

    /**
     * Returns the next page blob of the preset size.
     *
     */
    /*const void *getNextPage( int key, Dbc *iter, Dbc *s_iter, 
      u_int32_t flags, bool primary);*/

    /**
     * Returns the next page blob of the preset size.
     *
     */
    const void *getNextPage();

    /**
     * Returns the next page vertically for a given value
     * Assumes type2!
     *
     */
    const void *getNextPageVertical( int val, char* buf );

    /**
     * Returns the next page, using supplied, pre-allocated buffer.
     *
     */
    const void *getNextPage( char *buf );

    const void *getNextPageSecondary();

    const void* getNextPageSecondary( char* buf);


    /**
     * Returns the next page blob of the preset size.
     *
     */
    const void *getNextPageRange( int key );

    /**
     * Returns the next page blob of the preset size.
     *
     */
    const void *getNextPageSecondaryRange( int key );


    /**
     * New version of next page by key request. Takes a key
     * and the type.
     */
    const void *getNextPageRange( int key, int type );

    /**
     * Returns the next page blob of the preset size.
     *
     */
    const void *getNextPageSecondaryRange( int key, int type );

    /**
     * New version of getNextPage by iteration 
     */
    //const void *getNextPage( int index );

    // return the value of the very last index in the column
    int getLastIndexValue();

    static int compare_int(Db *dbp, const Dbt *a, const Dbt *b);
 
    // to be renamed!
    static int getSecondaryKey( Db *dbp,const Dbt *pkey,const Dbt *pdata, Dbt *skey );
    
    vector<int>* getType2Values(){  return &values; }

    /**
     * Returns the string representation of this IntegerColumn.
     * @return the string representation of this IntegerColumn.
     */
    string toString();
};

#endif // __IntegerColumn_H
