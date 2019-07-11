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

#include "DVAM.h"

DVAM::DVAM( string table_, bool ros)
{
  init=true;

  _numindicies = 2; // That's what I think it is for DV, always.

  _ros = ros;

  string s = table_;
  int loc;
  loc = s.rfind("/", string::npos);
  if(s.rfind("/", string::npos) != string::npos) 
    {
      table=s.substr(loc+1); 
    } 
  else 
    {
      table= table_;    
    }
  retval = 0;
}

DVAM::~DVAM()
{
  delete dvc;
  delete dpencoder;
  delete dpdecoder;
}

void DVAM::initCursors( )
{
}

const void* DVAM::skipToPagePrimary( char* key )
{
  //  ic->setKey(key, _fieldnum);
  init = false;
  return dpencoder->getPage();
}

const void* DVAM::skipToPageSecondary( char* key )
{
  //((IntEncoder*)encoder)->resetPair();
  //ic->setPos(key, _fieldnum);
  //_curPos=key+1;
  return dpencoder->getPage();
}

const void* DVAM::getNextPageSecondary( )
{
  //ic->setPos(_curPos, _fieldnum);
  //_curPos++;
  return dpencoder->getPage();
}

const void* DVAM::getNextPagePrimary()
{
  //if (init) {
    //ic->setKey(0, _fieldnum);
  //init = false;
  //}
  return dpencoder->getPage();
}

char* DVAM::getLastIndexValuePrimary() 
{
  retval = _wosMgr->getLastValue( _ros );
  //for now WOS only works on integers ... fix later ...
  return ((char*)&retval);
}

char* DVAM::getLastIndexValueSecondary() 
{
  retval = _wosMgr->getLastPosition( _ros );
  //for now WOS only works on integers ... fix later ...
  return ((char*)&retval);
}


string DVAM::toString()
{
	return "DV string representation of some column (add name)";
}

string DVAM::getTableName() 
{
	return table;	
}

bool DVAM::isROSDV()
{
  return _ros;
}

void DVAM::deleteTuple( int pos )
{
  if ( !_wosMgr )
    throw AccessException("This DVAM was initialized outside of WOSManager, which is wrong ");
  _wosMgr->deleteTuple( pos, this->isROSDV() );
}

void DVAM::setWOSManager( WOSManager *wm )
{
  _wosMgr = wm;
  
  // if this is not called, DVAM will not function properly
  // setWOSManager is practically part of a constructor.
  dvc = new DVConverter( _wosMgr, this->_ros ); //rwseg, 0);
  dpencoder = new IntEncoder(dvc, // data source
			   0,              // column index
			   //8*PAGE_SIZE);   // buffer size in bits
			   96);   // buffer size in bits
  dpdecoder = new IntDecoder( false );         // value sorted
  
  initCursors();
}

int DVAM::getLastStorageKey()
{  
  //int storageKey = -1;//rwseg->getLastStorageKey(); 
  return _wosMgr->getLastStorageKey( _ros );
}
