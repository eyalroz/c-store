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

#include <db_cxx.h>

#include "ROSAM.h"
//#include "CatalogInstance.h"

ROSAM::ROSAM( string table_, int numindicies, int primkeysize_, int seckeysize_ , short primkeytype_, short seckeytype_)
{
  cout<< " CREATE ROSAM for " << table_ << " Indices " << numindicies << endl;
	table=table_;
	
	if (numindicies == -1) {
	  numindicies = 2;
	  isType2=true;
	}
	else
	  isType2=false;

	_colName = table.substr( 0, table.size() - 4);

	_db = new Db( NULL, 0 );
	//CatalogInstance::getCatalog();

	try
	  {
	    //_db = new Db( CatalogInstance::dbEnv, 0 );
	    
	    
	    switch (primkeytype_){
	    case ValPos::INTTYPE:
	      _db->set_bt_compare( ROSAM::compare_int );
	      break;
	    case ValPos::STRINGTYPE:
	      _db->set_bt_compare( ROSAM::compare_key );
	      break;
	    case ValPos::DOUBLETYPE:
	      _db->set_bt_compare( ROSAM::compare_double );
	      break;
	    default:
	      throw new UnexpectedException("Unknown Type");
	    }
	    //_db->set_bt_compare( ROSAM::compare_key );		
	    _db->set_cachesize( 0, CACHE_SIZE, 1 ); // 10Mb cache
	    _db->set_pagesize( BDB_PAGE_SIZE );
	    _db->set_error_stream( &cout );
	    /*if (isType2) {
	      _db->set_flags(DB_DUPSORT);
	      switch (primkeytype_){
	      case ValPos::INTTYPE:
		_db->set_dup_compare( ROSAM::compare_int );
		break;
	      case ValPos::STRINGTYPE:
		_db->set_dup_compare( ROSAM::compare_key );
		break;
	      case ValPos::DOUBLETYPE:
		_db->set_dup_compare( ROSAM::compare_double );
		break;
	      default:
		throw new UnexpectedException("Unknown Type");
	      }
	      //_db->set_dup_compare(PageWriter::dup_compare);
	      }*/
	    
	  }
	catch(DbException &e)
	  {
	    // DB error handling goes here
	    cout << "ROSAM, exception: " << e.what() << endl;
	  }

	_iterator = NULL;
	
	_numindicies = numindicies;
	
	for ( int i = 0; i < numindicies; i++ )
	{
		_db_arr.push_back( new Db( NULL, 0 ) );
		
		short type;
		if (i ==0)
		  type = primkeytype_;
		else
		  type = seckeytype_;
		switch (type){
		case ValPos::INTTYPE:
		  _db_arr[ i ]->set_bt_compare( ROSAM::compare_int );
		  break;
		case ValPos::STRINGTYPE:
		  _db_arr[ i ]->set_bt_compare( ROSAM::compare_key );
		  break;
		case ValPos::DOUBLETYPE:
		  _db_arr[ i ]->set_bt_compare( ROSAM::compare_double );
		  break;
		default:
		  throw new UnexpectedException("Unknown Type");
		}
		//_db_arr[ i ]->set_bt_compare( ROSAM::compare_key );
		_db_arr[ i ]->set_error_stream( &cout );
		/*
		if (isType2) {
		  _db_arr[ i ]->set_flags(DB_DUPSORT);
		  switch (type){
		  case ValPos::INTTYPE:
		    _db_arr[ i ]->set_dup_compare( ROSAM::compare_int );
		    break;
		  case ValPos::STRINGTYPE:
		    _db_arr[ i ]->set_dup_compare( ROSAM::compare_key );
		    break;
		  case ValPos::DOUBLETYPE:
		    _db_arr[ i ]->set_dup_compare( ROSAM::compare_double );
		    break;
		  default:
		    throw new UnexpectedException("Unknown Type");
		  }
		  //_db_arr[ i ]->set_dup_compare(PageWriter::dup_compare);
		  }*/
		
		_iter_arr.push_back( NULL );
		//_iter_arr.push_back( NULL );
	}
		
	// No need to call heavy artillery for non-primary indices.
	_db_arr[ PRIMARY ]->set_cachesize( 0, CACHE_SIZE, 1 );
	_db_arr[ PRIMARY ]->set_pagesize( BDB_PAGE_SIZE );
	_db_arr[ PRIMARY ]->set_error_stream( &cout );
	

	loadTable( table);  // a.k.a. open primary table
	
	/* 
	 * Open/create secondary. Note that it supports duplicate data 
	 * items, since last names might not be unique. 
	 */ 
	//	_secondary_iterator = NULL;
	/*
	  _secondary_db = new Db( NULL, 0 );
	  _secondary_db->set_bt_compare( ROSAM::compare_int );
	  _secondary_db->set_error_stream( &cout );
	  
	  _secondary_db->open( NULL, (table +  SECONDARY_INDEX_SUFFIX).c_str(), 
	  NULL, DB_BTREE, DB_CREATE, 0644 );
	*/
	/* Associate the secondary with the primary. */ 
	//(ret=_db->associate( NULL, _secondary_db, getSecondaryKey, DB_CREATE ));
	initCursors();
	_internalBuffer = NULL;

  char* s = getenv("CSTORE_READ_AHEAD");
  if (s == NULL) {
    use_bulk_read = false;
    cout << endl;
    cout << "\t\tROSAM NO READ-AHEAD..." << endl;
    _bulkBuffer_size = 0;
  } else {
    _bulkBuffer_size = atoi(s);
    if (_bulkBuffer_size <= 0)
      _bulkBuffer_size = ROSAM::BULK_BUFFER_SIZE;   //default
    use_bulk_read = true;
    cout << endl;
    cout << "\t\tROSAM will utilize a read-ahead buffer of " << _bulkBuffer_size << " bytes "<< endl;
  }
  //+ bulk read
  if (_bulkBuffer_size > 0) {
    _bulkBuffer = new char[_bulkBuffer_size];
    bulk_data.set_flags(DB_DBT_USERMEM);
    bulk_data.set_data( _bulkBuffer );
    bulk_data.set_ulen( _bulkBuffer_size  );
    bulk_iter = NULL;
    bulk_empty = true;
  } else {
    _bulkBuffer = NULL;  
  }
  //-
  primkeysize=primkeysize_;
  seckeysize=seckeysize_;
  primkeytype=primkeytype_;
  seckeytype=seckeytype_;
}

ROSAM::~ROSAM()
{
  if ( _iterator != NULL )
  	_iterator->close( );
  
  if ( _db != NULL )
    _db->close( 0 );
    
  for ( int i = 0; i < _numindicies; i++ )
    if ( _db_arr[ i ] != NULL )
      {
	_db_arr[ i ]->close( 0 );
	
	delete _db_arr[ i ];
	_db_arr[ i ] = NULL;
      }

  if ( _internalBuffer )
    delete [] _internalBuffer;

  if ( _bulkBuffer ) {
    delete [] _bulkBuffer;
    _bulkBuffer = NULL;
  }
  delete _db;
}

char *ROSAM::acquireInternalBuffer()
{
  if ( _internalBuffer == NULL )
    _internalBuffer = new char[ PAGE_SIZE ];

  // not sure if new will zero out memory. To be safe -- do it always for now.
  memset(_internalBuffer, 0, PAGE_SIZE);

  return _internalBuffer;
}

void ROSAM::loadTable( string table )
{
  int ret;
  _tableName = table;
  
  try
    {
      for ( int i = 0; i < _numindicies; i++ )
	{
	  cout << " LOAD " << ( (RUNTIME_DATA+table) + 
				( i == PRIMARY ? PRIMARY_SUFF : "") +
				( i == SECONDARY ? SECONDARY_SUFF : "") +
				( i == TERTIARY ? TERTIARY_SUFF : "" ) +
				( i == QUATERNARY ? QUATERNARY_SUFF : "" )) << endl;
	  ret = _db_arr[ i ]->open
	    ( NULL, ( (RUNTIME_DATA+table) + 
		      ( i == PRIMARY ? PRIMARY_SUFF : "") +
		      ( i == SECONDARY ? SECONDARY_SUFF : "") +
		      ( i == TERTIARY ? TERTIARY_SUFF : "" ) +
		      ( i == QUATERNARY ? QUATERNARY_SUFF : "" )).c_str(), 
	      NULL, DB_BTREE, DB_CREATE, 0664 );
	  if ( ret < 0 )
	    {
	      cerr << "Throw AccessException, ROSAM.cpp unable to load table " << endl;
	      throw new AccessException("ROSAM.cpp: loadTable(): Unable to load table");
	    }
	}
    }
  catch (DbException& e) 
    {
      cout << "Exception " << e.what() << endl;
      exit( 1 );
    }
}

void ROSAM::initCursors( )
{
  //cout << " ROSAM called init cursors " << table << endl;
	Log::writeToLog("ROSAM",0,"Called init cursors");
	if ( _iterator != NULL )
		_iterator->close();
	
	if ( false/*m_type == TYPE2*/ )
	{
		ifstream infile( (_tableName+".values").c_str() );
		infile.clear();
		string str1;
		int val;
		
		while (getline(infile, str1))
		{
			val = atoi( str1.c_str() );
			t2cursors[val] = NULL; // creating the holders for cursors.

			t2dbs[ val ]->cursor( NULL, &t2cursors[val], 0 );
			aligned[ val ] = false;
		}
	}
	else {// non-type2 reading
	  for ( int i = 0; i < _numindicies; i++ )
		{
			if ( _iter_arr[ i ] != NULL )
				_iter_arr[ i ]->close( );
			_db_arr[ i ]->cursor( NULL, &(_iter_arr[ i ]), 0);
			
			Dbt key, data;
			memset(&key, 0, sizeof(key)); 
			memset(&data, 0, sizeof(data)); 

			//int ret = _iter_arr[ i ]->get( &key, &data, DB_FIRST );

			//if (true) cout << " RETURNING " << ret << endl;
			}
	}
	/*_db_arr[ 0 ]->cursor( NULL, &(_iter_arr[ 0 ]), 0);
	_db_arr[ 0 ]->cursor( NULL, &(_iter_arr[ 1 ]), 0);

	Dbt data, data2, pkey; // Dbt is a key / data pair
	int ret;
	int key = 20000;
	memset(&pkey, 0, sizeof(pkey)); 
	pkey.set_size( sizeof(int) );
	pkey.set_data( &key );	
	data.set_flags(DB_DBT_MALLOC);
	memset(&data, 0, sizeof(data)); 
	ret = _iter_arr[ 0 ]->get( &pkey, &data, DB_SET_RANGE );
	int* x =(int*)data.get_data();
	cout << "x[0]: " << x[0] << " " << x[1] << " " << x[2] << " " << x[3] << endl;	
	key = 80000;
	memset(&pkey, 0, sizeof(pkey)); 
	pkey.set_size( sizeof(int) );
	pkey.set_data( &key );	
	data.set_flags(DB_DBT_MALLOC);
	memset(&data, 0, sizeof(data)); 
	ret = _iter_arr[ 1 ]->get( &pkey, &data, DB_SET_RANGE );
	int* y =(int*)data.get_data();
	cout << "y[0]: " << y[0] << " " << y[1] << " " << y[2] << " " << y[3] << endl;

	data.set_flags(DB_DBT_MALLOC);
	memset(&data, 0, sizeof(data)); 
	ret = _iter_arr[ 0 ]->get( &pkey, &data, DB_NEXT );
	x =(int*)data.get_data();
	cout << "x[0]: " << x[0] << " " << x[1] << " " << x[2] << " " << x[3] << endl;	

	data.set_flags(DB_DBT_MALLOC);
	memset(&data, 0, sizeof(data)); 
	ret = _iter_arr[ 1 ]->get( &pkey, &data, DB_NEXT );
	y =(int*)data.get_data();
	cout << "y[0]: " << y[0] << " " << y[1] << " " << y[2] << " " << y[3] << endl;

	data.set_flags(DB_DBT_MALLOC);
	memset(&data, 0, sizeof(data)); 
	ret = _iter_arr[ 0 ]->get( &pkey, &data, DB_NEXT );
	x =(int*)data.get_data();
	cout << "x[0]: " << x[0] << " " << x[1] << " " << x[2] << " " << x[3] << endl;	

	data.set_flags(DB_DBT_MALLOC);
	memset(&data, 0, sizeof(data)); 
	ret = _iter_arr[ 1 ]->get( &pkey, &data, DB_NEXT );
	y =(int*)data.get_data();
	cout << "y[0]: " << y[0] << " " << y[1] << " " << y[2] << " " << y[3] << endl;
	*/
	

			//int ret = _iter_arr[ i ]->get( &key, &data, DB_FIRST );
	/*if ( _secondary_iterator != NULL )
	  _secondary_iterator->close();*/
	
	//(_db_arr[SECONDARY]->cursor(NULL, &_secondary_iterator, 0) );

	currcursor = 0;
}

/*const void* ROSAM::getNextPageRange( int key )
{
	
	cerr << " DO NOT USE THIS FUNCTION " << endl;

	return NULL;
}*/

void ROSAM::setNumCursors(int nc) {
  _numCursors = nc;	
  for ( int i = 0; i < nc; i++ )
    {
      _cursor_arr.push_back( NULL );
    }
}

const void* ROSAM::getCursorIndex(char* val, int& cursorindex) {
  _db_arr[ 0 ]->cursor( NULL, &(_cursor_arr[ currcursor ]), 0);
  cursorindex = currcursor;
  Dbt data, pkey; // Dbt is a key / data pair
  int ret;
  memset(&pkey, 0, sizeof(pkey)); 
  pkey.set_size( primkeysize );
  pkey.set_data( val );	
  data.set_flags(DB_DBT_MALLOC);
  memset(&data, 0, sizeof(data)); 
  ret = _cursor_arr[ currcursor ]->get( &pkey, &data, DB_SET );
  //ret = _cursor_arr[ currcursor ]->get( &pkey, &data, DB_GET_BOTH_RANGE );
  currcursor++;
  if ( ret < 0 )
    {
      return NULL;
    }
  
  return data.get_data();
}

const void* ROSAM::resetCursorIndex(char* val, int cursorindex) {
  //_db_arr[ 0 ]->cursor( NULL, &(_cursor_arr[ currcursor ]), 0);
  //cursorindex = currcursor;
  Dbt data, pkey; // Dbt is a key / data pair
  int ret;
  memset(&pkey, 0, sizeof(pkey)); 
  pkey.set_size( primkeysize );
  pkey.set_data( val );	
  data.set_flags(DB_DBT_MALLOC);
  memset(&data, 0, sizeof(data)); 
  ret = _cursor_arr[ cursorindex ]->get( &pkey, &data, DB_SET );
  //ret = _cursor_arr[ cursorindex ]->get( &pkey, &data, DB_GET_BOTH_RANGE );
  if ( ret < 0 )
    {
      return NULL;
    }
  
  return data.get_data();
  }

const void* ROSAM::skipToPagePrimary( char* key )
{
  //cout << " ROSAM calls skiptopage primary " << " name? " << getTableName() << endl;
  Dbt data, /*data2, */pkey; // Dbt is a key / data pair
	int ret;

	memset(&pkey, 0, sizeof(pkey)); 
	memset(&data, 0, sizeof(data)); 
	//memset(&data2, 0, sizeof(data2)); 

	// I don't *think* this is necessary, but we'll check...
	//pkey.set_flags(DB_DBT_MALLOC);
	//data.set_flags(DB_DBT_MALLOC);
	//data2.set_flags(DB_DBT_MALLOC);

	pkey.set_flags(DB_DBT_USERMEM);
	data.set_flags(DB_DBT_USERMEM);
	
	char* key_buf=new char[primkeysize];
	memcpy(key_buf, key, primkeysize);
	char* buffer= acquireInternalBuffer();
	data.set_data( buffer );
	data.set_size( PAGE_SIZE );
	// BDB documentation neglects to mention that once you use your own
	// buffer you MUST set this parameter or things will break horribly.
	// thanks to a helpful post on a google newsgroup...
	data.set_ulen( PAGE_SIZE );

	pkey.set_data( key_buf );
	pkey.set_size( primkeysize);
	pkey.set_ulen( primkeysize );
	
	/*if ( type == TYPE3 || type == TYPE1 )
	  {*/
	//pkey.set_size( primkeysize );
	//pkey.set_data( key );
		
	//data.set_flags(DB_DBT_MALLOC);
	//memset(&data, 0, sizeof(data)); 
		
		// last value index
		/*		if ( type == TYPE3 )
			ret = _iter_arr[ TERTIARY ]->get( &pkey, &data, DB_SET_RANGE );
		else
		if ( type == TYPE1 )*/
		if (isType2)
		  ret = _iter_arr[ PRIMARY ]->get( &pkey, &data, DB_SET );
		else
		  ret = _iter_arr[ PRIMARY ]->get( &pkey, &data, DB_SET_RANGE );

		//if (true) cout << " RETURN " << ret << " data " << data.get_data() << endl;
		
		if ( ret < 0 )
		{
			// Iterator on last value failed. That can only mean one thing
			// there is no such value in the DB.
		  //DNA: I changed it to return null instead of throwing an eception, since this might be a reasonable response to a query
		  delete[] key_buf;
		  return NULL;
		  //throw new AccessException("ROSAM::skipToPagePrimary: No such value");
		}

		/*if ( type == TYPE1 )
		  {*/
		/*int fst_key1 = *(((int *) data.get_data())+2);
		  //DNA: Why is this check necessary? I'm commenting it out ...
			if ( key < fst_key1 )  // special case 1st page. No such value
			{
				//if (DEBUG) cout << "R E T U R N -- Null " << endl;
				return NULL;
				}*/
			//else
				//;if (DEBUG) cout << " Got answer! " << endl;
			
		delete[] key_buf;
		return data.get_data();
			/*}*/
		/*}
	else
	{
		data.set_data( &key );
		data.set_size( sizeof(int) );
		}*/

	//if (DEBUG) cout << " KEY " << key << " pkey  " <<  " RECIEVE " << *((int*)data.get_data()) << endl;
	//if (DEBUG) cout << " KEY " << key << " pkey " << *((int*)pkey.get_data())<<  " RECIEVE " << *((int*)data.get_data()) << endl;

	/*ret = _iter_arr[ PRIMARY ]->get( &data, &data2, DB_SET_RANGE );

	//if (DEBUG) cout << " DB_NOTFOUND " << DB_NOTFOUND << endl;

	if ( ret < 0 )
	{
		cerr << "Additional index corrupt, ret " << ret << " key used " << *((int*)data.get_data()) <<  " returning NULL " << endl;
		return NULL;
	}

	int fst_key3 = *(((int *) data2.get_data())+2);

	if ( key < fst_key3 )  // special case of the first page. No such value
		return NULL;

	//if (DEBUG) cout << " GETTING " << fst_key3 << " vs  key " << key << endl;
	return data2.get_data();*/
}

/*
 *  I realize that this is just a copy of the previous function wiht
 *  some constant changed and will fix this soon
 *  TODO
 */
const void* ROSAM::skipToPageSecondary( char* key )
{
  //cout << " ROSAM calls skiptopage secondary " << " table " << table << " name " << getTableName() << endl;

	Dbt data, data2, pkey; // Dbt is a key / data pair
	int ret;

	memset(&pkey, 0, sizeof(pkey)); 
	memset(&data, 0, sizeof(data)); 
	memset(&data2, 0, sizeof(data2)); 

	// I don't *think* this is necessary, but we'll check...
	pkey.set_flags(DB_DBT_MALLOC);
	data.set_flags(DB_DBT_MALLOC);
	data2.set_flags(DB_DBT_MALLOC);
	//int temp = 0;
	if ( true /*type == TYPE3 || type == TYPE1*/)
	{
		pkey.set_size( seckeysize );
		pkey.set_data( key );
		
		data.set_flags(DB_DBT_MALLOC);
		//data.set_flags(DB_DBT_USERMEM);
		data.set_size(primkeysize);
		//data.set_data((char*)&temp);

		
		//memset(&data, 0, sizeof(data)); 
		
		// last value index
		if ( false /*type == TYPE3*/ )
			ret = _iter_arr[ QUATERNARY ]->get( &pkey, &data, DB_SET_RANGE );
		else
		  if ( true /*type == TYPE1*/ )
				ret = _iter_arr[ SECONDARY]->get( &pkey, &data, DB_SET_RANGE );
		
		if ( ret < 0 )
		{
			// Iterator on last value failed. That can only mean one thing
			// there is no such value in the DB.
			return NULL;
		}		
	}
	else
	{
		data.set_data( &key );
		data.set_size( sizeof(int) );
	}

	//if (DEBUG) cout << " KEY " << key << " pkey " << *((int*)pkey.get_data())<<  " RECIEVE " << *((int*)data.get_data()) << endl;

	ret = _iter_arr[ PRIMARY ]->get( &data, &data2, DB_SET_RANGE );

	if ( ret < 0 )
	{
		cerr << "Additional index corrupt, returning NULL " << endl;
		return NULL;
	}

	// int fst_key3 = *(((int *) data2.get_data())+2);

	//if ( key < fst_key3 )  // special case of the first page. No such value
	//return NULL;

	//if (DEBUG) cout << " GETTING " << fst_key3 << " vs  key " << key << endl;
	//cout << " returning " << key << " got " << data2.get_data() << endl;

	return data2.get_data();
}

const void* ROSAM::getNextPageSecondary( )
{
  /*if (!_internalBuffer)
    _internalBuffer = new char[ PAGE_SIZE ];
  else
  memset(_internalBuffer, 0, PAGE_SIZE);*/

  return getNextPage( NULL, _iter_arr[ PRIMARY ], DB_NEXT, false, 
		      acquireInternalBuffer() );
}

const void* ROSAM::getNextPagePrimary()
{
  /*if (_internalBuffer)
    _internalBuffer = new char[ PAGE_SIZE ];
  else
    memset( _internalBuffer, 0, PAGE_SIZE );*/
  // key does not matter here.
  return getNextPage( NULL, _iter_arr[ PRIMARY ], DB_NEXT, false, 
		      acquireInternalBuffer() );
}

const void* ROSAM::getNextPagePrimaryDup()
{
  /*if (_internalBuffer)
    _internalBuffer = new char[ PAGE_SIZE ];
  else
    memset( _internalBuffer, 0, PAGE_SIZE );*/
  // key does not matter here.
  return getNextPage( NULL, _iter_arr[ PRIMARY ], DB_NEXT_DUP, false, 
		      acquireInternalBuffer() );
}

const void* ROSAM::getNextPagePrimaryDup(int ci) {
  return getNextPage( NULL, _cursor_arr[ ci ], DB_NEXT_DUP, false, 
		      acquireInternalBuffer() );
}

const void* ROSAM::getNextPage() 
{
  //char* buf = new char[ PAGE_SIZE ];
  //if ( _internalBuffer )
  //_internalBuffer = new char[ PAGE_SIZE ];
  return getNextPage( NULL, _iter_arr[ PRIMARY ], DB_NEXT, false,
		      acquireInternalBuffer());
}

const void* ROSAM::getNextPage( char *buf )
{
	return getNextPage( NULL, _iter_arr[ PRIMARY ], DB_NEXT, false, buf );
}

const void* ROSAM::getNextPage( char* key, Dbc *iter, u_int32_t flags, 
          bool primary, char *buffer )
{
	
#ifdef CSTORE_PAPI 
  if (use_bulk_read)
    return getNextPageMulti( key, iter, flags, primary, buffer );
  else
    return getNextPageSingle( key, iter, flags, primary, buffer );
#else
  if (use_bulk_read)
  return getNextPageMulti( key, iter, flags, primary, buffer );
  Dbt skey, data, pkey; // Dbt is a key / data pair

	memset(&pkey, 0, sizeof(pkey)); 
	memset(&data, 0, sizeof(data)); 

	// I don't *think* this is necessary, but we'll check...
	//pkey.set_flags(DB_DBT_MALLOC);
	pkey.set_flags(DB_DBT_USERMEM);
	//data.set_flags(DB_DBT_MALLOC);
	data.set_flags(DB_DBT_USERMEM);
	
	int ret = -1/*, b = -1*/;
	char* a = key;
	char* key_buf=new char[primkeysize];
	//char* key_buf = new char[ 4 ];
	//memcpy( key_buf, &a, 4 );
	a = key_buf;

	// set_data() doesn't seem to dispose pre-assigned memory space.
	//if (pkey.get_data() != NULL) free(pkey.get_data());
	
	data.set_data( buffer );
	data.set_size( PAGE_SIZE );
	// BDB documentation neglects to mention that once you use your own
	// buffer you MUST set this parameter or things will break horribly.
	// thanks to a helpful post on a google newsgroup...
	data.set_ulen( PAGE_SIZE );

	pkey.set_data( key_buf );
	pkey.set_size( primkeysize);
	pkey.set_ulen( primkeysize );

	// memory is pre-allocated (and hopefully no one will deallocate it).
	//data.set_data( page );

	// always get by primary, if non-primary, there is a previous request.
	//ret = _iter_arr[ PRIMARY ]->get( &pkey, &data, flags );
		//else
	try
	{
	  if (isType2)
	    ret = iter->get( &pkey, &data, flags );
	  else
	    ret = _iter_arr[ PRIMARY ]->get( &pkey, &data, flags );
	  //ret = iter->get( &pkey, &data, flags );
	}
    catch(DbException& e) 
	{
		if (DEBUG) cout << "Exception " << e.what() << endl;
		exit( 1 );
	}

	if ( ret < 0 )
	{
	  //cout << ret << "Nothing found for key ... " << key << endl;
	  delete[] key_buf;
	  return NULL;
	}

	/* DNA: COMMENT OUT 8/31/06 - I think key is always NULL
	//memcpy( &b, pkey.get_data(), 4 );
	
	//cout << " (-1) RECEIVING  key " <<  b << " FOR GIVE " << a << endl;
	// this call is no loger used for the purpose of doing range query.
	// so this part is depricated.
	if ( (memcmp(key,pkey.get_data(),primkeysize) < 0 ) && ( flags == DB_SET_RANGE ) )
	{
	  cout << " Warning - I'm not sure if the code is ever supposed to get here" << endl;
		///if (DEBUG) cout << " Key Past " << endl;
		//_iterator->get( &dkey, &data, DB_PREV_NODUP );
		// if ( primary )
		iter->get( &pkey, &data, DB_PREV_NODUP );
			//else
			//iter->pget( &dkey, &pkey, &data, DB_PREV_NODUP );
	}

	//memcpy( &b, pkey.get_data(), sizeof( int ) );
	//int c;
	//memcpy( &c, (char*)data.get_data()+12, sizeof(int));
	//cout << "Receiving  key " <<  b << " for given key " << a << " TEST " << c << endl;
	*/
	delete[] key_buf;
	return data.get_data();
	
#endif
}

const void* ROSAM::getNextPageSingle ( char* key, Dbc *iter, u_int32_t flags,
          bool primary, char *buffer )
{
	Dbt skey, data, pkey; // Dbt is a key / data pair

	memset(&pkey, 0, sizeof(pkey)); 
	memset(&data, 0, sizeof(data)); 

	// I don't *think* this is necessary, but we'll check...
	pkey.set_flags(DB_DBT_MALLOC);
	//data.set_flags(DB_DBT_MALLOC);
	data.set_flags(DB_DBT_USERMEM);
	
	int ret = -1/*, b = -1*/;
	//char* key_buf = new char[ 4 ];
	//memcpy( key_buf, &a, 4 );
	char* a = key;
	char* key_buf=new char[primkeysize];
	a = key_buf;

	// set_data() doesn't seem to dispose pre-assigned memory space.
	if (pkey.get_data() != NULL) free(pkey.get_data());
	
	data.set_data( buffer );
	data.set_size( PAGE_SIZE );
	// BDB documentation neglects to mention that once you use your own
	// buffer you MUST set this parameter or things will break horribly.
	// thanks to a helpful post on a google newsgroup...
	data.set_ulen( PAGE_SIZE );

	pkey.set_data( key_buf );
	pkey.set_size( primkeysize );

	// memory is pre-allocated (and hopefully no one will deallocate it).
	//data.set_data( page );

	// always get by primary, if non-primary, there is a previous request.
	//ret = _iter_arr[ PRIMARY ]->get( &pkey, &data, flags );
		//else
	try
	{
	  if (isType2)
	    ret = iter->get( &pkey, &data, flags );
	  else
	    ret = _iter_arr[ PRIMARY ]->get( &pkey, &data, flags );
	  //ret = iter->get( &pkey, &data, flags );
	}
    catch(DbException& e) 
	{
		if (DEBUG) cout << "Exception " << e.what() << endl;
		exit( 1 );
	}

	if ( ret < 0 )
	{
	  //cout << ret << "Nothing found for key ... " << key << endl;
	  return NULL;
	}

	/* DNA: COMMENT OUT 8/31/06 - I think key is always NULL
	//memcpy( &b, pkey.get_data(), 4 );
	
	//cout << " (-1) RECEIVING  key " <<  b << " FOR GIVE " << a << endl;
	// this call is no loger used for the purpose of doing range query.
	// so this part is depricated.
	if ( (memcmp(key,pkey.get_data(),primkeysize) < 0 ) && ( flags == DB_SET_RANGE ) )
	{
	  cout << " Warning - I'm not sure if the code is ever supposed to get here" << endl;
		///if (DEBUG) cout << " Key Past " << endl;
		//_iterator->get( &dkey, &data, DB_PREV_NODUP );
		// if ( primary )
		iter->get( &pkey, &data, DB_PREV_NODUP );
			//else
			//iter->pget( &dkey, &pkey, &data, DB_PREV_NODUP );
	}

	memcpy( &b, pkey.get_data(), sizeof( int ) );
	//int c;
	//memcpy( &c, (char*)data.get_data()+12, sizeof(int));
	//cout << "Receiving  key " <<  b << " for given key " << a << " TEST " << c << endl;
	*/
	return data.get_data();
}

const void* ROSAM::getNextPageMulti ( char* key, Dbc *iter, u_int32_t flags,
          bool primary, char *buffer )
{
  u_int32_t rflags = DB_MULTIPLE_KEY | DB_NEXT;
  Dbt myKey, myData;
  myData.set_data( buffer );
  myData.set_size( PAGE_SIZE );
  myData.set_ulen( PAGE_SIZE );
  int ret = 0;

  if ( (bulk_empty) || (!bulk_iter->next(myKey, myData) ) ) {
    if (bulk_iter) {
        delete bulk_iter;
        memset(_bulkBuffer, 0, _bulkBuffer_size);
    }
    try
    {
      if (isType2)
        ret = iter->get( &bulk_key, &bulk_data, rflags );
      else
        ret = _iter_arr[ PRIMARY ]->get( &bulk_key, &bulk_data, rflags );
    }
      catch(DbException& e) 
    {
      if (DEBUG) cout << "Exception " << e.what() << endl;
      exit( 1 );
    }  
    if ( ret < 0 )
    {
      bulk_empty = true;
      return NULL;
    }
    bulk_iter = new DbMultipleKeyDataIterator(bulk_data);
    if ( !bulk_iter->next(myKey, myData) ) {
      bulk_empty = true;
      return NULL;
    }
  }
  if (myData.get_size() > 0)  {
    // memcpy(buffer, myData.get_data(), PAGE_SIZE);
    bulk_empty = false;
    return myData.get_data();
  } else {
    bulk_empty = true;
    return NULL;
  }
} 

char* ROSAM::getLastIndexValuePrimary() 
{
  cout << " ROSAM calls get last index value " << endl;

	Dbt key, data;
	char* answer=NULL;

	memset(&key, 0, sizeof(key)); 
	memset(&data, 0, sizeof(data)); 

	key.set_flags(DB_DBT_MALLOC);
	data.set_flags(DB_DBT_MALLOC);

        if ( _iterator != NULL ) {
  	  _iterator->close( );
          _iterator=NULL;
	}

	//+ this change the cursor position and may affect the operator that is being used as position filter
	_iter_arr[PRIMARY]->dup( &_iterator, DB_POSITION );
	_iterator->get( &key, &data, DB_LAST );
	//-

	if ( key.get_data() == NULL )
	  {
	    cerr << "Throw AccessException, ROSAM.cpp no data in index data " << endl;
	    
	  throw new AccessException("ROSAM: No data in index data");
	  }
	else
	  answer = (char*)key.get_data();

	// setting the cursor to first value is not an option, since
	// that would lose a page...

	//+ this change the cursor position and may affect the operator that is being used as position filter
	//initCursors();
	//-
	
	return answer;
}
char* ROSAM::getLastIndexValueSecondary()
{
  //cout << " ROSAM calls get last index val sec " << endl;

	Dbt key, data;
	char* answer;

	if (_numindicies> 1) {

	memset(&key, 0, sizeof(key)); 
	memset(&data, 0, sizeof(data)); 

	key.set_flags(DB_DBT_MALLOC);
	data.set_flags(DB_DBT_MALLOC);

        if ( _iterator != NULL ) {
  	  _iterator->close( );
          _iterator=NULL;
	}
	
	//+ this change the cursor position and may affect the operator that is being used as position filter
	_iter_arr[SECONDARY]->dup( &_iterator, DB_POSITION );
	_iterator->get( &key, &data, DB_LAST );
	//-
	
	if ( key.get_data() == NULL )
	  {
			    cerr << "Throw AccessException, ROSAM.cpp no data in index data 2 " << endl;

	  throw new AccessException("ROSAM: No data in index data");
	  }
	else
	  answer = (char*)key.get_data();

	// setting the cursor to first value is not an option, since
	// that would lose a page...

	//+ this change the cursor position and may affect the operator that is being used as position filter
	//initCursors();

	}
	else
	  {
	    			    cerr << "Throw AccessException, ROSAM.cpp no secondary index for this column " << endl;

	  throw new AccessException("ROSAM: no secondary index for this column");
	  }
	return answer;
}


/* 
 * Returns: 
 * < 0 if a < b 
 * = 0 if a = b 
 * > 0 if a > b 
 */ 

int ROSAM::compare_int(Db *dbp, const Dbt *a, const Dbt *b)
{
     int i = *((int *)a->get_data());
     int j = *((int *)b->get_data());

     if (i > j)
         return (1);
     if (i < j)
         return (-1);
     return (0);
}
int ROSAM::compare_double(Db *dbp, const Dbt *a, const Dbt *b)
{
     double i = *((double *)a->get_data());
     double j = *((double *)b->get_data());

     if (i > j)
         return (1);
     if (i < j)
         return (-1);
     return (0);
}

int ROSAM::compare_key(Db *dbp, const Dbt *a, const Dbt *b)
{
     char* i = ((char *)a->get_data());
     char* j = ((char *)b->get_data());
     int size1 = a->get_size();
     int size2 = b->get_size();
     assert(size1 == size2);
     return memcmp(i,j,size1);
     //if (i > j)
     //    return (1);
     //if (i < j)
     //    return (-1);
     //return (0);
}

/* 
 * getname -- extracts a secondary key
 * key/data pair 
 */ 
int ROSAM::getSecondaryKey( Db *dbp, const Dbt *pkey,const Dbt *pdata, 
									Dbt *skey )
{
	memset(skey, 0, sizeof(Dbt) );
	skey->set_data( (( char* )pdata->get_data())+12  );
	skey->set_size( sizeof( int ) );
	return (0);
}

string ROSAM::toString()
{
	return "Integer Column string representation";
}
string ROSAM::getTableName() {
	return table;	
}
