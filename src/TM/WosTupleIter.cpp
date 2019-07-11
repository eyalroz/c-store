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
/*
 * @(#)WosTupleIter.cpp
 * Author: Cheng
 * Copyright by CSTORE
 */

#include <string>
#include <vector>

#include "WosTuple.h"  //tuple representation of WOS projection
#include "WosTupleIter.h"

#define SEPARATOR '\t'
/**
 * Iterator for WOS tuples in sort-key order.
 * It is very critical to implement it more efficiently since it is heavily used
 * in merge out loop.
 */
using namespace std;


WosTupleIter::WosTupleIter(ROSWOSSegment* wosSegPtr_, int cols){
  useFile = false;
  useWOS = true;
  wosSegPtr = wosSegPtr_;
  numCol = cols;
  m_idx = 0;
}

WosTupleIter::WosTupleIter(string wosFile, int cols){
  useFile = true;
  useWOS = false;
  infile = new ifstream(wosFile.c_str());
  numCol = cols;
  m_idx = 0;
}

//special case with tuples vectors already;
WosTupleIter::WosTupleIter(vector<WosTuple*> tuplesVec){
        useFile = false;
        useWOS = false;
        wosTuples = tuplesVec;
        m_idx = 0;
}
    /**
     * Destructs this WosTupleIter.
     */
WosTupleIter::~WosTupleIter(){
  //do something to save memory
}

	/**
	get next WosTuple
	*/
WosTuple WosTupleIter::getNextTuple(){
  if(!useFile && !useWOS){
    WosTuple wosTuple = *wosTuples[m_idx];
    m_idx++;
    return  wosTuple;
  }
  else if(useFile){
          
          WosTuple wosTuple;
          for(int i=1; i<= numCol; i++){
                  string tempLine = line;
                  string tempStr = extractDelimitedSubstring(SEPARATOR, tempLine, i);
                  int val = atoi(tempStr.c_str());
                  wosTuple.addVal(val);
          }
          return wosTuple;
  }else{
          return *currentTuple;
  }
  
}

/**
   has next WosTuple
*/
bool WosTupleIter::hasNextTuple(){
  if(useFile){
    if(getline((*infile), line)){ 
      return true;
    }else{
      return false;
    }
  }else{
          if(useWOS){
                  char* rawTuple;
                  m_idx++;
                  if(m_idx==1){
                          currentTuple = NULL;
                          //rawTuple = wosSegPtr->getNextField(0,true);
                          rawTuple = wosSegPtr->getNextFieldBySortKey(0,0,true,NULL);
                  }else{
                          //rawTuple = wosSegPtr->getNextField(0,false);
                          rawTuple = wosSegPtr->getNextFieldBySortKey(0,0,false,NULL);
                  }
                  if(rawTuple != NULL){

                          if(currentTuple!= NULL) delete currentTuple;

                          currentTuple = new WosTuple();
                          for(int i=1; i<= numCol; i++){
                                  int val=0;
                                  memcpy(&val, rawTuple +i*sizeof(int), sizeof(int));
                                  currentTuple->addVal(val);

                          }
                          
                          return true;
                  }else{
                          return false;
                  }
                  
          }else{
                  return ((unsigned int) m_idx) < wosTuples.size();
          }
  }   
}

string WosTupleIter::extractDelimitedSubstring( char c, string str, int index )
{
	int col = 0, left = 0, right = 0;
	
	for (unsigned int i = 0; i < str.size(); i++ )
	{
		if ( str[ i ] == c )
		{
			col++;

			if ( col == index )
			{
				right = i;
				break;
			}
			else
				left = i;
		}
	}
	if (index != 1)
	  left++;
	return str.substr( left, ( right-left ) );
}

