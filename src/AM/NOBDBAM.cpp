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
#include "NOBDBAM.h"
#include "CatalogInstance.h"

NOBDBAM::NOBDBAM( string table_, int numindicies )
{
  cout<< " CREATE NOBDBAM for " << table_ << " Indices " << numindicies << endl;
	table=RUNTIME_DATA + table_;

	//_colName = table.substr( 0, table.size() - 4);

	 _iterator = NULL;
	
	_numindicies = numindicies;
	
	/*for ( int i = 0; i < numindicies; i++ )
	{
		_db_arr.push_back( new Db( NULL, 0 ) );
		
		_db_arr[ i ]->set_bt_compare( NOBDBAM::compare_int );
		_db_arr[ i ]->set_error_stream( &cout );
		
		_iter_arr.push_back( NULL );
		}*/
		
	loadTable( table);  // a.k.a. open primary table
	
	initCursors();
	_internalBuffer = NULL;
}

NOBDBAM::~NOBDBAM()
{
  //if ( _iterator != NULL )
  //	_iterator->close( );
  
  if ( _internalBuffer )
    delete _internalBuffer;
}

char *NOBDBAM::acquireInternalBuffer()
{
  if ( _internalBuffer == NULL ) {
    _internalBuffer = new char[ PAGE_SIZE ];

    // not sure if new will zero out memory. To be safe -- do it always for now.
    memset(_internalBuffer, 0, PAGE_SIZE);
  }

  return _internalBuffer;
}

void NOBDBAM::loadTable( string table )
{
	_tableName = table;
	
	Afile.open(table.c_str(), ios::in | ios::binary);
}

void NOBDBAM::initCursors( )
{
  //cout << " NOBDBAM called init cursors " << table << endl;
  Log::writeToLog("NOBDBAM",0,"Called init cursors");
  Afile.seekg(0, ios::beg);
  /*
  if ( _iterator != NULL )
    _iterator->close();
  
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
  */
}

const void* NOBDBAM::skipToPagePrimary( char* key )
{
  throw new UnimplementedException("Not yet done for NOBDB");
}

/*
 *  I realize that this is just a copy of the previous function wiht
 *  some constant changed and will fix this soon
 *  TODO
 */
const void* NOBDBAM::skipToPageSecondary( char* key )
{
  throw new UnimplementedException("Not yet done for NOBDB");
}

const void* NOBDBAM::getNextPageSecondary( )
{
  return getNextPage( acquireInternalBuffer() );
}

const void* NOBDBAM::getNextPagePrimary()
{
  return getNextPage( acquireInternalBuffer() );
}

const void* NOBDBAM::getNextPage() 
{
  return getNextPage( acquireInternalBuffer() );
}

const void* NOBDBAM::getNextPage( char *buf )
{  
  //need more error checking, but good enough for now
  if (!Afile.read (buf, PAGE_SIZE)) {
    return NULL;
  }
  return (const void*)buf;
}

char* NOBDBAM::getLastIndexValuePrimary() 
{
  throw new UnimplementedException("Not yet done for NOBDB");
}
char* NOBDBAM::getLastIndexValueSecondary()
{
  throw new UnimplementedException("Not yet done for NOBDB");
}


string NOBDBAM::toString()
{
	return "NOBDBAM";
}
string NOBDBAM::getTableName() {
	return table;	
}
