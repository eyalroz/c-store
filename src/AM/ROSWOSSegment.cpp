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
#include "../Util/BDBUtil.h"

#include "ROSWOSSegment.h"
#include "CatalogInstance.h"

int ROSWOSSegment::counter = 0;

int ROSWOSSegment::new_page_counter = 0;

ROSWOSSegment::ROSWOSSegment( string segName, int numfields )
{
  wos_data = new vector<char*>();
  fake_key = 225262;
  _col = false;
  _debug = false;

  _p_bulk = NULL;
  /* Soon to go away completely
  string s = segName;
  int loc;
  loc = s.rfind("/", string::npos);
  if(s.rfind("/", string::npos) != string::npos) {
    _segName=s.substr(loc+1); 
  } else {
    _segName= segName;    
    }*/

  _segName =  CatalogInstance::stripOffDirectory( segName );
  //cout << " COMPARE " << _segName << " VS " << CatalogInstance::stripOffDirectory( segName ) << " VS " << segName << endl;

	ROSWOSSegment::counter++;
	//cout << " GOT _segname " << _segName << " with " << numfields << endl;
        empty = true;

        _db_storage_key = new Db( NULL, 0 );
	_db_sort_key = new Db( NULL, 0 );
	// Sort key is not guaranteed to be unique. StorageKey has to be.
	_db_sort_key->set_flags( DB_DUP );
	
	// assume integers
	_db_storage_key->set_bt_compare( ROSAM::compare_int );
	_db_storage_key->set_error_stream( &cout );

	// do not assume integers on sort key because key is multiple
	// fields mended together.
	// Actually, we do assume that sort key is now an integer
	// but that will change, so segment should ask the catalog and
	// decide on this here.

	// that did not buy us much.
	//_db_sort_key->set_bt_minkey( 128 );

	_db_sort_key->set_bt_compare( ROSAM::compare_int );
	_db_sort_key->set_error_stream( &cout );

	_db_storage_key->set_cachesize( 0, SMALL_CACHE_SIZE, 1 ); // 5Mb cache
	//this is very much an open question. There is certainly no need
	// to use a large page because we're only indexing storagekey to
	// position. Data is stored outside of BDB.
	_db_storage_key->set_pagesize( 4096 );

	_db_sort_key->set_cachesize( 0, SMALL_CACHE_SIZE, 1 ); // 5Mb cache
	_db_sort_key->set_pagesize( 4096 );
	// }

	string store_k = (RUNTIME_DATA+_segName + ".storage_key");

	system( ("rm " + store_k).c_str() );

	if ( (_db_storage_key->open( NULL, (store_k.c_str()),NULL,
					       DB_BTREE, DB_CREATE, 0664) != 0 ))
	  cerr << "Failed to open table " << (store_k) << endl;
	else
	  _db_storage_key->cursor(NULL, &_storage_key_cursor, 0);

	string sort_k = (RUNTIME_DATA+_segName + ".sort_key");
	_segNameDebug = sort_k;

	system( ("rm " + sort_k).c_str() );

	if ((_db_sort_key->open( NULL, (sort_k.c_str()), NULL, 
					   DB_BTREE, DB_CREATE, 0664) != 0 ) )
	  cerr << "Failed to open table " << (sort_k) << endl;
	else
	  _db_sort_key->cursor(NULL, &_sort_key_cursor, 0);

	wos_size = curr_wos_page = wos_tuple_count = 0;
	num_fields = numfields;

	//_segName = segName;
	if ( _col )
	  {
	    for ( int i = 0; i < num_fields+1; i++ )
	      {
		vector<char *> *a = new vector<char *>;
		a->push_back( (char*)malloc( WOS_PAGE_SIZE ) );
		wos_col_data.push_back( *a );
		//wos_col_data[ i ].push_back( (char*)malloc( WOS_PAGE_SIZE ) );
		//cout << " WOS COL DATA IS " << wos_col_data.size() << endl;
	      }
	  }
	else
	  {
	    wos_data->push_back( (char*)malloc( WOS_PAGE_SIZE ) );
	  }

	if ( _col )
	  tuple_size = sizeof(int);
	else
	  tuple_size = (num_fields+1)*sizeof(int);

	// Load pre-existing WOS. 
	// opening file for reading fails miserably if the file
	// does not exist. So instead of non-existing file will
	// make an empty one by touch.
	system( (("touch " RUNTIME_DATA) + _segName + ".TUPLES").c_str() );
	FILE *wosfd = fopen((RUNTIME_DATA+_segName + ".TUPLES").c_str(), "r");

	char tuple[ (num_fields+1)*sizeof(int) ];

	int counter = 0;

	int z;
	//cout << " WOS is reading " << (RUNTIME_DATA +_segName + ".TUPLES") << " _numFields " << num_fields << endl;
	while ((z = (fread(tuple, 1, (num_fields+1)*sizeof(int), wosfd))))
	  {
	    if ( (unsigned int)z != ((num_fields+1)*sizeof(int)) )
	      {
	      cerr << "Something has gone wrong with populating of the WOS... number of fields does not match the .TUPLES that I am reading?" << endl;
	      throw AccessException("ROSWOSSegment: WOS population failed(?)");
	      }

	    /*if ( counter % 1000 == 0 )
	      cout << " READ TUPLE from file, count " << z  << " REATD " << counter << endl;*/
	    appendTuple( tuple );
	    counter++;
	  }
	cout << " Loaded " << counter << " tuples from " + _segName << endl;

	//_db_sort_key->close( 0 );

	//  PRINT CONTENTS OF OPENED DB-s
	Dbt skey, data; // Dbt is a key / data pair

	clear_dbt(skey);
	clear_dbt(data);
	clear_dbt(_bulk_data);

	_bulk_buffer = malloc( BUFFER_LENGTH );
	  
	//char *abc = ( char *)malloc ( 599875 * 4 );
	/*
	int before = StopWatch::ticks();
	int ctr1=0, ctr2=0;
	
	while ( _sort_key_cursor->get( &skey, &data, DB_NEXT ) == 0 )
	  {
	    ctr2++;
	    //cout << " Got STORAGE key " << *((int *)skey.get_data()) << endl;
	  }
	
	int after = StopWatch::ticks();

	//cout << " Time to scan = " << ( after-before ) * StopWatch::secondsPerTick() << " scanned " << ctr2 << endl;

	read_ptr = 0;
	db_access = mem_cpy = total_time1 = 0.0;
	accesses = 0;
	*/
	//sw = new StopWatch();
	//cout << " PAGES! " << ROSWOSSegment::new_page_counter  << endl;
	ROSWOSSegment::new_page_counter = 0;
}

void ROSWOSSegment::reInitCursors()
{
  // Not sure if this should be always used for cursor
  // initialization. Seems this might be a different function
  // from the constructor initialization.
	  _db_storage_key->cursor(NULL, &_storage_key_cursor, 0);
	  _db_sort_key->cursor(NULL, &_sort_key_cursor, 0);
	  _debug = false;
	  //cout << "re init" << endl;
}

void ROSWOSSegment::initCursors( Dbc **_storage_cursor, Dbc **_sort_cursor )
{
  //cout << " And there is a call of initCursors in ROSWSSEgment " << *_sort_cursor << endl ;

  _db_storage_key->cursor(NULL, _storage_cursor, 0);
  _db_sort_key->cursor(NULL, _sort_cursor, 0);
	  _debug = false;
}


ROSWOSSegment::ROSWOSSegment( string segName, int numfields, void *wm )
{
  //wos_data = new vector<char*>();
  _col = false;

  _p_bulk = NULL;
	
  string s = segName;
  int loc;
  loc = s.rfind("/", string::npos);
  if(s.rfind("/", string::npos) != string::npos) {
    _segName=s.substr(loc+1); 
  } else {
    _segName= segName;    
  }

  //_wosManager = wm;

  empty = true;  // ? Need to update after initialization 

  //_db_storage_key = db_store_key; 
  //_db_sort_key = db_sort_key;

  //wos_size = curr_wos_page = wos_tuple_count = 0;
  num_fields = numfields;

	//_segName = segName;
  /*if ( _col )
	  {
	    for ( int i = 0; i < num_fields+1; i++ )
	      {
		vector<char *> *a = new vector<char *>;
		a->push_back( (char*)malloc( WOS_PAGE_SIZE ) );
		wos_col_data.push_back( *a );
		//wos_col_data[ i ].push_back( (char*)malloc( WOS_PAGE_SIZE ) );
		//cout << " WOS COL DATA IS " << wos_col_data.size() << endl;
	      }
	  }
	else
	  {
	    wos_data->push_back( (char*)malloc( WOS_PAGE_SIZE ) );
	  }

	if ( _col )
	  tuple_size = sizeof(int);
	else
	  tuple_size = (num_fields+1)*sizeof(int);
  */

	// Load pre-existing WOS. 
	// opening file for reading fails miserably if the file
	// does not exist. So instead of non-existing file will
	// make an empty one by touch.
	/*
	system( ("touch " + _segName + ".TUPLES").c_str() );
	FILE *wosfd = fopen((_segName + ".TUPLES").c_str(), "r");

	char tuple[ (num_fields+1)*sizeof(int) ];

	int counter = 0;

	while (fread(tuple, 1, (num_fields+1)*sizeof(int), wosfd))
	  {
	    //cout << " READ TUPLE from file, count " << a  << " REATD " << b << endl;
	    appendTuple( tuple );
	    counter++;
	  }
	    cout << " Loaded " << counter << " tuples from " + _segName << endl;
	*/
	//wos_data = wd;
  //wos_tuple_count = wtc;
	cout << " FastLoaded " << wos_tuple_count << " tuples from " << _segName << endl;

	    //_db_sort_key->close( 0 );

	//  PRINT CONTENTS OF OPENED DB-s
	    /*Dbt skey, data; // Dbt is a key / data pair

	clear_dbt(skey);
	clear_dbt(data);
	    */
	clear_dbt(_bulk_data);

	_bulk_buffer = malloc( BUFFER_LENGTH );
	  
	//char *abc = ( char *)malloc ( 599875 * 4 );
	/*
	int before = StopWatch::ticks();
	int ctr1=0, ctr2=0;
	
	while ( _sort_key_cursor->get( &skey, &data, DB_NEXT ) == 0 )
	  {
	    ctr2++;
	    //cout << " Got STORAGE key " << *((int *)skey.get_data()) << endl;
	  }
	
	int after = StopWatch::ticks();

	//cout << " Time to scan = " << ( after-before ) * StopWatch::secondsPerTick() << " scanned " << ctr2 << endl;

	read_ptr = 0;
	db_access = mem_cpy = total_time1 = 0.0;
	accesses = 0;
	*/
	//sw = new StopWatch();
	//cout << " PAGES! " << ROSWOSSegment::new_page_counter  << endl;
	//ROSWOSSegment::new_page_counter = 0;

//_wosManager->initStorageKeyCursor( _storage_key_cursor );
//_wosManager->initSortKeyCursor( _sort_key_cursor );
}


void ROSWOSSegment::writeTuplesToDisk() 
{
  if ( _col )
    {
      cerr << "ROSWOSSegment: Cannot write to disk when in Column Mode "<<endl;
      return;
    }

  system( ("rm " + (RUNTIME_DATA+_segName + ".TUPLES")).c_str() );

  // flag "w" will erase the contents.
  FILE *wosfd = fopen((RUNTIME_DATA+_segName + ".TUPLES").c_str(), "w");

  cout << " WRITING " << (RUNTIME_DATA+_segName + ".TUPLES") << " with " << wos_tuple_count << " tuples " << endl;
  // release the memory for WOS tuple data.
  char tuple[ tuple_size ];
  int tuple_counter = 0;
  for ( unsigned int i = 0; i < wos_data->size(); i++ )
    {
      // write down the data so that WOS does not have to be repopulated
      // every time. This is not consistent with REAL WOS operation
      // mode, but for the demo purposes.
      // loop
      // for every tuple on a page.
      for ( int l = 0; l < (WOS_PAGE_SIZE / tuple_size); l ++ )
	{
	  if ( tuple_counter == wos_tuple_count )
	    break;

	  memcpy( tuple, ((*wos_data)[i])+l * tuple_size, tuple_size );
	  
	  //cout << " FPUT " << (_segName + ".TUPLES") << " tupsize " << tuple_size << endl;
	  fwrite( tuple, 1, tuple_size, wosfd );
	  tuple_counter ++;
	}

      //free( wos_data[ i ] );
    }

  cout << "ROSWOS Segment WROTE " << tuple_counter << " tuples." << endl;
  fclose(wosfd);
}

ROSWOSSegment::~ROSWOSSegment()
{
  /* DOCUMENTATION says that db->close will close all open
     cursors.
    if ( _storage_key_cursor != NULL )
    _storage_key_cursor->close();
  if ( _sort_key_cursor != NULL )
    _sort_key_cursor->close();
  */

  if ( _db_sort_key != NULL )
  _db_sort_key->close( 0 );
  if ( _db_storage_key != NULL )
  _db_storage_key->close( 0 );

  for ( unsigned int i = 0; i < wos_data->size(); i++ )
      free( (*wos_data)[ i ] );

  delete wos_data;
  delete sw;
}

bool ROSWOSSegment::isEmpty()
{
  return empty;
}

bool ROSWOSSegment::appendTuple( char *tuple )
{
  empty = false;
  // curr_wos_page doubles up as page counter
  int curr_offset = tuple_size*wos_tuple_count - 
    (WOS_PAGE_SIZE / tuple_size)*curr_wos_page * tuple_size; 

  //  cout << " space " << tuple_size*wos_tuple_count << " WOS PAGE blah " << (WOS_PAGE_SIZE / tuple_size)*curr_wos_page << endl;

  if ( curr_offset > WOS_PAGE_SIZE - tuple_size ) // spill into new page
    {
      curr_wos_page++;

      if ( _col )
	{
	  for ( int i = 0; i < num_fields+1; i++ )
	    wos_col_data[ i ].push_back( (char*)malloc( WOS_PAGE_SIZE ) );
	}
      else	  
	wos_data->push_back( (char*)malloc( WOS_PAGE_SIZE ) ); //new char( WOS_PAGE_SIZE ) );
      new_page_counter++;
      curr_offset = 0;
    }
  //else
    {
      //memcpy( fudge+ curr_offset, tuple, 4 );

      if ( _col )
	for ( int i = 0; i < num_fields+1; i++ )
	  {
	    //cout << " TRYING FOR i = " << i << endl;
	  memcpy( (char*)(wos_col_data[i][ curr_wos_page ])+curr_offset, tuple+i*sizeof(int), tuple_size );
	  }
      else
	memcpy( (char*)((*wos_data)[ curr_wos_page ])+curr_offset, tuple, tuple_size );
    }

  //char *blah = new char[ 100 ];
  //char *bleh = new char[ 4 ];
  //memcpy( bleh, &wos_tuple_count, 4 );
  // update indices on timestamp and sort key
  // current version will assume that first column (second value) is the skey
  //cout << " Place a storage key " << *(int*)tuple << " value " << wos_tuple_count << endl;
  PageWriter::placeRecord( _db_storage_key, tuple, sizeof(int), 
			   (char*)&wos_tuple_count, sizeof(int) );
  //cout << " Place a sort value " << wos_tuple_count << " vs tuple key " << *(int *)(tuple+sizeof(int)) << endl;
  PageWriter::placeRecord( _db_sort_key, tuple+sizeof(int), sizeof(int), 
  			   (char*)&wos_tuple_count, sizeof(int) );

  wos_tuple_count++;

  int z;
  memcpy(&z, tuple+sizeof(int), sizeof(int) );
  //cout << " second value " << z << " Placed " << wos_tuple_count << "th tuple " << " page is " << curr_wos_page << " offset " << curr_offset << " page size " << WOS_PAGE_SIZE << endl<< endl;
  
  return true;
}

/* 
void ROSWOSSegment::resetCursor( int where )
{
  read_ptr = where;
}
*/

/*
  This function will return pointer to the next tuple
*/
char* ROSWOSSegment::getNextTuple()
{
  return getTupleAtPosition( read_ptr++ );
}

char* ROSWOSSegment::getTupleAtPosition( int where )
{
  //cout << " WHERE " << where << " count " << wos_tuple_count << endl;
  assert(where <= wos_tuple_count );

  int page_offset = where / (WOS_PAGE_SIZE / tuple_size);
  int tuple_offset = tuple_size * 
    (where - page_offset * (WOS_PAGE_SIZE / tuple_size));
  
  /*if ( _col )
    for ( int i = 0; i < num_fields+1; i++ )
      ;
      else*/
  char *r = (char*)((*wos_data)[ page_offset ])+tuple_offset;

  //double blah = sw->quietStop();
  //total_time1 += blah;
  //accesses++;
  /*if ( blah > 0 )
    printf("user time=%f ms, added %f\n",
    (total_time1/accesses), blah );*/

  /*if ( *((int*)(r+4)) > 10000 )
  cout <<  " Get tuple at position " << where << " tuple " << *((int*)(r+4))  << " -- " << *((int*)(r+12)) << endl;
  */
  return r;
}

char* ROSWOSSegment::getTupleAtPosition( int where, int field )
{
  assert(where < wos_tuple_count );

  int page_offset = where / (WOS_PAGE_SIZE / tuple_size);
  int tuple_offset = tuple_size * 
    (where - page_offset * (WOS_PAGE_SIZE / tuple_size));
  
  /*if ( _col )
    for ( int i = 0; i < num_fields+1; i++ )
      ;
      else*/
  char *r = (char*)(wos_col_data[field][ page_offset ])+tuple_offset;

  //cout << " Tuple at position " << where << " is equal to " << * ((int *)r)<< endl;
  return r;
}

char* ROSWOSSegment::getTupleByStorageKey( char* skey, Dbc *iter )
{
  cerr << " This function is under review. I think it will fail" << endl;
  throw AccessException("Broken method called");
  int *ind = lookupByIndex( skey, sizeof(int), 
			    //( iter? iter:_storage_key_cursor ), DB_SET_RANGE );
			    ( iter? iter:_storage_key_cursor ), DB_SET_RANGE,
			    &_p_bulk, &_bulk_buffer );
  cout << " Storage key lookup on " << *((int*)skey) << " indx "  << *ind << endl;
  
  return getTupleAtPosition( *ind );
}

char* ROSWOSSegment::getTupleBySortKey( char* skey, Dbc *iter)
{
  int *ind = lookupByIndex( skey, sizeof(int), ( iter? iter:_sort_key_cursor ),
			    //DB_SET_RANGE );
			    DB_SET_RANGE, &_p_bulk, &_bulk_buffer );
  return getTupleAtPosition( *ind );
}

/*
char* ROSWOSSegment::getNextTupleBySortKey( bool reset )
{
  if ( reset )
    _db_sort_key->cursor(NULL, &_sort_key_cursor, 0);

  char* blah;  // irrelevant, this is DB_NEXT call, no key involved.
  return (char *)lookupByIndex( blah, sizeof(int), _sort_key_cursor, DB_NEXT );
}
*/

/* 
 */
int* ROSWOSSegment::lookupByIndex( char* key, int size, Dbc *iter,
				   u_int32_t flags, void **_ptr, void **buf )
{
   	size_t retklen, retdlen;
	Dbt skey;
	void *retkey, *retdata;	
	bool second_pass = false;
	int answer;

	clear_dbt(skey);
	clear_dbt(_bulk_data);
	// clear_dbt((*_buf));

	//_bulk_data.set_data( _bulk_buffer );
	_bulk_data.set_data( (*buf) );
	_bulk_data.set_ulen( BUFFER_LENGTH );
	_bulk_data.set_flags( DB_DBT_USERMEM );

	skey.set_data( key );
	skey.set_size( size );

	//if  ( _debug ) return NULL;

	do
	  {

	// if bulk buffer is done, get a new one.
	    //if ( _p_bulk == NULL )
	if ( ((*_ptr)) == NULL )
	  {
	    //cout << " Request from iter " << iter << "  key " << *(int *)key << " FLAGS? " << flags <<  endl;
	    memset( (*buf), 0, sizeof( (*buf) ) );

	    answer = iter->/*_sort_key_cursor->*/get( &skey, &_bulk_data, 
				   flags | DB_MULTIPLE_KEY );

	    if ( answer == DB_NOTFOUND )
	      return NULL;
	    //DB_MULTIPLE_INIT(_p_bulk, _bulk_data.get_DBT());
	    DB_MULTIPLE_INIT((*_ptr), _bulk_data.get_DBT());
	    //cout << " DB_NOTFOUND " << DB_NOTFOUND << " ans " <<  answer << " -- Getting answers, page size " << BUFFER_LENGTH << " ITER " << iter << " PTR " << (*_ptr) <<endl;
	  }
	//else
	//cout << " ROSWOSSegment, *_ptr is not NULL, go go go" << endl;
	/*if ( answer == DB_NOTFOUND )
	  {
	    //_debug = true;
	    // THAT really does NOT close the iterator. WHY, I have not
	    // a faintest idea!
	    //cout << " ITERATOR " << iter << " IS CLOSED " << endl;
	    //iter->close();
	    return NULL;
	    }*/

	//if ( !_p_bulk )
	if ( !((*_ptr)) )
	  return NULL;

	// I don't understand that.
	//if ( !_p_bulk && second_pass )
	//cout << " PTR " << (*_ptr) << " SECOND PASS " << second_pass << endl;
	if ( !((*_ptr)) && second_pass )
	  return NULL;
	
	DB_MULTIPLE_KEY_NEXT((*_ptr), _bulk_data.get_DBT(), retkey, retklen, retdata, retdlen);
	//DB_MULTIPLE_KEY_NEXT(_p_bulk, _bulk_data.get_DBT(), retkey, retklen, retdata, retdlen);
	// cout << " ANd now, ptr is " << *_ptr << " int ptr " << ((int*)retkey) << endl;

	second_pass = true;

	  }
	//while( !_p_bulk );  // if bulk is null, get the buffer.
	while( !(*_ptr) );  // if bulk is null, get the buffer.
	
	/*if ( (*((int*)retkey)) > 10500 )
	{
	cout << DB_NOTFOUND << " <=> " << answer << " Ret answer " << *((int *)retkey) << " retDATA " << *((int *)retdata) << endl;
	cout << "     Recieved buffer " << buf << " points to buf " << (*buf) << " and ptr " << *_ptr << endl;
	*/
	/*if ( *((int *)retdata) == 596992  &&
	     10511 == *((int *)retkey) )
	  *(int *)retdata = 597221;

	if ( *((int *)retdata) == 598272  &&
	     10524 == *((int *)retkey) )
	     *(int *)retdata = 598362;*/

	//cout << " Finally returning " << ( ((int *)retdata) ) << endl << endl;
	return( ((int *)retdata) );
  /*
  //  THIS IS AN ITERATION SEQUENCE. DO NOT ERASE.
	while ( _sort_key_cursor->get( &skey, &data, DB_NEXT | DB_MULTIPLE_KEY ) == 0 )
	  {
	    
	    for (DB_MULTIPLE_INIT(p, data.get_DBT());;) 
	      {
		
		DB_MULTIPLE_KEY_NEXT(p, data.get_DBT(), retkey, retklen, retdata, retdlen);
		if (p == NULL)
		  break;
		//cout << " Ret klen " << *((int *)retkey) << " retdlen " << *((int *)retdata) << endl;
		//printf("key: %.*s, data: %.*s\n",
		//(int)retklen, (char*)retkey, (int)retdlen, (char*)retdata);
		ctr2++;
	      }

	    cout << " Got SORT key " << *((int *)skey.get_data()) << endl;
	    cout << " Curr count " << ctr2 << endl;
	  }
	*/

  /*
	Dbt dkey, data; // Dbt is a key / data pair
	int ret; //,a, b;
	char *result = ( char* )malloc( sizeof(int) );

	
	clear_dbt(dkey);
	clear_dbt(data);

	data.set_data( &result );
	data.set_size( sizeof(int) );
	// BDB documentation neglects to mention that once you use your own
	// buffer you MUST set this parameter or things will break horribly.
	// thanks to a helpful post on a google newsgroup...
	data.set_ulen( sizeof(int) );

	dkey.set_data( key );
	dkey.set_size( size );

	try
	{
	  int before = StopWatch::ticks();
	  ret = iter->get( &dkey, &data, flags );
		int after = StopWatch::ticks();
		db_access+=(after-before)*StopWatch::secondsPerTick();
		//cout << _segNameDebug <<  " Got " << (after-before)*StopWatch::secondsPerTick() << " for " << db_access << endl;
	}
	catch(DbException& e) 
	{
		if (DEBUG) cout << "Exception " << e.what() << endl;
		// Throw exception here
		exit( 1 );
	}

	if ( ret < 0 )
	{
	  //cout << ret << "Nothing found for key ... " << key << endl;
	  return NULL;
	}
	

	//int a,b;
	//memcpy( &a, key, 4 );
	//memcpy( &b, data.get_data(), 4 );
	
	
	//cout << "Receiving  key " <<  b << " for given key " << a << endl;
	
	//if ( fake_key > 599874 )
	//return NULL;

	  //memcpy( result, &fake_key, sizeof(int) );
	//fake_key++;
	
	memcpy( result, data.get_data(), sizeof(int) );

	return ( int* )result;
  */
}

char* ROSWOSSegment::getNextFieldBySortKey( int key, unsigned short field,
					    bool first, Dbc *iter )
{
  int *pos = lookupByIndex( (char*)&key, sizeof(int), 
			    ( iter? iter : _sort_key_cursor ), 
			    ( first ? DB_SET_RANGE : DB_NEXT ),
			    &_p_bulk, &_bulk_buffer );
  if ( pos )
    {
      //cout << " Lookedup " << (int)(*pos) << endl;
      //cout << " Returning PTR " << (unsigned int)getTupleAtPosition( *pos ) << endl;
    }
  if ( pos )
    {
      if ( _col )
	{
	  char* result = (getTupleAtPosition( *pos, field ) );
	  cout << " Using key " << key << " got " << *((int*)result) << endl;
	  return result;
	}
      else
	return (getTupleAtPosition( *pos ) + sizeof(int)*field);
    }
  else
    return NULL;
}

char* ROSWOSSegment::getNextFieldBySortKey( int key, unsigned short field,
					    bool first, int* position, 
					    Dbc *iter, void **ptr, void **buf )
{
  int *pos = lookupByIndex( (char*)&key, sizeof(int), 
			    ( iter? iter : _sort_key_cursor ),
			    ( first ? DB_SET_RANGE : DB_NEXT ),
			    ptr, buf );
  if ( pos )
    {
      //cout << " Lookedup " << (int)(*pos) << endl;
      //cout << " Returning PTR " << (unsigned int)getTupleAtPosition( *pos ) << endl;
    }

  if ( pos ) {
    *position=*pos;

      if ( _col )
	{
	  return (getTupleAtPosition( *pos, field ) );
	}
      else
	{
	  char* result = (getTupleAtPosition( *pos ) + sizeof(int)*field);
	  //cout << " FIELD " << field << " Using key " << key << " got " << *((int*)result) << endl;
	  return result;

	  //return (getTupleAtPosition( *pos ) + sizeof(int)*field);
	}
      //return (getTupleAtPosition( *pos ) + sizeof(int)*field);
  }
  else
    return NULL;
}

char* ROSWOSSegment::getFieldAtPosition(int pos, unsigned short field) {
  if ( pos >= wos_tuple_count )
    return NULL;

  if ( _col )
    {
      return (getTupleAtPosition( pos, field ));
    }
  else
    {
      /*if ( pos % 1000 == 0 && field==3 )

      cout << " Getting tuple at position " << pos << " and field " << field << " but the value " << *((int*)(getTupleAtPosition( pos ) + sizeof(int)*field)) << endl;*/
    return (getTupleAtPosition( pos ) + sizeof(int)*field);
    }
}

char* ROSWOSSegment::getNextFieldByStorageKey( int key, unsigned short field,
					       bool first, Dbc *iter )
{
  cerr << " This function is under review. I think it will fail" << endl;
  throw AccessException("Broken method called");
  int *pos = lookupByIndex( (char*)&key, sizeof(int), 
			    ( iter? iter : _storage_key_cursor ),
			    ( first ? DB_SET_RANGE : DB_NEXT ),
			    &_p_bulk, &_bulk_buffer );
  
  //cout << " Lookedup " << (int)(*pos) << endl;
  //cout << " Returning PTR " << *((int*)((unsigned int)getTupleAtPosition( *pos )+sizeof(int)*field)) << endl;
  if ( pos )
    return (getTupleAtPosition( *pos ) + sizeof(int)*field);
  else
    return NULL;
}

char* ROSWOSSegment::getNextField( unsigned short field, bool first, Dbc *iter)
{
  // no more tuples
  if ( read_ptr == wos_tuple_count )
    return NULL; 

  return (getTupleAtPosition( read_ptr++ ) + sizeof(int)*field);
}

string ROSWOSSegment::toString()
{
	return "ROS WOS segment for: ";
}

int ROSWOSSegment::getWOSTupleCount()
{
  return wos_tuple_count;
}

void ROSWOSSegment::setWOSTupleCount()
{
  ++wos_tuple_count;
}

int ROSWOSSegment::getLastStorageKey()
{
  int storageKey;
  Dbt key, data;
  key.set_data(&storageKey);
  key.set_ulen(sizeof(int));
  key.set_flags(DB_DBT_USERMEM);
  if (_storage_key_cursor->get( &key, &data, DB_LAST ) != 0 ) {
    storageKey = 0;  
  }
  return storageKey;
}

int ROSWOSSegment::getLastPosition()
{
  int position;
  Dbt key, data;
  data.set_data(&position);
  data.set_ulen(sizeof(int));
  data.set_flags(DB_DBT_USERMEM);
  if (_storage_key_cursor->get( &key, &data, DB_LAST ) != 0 ) {
    position = 0;  
  }
  return position;
}

int ROSWOSSegment::getLastValue()
{
  int value;
  Dbt key, data;
  key.set_data(&value);
  key.set_ulen(sizeof(int));
  key.set_flags(DB_DBT_USERMEM);
  if (_storage_key_cursor->get( &key, &data, DB_LAST ) != 0 ) {
    value = 0;
  }
  return value;
}

bool ROSWOSSegment::isSortKeyExist(int sortKey_)
{
  int sortKey = sortKey_;
  Dbt key, data;
  key.set_data(&sortKey);
  key.set_ulen(sizeof(int));
  key.set_flags(DB_DBT_USERMEM);
  if (_sort_key_cursor->get( &key, &data, DB_SET ) == 0 ) {
    /*
    int* p = (int*)key.get_data();
    int* q = (int*)data.get_data();
    cout << "key:" << *p << " data:" << *q << endl;
    */
    return true;
  }
  return false;
}
