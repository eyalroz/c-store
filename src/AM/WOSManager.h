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
#ifndef __WOSManager_H
#define __WOSManager_H

class DVAM;

#include <db_cxx.h>
#include <string>
#include <vector>
#include <map>
#include <ctime>
#include "AccessException.h"
#include "../common/UnimplementedException.h"
#include "../common/Constants.h"
#include "../common/Util.h"
#include "WOSAM.h"
#include "DVAM.h"
//#include "WOSManager.h"
#include "ROSWOSSegment.h"
#include "CatalogInstance.h"
//#include "../Wrappers/Encoder/IntEncoder.h"
//#include "../Wrappers/Decoder/IntDecoder.h"
//#include "../Operators/IntConverter.h"

class ROSWOSSegment;
class WOSAM;

/**
 * This is a class that will manage a WOS of a single projection 
 * It will issue instances of WOSAM and DVAM for a given projection
 * while keeping the shared resources (such as index)
 */
using namespace std;
class WOSManager   // : public AM
{
  protected:

  // DV stuff. cursors should be in DVAM!
    Db *_db_ros, *_db_wos;
    Dbc *_dv_ros_cursor, *_dv_wos_cursor;
    Dbt _bulk_data;
  
    string _segName, _segNameDebug;
    void *_bulk_buffer, *_p_bulk;

    int _numindicies;    
    int _numColumns;
    int _fieldnum;
    int _curPos;
    bool init;
    //Encoder* encoder;
    //Decoder* decoder;
    //IntConverter* ic;
    string table, _fName, _projName;

    int lst_fst_key;
    bool rnd1;

    // WOS stuff (tuples, counters);
    Db *_db_storage_key, *_db_sort_key;
    vector<char*>* wos_data;
    vector< vector<char*> > wos_col_data;

    int wos_size, curr_wos_page, wos_tuple_count;

    void createWOS();

  // Methods
  public:
    
    ROSWOSSegment *rwseg;
    /**
     * Constructs an column of integers from a given table name
     */
    WOSManager(string table, int numindicies, int numColumns/*, Encoder* src_, Decoder* decoder_  */);

    // this is a special, slightly hacky constructor that will
    // load an existing ROSWOSSegment. GONE
    // WOSManager( string table, int numColumns, int fieldnum );
      //WOSManager(string table, int numindicies, int numColumns/*, Encoder* src_, Decoder* decoder_  */);
    /**
     * Destructs this IntegerColumn.
     */
    virtual ~WOSManager();

    // Initializes the cursors (primary/secondary) for this table.
    // virtual void initCursors();
	
    // Gets the next page along particular index
    //virtual const void* getNextPagePrimary();
    //virtual const void* getNextPageSecondary();
    //virtual const void* skipToPagePrimary(int key);
    //virtual const void* skipToPageSecondary(int key);
	
    
    //virtual int getLastIndexValuePrimary();
    //virtual int getLastIndexValueSecondary();
    
    virtual string toString();
    //virtual string getTableName();  // I don't think this is meaningful here
    //virtual int getNumIndices() { return _numindicies; }
    int getNumColumns() { return _numColumns; }
    //int getFieldNum() { return _fieldnum; }

    void populateWOS();
    void writeTuplesToDisk();

    WOSAM* getWOSAM( int colNum );
    DVAM*  getDVAM ( bool ros );
    ROSWOSSegment *getROSWOSSegment();
    //int getLastStorageKey();

    string getFileName(){ return _fName; };

    int* getNextPosition( bool ros );

    void deleteTuple( int pos, bool ros );

    void appendTuple( char *tuple );

    int getLastStorageKey( bool ros );
    
    int getLastPosition( bool ros );
    
    int getLastValue( bool ros );

    void initStorageKeyCursor( Dbc *stkc )
      { if ( _db_storage_key )_db_storage_key->cursor(NULL, &stkc, 0);
      else cout << " DB_storage_key is a NULL. Please fix " << endl; }
    void initSortKeyCursor( Dbc *sokc )
      { if ( _db_sort_key )_db_sort_key->cursor(NULL, &sokc, 0);
      else cout << "DB_sort_key is NULL. PLease fix" << endl;}
    
  private:
    //helper 
    void Tokenize(const string& str, vector<string>& tokens, const string& delimiters);
    
    //Encoder* getEncoder() { return encoder; }
    //Decoder* getDecoder() { return decoder; }
};

#endif // __WOSManager_H
