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
#ifndef __ROSWOSSEGMENT_H
#define __ROSWOSSEGMENT_H

#include <db_cxx.h>
#include <string>
#include <vector>
#include <map>
#include "AccessException.h"
#include "../common/Constants.h"
#include "AM.h"
//#include "WOSManager.h"
#include "PageWriter.h"
#include "../Util/StopWatch.h"

//class WOSManager;

// WOS pages can have an arbitrary size, they have no connection
// to the database.
#define WOS_PAGE_SIZE  1024

#define BUFFER_LENGTH ( 5 * 1024 ) // 5KByte bulk get buffer

/**
 * Represents accesses methods for ROS and WOS segment
 * 
 */
using namespace std;

class ROSWOSSegment
{
 protected:
    bool _col;

    bool _debug;

    Db *_db_storage_key, *_db_sort_key;
  Dbt _bulk_data;

  //WOSManager *_wosManager;
  
  string _segName, _segNameDebug;
  void *_bulk_buffer, *_p_bulk;

  StopWatch *sw;

  static int counter, new_page_counter;
  // for now, store data row-based
    vector<char*>* wos_data;

    vector< vector<char*> > wos_col_data;

    int wos_size, curr_wos_page, wos_tuple_count;
    int num_fields, tuple_size;
    int read_ptr, accesses, fake_key;
    double total_time1, db_access, mem_cpy;

    char a;
    bool empty;
  // Methods
    char* getNextTuple();

    char* getTupleAtPosition( int where );

    char* getTupleAtPosition( int where, int field );

    char* getTupleByStorageKey( char* skey, Dbc *iter );

    char* getTupleBySortKey( char* skey, Dbc *iter );

  public:

    // Yes, these should not be here!  But I could not resolve the
    // dependency problem between ROSWOSSegmetna and WOSManager.
    // so they will be initialized in an open, public way.
    // -alexr
    Dbc *_storage_key_cursor, *_sort_key_cursor;
    /**
     * Constructs a segment for a supplied name and a set number
     * of fields. Note that we assume all fields to be integers for now.
     */
    ROSWOSSegment( string segName, int numfields );

    //ROSWOSSegment( string segName, int numfields, Db* db_store_key, Db* db_sort_key, vector<char*>* wd, int wtc );
    ROSWOSSegment( string segName, int numfields, void* wm );

    /**
     * Destructs this segment.
     */
    ~ROSWOSSegment();


    // NOTE: tuple MUST be of size (numfields+1)*sizeof(int), assuming
    // timestamp is int. For variable size fields, we store actual data
    // elsewhere, and pointer to it as an int.
    bool appendTuple( char *tuple );

    bool isEmpty();
    /* set the cursor to the n-th tuple in the WOS
     */
    // void resetCursor( int where );

    // resets the cursor to beginning of DB if true. Otherwise return next
    // char* getNextTupleBySortKey( bool reset );


    // return an integer offset of the tuple, a physical order one.
    int* lookupByIndex( char* key, int size, Dbc *iter, u_int32_t flags,
			void **_ptr, void **buf );


    // new set of interfaces, for current needs of operators/merge-out

    // returns a pointer to the field at offset "field", based on a provided
    // position
    char* getFieldAtPosition(int pos, unsigned short field);

    // returns a pointer to the field at offset "field", based on a provided
    // key. It should return a NULL once it reaches the last key.
    char* getNextFieldBySortKey( int key, unsigned short field, bool first,
				 Dbc *iter );

    // returns a pointer to the field at offset "field", based on a provided
    // key. It should return a NULL once it reaches the last key. Also puts 
    // the position value in the position pointer
    char* getNextFieldBySortKey( int key, unsigned short field, bool first, 
				 int* position, Dbc *iter, void **ptr,
				 void **buf );
    
    // returns a pointer to the field at offset "field", based on a provided
    // key. It should return a NULL once it reaches the last key.
    char* getNextFieldByStorageKey( int key, unsigned short field, bool first,
				    Dbc *iter );
    
    // returns a pointer to the field at offset "field", based on the
    // insert order of the data.  boolean parameter will reset to
    // beginning if true.
    char* getNextField( unsigned short field, bool first, Dbc *iter );

    void writeTuplesToDisk();

    //char* getNextBySortKey( int key, unsigned short field );
    
    int getWOSTupleCount();

    int getLastStorageKey();
    
    int getLastPosition();
    
    int getLastValue();
    
    bool isSortKeyExist(int sortKey_);

    void reInitCursors();

    void initCursors( Dbc **_storage_cursor, Dbc **_sort_cursor );

    string toString();
    
  private:
  
    void setWOSTupleCount();
};

#endif // __ROSWOSSEGMENT_H
