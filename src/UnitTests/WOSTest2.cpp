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
/**
 * WOSTest:
 *
 */
#include "WOSTest.h"

WOSTest2::WOSTest2() {
}

WOSTest2::~WOSTest2() {
}

/* args is not used here */
bool WOSTest2::run(Globals* g, const vector<string>& args) 
{
  //Log::writeToLog("WOSTest2", 0, "WOSTest2 is starting...");
  bool success=true;
  
  // that's a bad idea if you're trying to teach WOS to recover from
  // data on disk...
  //system("rm Segment1.wos.*");

  // WOS with 5 int columns
    ROSWOSSegment *rwseg = new ROSWOSSegment( "Segment1", 5 );

    if ( !rwseg->isEmpty() )
      return true;

  char *tuple  = new char[ 6 * sizeof(int) ];  // 5 columns + timestamp
  for (int i = 0; i < 17; i ++ )
    {
      memcpy( tuple+sizeof(int), &i, sizeof(int));  // sort key, col 1, 4 bytes
  
      int ii = (i * 5) % 41;  // to make storage key different 
      memcpy( tuple, &ii, sizeof(int) ); // storage key.
      
      //cout << " Append storage key " << ii << " SortKey " << i << endl;

      rwseg->appendTuple( tuple );
    }
  
  char *a = NULL;
  unsigned int storage_key = 0, sort_key = 0;

  // Please not that the boolean says that the iterator should be set
  // to that value. Following the first call, all calls should specify false.
  // 
  // 0 returns the pointer to the storage key part of the tuple. The tuple
  // is stored continguously [SK][Field1][Field2]...etc.
  a = rwseg->getNextFieldByStorageKey( 0, 0, true, NULL );
  
  memcpy( &storage_key, a, sizeof(int ) );
  memcpy( &sort_key, a+sizeof(int), sizeof(int ) );
  cout << " first Tuple: storage key " << storage_key << " sort key " << sort_key << endl;

  while ( ( a = rwseg->getNextFieldByStorageKey( 0, 0, false, NULL )) != NULL )
    {
      memcpy( &storage_key, a, sizeof(int ) );
      memcpy( &sort_key, a+sizeof(int), sizeof(int ) );
  cout << " Tuple: storage key " << storage_key << " sort key " << sort_key << endl;
    }
  
  cout << " DONE " <<  endl;
  delete rwseg;
  return success;	
}
