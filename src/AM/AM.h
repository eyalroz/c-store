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
#ifndef __AM_H
#define __AM_H

#include <db_cxx.h>
#include <string>
#include <vector>
#include <map>
#include "AccessException.h"
#include "../common/Constants.h"
#include "../common/Util.h"

/**
 * Access BDB columns
 * Basically IntegerColumn with some modifications
 */
using namespace std;
class AM
{
  public:
    
  // destructor
  virtual ~AM();
  
  // Initializes the cursors (primary/secondary) for this table.
  virtual void initCursors()=0;
  
  // Gets the next page along particular index
  virtual const void* getNextPagePrimary()=0;
  virtual const void* getNextPageSecondary()=0;
  virtual const void* skipToPagePrimary(char* key)=0;
  virtual const void* skipToPageSecondary(char* key)=0;
	
  
  virtual char* getLastIndexValuePrimary()=0;
  virtual char* getLastIndexValueSecondary()=0;
  virtual string toString()=0;
  virtual string getTableName(){ return string("I am just an AM! "); };
  virtual string getColumnName()=0;
  virtual int getNumIndices()=0;
  
 protected:
  AM();
};

#endif // __AM_H
