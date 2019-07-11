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

#include "WOSManager.h"

WOSManager::WOSManager( string table_, int numindicies, int numColumns /*, Encoder* src_, Decoder* decoder_ */ )
{
  //cout << " CONSTRUCT WOSMANAGER for " << table_ << endl;
  lst_fst_key = -1;
  rnd1=true;
  _numindicies = 2;
  _numColumns = numColumns;
  init=true;
  string s = table_;
  string::size_type loc;
  loc = s.rfind("/", string::npos);
  if(loc != string::npos) {
    table=s.substr(loc+1); 
  } else {
    table=table_;
  }

  s = table_;
  loc = s.rfind("/", string::npos);
  if(s.rfind("/", string::npos) != string::npos) {
    _fName=s.substr(loc+1); 
  } else {
    _fName= table_;    
  }

  if ( _fName.rfind( ".wos" ) != string::npos )
    _projName = _fName.substr( 0, _fName.size()-4 );
  else
    _projName = _fName;
  cout << "This wos manager has " << _projName << endl;

  _p_bulk = NULL;

  table = table_;
  //cout << "CREATING ROSWOSSEGMENT from " << table << endl;
  rwseg = NULL;
  /*
    // moved to a protected method, so that it can be delayed.
  rwseg = new ROSWOSSegment( table, numColumns);

  if ( rwseg->isEmpty() )
    this->populateWOS();
  */
  /*ic = new IntConverter(rwseg, 0);
	encoder = new IntEncoder(ic, // data source
				 0,              // column index
				 //8*PAGE_SIZE);   // buffer size in bits
				 96);   // buffer size in bits
	decoder = new IntDecoder(false);         // value sorted
  */
	//encoder=src_;
	//decoder=decoder_;

  _db_ros = new Db( NULL, 0 );
  _db_wos = new Db( NULL, 0 );

  _db_ros->set_bt_compare( ROSAM::compare_int );
  _db_wos->set_bt_compare( ROSAM::compare_int );
  
  _db_ros->set_error_stream( &cout );
  _db_wos->set_error_stream( &cout );


  _db_ros->set_cachesize( 0, SMALL_CACHE_SIZE/5, 1 ); // 5Mb cache
  _db_ros->set_pagesize( 4096 );

  _db_wos->set_cachesize( 0, SMALL_CACHE_SIZE/5, 1 ); // 5Mb cache
  _db_wos->set_pagesize( 4096 );
 
  string ros = (RUNTIME_DATA+_fName + ".dv.ros");
 
  system( ("rm -f " + ros).c_str() );
  if ( (_db_ros->open( NULL, (ros.c_str()),NULL,
			       DB_BTREE, DB_CREATE, 0664) != 0 ))
    cerr << "Failed to open table " << (ros) << endl;
  else
    _db_ros->cursor(NULL, &_dv_ros_cursor, 0);

  string wos = (RUNTIME_DATA+_fName + ".dv.wos");

  system( ("rm -f " + wos).c_str() );
  if ( (_db_wos->open( NULL, (wos.c_str()),NULL,
			       DB_BTREE, DB_CREATE, 0664) != 0 ))
    cerr << "Failed to open table " << (wos) << endl;
  else
    _db_wos->cursor(NULL, &_dv_wos_cursor, 0);

  memset(&_bulk_data, 0, sizeof(_bulk_data)); 

  _bulk_buffer = malloc( BUFFER_LENGTH );
  //initCursors();
}

/*
  This function call will create ROSWOSSegment for this manager
  I am delaying this creation to reduce startup loading time.
*/

void WOSManager::createWOS()
{
  rwseg = new ROSWOSSegment( table, _numColumns);

  if ( rwseg->isEmpty() )
    this->populateWOS();
}


void WOSManager::populateWOS()
{
  cout << " WOSManager: populating from TABLE " << table << endl;

  //loading tuple from table, assume the following
  //-table is a text file, space delimited between columns
  // each row in the table is less than 1024 bytes
  //-all integer column
  //-first column of the table is the sort key
  //-first 4 bytes is the storage key, then each
  // subsequent 4 bytes will be loaded from each column

  // This table does not run from RuntimeData directory
  // unlike all other file access!
  int sz = (_numColumns+1)*sizeof(int);
  char* tuple  = new char[sz];
  fstream inFile;
  inFile.open((table).c_str(), ios::in);
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

  cout << " WOSManager finished populating " << table << " last storageKey:" << storageKey << endl;
  //cout << "This can probably be avoided in some cases, but I will write tuples to disk now" << endl;
    writeTuplesToDisk();
}

// load data from existing segment.
/* I think this is fully depricated (?)

WOSManager::WOSManager( string table_, int numColumns, int fieldnum )
{
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

	ic = new IntConverter(rwseg, 0);
	encoder = new IntEncoder(ic, // data source
				 0,              // column index
				 //8*PAGE_SIZE);   // buffer size in bits
				 96);   // buffer size in bits
	decoder = new IntDecoder(false);         // value sorted

	initCursors();
}
*/

WOSManager::~WOSManager()
{
  // What goes here?
  //delete rwseg;
}

WOSAM* WOSManager::getWOSAM( int colNum )
{
  if ( rwseg == NULL )
    createWOS();
  //CatalogInstance::getCatalog()->getWOSAM();
  //cout << " Getting a WOSAM from WOSManager " << _fName << endl;
  WOSAM* wm = new WOSAM( table, _numindicies, _numColumns, colNum, rwseg );

  CatalogInstance::allocatedAMs->push_back( wm );
  // initStorageKeyCursor( wm->rwseg->_storage_key_cursor );
  //initSortKeyCursor( wm->rwseg->_sort_key_cursor );

  return wm;
}

DVAM* WOSManager::getDVAM( bool ros )
{
  if ( rwseg == NULL )
    createWOS();

  DVAM *da = new DVAM( table, ros );

  CatalogInstance::allocatedAMs->push_back( da );
  // this is a deliverate step of not including the WOSManager in constructor
  // we do not want DV objects to be issued outside of WOSManager, since
  // WOSManager will provide sharing of  BDB. 
  da->setWOSManager( this );

  return da;
}

string WOSManager::toString()
{
	return string("This is a WOSManager reporting ") + 
	  string( " name? " );
}

void WOSManager::deleteTuple( int pos, bool ros )
{
  // Note this is a special case -- the data field is zero
  // only need to store offsets.
  if ( ros )
    PageWriter::placeRecord( _db_ros, (char*)&pos, sizeof(int), 
			     (char*)&pos, sizeof(int));
  //NULL, 0 );
  else
    PageWriter::placeRecord( _db_wos, (char*)&pos, sizeof(int), 
			     (char*)&pos, sizeof(int));
  //NULL, 0 );
}

void WOSManager::writeTuplesToDisk()
{
  if ( rwseg )
    rwseg->writeTuplesToDisk();
  else
    cout << "RWSEG is a null. Why? " << endl;
}

void WOSManager::appendTuple( char *tuple )
{
  rwseg->appendTuple( tuple );
}

int WOSManager::getLastStorageKey( bool ros )
{
  if ( ros ) // that can only work for WOS. Not sure about ROS.
    {
      cerr << "Throw AccessException, NOT IMPLEMENTED " <<endl;
      throw UnimplementedException(" NOT IMPLEMENTED ");
    }

  if ( !rwseg )
    createWOS();

  return rwseg->getLastStorageKey();
}

ROSWOSSegment *WOSManager::getROSWOSSegment()
{
  if (!rwseg)
    createWOS();

  return rwseg;
}

int WOSManager::getLastPosition( bool ros )
{
  if (!ros) {
    return rwseg->getLastPosition();
  } else {
    //throw UnimplementedException(" NOT IMPLEMENTED ");
    CatalogInstance* pCI = CatalogInstance::getCatalog();//new CatalogInstance();
    ROSAM* pROS = pCI->getROSAM(_projName, pCI->getPrimaryColumnName(_projName));

    //for now WOS only works on integers ... fix later ...
    int lastPosition = *(int*)pROS->getLastIndexValuePrimary();
    delete pROS;
    delete pCI;
    return lastPosition;
  }
}

int WOSManager::getLastValue( bool ros )
{
  if (!ros) {
    return rwseg->getLastValue();
  } else {
    //throw UnimplementedException(" NOT IMPLEMENTED ");
    CatalogInstance* pCI = CatalogInstance::getCatalog();
    //cout << " FNAME " << _fName << " primary " << pCI->getPrimaryColumnName(_fName) << endl;
    ROSAM* pROS = pCI->getROSAM(_projName, pCI->getPrimaryColumnName(_projName));
    //for now WOS only works on integers ... fix later ...
    int lastValue = *(int*)pROS->getLastIndexValueSecondary();
    delete pROS;
    //delete pCI;
    return lastValue;
  }
}

int* WOSManager::getNextPosition( bool ros )
{
    size_t retklen, retdlen;
  Dbt skey;
	void *retkey, *retdata;	
	bool second_pass = false;
	int answer;

	memset(&skey, 0, sizeof(skey)); 
	memset(&_bulk_data, 0, sizeof(_bulk_data)); 

	_bulk_data.set_data( _bulk_buffer );
	_bulk_data.set_ulen( BUFFER_LENGTH );
	_bulk_data.set_flags( DB_DBT_USERMEM );

	char *a = ( char* )malloc( sizeof(int) );
	skey.set_data( a );
	skey.set_size( sizeof(int) );


	do
	  {

	// if bulk buffer is done, get a new one.
	if ( _p_bulk == NULL )
	  {
	    //cout << " THIS " << this << " -- Getting answers, page size " << BUFFER_LENGTH << endl;
	    //memset( (_bulk_buffer), 0, sizeof( (_bulk_buffer) ) );

	    answer = ( ros ? _dv_ros_cursor->get( &skey, &_bulk_data, 
						    DB_NEXT | DB_MULTIPLE_KEY )
		       : _dv_wos_cursor->get( &skey, &_bulk_data, 
					      DB_NEXT | DB_MULTIPLE_KEY ) );

	    if ( answer == DB_NOTFOUND )
	      return NULL;	    
	    rnd1=true;
	    DB_MULTIPLE_INIT(_p_bulk, _bulk_data.get_DBT());
	  }

	if ( answer == DB_NOTFOUND )
	  return NULL;

	if ( !_p_bulk )
	  return NULL;
	
	if ( !_p_bulk && second_pass )
	  return NULL;
	
	DB_MULTIPLE_KEY_NEXT(_p_bulk, _bulk_data.get_DBT(), retkey, retklen, retdata, retdlen);
	
	second_pass = true;
	//cout << " second pass " << _p_bulk << endl;
	  }
	while( !_p_bulk );  // if bulk is null, get the buffer.
	
	if ( *((int *)retkey) == lst_fst_key )
	  {
	    cout << "Correcting an obvious fit of madness of BDB. DB_NEXT is not supposed to continuosly return the last page... " << endl;
	    return NULL;
	  }
	
	if ( rnd1 )
	  lst_fst_key = *((int *)retkey);

	rnd1 = false;

	//cout << DB_NOTFOUND << " <=> " << answer << " Ret klen " << *((int *)retkey) << " resp-len " << retklen << " retdlen " << ((int *)retdata) << " resp-len " << retdlen << " BULK BUFFER " << sizeof( (_bulk_buffer) ) << endl;
	
	return( ((int *)retkey) );
}

void WOSManager::Tokenize(const string& str,
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
