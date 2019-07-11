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
 * @(#)TMMerger.h
 * Author: Cheng xuedchen@cs.umb.edu
 * Copyright by CSTORE
 */

#ifndef __TMMerger_H
#define __TMMerger_H

#include <db_cxx.h>
#include <vector>
#include <map>
#include <algorithm>
#include <sstream>

#include "../Util/StopWatch.h"
#include "../common/Constants.h"
#include "../common/Interfaces.h"
#include "../AM/CatalogInstance.h"
#include "../AM/AM.h" //use Access Manager modified IntegerColumn
#include "../AM/PageWriter.h"
#include "../AM/PagePlacer.h"
#include "../common/Block.h"
#include "../Wrappers/RLEBlock.h"
#include "../Wrappers/BasicBlock.h"
#include "../Wrappers/DeltaPosBlock.h"
#include "../Wrappers/RLETriple.h"
#include "../Wrappers/Encoder/Encoder.h"
#include "../Wrappers/Decoder/Decoder.h"
#include "../Wrappers/Encoder/IntEncoder.h"
#include "../Wrappers/Decoder/IntDecoder.h"
#include "../Wrappers/Encoder/RLEEncoder.h"
#include "../Wrappers/Decoder/RLEDecoder.h"
#include "../Wrappers/Encoder/DeltaPosEncoder.h"
#include "../Wrappers/Decoder/DeltaPosDecoder.h"
#include "../Wrappers/RLEDataSource.h"
#include "../Wrappers/CodingException.h"
#include "../Wrappers/RLEVecDataSource.h"  //newly created RLE data source based on vector
#include "../Wrappers/DeltaPosDataSource.h"
#include "../Wrappers/IntDataSource.h"


#include "WosTuple.h"  //con
#include "WosTupleIter.h"  //tuples iterator


#define BDBTMPTABLEPATH "./RuntimeData/"
#define MergeSuffix ".ros"
#define MergePrefix "MO_"
#define TMDEBUG false
#define MAXCOLUMNS 8

/**
 *Tuple mover Merger for merge WOS and ROS correspondence projection(segament).
 *column index is 0-based.
 *projection is sorted multiple sort-keys, and sort-key columns are as of
 *type1/type1A/type1A format
 */

using namespace std;

class TMMerger
{
  
 public:
  
  struct Bound{
    int low;
    int high;
  };
  
  /**
   * Constructs a TupleMoverMerger
   * in order to handle multiple sort keys all sort-key columns should be passed in in order
   * and have an int value lastSortkeyIdx to indicate the last sort-key column index.
   * also the colNames_ is passed in to be used to generate the final mergeout bdb table file name
   * there final mergeout bdb table file name will have _MO suffix
   *
   */
  TMMerger(ROSAM** ams, int lastSortkeyIdx, vector<int> colTypes, WosTupleIter* tuplesIter, vector<string>* colNames_);
  
  /**
   * Constructs a TupleMoverMerger
   * Thanks to CatalogInstance. I can get all my need by giving just projection name and the CatalogInstance pointer
   */
  TMMerger(CatalogInstance* ci, string projectionName);
  
  /**
   * Destructs this TupleMoverMerger.
   */
  ~TMMerger();
  
  /**
     Do the merge process described by Tuple Mover Merge Out proposal.
  */
  void doMerge();
  void printMergedCol(int idx, stringstream& outSS);
  void printProfile();

 private:
  double wosTupleTimer; //time recorder for geting all tuples from wos;
  
  vector<double> timer; //time recorder for each column merge out

  vector<DataSource*> m_ics; //holding columns

  vector<int> m_colTypes;  //since new design AM has no idea about encoding type, have to stored separately
  vector<string>* m_columnNames;  //holding column name so that we can refer to BDB table

  /**
   *The tuple iterator is constructed from WOS Based on WOS's sort-key-storage-key we can reform
   *tuples in sort-key col order. Detailed inplementation will based on WOS implementation
   */
  WosTupleIter *m_wosTupleIter;  
  WosTuple tuple;


 /**
  *Based on current type2 implementation
  *each col value of type2 correspond to a BDB table and thus an AM
  *thus for multiple type2 cols we need a nested vector similar to 2 dimensional array
  *Please refer to DeltaPosDataSource for details.
  */
  
  map<int,vector<DeltaPosBlock*> >  deltaPosBlockMap;

  map<int,BasicBlock*> nextIntBlockMap;
  map<int,IntEncoder*> moIntEncoderMap;
  map<int,PageWriter*> moIntPageWriterMap;


  //fields that used to handle type2 columns
  map<int,vector<int>*> colsVals;  //for type2 columns

  map<int,vector<int>*> colsNewVals; //for type2 columns especially the new values in WS than RS
  map<int,vector<int>*> nextVals; 
  
  
  //fields for handling type1(RLE) column 
  vector<RLETriple>** sortKeyTriples;  
  RLETriple** nextRSTriplePtr; //  holds the next triple value from RS.
  RLETriple** moTriplePtr;     // holds the current merge out triple 
  
  int m_numInsert; //record number ws tuples have been merged 
  int m_insertIndex; //indicate the current inserting WS tuple final position on mergeout

  bool firstRun;
  Bound prevBound;

  //general fields about projection
  int m_lastSortkeyIdx; //last sort-key column index
  int m_numCols;    //number of column

  int lastPosition; //for all non-sort key column use this and m_insertIndex to determine those values in ROS to be merged



  // Methods
	

  /*after constructor takes require params
    init is called to set up BDB tables for write out merge out
    and initialized the class fields such as those maps and vectors
    delegate to initType1Col - initType4Col as necessary.
  */
  void init();

  /*
   *initializer for type1-4 column mergeout ROS wrapper, writer, and BDB table
   */

  void initDELTA_POS_Col(int);
  void initINT_SORT_Col(int);
  void initINT_UNSORT_Col(int);

  
  ///// handling multiple sort keys 
  void moNextWSTupleMultiSortkeyCols();
  //During prefix sort key columns processing, if there is prefix match case then it will determine the
  //bound for processing next column. Obviously for all those RS triples before the bound, just mergeout(mo) by 
  //pushing them to sortKey Triples vector 

  void checkNextSortkeyIntColumn(int idx);
  void moTriplesBeforeBound(int sortColumnIndex);

  //Within a bound push all triples that have value smaller than current WS tuple column value
  void moSmallerTriplesInBound(int sortColumnIndex);

  //Prefix sort-key column already determine the insert position thus for consequent columns
  //simply merge out triples before the insert position
  void moTriplesBeforeInsertPosition(int sortColumnIndex);

  //prefix sort key column match determine the consequent columns merge out look up bound
  void prefixMatchSetup(int sortColumnIndex);
  void prefixNoMatchSetup(int sortColumnIndex);

  ////----------------------------

  void moNextWSTupleIntSortkeyCol(); //type3 sort key column merge


  /**
     Process merge on cols in Type2 encoding:
  */
  void moNextWSTupleDeltaPosCol(int idx, int value);
  
  /**
   * Process merge on cols in Type4 encoding:
  */
  void moNextWSTupleIntCol(int idx, int value);
  
  
  /**
     do finish up if no more tuple to be merged
     it will delegate to column finish up methods
  */
  void finishMerge();
  
  /**
   */
  void finishSortkeyCol();

  /**
   */
  void finishMoWSMultiSortkeyCols();
  void finishMoWSIntSortkeyCol();
  /**
   */
  void finishMoWSDeltaPosCol(int idx);
  
  /**
   */
  void finishMoWSIntCol(int idx);
  
};

#endif // __TMMerger_H
