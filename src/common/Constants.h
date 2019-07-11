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
#ifndef CONSTANTS_H
#define CONSTANTS_H

#pragma once

#define RLE_TYPE 1
#define DELTA_POS_TYPE 2
#define INT_SORT_TYPE 3
#define INT_UNSORT_TYPE 4

#define TYPE_INTEGER 1
#define TYPE_FLOAT 2
#define TYPE_STRING 3
#define TYPE_STAR 4

//#define MATERIAL_EXPS 1
//#define MINICSTORE 1

typedef unsigned char byte;
static const int PAGE_SIZE=65000;
#define NULLINT (1<<31)
#define NULLBYTE (1<<7)
//static const int PAGE_SIZE=32;

#include <cassert>
#include <cstddef>
#include <iostream>
#include <string>

#define INT_VALUE_TYPE 1

#define COMPRESSION_TYPE1 1
#define COMPRESSION_TYPE1A 2
#define COMPRESSION_TYPE2 3
#define COMPRESSION_TYPE3 4
#define COMPRESSION_TYPE3A 5
#define COMPRESSION_TYPE4 6

#define BDB_PAGE_SIZE 65536
//#define CACHE_SIZE 104857600  // 100Mb for now
#define CACHE_SIZE 1048576  // 10Mb for now
#define SMALL_CACHE_SIZE 5242880   // 5Mb for now
#define FILE_DELIMITER '|'
#define HEADER_SIZE 8
#define PRIMARY    0
#define SECONDARY  1
#define TERTIARY   2
#define QUATERNARY 3

#define PRIMARY_SUFF    ""
#define SECONDARY_SUFF  ".secondary"
#define TERTIARY_SUFF   ".tertiary"
#define QUATERNARY_SUFF ".quaternary"

#define TEMP_SORTING_DB        "sorted_temp_db"

#define DEBUG false         // output debug info
//#define BUILD_TABLES false;

#define RUNTIME_DATA "../RuntimeData/"

//#define FORCE_REBUILD false

#define NUM_INTS_IN_LZOP 0

//system tables(catalog) file names
#define CSTORE_SYSTABLE_PROJECTION  string("CstoreSysTblProjection.bdb")  //projection primary 
#define CSTORE_SYSTABLE_COLUMN      string("CstoreSysTblColumn.bdb")      //column primary
#define CSTORE_SYSTABLE_COLUMN_AK   string("CstoreSysTblColumnAK.bdb")   //column secondary (alt. key = projectinID)

#define TYPE2MAXVALUES 256

#endif // CONSTANTS_H

