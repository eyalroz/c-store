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
#ifndef __WOSAM_H
#define __WOSAM_H

#include <db_cxx.h>
#include <string>
#include <vector>
#include <map>
#include <ctime>
#include "AccessException.h"
#include "../common/Constants.h"
#include "../common/Util.h"
#include "AM.h"
//#include "WOSManager.h"
//#include "ROSWOSSegment.h"
//#include "../Wrappers/Encoder/IntEncoder.h"
//#include "../Wrappers/Decoder/IntDecoder.h"
#include "../Operators/IntConverter.h"

class ROSWOSSegment;

class Encoder;
class Decoder;

/**
 * Access BDB columns
 * 
 */
using namespace std;
class WOSAM : public AM
{
  protected:
    Dbc *_storage_key_cursor, *_sort_key_cursor;
    int _numindicies;    
    int _numColumns;
    int _fieldnum;
    int _curPos;
    bool init;
    Encoder* encoder;
    Decoder* decoder;
    IntConverter* ic;
    string table;

    void *_bulk_buffer, **_p_bulk_buffer, **_pp_bulk, *_p_bulk;

  // Methods
  public:
    
    ROSWOSSegment *rwseg;

    /**
     * Constructs an column of integers from a given table name
     * depricated. Now share roswos segment among multiple wosams.
     */
    WOSAM(string table, int numindicies, int numColumns, int fieldnum/*, Encoder* src_, Decoder* decoder_  */);

    // this is a special, slightly hacky constructor that will
    // load an existing ROSWOSSegment.
    WOSAM( string table, int numColumns, int fieldnum );
      //WOSAM(string table, int numindicies, int numColumns/*, Encoder* src_, Decoder* decoder_  */);

    WOSAM(string table, int numindicies, int numColumns, int fieldnum, ROSWOSSegment* rws );

    /**
     * Destructs this IntegerColumn.
     */
    virtual ~WOSAM();

    // Initializes the cursors (primary/secondary) for this table.
    virtual void initCursors();
	
    // Gets the next page along particular index
    virtual const void* getNextPagePrimary();
    virtual const void* getNextPageSecondary();
    virtual const void* skipToPagePrimary(char* key);
    virtual const void* skipToPageSecondary(char* key);
	
    
    virtual char* getLastIndexValuePrimary();
    virtual char* getLastIndexValueSecondary();
    
    virtual string toString();
    string getTableName();
    virtual string getColumnName();
      //cerr << "Why is this called??? " << endl; return NULL;};
    virtual int getNumIndices() { return _numindicies; }
    int getNumColumns() { return _numColumns; }
    int getFieldNum() { return _fieldnum; }

    void populateWOS();
    void writeTuplesToDisk();

    int getLastStorageKey();
    
  private:
    //helper 
    void Tokenize(const string& str, vector<string>& tokens, const string& delimiters);
    
    //Encoder* getEncoder() { return encoder; }
    //Decoder* getDecoder() { return decoder; }
};

#endif // __WOSAM_H
