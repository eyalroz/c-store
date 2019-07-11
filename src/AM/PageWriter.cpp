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
#include <math.h>

#include <db_cxx.h>

#include "PageWriter.h"

using namespace std;

PageWriter::PageWriter(int numindicies, int primkeysize_, int seckeysize_,short primkeytype_, short seckeytype_)
{
	//_db = new Db( NULL, 0 );
	//_s_db = new Db( NULL, 0 );

  _numindicies = numindicies;

	for ( int i = 0; i < numindicies; i++ )
	{
		_db_arr.push_back( new Db( NULL, 0 ) );

		_db_arr[ i ]->set_bt_compare( ROSAM::compare_int );
		_db_arr[ i ]->set_error_stream( &cout );
	}

	// No need to call heavy artillery for non-primary indices.
	_db_arr[ PRIMARY ]->set_cachesize( 0, CACHE_SIZE, 1 );
	_db_arr[ PRIMARY ]->set_pagesize( BDB_PAGE_SIZE );
	//_db_arr[ PRIMARY ] = NULL;


	// TODO: this should not be happening for string values.
	//_db->set_bt_compare( IntegerColumn::compare_int );
	//_s_db->set_bt_compare( IntegerColumn::compare_int );

	//_db->set_cachesize( 0, CACHE_SIZE, 1 ); // 100Mb cache
	//_db->set_pagesize( PAGE_SIZE );

	//_db->set_error_stream( &cout );
	//_s_db->set_error_stream( &cout );
	
	/* Associate the secondary with the primary. */ 
	//(_db->associate( NULL, _s_db, IntegerColumn::getSecondaryKey, 0));
	primkeysize=primkeysize_;
	seckeysize=seckeysize_;
	assert(primkeysize_);
	assert(numindicies==1 || seckeysize_);
	primkeytype=primkeytype_;
	seckeytype=seckeytype_;
	assert(primkeytype_);
	assert(numindicies==1 || seckeytype_);
}

PageWriter::~PageWriter()
{
	/*if ( _db != NULL )
		_db->close( 0 );
	if ( _s_db != NULL )
	_s_db->close( 0 );*/

	for ( int i = 0; i < _numindicies; i++ )
	{
		//if ( _db_arr[ i ] != NULL )
		//_db_arr[ i ]->close( 0 );

		delete _db_arr[ i ];
	}
}

void PageWriter::initDB(string c_name, bool dupsort) {
  int ret;
  for (int which = 0; which < _numindicies; which++) {
    _db_local[ which ] = new Db( NULL, 0 );
  
    _db_local[ which ]->set_cachesize( 0, CACHE_SIZE, 1 );
    _db_local[ which ]->set_pagesize( BDB_PAGE_SIZE );
    
    short type;
    if (which ==0)
      type = primkeytype;
    else
      type = seckeytype;
    switch (type){
    case ValPos::INTTYPE:
      _db_local[ which ]->set_bt_compare( ROSAM::compare_int );
      break;
    case ValPos::STRINGTYPE:
      _db_local[ which ]->set_bt_compare( ROSAM::compare_key );
      break;
    case ValPos::DOUBLETYPE:
      _db_local[ which ]->set_bt_compare( ROSAM::compare_double );
      break;
    default:
      throw new UnexpectedException("Unknown Type");
    }
    _db_local[ which ]->set_error_stream( &cout );
    if (dupsort) {
      _db_local[ which ]->set_flags(DB_DUP);
      /*  //if (which == 0)
      switch (type){
      case ValPos::INTTYPE:
	_db_local[ which ]->set_dup_compare( ROSAM::compare_int );
	break;
      case ValPos::STRINGTYPE:
	_db_local[ which ]->set_dup_compare( ROSAM::compare_key );
	break;
      case ValPos::DOUBLETYPE:
	_db_local[ which ]->set_dup_compare( ROSAM::compare_double );
	break;
      default:
	throw new UnexpectedException("Unknown Type");
	}*/ //don't think I need this code since we will be inserting in order anyway
      //_db_local[ which ]->set_dup_compare(ROSAM::compare_int);
	//else
	//_db_local[ which ]->set_dup_compare(PageWriter::dup_compare2);
    }
    char* dbname = new char[1024];
    strcpy(dbname, (c_name+ 
		     ( which == PRIMARY ? PRIMARY_SUFF : "") +
		     ( which == SECONDARY ? SECONDARY_SUFF : "") +
		     ( which == TERTIARY ? TERTIARY_SUFF : "" ) +
		     ( which == QUATERNARY ? QUATERNARY_SUFF : "" )).c_str());
    ret = _db_local[ which ]->open( NULL, dbname,
				    NULL, DB_BTREE, DB_CREATE, 0664 );
    delete[] dbname;
    if ( ret < 0 ) {
      cout << "Failed to open table i = " << which << endl;
      throw new AccessException("PageWriter.cpp: Failed to open table");
    }
  }

}

void PageWriter::closeDB() {
  for (int which = 0; which < _numindicies; which++)
    _db_local[ which ]->close( 0 );
}


void PageWriter::placePage(char* page, char* primkey, char* seckey) {
  placeRecord( _db_local[ PRIMARY ], primkey, 
               primkeysize, page, PAGE_SIZE );

  placeRecord( _db_local[ SECONDARY ], seckey, seckeysize, primkey, primkeysize);
}

void PageWriter::placePage(char* page, char* primkey) {
  placeRecord( _db_local[ PRIMARY ], primkey, primkeysize, page, PAGE_SIZE );

}

void PageWriter::placeRecord( Db *db, char *key, int key_size, 
			      char *data, int data_size )
{
	Dbt dkey, ddata;  

	memset(&dkey, 0, sizeof(key)); 
	memset(&ddata, 0, sizeof(data)); 

	dkey.set_data( key );
	dkey.set_size( key_size );

	ddata.set_data( data );
	ddata.set_size( data_size );

	try
	  {
	    if (db->put(NULL, &dkey, &ddata, 0) == 0)
	      {
		/*int c;
		memcpy( &c, dkey.get_data(), sizeof( int ) );
		
		cout << "Placing key " << c << endl;
		*/
	      }
	    else {
	      throw new AccessException("PageWriter.cpp: placeRecord:  Failed");
	    }
	  } 
	catch(DbException& e) 
	  {
	    cout << "Exception " << e.what() << endl;
	    exit( 1 );
	  }
}

int PageWriter::dup_compare(Db *dbp, const Dbt *a, const Dbt *b)
{
     char* ii = ((char *)a->get_data());
     char* jj = ((char *)b->get_data());
     int size1 = a->get_size();
     int size2 = b->get_size();
     assert(size1=size2);
     if (!jj && !ii)
       return 0;
     if (!jj)
       return 1;
     if (!ii)
       return -1;
     return memcmp(ii,jj,size1);

     /*     int* ii = ((int *)a->get_data());
     int* jj = ((int *)b->get_data());
     int i, j;
    if (!ii)
      i = 0;
    else
      i=*ii;
    if (!jj)
      j=0;
    else
      j=*jj;
     if (i > j)
         return (1);
     if (i < j)
         return (-1);
	 return (0);*/
}

/*int PageWriter::dup_compare2(Db *dbp, const Dbt *a, const Dbt *b)
{
     char* ii = ((char *)a->get_data());
     char* jj = ((char *)b->get_data());
     if (!jj && !ii)
       return 0;
     if (!jj)
       return 1;
     if (!ii)
       return -1;
     return memcmp(ii,jj,seckeysize);
}*/

/*
void* PageWriter::getRecord( int key, Dbc *cursor, u_int32_t flags, 
			    bool primary, char *buffer )
{
	Dbt skey, data, pkey; // Dbt is a key / data pair

	memset(&pkey, 0, sizeof(pkey)); 
	memset(&data, 0, sizeof(data)); 

	// I don't *think* this is necessary, but we'll check...
	pkey.set_flags(DB_DBT_MALLOC);
	//data.set_flags(DB_DBT_MALLOC);
	data.set_flags(DB_DBT_USERMEM);
	
	int a = key, ret = -1, b = -1, c = -1, key_buf = 0;
	//char* key_buf = new char[ 4 ];
	//memcpy( key_buf, &a, 4 );
	a = key_buf;

	// set_data() doesn't seem to dispose pre-assigned memory space.
	if (pkey.get_data() != NULL) free(pkey.get_data());
	
	data.set_data( buffer );
	data.set_size( PAGE_SIZE );
	// BDB documentation neglects to mention that once you use your own
	// buffer you MUST set this parameter or things will break horribly.
	// thanks to a helpful post on a google newsgroup...
	data.set_ulen( PAGE_SIZE );

	pkey.set_data( (char*)&key_buf );
	pkey.set_size( sizeof( int ) );

	// memory is pre-allocated (and hopefully no one will deallocate it).
	//data.set_data( page );

	// always get by primary, if non-primary, there is a previous request.
	//ret = _iter_arr[ PRIMARY ]->get( &pkey, &data, flags );
		//else
	try
	{
		ret = iter->get( &pkey, &data, flags );
	}
    catch(DbException& e) 
	{
		if (DEBUG) cout << "Exception " << e.what() << endl;
		exit( 1 );
	}

	if ( ret < 0 )
	{
	  if (DEBUG) cout << ret << "Nothing found for key ... " << key << endl;
		return NULL;
	}

	memcpy( &b, pkey.get_data(), 4 );
	
	//cout << " (-1) RECEIVING  key " <<  b << " FOR GIVE " << a << endl;
	// this call is no loger used for the purpose of doing range query.
	// so this part is depricated.
	if ( (key < b ) && ( flags == DB_SET_RANGE ) )
	{
		///if (DEBUG) cout << " Key Past " << endl;
		//_iterator->get( &dkey, &data, DB_PREV_NODUP );
		// if ( primary )
		iter->get( &pkey, &data, DB_PREV_NODUP );
			//else
			//iter->pget( &dkey, &pkey, &data, DB_PREV_NODUP );
	}

	memcpy( &b, pkey.get_data(), sizeof( int ) );
	if (DEBUG) cout << "Receiving  key " <<  b << " for given key " << a << endl;
	
	return data.get_data();
	return 0;
}
*/
