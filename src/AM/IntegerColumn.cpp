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

#include "IntegerColumn.h"

IntegerColumn::IntegerColumn( string table, int type )
{
	int ret;
	m_type = type;

	if ( m_type != TYPE2 )
	{
		_db = new Db( NULL, 0 );
		_db->set_bt_compare( IntegerColumn::compare_int );
		
		_db->set_cachesize( 0, CACHE_SIZE, 1 ); // 100Mb cache
		_db->set_pagesize( BDB_PAGE_SIZE );
		_db->set_error_stream( &cout );
		
		_iterator = NULL;
		
		for ( int i = 0; i < 4; i++ )  // up to 4 indices.
		{
			_db_arr.push_back( new Db( NULL, 0 ) );
			
			_db_arr[ i ]->set_bt_compare( IntegerColumn::compare_int );
			_db_arr[ i ]->set_error_stream( &cout );
			
			_iter_arr.push_back( NULL );
		}
		
		// No need to call heavy artillery for non-primary indices.
		_db_arr[ PRIMARY ]->set_cachesize( 0, CACHE_SIZE, 1 );
		_db_arr[ PRIMARY ]->set_pagesize( BDB_PAGE_SIZE );
		_db_arr[ PRIMARY ]->set_error_stream( &cout );
	}
		
	// will open other indices... should not happen when there is no file
	// TODO
	loadTable( table, type );  // a.k.a. open primary table
	
	/* 
	 * Open/create secondary. Note that it supports duplicate data 
	 * items, since last names might not be unique. 
	 */ 
	//	_secondary_iterator = NULL;
	/*
	  _secondary_db = new Db( NULL, 0 );
	  _secondary_db->set_bt_compare( IntegerColumn::compare_int );
	  _secondary_db->set_error_stream( &cout );
	  
	  _secondary_db->open( NULL, (table +  SECONDARY_INDEX_SUFFIX).c_str(), 
	  NULL, DB_BTREE, DB_CREATE, 0644 );
	*/
	/* Associate the secondary with the primary. */ 
	//(ret=_db->associate( NULL, _secondary_db, getSecondaryKey, DB_CREATE ));
}

IntegerColumn::~IntegerColumn()
{
	if ( _iterator != NULL )
		_iterator->close( );
	/*if ( _secondary_iterator != NULL )
		_secondary_iterator->close( );*
	
	if ( _db != NULL )
		_db->close( 0 );
		*if ( _secondary_db != NULL )
		_secondary_db->close( 0 );*/

	for ( int i = 0; i < 4; i++ )
	{
		if ( _db_arr[ i ] != NULL )
			_db_arr[ i ]->close( 0 );

		delete _db_arr[ i ];
	}
}

void IntegerColumn::loadTable( string table, int type )
{
	int ret, val;
	_tableName = table;

	try
	  {
	if ( type == TYPE2 )
	{
		ifstream infile( (table+".values").c_str() );
		string str1;

		while (getline(infile, str1))
		{
			val = atoi(str1.c_str());
			t2dbs[ val ] = new Db( NULL, 0 );

			t2dbs[ val ]->set_bt_compare( IntegerColumn::compare_int );
			t2dbs[ val ]->set_error_stream( &cout );
			
			t2dbs[ val ]->set_cachesize( 0, CACHE_SIZE/25, 1 );
			t2dbs[ val ]->set_pagesize( BDB_PAGE_SIZE );
			t2dbs[ val ]->set_error_stream( &cout );

			ret = t2dbs[ val ]->open( NULL, (table+str1).c_str(),
											 NULL, DB_BTREE, DB_CREATE, 0644 );

			values.push_back( val );
		}
		infile.close();
	}
	else  // open regular indices.
		for ( int i = 0; i < 4; i++ )  // up to 4 indices.
		{
			ret = _db_arr[ i ]->open
				( NULL, ( table + 
						  ( i == PRIMARY ? PRIMARY_SUFF : "") +
						  ( i == SECONDARY ? SECONDARY_SUFF : "") +
						  ( i == TERTIARY ? TERTIARY_SUFF : "" ) +
						  ( i == QUATERNARY ? QUATERNARY_SUFF : "" )).c_str(), 
				  NULL, DB_BTREE, DB_CREATE, 0664 );
			if ( ret < 0 )
				if (DEBUG) cout << " ERROR opening with i = " << i << endl;
		}
	  }
    catch(DbException& e) 
	{
		cout << "Exception " << e.what() << endl;
		exit( 1 );
	}

}

void IntegerColumn::setType2Cursors( int offset )
{
	Dbt key;

	_offset=offset;

	/*memset(&key, 0, sizeof(key)); 

    key.set_data( &offset );
	key.set_size( sizeof(int) );*/

}

void IntegerColumn::initCursors( )
{
	if ( _iterator != NULL )
		_iterator->close();
	
	if ( m_type == TYPE2 )
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
	else // non-type2 reading
		for ( int i = 0; i < 4; i++ )  // up to 4 indices.
		{
			if ( _iter_arr[ i ] != NULL )
				_iter_arr[ i ]->close( );
			_db_arr[ i ]->cursor( NULL, &(_iter_arr[ i ]), 0);
			/*
			Dbt key, data;
			memset(&key, 0, sizeof(key)); 
			memset(&data, 0, sizeof(data)); 

			int ret = _iter_arr[ i ]->get( &key, &data, DB_FIRST );

			if (DEBUG) cout << " RETURNING " << ret << endl;*/
		}
	
	/*if ( _secondary_iterator != NULL )
	  _secondary_iterator->close();*/
	
	//(_db_arr[SECONDARY]->cursor(NULL, &_secondary_iterator, 0) );
}

const void* IntegerColumn::getNextPageRange( int key )
{
	
	cerr << " DO NOT USE THIS FUNCTION " << endl;

	return NULL;
}

const void* IntegerColumn::getNextPageRange( int key, int type )
{
	void *buf;
	Dbt data, data2, pkey; // Dbt is a key / data pair
	int ret;

	memset(&pkey, 0, sizeof(pkey)); 
	memset(&data, 0, sizeof(data)); 
	memset(&data2, 0, sizeof(data2)); 

	// I don't *think* this is necessary, but we'll check...
	pkey.set_flags(DB_DBT_MALLOC);
	data.set_flags(DB_DBT_MALLOC);
	data2.set_flags(DB_DBT_MALLOC);
	
	if ( type == TYPE3 || type == TYPE1 )
	{
		pkey.set_size( sizeof(int) );
		pkey.set_data( &key );
		
		data.set_flags(DB_DBT_MALLOC);
		memset(&data, 0, sizeof(data)); 
		
		// last value index
		if ( type == TYPE3 )
			ret = _iter_arr[ TERTIARY ]->get( &pkey, &data, DB_SET_RANGE );
		else
			if ( type == TYPE1 )
				ret = _iter_arr[ PRIMARY ]->get( &pkey, &data, DB_SET_RANGE );

		//if (DEBUG) cout << type << " RETURN " << ret << " data " << data.get_data() << endl;
		
		if ( ret < 0 )
		{
			// Iterator on last value failed. That can only mean one thing
			// there is no such value in the DB.
			return NULL;
		}

		if ( type == TYPE1 )
		{
			int fst_key1 = *(((int *) data.get_data())+2);

			if ( key < fst_key1 )  // special case 1st page. No such value
			{
				//if (DEBUG) cout << "R E T U R N -- Null " << endl;
				return NULL;
			}
			//else
				//;if (DEBUG) cout << " Got answer! " << endl;
			
			return data.get_data();
		}
	}
	else
	{
		data.set_data( &key );
		data.set_size( sizeof(int) );
	}

	//if (DEBUG) cout << " KEY " << key << " pkey  " <<  " RECIEVE " << *((int*)data.get_data()) << endl;
	//if (DEBUG) cout << " KEY " << key << " pkey " << *((int*)pkey.get_data())<<  " RECIEVE " << *((int*)data.get_data()) << endl;

	ret = _iter_arr[ PRIMARY ]->get( &data, &data2, DB_SET_RANGE );

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
	return data2.get_data();
}

/*
 *  I realize that this is just a copy of the previous function wiht
 *  some constant changed and will fix this soon
 *  TODO
 */
const void* IntegerColumn::getNextPageSecondaryRange( int key, int type )
{
	void *buf;
	Dbt data, data2, pkey; // Dbt is a key / data pair
	int ret;

	memset(&pkey, 0, sizeof(pkey)); 
	memset(&data, 0, sizeof(data)); 
	memset(&data2, 0, sizeof(data2)); 

	// I don't *think* this is necessary, but we'll check...
	pkey.set_flags(DB_DBT_MALLOC);
	data.set_flags(DB_DBT_MALLOC);
	data2.set_flags(DB_DBT_MALLOC);
	
	if ( type == TYPE3 || type == TYPE1)
	{
		pkey.set_size( sizeof(int) );
		pkey.set_data( &key );
		
		data.set_flags(DB_DBT_MALLOC);
		memset(&data, 0, sizeof(data)); 
		
		// last value index
		if ( type == TYPE3 )
			ret = _iter_arr[ QUATERNARY ]->get( &pkey, &data, DB_SET_RANGE );
		else
			if ( type == TYPE1 )
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

	int fst_key3 = *(((int *) data2.get_data())+2);

	//if ( key < fst_key3 )  // special case of the first page. No such value
	//return NULL;

	//if (DEBUG) cout << " GETTING " << fst_key3 << " vs  key " << key << endl;
	return data2.get_data();
}

const void* IntegerColumn::getNextPageSecondaryRange( int key )
{
	cerr << " DO NOT USE THIS FUNCTION " << endl;

	return NULL;
}

const void* IntegerColumn::getNextPageSecondary( )
{
	char* buf = new char[ PAGE_SIZE ];

	return getNextPage( -1, _iter_arr[ SECONDARY ], DB_NEXT, false, buf );
}

/*const void* IntegerColumn::getNextPageSecondary( char* buf)
{
  //char* buf = new char[ PAGE_SIZE ];

	return getNextPage( -1, _iter_arr[ SECONDARY ], DB_NEXT, false, buf );
}*/

const void* IntegerColumn::getNextPage()//Key( int key )
{
	char* buf = new char[ PAGE_SIZE ];
	// key does not matter here.
	return getNextPage( -1, _iter_arr[ PRIMARY ], DB_NEXT, false, buf );
}


const void* IntegerColumn::getNextPage( char *buf )
{
	return getNextPage( -1, _iter_arr[ PRIMARY ], DB_NEXT, false, buf );
}


const void* IntegerColumn::getNextPageVertical( int val, char* buf )
{
	if ( m_type != TYPE2 )
	  {
	    cerr <<"getNextPageVertical can not be called for type != TYPE2"<<endl;
	    exit( 1 );
	  }
	// if (DEBUG) cout << "IntegerColumn: getNextPageVertical" << endl;
	if ( !aligned[ val ] )
	{
		aligned[ val ] = true;
		Dbt data, pkey;
		int ret;
		// if (DEBUG) cout << "IntegerColumn: in before memset with val " << val << endl;
		memset(&pkey, 0, sizeof(pkey)); 
		memset(&data, 0, sizeof(data)); 
		

		pkey.set_flags(DB_DBT_MALLOC);
		pkey.set_size( sizeof(int) );
		pkey.set_data( &_offset );

		data.set_flags(DB_DBT_USERMEM);
		data.set_size( PAGE_SIZE );
		data.set_ulen( PAGE_SIZE );
		data.set_data( buf );		

		ret = t2cursors[ val ]->get( &pkey, &data, DB_SET_RANGE );
		//if (DEBUG) cout << "IntegerColumn: here we are (2) " << endl;
		if ( ret < 0 )
			return NULL;		

		// if (DEBUG) cout << "Key/offset " << _offset << " value " << val << endl;
		return data.get_data();
	}
	else  // just scan vertically.
	  {
	    
	    // if (DEBUG) cout << "IntegerColumn: in else" << endl;
	    return getNextPage( val, t2cursors[ val ], DB_NEXT, false, buf );
	  }
}

/*
const void* IntegerColumn::getNextPage( int index )
{
	// key does not matter here.
	//return getNextPage( -1, _iter_arr[ index ], DB_NEXT, ( index == PRIMARY ? true : false ) );
	return NULL;

	// depricated.
}
*/

const void* IntegerColumn::getNextPage( int key, Dbc *iter, u_int32_t flags, 
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
}
/*
const void* IntegerColumn::getNextPage( int key, Dbc *iter, 
										u_int32_t flags, bool primary )
{
	Dbt dkey, data, pkey; // Dbt is a key / data pair
	memset(&dkey, 0, sizeof(dkey)); 
	memset(&pkey, 0, sizeof(pkey)); 
	memset(&data, 0, sizeof(data)); 
	// I don't *think* this is necessary, but we'll check...
	dkey.set_flags(DB_DBT_MALLOC);
	pkey.set_flags(DB_DBT_MALLOC);
	data.set_flags(DB_DBT_MALLOC);
	
	// This is positively NOT working. Or working in a crashing sort of way.
	//data.set_flags( DB_DBT_USERMEM );
	
	int a = key, ret = -1;
	//char* buffer = new char[ 4 ];
	int buffer=a;
	char* page = new char[ PAGE_SIZE ];
	//memcpy(buffer, &a, 4 );

	// set_data() doesn't seem to dispose pre-assigned memory space.
	if (dkey.get_data() != NULL) free(dkey.get_data());
	if (pkey.get_data() != NULL) free(pkey.get_data());
	
	dkey.set_data( (char *)&buffer );
	dkey.set_size( sizeof( int ) );
	
	pkey.set_data( buffer );
	pkey.set_size( sizeof( int ) );

	//if (DEBUG) cout << " ----  PRIMARY " << primary << endl;
	
	// memory is pre-allocated (and hopefully no one will deallocate it).
	//data.set_data( page );

	//if ( primary )
		ret = iter->get( &dkey, &data, flags );
		//else
		//ret = iter->pget( &dkey, &pkey, &data, flags );

	if ( ret < 0 )
	{
		if (DEBUG) cout << " Nothing found for key ... " << key << endl;
		return NULL;
	}

	int b;
	memcpy( &b, dkey.get_data(), 4 );
	
	//if (DEBUG) cout << " RECEIVING  key " <<  b << " FOR GIVE " << a << endl;
	if ( (key < b ) && ( flags == DB_SET_RANGE ) )
	{
		///if (DEBUG) cout << " Key Past " << endl;
		//_iterator->get( &dkey, &data, DB_PREV_NODUP );
		// if ( primary )
		iter->get( &dkey, &data, DB_PREV_NODUP );
			//else
			//iter->pget( &dkey, &pkey, &data, DB_PREV_NODUP );
	}

	memcpy( &b, dkey.get_data(), sizeof( int ) );
	if (DEBUG) cout << " Receiving  key " <<  b << " for given KEY " << a << endl;
	
	return data.get_data();
}
*/

int IntegerColumn::getLastIndexValue()
{
  if (DEBUG) cout << " CALL ME " << endl;

	Dbt key, data;
	int answer = -100;

	memset(&key, 0, sizeof(key)); 
	memset(&data, 0, sizeof(data)); 

	key.set_flags(DB_DBT_MALLOC);
	data.set_flags(DB_DBT_MALLOC);
	
	if ( m_type == TYPE3 && _iter_arr[ QUATERNARY ] == NULL )
		return -1;

	if ( m_type == TYPE3 )
		_iter_arr[ QUATERNARY ]->get( &key, &data, DB_LAST );
	else
		if ( m_type == TYPE1 )
		{
			//if (DEBUG) cout << " Find last index here " << endl;
			_iter_arr[ SECONDARY ]->get( &key, &data, DB_LAST );

			//if (DEBUG) cout << " KEY " << key.get_data() << endl;
		}
		else
			cerr << "IntegerColumn: Not sure if this should be called with type2/4." << endl;
	
	if ( key.get_data() == NULL )
	  answer = -100;
	  //return -1;
	else
	  answer = *((int *)key.get_data());

	// setting the cursor to first value is not an option, since
	// that would lose a page...
	initCursors();
	
	return answer;
}


/* 
 * Returns: 
 * < 0 if a < b 
 * = 0 if a = b 
 * > 0 if a > b 
 */ 

int IntegerColumn::compare_int(Db *dbp, const Dbt *a, const Dbt *b)
{
     int i = *((int *)a->get_data());
     int j = *((int *)b->get_data());

     if (i > j)
         return (1);
     if (i < j)
         return (-1);
     return (0);
}

/* 
 * getname -- extracts a secondary key
 * key/data pair 
 */ 
int IntegerColumn::getSecondaryKey( Db *dbp, const Dbt *pkey,const Dbt *pdata, 
									Dbt *skey )
{
	memset(skey, 0, sizeof(Dbt) );
	//skey->set_flags( DB_DBT_APPMALLOC );

	// don't know if I should copy the key or not... just to be safe though..
	//char *kc = new char[ sizeof(int) ];
	//memcpy( kc, (( char* )pdata->get_data())+12, sizeof(int) );

	//int b;
	//memcpy( &b, (( char* )pdata->get_data())+12 , 4 );
	//if (DEBUG) cout << " VALUE " << b << endl;

	skey->set_data( (( char* )pdata->get_data())+12  );
	//skey->set_data( kc );
	skey->set_size( sizeof( int ) );

	return (0);
}

string IntegerColumn::toString()
{
	return "Integer Column string representation";
}
