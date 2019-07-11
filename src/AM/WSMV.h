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
 * @(#)WSMV.h
 *
 */

#ifndef __WSMV_H
#define __WSMV_H

#include "../common/Constants.h"
//#include <db_cxx.h>
#include <string>
#include "PageWriter.h"
//#include "PageSorter.h"
#include "ROSAM.h"
//#include "Utility.h"

/**
 * Comments go here
 */
using namespace std;

struct BDBPage 
{
    char        enc_type;       // 1-4 (for Type1-4) for now
    char        val_type;       // 0 = integer, 1 = string
    short       val_size;       // 4 for integer, str length for string

    int         page_length;    // number of bits used on the page
    unsigned char        data[ PAGE_SIZE - 8 ];    // actual data contents

    BDBPage( char e_t, char val_t, short v_s, int p_l ) //, char *d )
    {
        enc_type = e_t;
        val_type = val_t;
        val_size = v_s;

        page_length = p_l;
    }
  
    ~BDBPage(){ };//free( data );};
};

class WSMV
{
 private:
  
  Db *_db_storage_key, *_db_sort_key;
  Dbc *_storage_key_cursor, *_sort_key_cursor;
  //Dbc *_cursor;

  //int _counter;

  bool returned_curr;
  BDBPage *_sk_page;
  
  // Methods
  public:

    /**
     * Constructs a Write Store Materalized View
     */
    WSMV( string name, int sid);

    /**
     * Destructs this WSMV.
     */
    ~WSMV();

    void addValueSKPair( int val, int sk );

    BDBPage *getNextSKPage();


    /**
     * Place a given (key, data) pair into a database
     *
     */
    void placeRecord( char *key, char *data );
    
    /**
     * Place a given (key, data) pair into a database
     *
     */
    void addSortKeySKPair( char *sort_key, int size1, 
			   char *store_key, int size2 );
    

    int getStorageKey( char *sort_key, int size );

    void mergeAndPrint( Dbt ddata, string s2, ofstream *out_f );

    /**
     * Place a given (key, data) pair into a database
     *
     */
    int getNextIntSorted();
    
    char* getNextRecordSorted();
  
    
    static void test();
    
    /**
     * Returns the string representation of this WSMV.
     * @return the string representation of this WSMV.
     */
    string toString();
};

#endif // __WSMV_H
