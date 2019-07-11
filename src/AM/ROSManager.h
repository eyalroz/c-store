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
#ifndef __ROSManager_H
#define __ROSManager_H

#include <db_cxx.h>
#include <string>
#include <vector>
#include <map>
#include <ctime>
#include "AccessException.h"
#include "../common/UnimplementedException.h"
#include "../common/Constants.h"
#include "../common/Util.h"
#include "CatalogInstance.h"

/**
 * This is a class that will manage a ROS of a single projection 
 * It will issue instances of ROSAM and 
 * while keeping the shared resources (such as index)
 */
using namespace std;
class ROSManager
{
  protected:

  // DV stuff. cursors should be in DVAM!
  /*Db *_db_ros, *_db_wos;
    Dbc *_dv_ros_cursor, *_dv_wos_cursor;*/
  
    int _numColumns;
    string _segName, _segNameDebug;
    string _table, _fName;

  // Methods
  public:
    
    /**
     * Constructs an column of integers from a given table name
     */
    ROSManager(string table, int numColumns );

    virtual ~ROSManager();

    //virtual const void* getNextPagePrimary();
    //virtual const void* getNextPageSecondary();
    //virtual const void* skipToPagePrimary(int key);
    //virtual const void* skipToPageSecondary(int key);
    
    virtual string toString();
    int getNumColumns() { return _numColumns; }

    ROSAM* getROSAM( int colNum );
    int getLastStorageKey();

    string getFileName(){ return _fName; };

    //int getLastStorageKey( bool ros );
    
    //int getLastPosition( bool ros );

  private:
};

#endif // __ROSManager_H
