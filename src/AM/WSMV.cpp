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
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <cmath>

#include <db_cxx.h>
#include "../Util/BDBUtil.h"

#include "WSMV.h"

using namespace std;


WSMV::WSMV( const string name, int sid )
{
	_db_storage_key = new Db( NULL, 0 );
	_db_sort_key = new Db( NULL, 0 );

	// assume integers
	_db_storage_key->set_bt_compare( ROSAM::compare_int );
	_db_storage_key->set_error_stream( &cout );

	//_db_sort_key->set_bt_compare( AM::compare_int );
	_db_sort_key->set_error_stream( &cout );

	_db_storage_key->set_cachesize( 0, SMALL_CACHE_SIZE, 1 ); // 5Mb cache
	_db_storage_key->set_pagesize( BDB_PAGE_SIZE );

	_db_sort_key->set_cachesize( 0, SMALL_CACHE_SIZE, 1 ); // 5Mb cache
	_db_sort_key->set_pagesize( BDB_PAGE_SIZE );

	string store_k = name + ".storage_key";

	// why must string concatination be SO painful in C++? Why?
	if (_db_storage_key->open( NULL, (store_k.c_str()),NULL,
				   DB_BTREE, DB_CREATE, 0664) != 0 )
	  cerr << "Failed to open table " << (store_k) << endl;
	else
	  {
	    _db_storage_key->cursor(NULL, &_storage_key_cursor, 0);
	  }

	returned_curr = false;
	
	string sort_k = name + ".sort_key";

	if (_db_sort_key->open( NULL, (sort_k.c_str()), NULL, 
				DB_BTREE, DB_CREATE, 0664) != 0 )
	  cerr << "Failed to open table " << (sort_k) << endl;
	else
	  _db_sort_key->cursor(NULL, &_sort_key_cursor, 0);

	_sk_page = new BDBPage( (char)4, (char)0, 0, 64 );
}

WSMV::~WSMV() 
{
	if ( _storage_key_cursor != NULL )
		_storage_key_cursor->close();

	if ( _db_sort_key != NULL )
		_db_sort_key->close( 0 );
	if ( _db_storage_key != NULL )
		_db_storage_key->close( 0 );
}


void WSMV::addValueSKPair( int val, int sk )
{
  if ( _sk_page->page_length+64 > PAGE_SIZE * 8 )
    {
      int index;
      //index = *(( int* )_sk_page->data[ ( _sk_page->page_length-32 )/8 ]);
      //memcpy( &index, &_sk_page->data, 4 );

      memcpy( &index, &_sk_page->data[ ( _sk_page->page_length-96 )/8 ], 4 );
      cout << " GOT index of " << index << endl;
      // index on last value...
      PageWriter::placeRecord( _db_storage_key,
			       ( char * )&index,
			       sizeof( int ), (char*)(_sk_page), PAGE_SIZE );

      _sk_page->page_length = 64;
    }

      memcpy( &_sk_page->data[ ( _sk_page->page_length-64  )/8 ], &val, 4 );
      memcpy( &_sk_page->data[ ( _sk_page->page_length -32 )/8 ], &sk, 4 );
      _sk_page->page_length += 64;  // two integers
}

BDBPage* WSMV::getNextSKPage( )
{
  Dbt dkey, ddata;  

  clear_dbt(dkey);
  clear_dbt(ddata);

  _storage_key_cursor->get( &dkey, &ddata, DB_NEXT );
  
  if ( dkey.get_data() == NULL ) {
    if ( !returned_curr )
      {
	returned_curr = true;
	return _sk_page;
      }
    else
      return NULL;
  }

  //cout << " key " << *((int*)dkey.get_data()) << " data " << *((int*)(((char*)ddata.get_data())+8)) << endl;
  //memcpy( &page, ddata.get_data(), PAGE_SIZE );
  
  BDBPage *page = (BDBPage *)(ddata.get_data());
  return page;
}

/*
  We know that key is an integer, but for speed-up purposes, pass it as
  a char array.
*/

void WSMV::addSortKeySKPair( char *sort_key, int size1, 
			     char *store_key, int size2 )
{
  	Dbt dkey, ddata;  

	clear_dbt(dkey);
	clear_dbt(ddata);

	dkey.set_data( sort_key );
	dkey.set_size( size1 );

	ddata.set_data( store_key );
	ddata.set_size( size2 );

	if (_db_sort_key->put( NULL, &dkey, &ddata, 0) == 0)
	{
	  int c;
	  memcpy( &c, dkey.get_data(), sizeof( int ) );
	  //if ( DEBUG )
	  cout << "WSMV: key " << c << " stored." << endl;
	}
	else
	  cerr << "WSMV::addSortKeySKPair:  Failed " << endl;
}


int WSMV::getStorageKey( char *sort_key, int size )
{
  Dbt dkey, ddata;  
  
  clear_dbt(dkey);
  clear_dbt(ddata);

  dkey.set_data( sort_key );
  dkey.set_size( size );

  _sort_key_cursor->get( &dkey, &ddata, DB_SET );
  
  if ( ddata.get_data() == NULL )
    return -1;
  
  int a = -1;
  memcpy( &a, ddata.get_data(), 4 );

  return a;
}

/*
 * return pages sorted in key (integer) order
 */
char* WSMV::getNextRecordSorted()
{
	int ret;
	Dbt dkey, data;

	clear_dbt(dkey);
	clear_dbt(data);

	ret = -1; //_cursor->get( &dkey, &data, DB_NEXT );

	if ( ret < 0 )
	{
		if ( DEBUG )
			cout << "Nothing more can be extracted ... " << endl;
		return NULL;
	}

	char *page;
	memcpy( &page, data.get_data(), sizeof( char* ) );
	
	return page;
}

int WSMV::getNextIntSorted()
{
	int ret;
	Dbt dkey, data;

	clear_dbt(dkey);
	clear_dbt(data);

	ret = -1 ; //_cursor->get( &dkey, &data, DB_NEXT );

	if ( ret < 0 )
	{
		if ( DEBUG )
			cout << "Nothing more can be extracted ... " << endl;
		return -1;
	}

	/*char *page;
	  memcpy( &page, data.get_data(), sizeof( char* ) );*/
	
	return * ((int *)(dkey.get_data()));
}

void WSMV::test()
{
	WSMV *wsmv = new WSMV( "WriteStore1", 5 );
	wsmv->addValueSKPair( 10, 1 );

	wsmv->addValueSKPair( 3, 2 );
	wsmv->addValueSKPair( 55, 3 );
	wsmv->addValueSKPair( 111, 4 );
	wsmv->addValueSKPair( 12, 5 );
	wsmv->addValueSKPair( 1211, 6 );
	wsmv->addValueSKPair( 12141, 7 );
	wsmv->addValueSKPair( 1141, 8 );

	BDBPage *page; // = new BDBPage( '0', '0', 0, 0 ); 
	page = wsmv->getNextSKPage();
	int a, b;
	
	memcpy( &a, page->data, 4 );
	memcpy( &b, page->data+4, 4 );
	cout << " Page size in bytes is " << page->page_length/8 << " and first value " << a << " and second value is " << b << endl;
	free( page );

	page = wsmv->getNextSKPage();
	free( page );

	int sortk = 11, storek = 1;
 
	wsmv->addSortKeySKPair( ( char* )&sortk, 4, ( char* )&storek, 4 );
	sortk*=2; storek++;
	wsmv->addSortKeySKPair( ( char* )&sortk, 4, ( char* )&storek, 4 );
	sortk*=2; storek++;
	wsmv->addSortKeySKPair( ( char* )&sortk, 4, ( char* )&storek, 4 );
	sortk*=2; storek++;
	wsmv->addSortKeySKPair( ( char* )&sortk, 4, ( char* )&storek, 4 );
	sortk*=2; storek++;
	wsmv->addSortKeySKPair( ( char* )&sortk, 4, ( char* )&storek, 4 );
	
	int key = 111;
	cout << " Lookup store key " << key << " == " << wsmv->getStorageKey( ( char* )&key, 4 ) << endl;
	key = 5;
	cout << " Lookup store key " << key << " == " << wsmv->getStorageKey( ( char* )&key, 4 ) << endl;
	key = 22;
	cout << " Lookup store key " << key << " == " << wsmv->getStorageKey( ( char* )&key, 4 ) << endl;
	key = 176;
	cout << " Lookup store key " << key << " == " << wsmv->getStorageKey( ( char* )&key, 4 ) << endl;
}

string WSMV::toString()
{
	return "Write only Store string representation";
}
