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
#ifndef __DVAM_H
#define __DVAM_H

class WOSManager;

#include <db_cxx.h>
#include <string>
#include <vector>
#include <map>
#include <ctime>
#include "AccessException.h"
#include "../common/Constants.h"
#include "../common/Util.h"
#include "AM.h"
//#include "ROSWOSSegment.h"
#include "WOSManager.h"
//#include "../Wrappers/Encoder/DeltaPosEncoder.h"
//#include "../Wrappers/Decoder/DeltaPosDecoder.h"
#include "../Wrappers/Encoder/IntEncoder.h"
#include "../Wrappers/Decoder/IntDecoder.h"
#include "../Operators/DVConverter.h"

class DVConverter;  // WHY do I have to do this??

/**
 * Access BDB columns
 * Basically IntegerColumn with some modifications
 */
using namespace std;
class DVAM : public AM
{
  protected:
  
    WOSManager *_wosMgr;
    int _numindicies;    
  //int _numColumns;
  //int _fieldnum;
  //int _curPos;
    bool _ros;
    bool init;
    IntEncoder* dpencoder;
    IntDecoder* dpdecoder;
    //IntConverter* ic;
    DVConverter* dvc;
    string table;


  // Methods
  public:
    
    //ROSWOSSegment *rwseg;

    /**
     * Constructs an column of integers from a given table name
     */
    DVAM( string table, bool ros );
      //, int numindicies, int numColumns, int fieldnum/*, Encoder* src_, Decoder* decoder_  */);

    /**
     * Destructs this DeleteVector
     */
    virtual ~DVAM();

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
    virtual string getColumnName(){ cerr << "Why is this called??? " << endl; return NULL;};
    virtual int getNumIndices() { return _numindicies; }
    //int getNumColumns() { return _numColumns; }
    //int getFieldNum() { return _fieldnum; }

    void deleteTuple( int pos );
    //void writeTuplesToDisk();
    bool isROSDV();
    void setWOSManager( WOSManager *wm );

    int getLastStorageKey();
    
  private:
    //helper 
    //void Tokenize(const string& str, vector<string>& tokens, const string& delimiters);
    
    //Encoder* getEncoder() { return encoder; }
    //Decoder* getDecoder() { return decoder; }
    int retval;
};

#endif // __DVAM_H
