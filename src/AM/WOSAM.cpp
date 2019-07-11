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
//#include <iostream>
#include <fstream>
#include <sstream>
#include <strstream>
#include <iterator>
#include <algorithm>
#include <db_cxx.h>
#include <vector>

#include "WOSAM.h"
#include "CatalogInstance.h"

WOSAM::WOSAM( string table_, int numindicies, int numColumns, int fieldnum/*, Encoder* src_, Decoder* decoder_ */ )
{
  //cout << "Constructing WOSAM, table " << table << " num indicies " << numindicies << " num cols " << numColumns << " field " << fieldnum << endl;
  // cout << "WOSAM: this constructor is depricated and will result in memory leaks. Please use Catalog to get WOSAM" << endl;
  _numindicies = 2;
  _numColumns = numColumns;
  _fieldnum = fieldnum;
  init=true;

  /*string s = table_;
  string::size_type loc;
  loc = s.rfind("/", string::npos);
  if(loc != string::npos) {
    table=s.substr(loc+1); 
  } else {
    table=table_;    
    }*/
  table = CatalogInstance::stripOffDirectory( table_ );

  table = table_;

  _p_bulk = NULL;
  _pp_bulk = &_p_bulk;
  _bulk_buffer = malloc( BUFFER_LENGTH );
  _p_bulk_buffer = &_bulk_buffer;

  rwseg = new ROSWOSSegment( table /*wos_name*/, numColumns);
  /*
	char *tuple  = new char[ 6 * sizeof(int) ];  // 5 columns + timestamp
	for (int i = 0; i < 20; i ++ )
	  {
	    int x = i;
	    memcpy( tuple, &(x), sizeof(int));  // storage key, first 4 bytes
	    
	    int ii = ((i+1) % 5);  // to make sort key different and repeating.
	    memcpy( tuple + sizeof(int), &ii, sizeof(int) ); // sort key, col 1.
	    
	    cout << " Append storage key " << i << " SortKey " << ii << endl;
	    rwseg->appendTuple( tuple );
	    }                                                                          
  */
  
  /* for now WOSAManager has its own ROSWOSSegment so this exception will hold true once WOSAM is convertructed
  // WOSAM no longer populates the data...
  if ( rwseg->isEmpty() )
    throw ( AccessException( "WOSAM should not be responsible for populating the WOS. Most likely you forgot to init a WOSAManager or to build the WOS tables" ) );
  */
  
  rwseg->initCursors( &_storage_key_cursor, &_sort_key_cursor );

	ic = new IntConverter(rwseg, 0, _sort_key_cursor, _pp_bulk, _p_bulk_buffer  );
	encoder = new IntEncoder(ic, // data source
				 0,              // column index
				 //8*PAGE_SIZE);   // buffer size in bits
				 96);   // buffer size in bits
	decoder = new IntDecoder(false);         // value sorted
	//encoder=src_;
	//decoder=decoder_;

	initCursors();
}

WOSAM::WOSAM( string table_, int numindicies, int numColumns, int fieldnum, ROSWOSSegment* rws )
{
  // New WOSAM -- shared resources.  Does indexing work right?
  // Single ROSWOSSegment might not have enough indices
  //cout << "Constructing WOSAM for " << CatalogInstance::stripOffDirectory( table_ ) << " actual table " << table_ << " with shared resources and fieldnum " << fieldnum << endl;

  _numindicies = 2;
  _numColumns = numColumns;
  _fieldnum = fieldnum;
  init=true;

  /*string s = table_;
  string::size_type loc;
  loc = s.rfind("/", string::npos);
  if(loc != string::npos) {
    table=s.substr(loc+1); 
  } else {
    table=table_;    
    }*/

  _bulk_buffer = malloc( BUFFER_LENGTH );
  _p_bulk_buffer = &_bulk_buffer;
  _p_bulk = NULL;
  _pp_bulk = &_p_bulk;

  table =  CatalogInstance::stripOffDirectory( table );
  //cout << " COMPARE " << table << " VS " << CatalogInstance::stripOffDirectory( table ) << " VS " << table_ << endl;


  table = table_;
  rwseg = rws;//new ROSWOSSegment( table /*wos_name*/, numColumns);
  //rwseg->reInitCursors();
  rwseg->initCursors( &_storage_key_cursor, &_sort_key_cursor );
  /*
	char *tuple  = new char[ 6 * sizeof(int) ];  // 5 columns + timestamp
	for (int i = 0; i < 20; i ++ )
	  {
	    int x = i;
	    memcpy( tuple, &(x), sizeof(int));  // storage key, first 4 bytes
	    
	    int ii = ((i+1) % 5);  // to make sort key different and repeating.
	    memcpy( tuple + sizeof(int), &ii, sizeof(int) ); // sort key, col 1.
	    
	    cout << " Append storage key " << i << " SortKey " << ii << endl;
	    rwseg->appendTuple( tuple );
	    }                                                                          
  */
  
  /* for now WOSAManager has its own ROSWOSSegment so this exception will hold true once WOSAM is convertructed
  // WOSAM no longer populates the data...
  if ( rwseg->isEmpty() )
    throw ( AccessException( "WOSAM should not be responsible for populating the WOS. Most likely you forgot to init a WOSAManager or to build the WOS tables" ) );
  */
    rwseg->initCursors( &_storage_key_cursor, &_sort_key_cursor );

	ic = new IntConverter(rwseg, 0, _sort_key_cursor, _pp_bulk, _p_bulk_buffer  );
	encoder = new IntEncoder(ic, // data source
				 0,              // column index
				 //8*PAGE_SIZE);   // buffer size in bits
				 96);   // buffer size in bits
	decoder = new IntDecoder(false);         // value sorted
	//encoder=src_;
	//decoder=decoder_;

	initCursors();
}

void WOSAM::populateWOS()
{
  cout << " WOSAM: populating from TABLE " << table << endl;

  //loading tuple from table, assume the following
  //-table is a text file, space delimited between columns
  // each row in the table is less than 1024 bytes
  //-all integer column
  //-first column of the table is the sort key
  //-first 4 bytes is the storage key, then each
  // subsequent 4 bytes will be loaded from each column

  int sz = (_numColumns+1)*sizeof(int);
  char* tuple  = new char[sz];
  fstream inFile;
  inFile.open(table.c_str(), ios::in);
  int storageKey=1;
  if (inFile.is_open()) {
    char str[1024];
    int iCol, iVal;
    while(!inFile.eof())
    {
      memcpy(tuple, &(storageKey), sizeof(int));  // storage key, first 4 bytes
      inFile.getline(str, 1024);
      string s(str);
      if (!s.empty()) {
        // Skip delimiters at beginning.
        string::size_type lastPos = s.find_first_not_of(" ", 0);
        // Find first "non-delimiter".
        string::size_type pos = s.find_first_of(" ", lastPos);
        //cout << " Append storage key " << storageKey << " SortKey ";
        iCol = 0;
        while (string::npos != pos || string::npos != lastPos)
        {
          // Found a token, convert to integer
          iVal = atoi((s.substr(lastPos, pos - lastPos)).c_str());
          memcpy( tuple + (iCol+1)*sizeof(int), &(iVal), sizeof(int) );          
          //cout << iVal << " ";
          // Skip delimiters.  Note the "not_of"
          lastPos = s.find_first_not_of(" ", pos);
          // Find next "non-delimiter"
          pos = s.find_first_of(" ", lastPos);
          iCol++;
        }
        //cout << endl;
        if (iCol > 0) {
          rwseg->appendTuple( tuple );
          ++storageKey;
        }
      }
    }
    inFile.close();
  }
  delete[] tuple;	  

  cout << " WOSAM finished populating " << table << endl;
}

// load data from existing segment.
WOSAM::WOSAM( string table_, int numColumns, int fieldnum )
{
  cout << "WOSAM: this constructor is depricated and will result in memory leaks. Please use Catalog to get WOSAM" << endl;
	_numindicies = 2;
	_numColumns = numColumns;
	_fieldnum = fieldnum;
  string s = table_;
  string::size_type loc;
  loc = s.rfind("/", string::npos);
  if(loc != string::npos) {
    table=s.substr(loc+1); 
  } else {
    table=table_;    
  }

  table = table_;
  rwseg = new ROSWOSSegment( s, numColumns);

  rwseg->initCursors( &_storage_key_cursor, &_sort_key_cursor );

	ic = new IntConverter(rwseg, 0, _sort_key_cursor, _pp_bulk, _p_bulk_buffer  );
	encoder = new IntEncoder(ic, // data source
				 0,              // column index
				 //8*PAGE_SIZE);   // buffer size in bits
				 96);   // buffer size in bits
	decoder = new IntDecoder(false);         // value sorted

	initCursors();
}


WOSAM::~WOSAM()
{
  delete ic;
  delete encoder;
  delete decoder;

  _storage_key_cursor->close();
  _sort_key_cursor->close();

  delete (( char * )(*_p_bulk_buffer));  // it is a double pointer. Not a bug.
}

void WOSAM::initCursors( )
{
  //cout << " WOSAM: please not that initCursors does not seem to do anything " << endl;
  // Calling reinit cursors here causes a failure -- query gets stuck.
  // why, I am not sure, this should have been safe...
  //rwseg->reInitCursors();
}

const void* WOSAM::skipToPagePrimary( char* key )
{
  //for now WOSAM only works on integers ... fix later ...
  int newkey = *(int*)key;
  ic->setKey(newkey, _fieldnum);
  init = false;
  return encoder->getPage();
}

const void* WOSAM::skipToPageSecondary( char* key )
{
  //for now WOSAM only works on integers ... fix later ...
  int newkey = *(int*)key;
  ((IntEncoder*)encoder)->resetPair();
  ic->setPos(newkey, _fieldnum);
  _curPos=newkey+1;
  return encoder->getPage();
}

const void* WOSAM::getNextPageSecondary( )
{
  //ic->setPos(_curPos, _fieldnum);
  //_curPos++;
  return encoder->getPage();
}

const void* WOSAM::getNextPagePrimary()
{
  if (init) {
    ic->setKey(0, _fieldnum);
    init = false;
  }
  return encoder->getPage();
}

char* WOSAM::getLastIndexValuePrimary() {
  throw UnimplementedException("Still need to code");
}

char* WOSAM::getLastIndexValueSecondary() {
  throw UnimplementedException("Still need to code");
}


string WOSAM::toString()
{
	return "WOS Integer Column string representation";
}

string WOSAM::getTableName() 
{
	return table;	
}

void WOSAM::writeTuplesToDisk()
{
  rwseg->writeTuplesToDisk();
}


void WOSAM::Tokenize(const string& str,
              vector<string>& tokens,
              const string& delimiters = " ")
{
    // Skip delimiters at beginning.
  string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
  string::size_type pos     = str.find_first_of(delimiters, lastPos);

  while (string::npos != pos || string::npos != lastPos)
  {
        // Found a token, add it to the vector.
    tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
    lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
    pos = str.find_first_of(delimiters, lastPos);
  }
}

int WOSAM::getLastStorageKey()
{  
  int storageKey = rwseg->getLastStorageKey(); 
  /*
  int lastPos = rwseg->getWOSTupleCount();
  int skVal = 0, storageKey = 1;
  int sz = (_numColumns+1)*sizeof(int);
  char* lastTuple = new char[sz];
  lastTuple = rwseg->getTupleAtPosition(lastPos);
  memcpy(lastTuple, &(skVal), sizeof(int));                           // storage key, first 4 bytes
  delete lastTuple;
  if (skVal > 0) storageKey = skVal + 1;
  */
     
  /*
  time_t t = time(NULL);
  storageKey = (int) t;
  */
  
  return storageKey;
}

string WOSAM::getColumnName()
{
  return (CatalogInstance::stripOffDirectory(table)); 
}
