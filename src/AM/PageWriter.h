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
 * @(#)PageWriter.h
 *
 */

#ifndef __PageWriter_H
#define __PageWriter_H

#include <db_cxx.h>
#include <string>
#include <vector>
#include <map>
#include "ROSAM.h"

/**
 * Comments go here
 */
using namespace std;

class PageWriter
{
 private:

    vector<Db*> _db_arr;

    string _tableName;
    map<int, Db*> _db_local;

    int _numindicies;
    int primkeysize;
    int seckeysize;
    int primkeytype;
    int seckeytype;

  // Methods
  public:

    /**
     * Constructs a page encoder
     */
    PageWriter( int numindicies, int primkeysize_, int seckeysize_,short primkeytype_, short seckeytype_);

    /**
     * Destructs this PageEncoder.
     */
    ~PageWriter();

    static void placeRecord( Db *db, char *key, int key_size,
				char *data, int data_size );

    //static void* PageWriter::getRecord( int key, Dbc *cursor, u_int32_t flags, 
    //bool primary, char *buffer );

    void initDB(string c_name, bool dupsort);

    void closeDB();

    void placePage(char* page, char* primkey, char* seckey);
    void placePage(char* page, char* primkey);
    static int dup_compare(Db *dbp, const Dbt *a, const Dbt *b);
    //static int dup_compare2(Db *dbp, const Dbt *a, const Dbt *b);
};

#endif // __PageWriter_H
