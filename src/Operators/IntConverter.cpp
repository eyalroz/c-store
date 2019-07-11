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
#include "IntConverter.h"

IntConverter::IntConverter(ROSWOSSegment* rw_, int c_index, Dbc *iter, 
			   void **ptr, void **buf )
{
  bb = new BasicBlock(true, true, true);
  p = new IntValPos();
  curPos = -1;
  key = -1;
  fieldnum = -1;
  firstCall = true;
  _first_call_failed = false;
  byPos=false;
  //ASSUMPTION: rw iterators are already initialized to point to the right thing
  // Passing the iterator from WOSAM.
  // this should change eventually into a WOSAM used by IntConverter.
  rw=rw_;

  _iter = iter;
  _ptr = ptr;

  _buf = buf;

  _bdb_stopper = false;
}

void IntConverter::setKey(int key_, int fieldnum_) 
{
  key=key_;
  fieldnum = fieldnum_;
  firstCall = true;
  byPos = false;
}

void IntConverter::setPos(int key_, int fieldnum_) 
{
  curPos=key_;
  fieldnum = fieldnum_;
  byPos = true;
}

IntConverter::~IntConverter()
{
  delete bb;
  delete p;
}
	
Block* IntConverter::getNextValBlock(int colIndex_) 
{
  //cout << " GET nextValBlcok in IntConverter with " << colIndex_ << endl;

  if (colIndex_!=0) return NULL;
  if (byPos)
    assert(curPos!=-1);
  else
    assert(key!=-1);

  assert(fieldnum!=-1);
  char* a = NULL;
  if (!byPos) {
    if (firstCall) {
      a = rw->getNextFieldBySortKey( key, fieldnum, true, &curPos, _iter,
				     _ptr, _buf );
      firstCall = false;

      // perhaps this should be handled in ROSWOSSegment?
      // it is a mistake to call getFieldBySortKey with false if the
      // very first call has failed. 
      if ( a == NULL )
	_first_call_failed = true;
    }
    else
      if ( !_first_call_failed )
	a = rw->getNextFieldBySortKey( key, fieldnum, false, &curPos, _iter,
				       _ptr, _buf );

    // THIS is not a permanent solution. BDB goes insane and refuses to
    // stop calling getNext. I am not even sure how it eventually stops.
    // Until I fugre it out, this will stop the iterator, note that there
    // is an IntConverter per WOS and WOS is generally used for only
    // one scan, so that is valid approach (?)
    if (a == NULL || _bdb_stopper )
      {
	if ( _bdb_stopper )  // NOT USED
	  {
	    cout << " I am here, because BDB Stopper said so " << endl;
	  }
	/*if ( !byPos )
	  _bdb_stopper = true;*/
      return NULL;
      }
    //else
    //cout << " GOT a " << a << " *a " << *a << endl;
  }
  else {
    //want to stay consistent with WOS which counts positions from 1 so we need to subtract 1 to the curPos which is sent to the ROSWOSSegment which counts positions from 0
    //curPos--;
    a = rw->getFieldAtPosition(curPos-1, fieldnum);
    //cout << " calling by position at " << (curPos-1) << " FIELD " << fieldnum << " got? " << a << endl;
    if (a == NULL)
      return NULL;
  }
  if (!byPos)
    curPos++;
  memcpy( &curVal, a, sizeof(int ) );
  //if ( curPos % 1000 == 0 )
  //cout << "value: " << curVal << " position: " << curPos << endl;
  p->set(curPos, (byte*)&curVal);
  //p->value = curVal;
  //p->position = curPos;
  if (byPos)
    curPos++;
  bb->setValue(p);
  return bb;
}
	
PosBlock* IntConverter::getNextPosBlock(int colIndex_) {
	if (colIndex_!=0) return NULL;
	return NULL;	
}
	
