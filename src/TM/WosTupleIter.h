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
 * @(#)WosTupleIter.h
 * Author: Cheng
 * Copyright by CSTORE
 */

#ifndef __WosTupleIter_H
#define __WosTupleIter_H

#include <db_cxx.h>

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include "../AM/ROSWOSSegment.h"

#include "WosTuple.h"  //tuple representation of WOS projection
/**
 * Iterator for WOS tuples in sort-key order.
 * It is very critical to implement it more efficiently since it is heavily used
 * in merge out loop.
 */
using namespace std;

class WosTupleIter
{

  public:

    /**
     * Constructs a WosTupleIter
     */
    WosTupleIter(string wosFile, int col);
    WosTupleIter(ROSWOSSegment* wosSeg, int col);
    WosTupleIter(vector<WosTuple*> tuplesVec);
    /**
     * Destructs this WosTupleIter.
     */
    ~WosTupleIter();

	/**
	get next WosTuple
	*/
	WosTuple getNextTuple();
	/**
	has next WosTuple
	*/
	bool hasNextTuple();
	string extractDelimitedSubstring( char c, string str, int index );

 private:

    vector<WosTuple*> wosTuples;  //used for special case that tuples are passed in directly
    ROSWOSSegment* wosSegPtr;
    int m_idx;                  //used to track current idx in this iterator.

    bool useFile;
    ifstream* infile;
    string line;
    int numCol;
    
    bool useWOS;
    WosTuple* currentTuple;

};

#endif // __WosTupleIter_H
