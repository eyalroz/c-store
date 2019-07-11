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
 * @(#)TMMerger.cpp
 * Author: Cheng xuedchen@cs.umb.edu
 * Copyright by CSTORE
 */

#include <db_cxx.h>
#include <string>
#include <vector>
#include <fstream>


#include "TMMerger.h"

/**
 *Simple version of constructor with only CatalogInstance and projectionName string 
 *Thanks to CatalogInstance from which we can extract enough info from it
 *assuming the projection sortkey and column ordering is consisitent in WOS tuples
 *and ROS column sequence. 
 **/
TMMerger::TMMerger(CatalogInstance* ci, string projectionName){
        m_lastSortkeyIdx = ci->getSortOrderColumns(projectionName)->size()-1;
        m_columnNames = ci->getColumnNamesInOrder(projectionName);
        m_numCols = m_columnNames->size();   
        if(m_numCols >MAXCOLUMNS){
                m_numCols = MAXCOLUMNS;
        }
        
        cout << "lastSortkeyIdx:" << m_lastSortkeyIdx << endl; 
        for(int i=0; i<m_numCols; i++){
                string colName = (*m_columnNames)[i];
                cout << "column name: " << colName << " type: " << ci->getEncodingType(projectionName,colName) << endl;
                m_colTypes.push_back(ci->getEncodingType(projectionName, colName));
                DataSource* ds;
                switch(m_colTypes[i]){
                case  COMPRESSION_TYPE1:
                        //if RLE columns is the first sort-key column then it is sorted
                        ds = new RLEDataSource(ci->getROSAM(projectionName,colName), i==0);
                        break;
                case COMPRESSION_TYPE2: //ie. type2
                        ds = new DeltaPosDataSource(ci->getROSAM(projectionName,colName),false);
                        break;
                case COMPRESSION_TYPE3:  //ie. type3
                        ds = new IntDataSource(ci->getROSAM(projectionName,colName),true,true);
                        ((IntDataSource*)ds)->setSingles(true);
                        break;
                case  COMPRESSION_TYPE4: //ie. type4
                        ds = new IntDataSource(ci->getROSAM(projectionName,colName),false,true);
                        ((IntDataSource*)ds)->setSingles(true);
                        break;
                default :
                        ds = new DataSource(ci->getROSAM(projectionName,colName));
                        break;
                }
                m_ics.push_back(ds);   //hold AM in a vector
        }
        
        m_wosTupleIter = new WosTupleIter(ci->getWOSManager(projectionName)->getROSWOSSegment(),m_numCols);
        
        
        init();  //initial setup

}


/**
 *TupleMover merger takes an array of AM, WosTuple iterator as well encoding type info of columns of given projection 
 *The projection has to be in some sort order (no foreign order).
 *The sequence of columns have to be arranged as following: 
 *   Suppose there are 5 columns (ColA, ColB, ColC, ColD, ColE) whereas sortkeys are ColA+ColE+ColB
 *   then AM array has to be in order as (ColA,ColE,ColB, followed by (ColC,ColD)|(ColD,ColC))
 *   The encoding of above column should generally be:
 *      ColA: COMPRESSION_TYPE1(SORT)
 *      ColE: COMPRESSION_TYPE1(UNSORT) : allowing repeating triples for same column value
 *      ColB: COMPRESSION_TYPE1(UNSORT) or  COMPRESSION_TYPE4(it is rare, but our current query need this support)
 *      ColC and ColD:  COMPRESSION_TYPE4
 *   (The above example show that the strict prefix sort key columns must be in COMPRESSION_TYPE1, the last sort key column
 *    could be  COMPRESSION_TYPE4)
 *   
 *   For projection with multiple sort keys, sort key columns are rarely encoded as COMPRESSION_TYPE3 (type3).
 *   For projection with only one sort key, the sort key could be either COMPRESSION_TYPE1 or COMPRESSION_TYPE3
 *   We can determine whether the projection has multiple sort keys by checking the lastSortkeyIdx parameter passed in
 *   during the construction.
 *   
 *   in order to use the bdb table for each column, we have to give the explicit name for each column       
 */


TMMerger::TMMerger(ROSAM** ams,int lastSortkeyIdx, vector<int> colTypes, WosTupleIter* tuplesIter, vector<string>* columnNames){
        
        m_lastSortkeyIdx = lastSortkeyIdx;
        m_colTypes = colTypes;
        m_numCols = columnNames->size();   
        
        for(int i=0; i<m_numCols; i++){
                DataSource* ds;
                switch(m_colTypes[i]){
                case COMPRESSION_TYPE1:
                        ds = new RLEDataSource(ams[i], i==0);  //if RLE columns is the first sort-key column then it is sorted
                        break;
                case COMPRESSION_TYPE2: //ie. type2
                        ds = new DeltaPosDataSource(ams[i],false);
                        break;
                case COMPRESSION_TYPE3:  //ie. type3
                        ds = new IntDataSource(ams[i],true,true);
                        ((IntDataSource*)ds)->setSingles(true);
                        break;
                case COMPRESSION_TYPE4: //ie. type4
                        ds = new IntDataSource(ams[i],false,true);
                        ((IntDataSource*)ds)->setSingles(true);
                        break;
                default :
                        ds = new DataSource(ams[i]);
                        break;
                }
                m_ics.push_back(ds);   //hold AM in a vector
        }
        
        m_wosTupleIter = tuplesIter;
        m_columnNames = columnNames;
        
        init();  //initial setup
}

/**
 * Destructs this TupleMoverMerger.
 */
TMMerger::~TMMerger(){
        
        //delete maps, need to clean up memory pointed by map elements also
        //Todo: loop throug map clean up pointers
        int i=0;
        /*
        for(i=m_lastSortkeyIdx+1; i<m_numCols; i++){
                if(deltaPosBlockMap[i]!=NULL){
                        for(j=0; j< deltaPosBlockMap[i].size(); i++){
                                delete deltaPosBlockMap[i][j];
                        }
                }
        }
        */

        
        for(i=1; i<m_numCols; i++){
                if(nextIntBlockMap[i]!=NULL){
                        delete nextIntBlockMap[i];
                }
        }
        
        for(i=1; i<m_numCols; i++){
                if(moIntEncoderMap[i]!=NULL){
                        delete moIntEncoderMap[i];
                }
        }
        
        
        for(i=1; i<m_numCols; i++){
                if(moIntPageWriterMap[i]!=NULL){
                        delete moIntPageWriterMap[i];
                }
        }
        if(sortKeyTriples!=NULL){
                delete [] sortKeyTriples;
        }

        if(nextRSTriplePtr!=NULL){
                /*
                  for(i=0; i<=m_lastSortkeyIdx ; i++){
                  delete nextRSTriplePtr[i];
                  }
                */
                delete [] nextRSTriplePtr;
        }

        if(moTriplePtr!=NULL){
                
                for(i=0; i<1; i++){
                        if(moTriplePtr[i] != NULL)
                                delete moTriplePtr[i];
                }
                
                delete [] moTriplePtr;
        }

        
        /*
        i=0; j=0;
        for(i=m_lastSortKeyIdx+1; i<m_numCols; i++){
                if(colsVals[i]!=NULL){
                        delete colsVals[i];
                }
        }

        i=0; j=0;
        for(i=m_lastSortKeyIdx+1; i<m_numCols; i++){
                if(colsNewVals[i]!=NULL){
                        delete colsNewVals[i];
                }
        }
        */
       
}

/*init the field vars especially the wrappers, writers, BDB tables
 *
 */
void TMMerger::init(){
        //fields must be initialized
        m_numInsert =0;
        lastPosition = 0;
        firstRun = true; 

        for(int t=0; t< m_numCols; t++){
                timer.push_back(0);
        }
        wosTupleTimer = 0;
        
        //for all non-sort-key colums, do initialization
        int i=0;

        //handle special case that the last sort key column is in type4 encoding
        int lastRLEColIndex = m_lastSortkeyIdx;
        if(m_colTypes[m_lastSortkeyIdx] ==  COMPRESSION_TYPE4){
                lastRLEColIndex=m_lastSortkeyIdx-1;
        }

        for(i=lastRLEColIndex+1; i<m_numCols; i++){            
                switch(m_colTypes[i]){
                case COMPRESSION_TYPE2:
                        initDELTA_POS_Col(i);
                        break;
                case  COMPRESSION_TYPE4:
                        initINT_UNSORT_Col(i);
                        break;
                default:
                        cout <<"!!!can't process it due to unknown type: " << m_colTypes[i] <<endl;
                        exit(-1);
                }
        }

        
        //for all sort-key columns do initialization
        //Two cases: 1. only one sort-key column with many values in COMPRESSION_TYPE3 encoding
        //           2. First sort-key in COMPRESSION_TYPE1(SORT) and others are in COMPRESSION_TYPE1(NON_SORT) encoding or  COMPRESSION_TYPE4
        //              for the last key column.
        
        if(m_lastSortkeyIdx ==0 && m_colTypes[m_lastSortkeyIdx]==COMPRESSION_TYPE3) return; //no special setup for COMPRESSION_TYPE3 
        
        //special setup for COMPRESSION_TYPE1(SORT/UNSORT) columns
        //Below are creating pointer arrays. To make compiler happy, I use int array and then cast it
        //The trick is that pointer has some size as int.
        
        nextRSTriplePtr = (RLETriple **) (new int[m_lastSortkeyIdx+1]);
        moTriplePtr = (RLETriple **) (new int[m_lastSortkeyIdx+1]);
        sortKeyTriples = (vector<RLETriple>**)(new int[m_lastSortkeyIdx+1]);
        
        
        for(i=0; i<=lastRLEColIndex; i++){
                //sort key in type1 or type1A
                
                //just have nextRSTriplePtr points to the first RS triples for each sort-key column, null means end of RS
                RLEBlock* rleBlk = (RLEBlock*)((RLEDataSource*)m_ics[i])->getNextValBlock(0); 
                
                if(rleBlk != NULL){
                        nextRSTriplePtr[i] = rleBlk->getTriple();
                }else{
                        nextRSTriplePtr[i] = NULL;
                }
                
                //no merge out triples yet
                moTriplePtr[i] = NULL;
                
                //set up the triple vectors for each sort-key column
                sortKeyTriples[i] = new vector<RLETriple>; 
        }
        
}

void TMMerger::initDELTA_POS_Col(int i){
        //Todo: 
}

void TMMerger::initINT_UNSORT_Col(int i){
        nextIntBlockMap[i] = (BasicBlock*)m_ics[i]->getNextValBlock(0);

        moIntEncoderMap[i] = new IntEncoder(NULL, 0, 8*PAGE_SIZE);
        moIntPageWriterMap[i]=new PageWriter(1, sizeof(int), 0, ValPos::INTTYPE, ValPos::NOTYPE);

        //string mo_bdbTable = (*m_columnNames)[i]+MergeSuffix;
        
        string mo_bdbTable = MergePrefix + (*m_columnNames)[i] + MergeSuffix;
        
        mo_bdbTable = BDBTMPTABLEPATH + mo_bdbTable;
        system(("rm -f " + mo_bdbTable).c_str());
        
        moIntPageWriterMap[i]->initDB(mo_bdbTable, false);	

}



//   Do the merge process described by Tuple Mover Merge Out proposal.
void TMMerger::doMerge(){
        while(m_wosTupleIter->hasNextTuple()){
                StopWatch wosWatch;
                wosWatch.start();

                tuple = m_wosTupleIter->getNextTuple();

                wosTupleTimer += wosWatch.quietStop();
                
                moNextWSTupleMultiSortkeyCols();

                int lastRLEColIndex = m_lastSortkeyIdx;
                if(m_colTypes[m_lastSortkeyIdx] ==  COMPRESSION_TYPE4){
                        lastRLEColIndex=m_lastSortkeyIdx-1;
                }
                
                for(int i=lastRLEColIndex+1; i<m_numCols; i++){
                        StopWatch swatch;
                        swatch.start();
                        if(m_colTypes[i] == COMPRESSION_TYPE2){
                                moNextWSTupleDeltaPosCol(i,tuple.getVal(i));
                                
                        }else if(m_colTypes[i] ==  COMPRESSION_TYPE4){
                                if(i!=1 || m_lastSortkeyIdx != 1){
                                        moNextWSTupleIntCol(i,tuple.getVal(i));
                                }
                                
                        }else if(m_colTypes[i] == COMPRESSION_TYPE3){
                                //to be supported
                        }else{
                                cout <<" can't process it due to unknown type" <<endl;
                        }
                        timer[i] += swatch.quietStop();
                }
                
                //increase the m_numInsert
                m_numInsert ++;
                lastPosition = m_insertIndex;
        }
        
        finishMerge();
  
}

void TMMerger::printProfile(){
        cout << "Mergeout detailed profile (user time)->>" << endl;
        double totalTime = 0;
        cout << "\tNumber of inserts: " << m_numInsert << endl;
        cout << "\tWOS tuple retrieval time: " << wosTupleTimer << " second" << endl;
        cout << "\tCost per wos tuple: " << (wosTupleTimer*(double)1000)/m_numInsert << " ms" << endl;
        totalTime +=wosTupleTimer;
        cout << "\tBreakdown mergeout time by column: \n";

        for(int i=0; i< m_numCols; i++){
                cout << "\t\tcol " << i <<" : " << timer[i] << " second" << endl;
                totalTime += timer[i];
        }

        cout << "\tTotalTime: " <<totalTime <<" second" << endl;
        cout << "\tCost per wos tuple: " << (totalTime * (double)1000)/m_numInsert << " ms" << endl;
}


void TMMerger::moNextWSTupleMultiSortkeyCols(){

        bool foundInsertPos = false;
        
        //flag indicates that for each subsequent sort key columns do simple
        
        //insert with previous calculated m_insertIndex.
        
        prevBound.low = 1;
        prevBound.high = 1000000000;  //MAXIMUM get from catalog's info about project rows
        
        if(firstRun){
                firstRun = false;
                //init all merge out triples as (0,0,0)
                /*
                  for(int i=0; i<=m_lastSortkeyIdx; i++){
                        moTriplePtr[i] = new RLETriple(1,1,1);
                        }
                */
                //handle each sort key column subsequently
                for(int i=0; i<=m_lastSortkeyIdx; i++){
                        StopWatch swatch;
                        swatch.start();
                        if(i==m_lastSortkeyIdx && m_colTypes[i]== COMPRESSION_TYPE4){
                                //last column is an unsort int column
                                if(foundInsertPos){
                                        moNextWSTupleIntCol(i, tuple.getVal(i));                                        
                                }else{
                                       
                                         checkNextSortkeyIntColumn(i);
                                        foundInsertPos = true;
                                        
                                }
                        }else{
                                if(foundInsertPos){ //means during merge sort key prefix, we can already determine
                                        //the final insert position. So for the subsequent columns we can
                                        //simply merge out all RS triples up to m_insertIndex position
                                        
                                        moTriplesBeforeInsertPosition(i);
                                        
                                        //continue for subsequent sort key columns
                                }else{
                                        //prefix sort key columns merge can't determine the final insert position.
                                        //Then we try it here
                                        
                                        moTriplesBeforeBound(i);
                                        moSmallerTriplesInBound(i);
                                        
					assert(nextRSTriplePtr[i] == NULL || nextRSTriplePtr[i] ->value->type==ValPos::INTTYPE);
                                        if(nextRSTriplePtr[i] != NULL && (*(int*)(nextRSTriplePtr[i]->value->value)) == tuple.getVal(i)){
                                                //it is a prefix match, setup prevBound for the subsequent column
                                                
                                                prefixMatchSetup(i);
                                                if(i==m_lastSortkeyIdx){
                                                        //Be careful here the prefixMatchSetup will increase the reps
                                                        //so we need to -1 to adjust it.!!!!
                                                        m_insertIndex = moTriplePtr[i]->startPos + moTriplePtr[i]->reps -1;
                                                        foundInsertPos = true; //no subsequent columns
                                                }
                                        }else{
                                                
                                                prefixNoMatchSetup(i);
                                                foundInsertPos = true;  //for all subsequent columns do simple insert
                                                
                                        }
                                        //continue for subsequent sort key columns
                                        
                                }
                        }
                        timer[i] += swatch.quietStop();
                }
        }else{//not the first run
                //based on above first run code we can guarantee that moTriplePtr[i] won't be null
                
                for(int i=0; i<=m_lastSortkeyIdx; i++){
                        StopWatch swatch;
                        swatch.start();
                        
                        
                         if(i==m_lastSortkeyIdx && m_colTypes[i]== COMPRESSION_TYPE4){
                                  //last column is an unsort int column
                                if(foundInsertPos){
                                        moNextWSTupleIntCol(i, tuple.getVal(i));                                        
                                }else{
                                         checkNextSortkeyIntColumn(i);
                                        foundInsertPos = true;
                                        
                                }
                                 
                         }else{
                                 if(foundInsertPos){ //means during merge sort key prefix, we can already determine
                                         //the final insert position. So for the subsequent columns we can
                                         //simply merge out all RS triples up to m_insertIndex position
                                         
                                         sortKeyTriples[i]->push_back(*(moTriplePtr[i]));
                                         
                                         //next RS triple is the same as moTriple means they point to the same place
					 assert(moTriplePtr[i] ->value->type==ValPos::INTTYPE);
					 assert(nextRSTriplePtr[i] == NULL || nextRSTriplePtr[i] ->value->type==ValPos::INTTYPE);
                                         if(nextRSTriplePtr[i] !=NULL  &&
                                            *(int*)(moTriplePtr[i]->value->value) == *(int*)(nextRSTriplePtr[i]->value->value)){
                                                 RLEBlock* rleBlk = (RLEBlock*)((RLEDataSource*)m_ics[i])->getNextValBlock(0); 
                                                 nextRSTriplePtr[i] = rleBlk->getTriple();
                                                 
                                         }
                                         moTriplesBeforeInsertPosition(i);
                                         //continue for subsequent sort key columns
                                 }
                                 else{
				   
					assert(moTriplePtr[i]->value->type==ValPos::INTTYPE);
                                         if((*(int*)(moTriplePtr[i]->value->value)) == tuple.getVal(i) ){

                                                 moTriplePtr[i]->reps ++;
                                                 
                                                 prevBound.high = moTriplePtr[i]->startPos + moTriplePtr[i]->reps-1;
                                                 //increase its reps by all means
                                                 
                                                
                                                 if(i==m_lastSortkeyIdx){
                                                         m_insertIndex = moTriplePtr[i]->startPos + moTriplePtr[i]->reps-1;
                                                         foundInsertPos = true; //no subsequent columns
                                                 }
                                                 
                                         }else{
                                                 //due to WS tuples are sorted then we can make following assertion
                                                 
                                                 assert(tuple.getVal(i) > *(int*)(moTriplePtr[i]->value->value));
                                                 //need to push moTriplePtr[i]
                                                 if(TMDEBUG) cout << "mo value: " << moTriplePtr[i]->value
                                                      <<" startPos: " << moTriplePtr[i]->startPos
                                                      <<" reps: " << moTriplePtr[i]->reps;
                                                 sortKeyTriples[i]->push_back(*(moTriplePtr[i]));
                                                 
                                                 //if next RS triple is the same as moTriple, then they point to the same place
						 
						 assert(nextRSTriplePtr[i] == NULL || nextRSTriplePtr[i] ->value->type==ValPos::INTTYPE);
						 assert(moTriplePtr[i] ->value->type==ValPos::INTTYPE);
                                                 if(nextRSTriplePtr[i] !=NULL  &&
                                                    *(int*)(moTriplePtr[i]->value->value) == *(int*)(nextRSTriplePtr[i]->value->value)){
                                                         RLEBlock* rleBlk = (RLEBlock*)((RLEDataSource*)m_ics[i])->getNextValBlock(0); 
                                                         
                                                         if(rleBlk!= NULL){
                                                                 nextRSTriplePtr[i] = rleBlk->getTriple();
                                                                 
                                                         }else{
                                                                 nextRSTriplePtr[i] =NULL;
                                                         }
                                                         
                                                         
                                                 }
                                                 
                                                 moTriplesBeforeBound(i);
                                                 moSmallerTriplesInBound(i);
						 
						 assert(nextRSTriplePtr[i] == NULL || nextRSTriplePtr[i] ->value->type==ValPos::INTTYPE);
                                                 if(nextRSTriplePtr[i] !=NULL &&
                                                    tuple.getVal(i) == (*(int*)(nextRSTriplePtr[i] ->value->value))){
                                                         //it is a prefix match, setup prevBound for the subsequent column
                                                         prefixMatchSetup(i);
                                                         if(i==m_lastSortkeyIdx){
                                                                 m_insertIndex = moTriplePtr[i]->startPos + moTriplePtr[i]->reps-1;
                                                                 foundInsertPos = true; //no subsequent columns
                                                         }
                                                 }else{
                                                         prefixNoMatchSetup(i);
                                                         foundInsertPos = true;  //for all subsequent columns do simple insert
                                                         
                                                 }
                                                 //continue for subsequent sort key columns
                                         }
                                 }
                         }
                         timer[i] += swatch.quietStop();
                }
        }
}


void TMMerger:: checkNextSortkeyIntColumn(int i){

        int position=0,value=0;
        ValPos* pair=NULL;
        if(nextIntBlockMap[i] == NULL){
                position = prevBound.high +1;
                
        }else{
                pair = nextIntBlockMap[i]->peekNext();
                position =   pair->position + m_numInsert;
		assert(pair->type == ValPos::INTTYPE);
                value = *(int*)pair->value;
        }
        IntDecoder intDecoder(false);
        
        while(pair!=NULL &&
                 (position < prevBound.low  ||
                     (position< prevBound.high && value <= tuple.getVal(i))
             )){
                if(!moIntEncoderMap[i]->writeVal(value,position)){
                        
                        //page is full need to write to BDB
		  if(TMDEBUG) cout << " page is full -->write to BDB i = " << i << endl;

                        
                                byte* page = moIntEncoderMap[i]->getPage();

                                intDecoder.setBuffer(page);

				int temppos = intDecoder.getEndPos();
                                moIntPageWriterMap[i]->placePage((char*)page, (char*)&temppos);

                                //after getPage() buffer is reset, then try the write again
                                moIntEncoderMap[i]->writeVal(value, position);
                }
                nextIntBlockMap[i]->getNext(); //last time is just peek, should move forward
                
                if(nextIntBlockMap[i]->hasNext()){
                        
                        pair = nextIntBlockMap[i]->getNext();
                }else{
                        nextIntBlockMap[i] = (BasicBlock*)m_ics[i]->getNextValBlock(0);
                        if(nextIntBlockMap[i]!=NULL && nextIntBlockMap[i]->hasNext()){
                                pair = nextIntBlockMap[i]->peekNext();
                                position =   pair->position + m_numInsert;
				assert(pair->type == ValPos::INTTYPE);
                                value = *(int*)pair->value;
                        }else{
                                pair = NULL;
                                position++;
                        }
                }

        }


        m_insertIndex = position;

        if(!moIntEncoderMap[1]->writeVal(tuple.getVal(i), m_insertIndex)){
              
                //page is full, then need to write to BDB
                if(TMDEBUG) cout << " page is full -->write to BDB 1 = " << endl;

                
                byte* page = moIntEncoderMap[i]->getPage();
                
                //for some reason there is one extra position
                intDecoder.setBuffer(page);
                

		int temppos = intDecoder.getEndPos();
                //intDecoder.setBuffer(moIntEncoderMap[i]->getPage());
                moIntPageWriterMap[i]->placePage((char*)page, (char*)&temppos);


                //after getPage() buffer is reset, then try the write again
                moIntEncoderMap[i]->writeVal(tuple.getVal(i), m_insertIndex);
        }
}


//During prefix sort key columns processing, if there is prefix match case then it will determine the
//bound for processing next column. Obviously for all those RS triples before the bound, just mergeout(mo) by
//pushing them to sortKey Triples vector
void TMMerger::moTriplesBeforeBound(int sortColumnIndex)
{
        //sortColumnIndex is the 0 based index for  current column position on sort keys sequence
        //If sort keys sequence is (col2,col1, col4) the col4's sortColumnIndex should be 2.
        int i=sortColumnIndex;
	//assume int type for now ...
	ValPos* vp = new IntValPos();
        while(nextRSTriplePtr[i] !=NULL &&
              ((int)nextRSTriplePtr[i]->startPos +m_numInsert) <prevBound.low){
                
		assert(nextRSTriplePtr[i]->value->type == ValPos::INTTYPE);
                int value = *(int*)(nextRSTriplePtr[i]->value->value);
                //need to update the startPosition since there are some insertions
                int startPos = nextRSTriplePtr[i]->startPos + m_numInsert;
                int reps = nextRSTriplePtr[i]->reps;

                //modified to save memory
                if(moTriplePtr[i]!=NULL){
		  moTriplePtr[i]->value->set((byte*)&value);
		    //moTriplePtr[i]->value = value;
                        moTriplePtr[i]->startPos = startPos;
                        moTriplePtr[i]->reps = reps;
                }else{
		  vp->set((byte*)&value);
                        moTriplePtr[i]  = new RLETriple(vp,startPos,reps);
                }
                if(TMDEBUG) cout << "mo value: " << moTriplePtr[i]->value
                     <<" startPos: " << moTriplePtr[i]->startPos
                     <<" reps: " << moTriplePtr[i]->reps;
                
                
                sortKeyTriples[i]->push_back(*(moTriplePtr[i]));
                RLEBlock* rleBlk = (RLEBlock*)((RLEDataSource*)m_ics[i])->getNextValBlock(0);


                if(rleBlk!=NULL){
                        nextRSTriplePtr[i] = rleBlk->getTriple();
                }else{
                        nextRSTriplePtr[i] = NULL;
                }

        }
	delete vp;
}

//Within a bound push all triples that have value smaller than current WS tuple column value
void TMMerger::moSmallerTriplesInBound(int sortColumnIndex)
{
        //sortColumnIndex is the 0 based index for  current column position on sort keys sequence
        //If sort keys sequence is (col2,col1, col4) the col4's sortColumnIndex should be 2.
        int i=sortColumnIndex;
	//assume int type for now ...
	ValPos* vp = new IntValPos();
	assert(nextRSTriplePtr[i] == NULL || nextRSTriplePtr[i]->value->type == ValPos::INTTYPE);
        while(nextRSTriplePtr[i] !=NULL &&
              tuple.getVal(i) > *(int*)(nextRSTriplePtr[i]->value->value) &&
              (int) nextRSTriplePtr[i]->startPos <= prevBound.high )
                {
                        int value = *(int*)(nextRSTriplePtr[i]->value->value);
                        //need to update the startPosPosition since there are some insertions
                        int startPos = nextRSTriplePtr[i]->startPos + m_numInsert;
                        int reps = nextRSTriplePtr[i]->reps;

                        //modified to save memory
                        if(moTriplePtr[i]!=NULL){
			  moTriplePtr[i]->value->set((byte*)&value);
			  //moTriplePtr[i]->value = value;
                                moTriplePtr[i]->startPos = startPos;
                                moTriplePtr[i]->reps = reps;
                        }else{
			  vp->set((byte*)&value);
                                moTriplePtr[i]  = new RLETriple(vp,startPos,reps);
                        }
                        cout << "mo value: " << moTriplePtr[i]->value
                             <<" startPos: " << moTriplePtr[i]->startPos
                             <<" reps: " << moTriplePtr[i]->reps;
                        
                        
                        sortKeyTriples[i]->push_back(*(moTriplePtr[i]));
                        RLEBlock* rleBlk = (RLEBlock*)((RLEDataSource*)m_ics[i])->getNextValBlock(0); 
                        if(rleBlk!=NULL){
                                nextRSTriplePtr[i] = rleBlk->getTriple();
                        }else{
                                nextRSTriplePtr[i] = NULL;
                        }
                }
	delete vp;
}


//Prefix sort-key column already determine the insert position thus for subsequent columns
//simply merge out triples before the insert position
void TMMerger::moTriplesBeforeInsertPosition(int sortColumnIndex){
        int i=sortColumnIndex;
	//assume int type for now ...
	ValPos* vp = new IntValPos();
	assert(nextRSTriplePtr[i] == NULL || nextRSTriplePtr[i]->value->type == ValPos::INTTYPE);
        while(nextRSTriplePtr[i] !=NULL &&
              (int)nextRSTriplePtr[i]->startPos + m_numInsert < m_insertIndex){
                nextRSTriplePtr[i]->startPos +=m_numInsert;

                cout << "mo value: " << nextRSTriplePtr[i]->value
                     <<" startPos: " << nextRSTriplePtr[i]->startPos
                     <<" reps: " << nextRSTriplePtr[i]->reps;
                
                sortKeyTriples[i]->push_back(*(nextRSTriplePtr[i]));
                RLEBlock* rleBlk = (RLEBlock*)((RLEDataSource*)m_ics[i])->getNextValBlock(0); 
                if(rleBlk!=NULL){
                        nextRSTriplePtr[i] = rleBlk->getTriple();
                }else{
                        nextRSTriplePtr[i] = NULL;
                }
        }
        
        //also set up last moTriple
        int value = tuple.getVal(i);
        int startPos = m_insertIndex;
        int reps = 1;

        //modified to save memory
        if(moTriplePtr[i]!=NULL){
	  moTriplePtr[i]->value->set((byte*)&value);
	  //moTriplePtr[i]->value = value;
                moTriplePtr[i]->startPos = startPos;
                moTriplePtr[i]->reps = reps;
        }else{
	  vp->set((byte*)&value);
                moTriplePtr[i]  = new RLETriple(vp,startPos,reps);
        }
	delete vp;
        
}
//prefix sort key column match, used to determine the next sort key column merge out look up bound
void TMMerger::prefixMatchSetup(int sortColumnIndex){
        int i=sortColumnIndex;
	//assume int type for now ...
	ValPos* vp = new IntValPos();
	assert(nextRSTriplePtr[i]->value->type == ValPos::INTTYPE);
        int value = *(int*)(nextRSTriplePtr[i]->value->value);
        int startPos = nextRSTriplePtr[i]->startPos + m_numInsert;
        int reps = nextRSTriplePtr[i]->reps;

        //modified to save memory
        if(moTriplePtr[i]!=NULL){
	  moTriplePtr[i]->value->set((byte*)&value);
	  //moTriplePtr[i]->value = value;
                moTriplePtr[i]->startPos = startPos;
                moTriplePtr[i]->reps = reps;
        }else{
	  vp->set((byte*)&value);
                moTriplePtr[i]  = new RLETriple(vp,startPos,reps);
        }

        moTriplePtr[i]->reps++;   //increase its repsetition by all means
        
        //prepare for the next sort key column look up
        prevBound.low = moTriplePtr[i]->startPos;
        prevBound.high = moTriplePtr[i]->startPos + moTriplePtr[i]->reps -1;
	delete vp;
}

void TMMerger::prefixNoMatchSetup(int sortColumnIndex){
        int i=sortColumnIndex;
	//assume int type for now ...
	ValPos* vp = new IntValPos();
        //three cases:
        //1.nextRS triple read is out of range i.e. > prevBound.high
        //2.nextWSVal[i] < nextRSTriple[i]->value
        //nextRS triple is null
        //They all have the same result.

        int value = tuple.getVal(i);
        int startPos =1;
        if(moTriplePtr[i]!=NULL){
                startPos = moTriplePtr[i]->startPos + moTriplePtr[i]->reps;
        }
        int reps = 1;

        //modified to save memory
        if(moTriplePtr[i]!=NULL){
	  moTriplePtr[i]->value->set((byte*)&value);
	  //moTriplePtr[i]->value = value;
                moTriplePtr[i]->startPos = startPos;
                moTriplePtr[i]->reps = reps;
        }else{
	  vp->set((byte*)&value);
                moTriplePtr[i]  = new RLETriple(vp,startPos,reps);
        }
        
        //we claim that we find the final insert position
        m_insertIndex = moTriplePtr[i]->startPos;
	delete vp;
}


void TMMerger::moNextWSTupleIntSortkeyCol(){
        //int value = tuple.getVal(0);
        //Todo: add support for type3
}
	
/**
   Process merge on cols in Type2 encoding:
*/
void TMMerger::moNextWSTupleDeltaPosCol(int idx, int value){
        //Todo:
}

/**
   do type4 column merge
*/

void TMMerger::moNextWSTupleIntCol(int i, int value){
        IntDecoder intDecoder(false);
        ValPos* pair=NULL;
        for(int k=lastPosition+1; k < m_insertIndex && nextIntBlockMap[i]!=NULL;k++ ){
                if(nextIntBlockMap[i]->hasNext()){
                        pair = nextIntBlockMap[i]->getNext();
			assert(pair->type == ValPos::INTTYPE);

                }else{
                        nextIntBlockMap[i] = (BasicBlock*)m_ics[i]->getNextValBlock(0);
                        if(nextIntBlockMap[i]!=NULL && nextIntBlockMap[i]->hasNext()){
                        
                                pair = nextIntBlockMap[i]->getNext();
				assert(pair->type == ValPos::INTTYPE);
                                
                        }else{
                        
                                pair = NULL;
                        }
                }
                if(pair!=NULL && !moIntEncoderMap[i]->writeVal(*(int*)pair->value, k)){
                        
                                   //page is full need to write to BDB
		  if(TMDEBUG) cout << " page is full -->write to BDB i= " << i << endl;
                                   
                        
                                   byte* page = moIntEncoderMap[i]->getPage();
                                   
                                   //for some reason there is one extra position
                                   intDecoder.setBuffer(page);
                                   
                                   //            intDecoder.setBuffer(moIntEncoderMap[i]->getPage());
				   int temppos = intDecoder.getEndPos();
                                   moIntPageWriterMap[i]->placePage((char*)page, (char*)&temppos);

                                   //after getPage() buffer is reset, then try the write again
                                   moIntEncoderMap[i]->writeVal(*(int*)pair->value,k);
                }
                
                
        }
        
        //if(TMDEBUG) cout << "insert col- " << i << ": (" << value << ", " << m_insertIndex << ")" <<endl;
        if(!moIntEncoderMap[i]->writeVal(value, m_insertIndex)){
              
                //page is full, then need to write to BDB
	  if(TMDEBUG) cout << " page is full -->write to BDB i= " << i << endl;

                
                byte* page = moIntEncoderMap[i]->getPage();
                
                //for some reason there is one extra position
                intDecoder.setBuffer(page);
                

		int temppos = intDecoder.getEndPos();
                //intDecoder.setBuffer(moIntEncoderMap[i]->getPage());
                moIntPageWriterMap[i]->placePage((char*)page, (char*)&temppos);

                //after getPage() buffer is reset, then try the write again
                moIntEncoderMap[i]->writeVal(value, m_insertIndex);
        }
}

/**
   do finish up when no more WS tuple
*/
void TMMerger::finishMerge(){

        if(m_colTypes[0]==COMPRESSION_TYPE1){
                finishMoWSMultiSortkeyCols();
        }else{
                finishMoWSIntSortkeyCol();
        }
        

        int lastRLE = m_lastSortkeyIdx;
        if(m_colTypes[m_lastSortkeyIdx]==  COMPRESSION_TYPE4){
                lastRLE -=1;
        }
        
        for(int i=lastRLE+1; i<m_numCols; i++){

                 StopWatch swatch;
                 swatch.start();
        
                if(m_colTypes[i] == COMPRESSION_TYPE2){
                        finishMoWSDeltaPosCol(i);
                        
                }else if(m_colTypes[i] == COMPRESSION_TYPE4){
                        
                        finishMoWSIntCol(i);
                        
                }else{
                        cout <<"unknown type" <<endl;
                }
                timer[i] += swatch.quietStop();
        }
}


void TMMerger::finishMoWSIntSortkeyCol(){
        //Todo: support type3
}

void TMMerger::finishMoWSMultiSortkeyCols(){
        int lastRLE = m_lastSortkeyIdx;
        if(m_colTypes[m_lastSortkeyIdx]==  COMPRESSION_TYPE4){
                lastRLE -=1;
        }

        for(int i=0; i<=lastRLE; i++){
                StopWatch swatch;
                swatch.start();

                if(moTriplePtr[i] != NULL){
                        sortKeyTriples[i]->push_back(*(moTriplePtr[i]));
			assert(nextRSTriplePtr[i] == NULL || nextRSTriplePtr[i]->value->type == ValPos::INTTYPE);
			assert(moTriplePtr[i]->value->type == ValPos::INTTYPE);
                        if(nextRSTriplePtr[i]!=NULL &&
                           *(int*)(moTriplePtr[i]->value->value) == *(int*)(nextRSTriplePtr[i]->value->value)){
                                //mean last ros triple pointer need to advance
                                RLEBlock* rleBlk = (RLEBlock*)((RLEDataSource*)m_ics[i])->getNextValBlock(0);   
                                if(rleBlk != NULL){
                                        nextRSTriplePtr[i] = rleBlk->getTriple();
                                }else{
                                        nextRSTriplePtr[i] = NULL;
                                }
                        }
                }
                
                while(nextRSTriplePtr[i]!=NULL){
                        nextRSTriplePtr[i]->startPos += m_numInsert;

                        sortKeyTriples[i]->push_back(*(nextRSTriplePtr[i]));
                        RLEBlock* rleBlk = (RLEBlock*)((RLEDataSource*)m_ics[i])->getNextValBlock(0);   
                        if(rleBlk != NULL){
                                nextRSTriplePtr[i] = rleBlk->getTriple();
                        }else{
                                nextRSTriplePtr[i] = NULL;
                        }
                        
                }
    
                //use RLEEncoder to write to output database
                string str_MergeSuffix = MergeSuffix;  
                string str_MergePrefix = MergePrefix;
                

                //Todo create a new RLEEncoder from RLETriple vector
                //which means to create a new Operator class takes RLETriple vector
                RLEVecDataSource* rleVecDs = new RLEVecDataSource(sortKeyTriples[0]);
                
                //load to bdb by PagePlacer
                RLEEncoder* encoder=
                        new RLEEncoder((Operator*)rleVecDs, 0, 8*PAGE_SIZE, (byte) INT_VALUE_TYPE, (short)16, (unsigned int) 27, (unsigned int) 16);
                RLEDecoder* decoder = new RLEDecoder(true);
                PagePlacer* pagePlacer = new PagePlacer(encoder, decoder, 2, false);
                
                //string mo_bdbTable = (*m_columnNames)[0]+str_MergeSuffix; 
                string mo_bdbTable = str_MergePrefix + (*m_columnNames)[0] +str_MergeSuffix; 
                mo_bdbTable = BDBTMPTABLEPATH + mo_bdbTable;
                system(("rm -f " + mo_bdbTable).c_str());
                
                pagePlacer->placeColumn(mo_bdbTable,false, true);
                
                delete pagePlacer;
                
                delete sortKeyTriples;  //release triples vector space in heap
                timer[i] += swatch.quietStop();
        }
}

/**
   finish up Type2 column
*/
void TMMerger::finishMoWSDeltaPosCol(int idx){
        
}

/**
   finish up type4 column
*/
void TMMerger::finishMoWSIntCol(int i){

        IntDecoder intDecoder(false);  
        ValPos* pair=NULL;
        if(nextIntBlockMap[i]!=NULL && nextIntBlockMap[i]->hasNext()){
                pair = nextIntBlockMap[i]->getNext();
        }else{
                pair = NULL;
        }
        while(pair != NULL){
	  assert(pair->type == ValPos::INTTYPE);
                if(!moIntEncoderMap[i]->writeVal(*(int*)pair->value, (pair->position + m_numInsert))){
		  if(TMDEBUG) cout << " page is full -->write to BDB i= " << i  << endl;
                        //page is full need to write to BDB

                                byte* page = moIntEncoderMap[i]->getPage();

                                //for some reason there is one extra position
                                intDecoder.setBuffer(page);

				int temppos = intDecoder.getEndPos();
                                moIntPageWriterMap[i]->placePage((char*)page, (char*)&temppos);
                                if(TMDEBUG) cout << "write a page:end pos: " << intDecoder.getEndPos() << " value:" << *(int*)intDecoder.getEndVal()->value << endl;
                                //after getPage() buffer is reset, then try the write again
                                moIntEncoderMap[i]->writeVal(*(int*)pair->value, pair->position + m_numInsert);
              
                }
                
                if(nextIntBlockMap[i]->hasNext()){
                        pair = nextIntBlockMap[i]->getNext();
                }else{
                        nextIntBlockMap[i] = (BasicBlock*)m_ics[i]->getNextValBlock(0);
                        if(nextIntBlockMap[i]!=NULL && nextIntBlockMap[i]->hasNext()){
                                pair = nextIntBlockMap[i]->getNext();
                        }else{
                                pair = NULL;
                        }
                }
        }

        if(TMDEBUG) cout << "Write the the last page " << endl;
        //write last page

        byte* page = moIntEncoderMap[i]->getPage();

        //for some reason there is one extra position
        intDecoder.setBuffer(page);
       
	int temppos = intDecoder.getEndPos();
        moIntPageWriterMap[i]->placePage((char*)page, (char*)&temppos);

        moIntPageWriterMap[i]->closeDB();
}

void TMMerger::printMergedCol(int idx, stringstream& outSS ){
        string columnName = MergePrefix + (*m_columnNames)[idx] + MergeSuffix;
        //string columnName = (*m_columnNames)[idx]+MergeSuffix;
        
        ROSAM* am = new ROSAM(columnName,m_colTypes[idx]==COMPRESSION_TYPE1?2:1);
        DataSource* ds;
        switch(m_colTypes[idx]){
        case COMPRESSION_TYPE1:
                ds = new RLEDataSource(am, idx==0);  //if RLE columns is the first sort-key column then it is sor\ted
                break;
        case COMPRESSION_TYPE2: //ie. type2
                ds = new DeltaPosDataSource(am,false);
                break;
        case COMPRESSION_TYPE3:  //ie. type3
                ds = new IntDataSource(am,true,true);
                ((IntDataSource*)ds)->setSingles(true);
                break;
        case  COMPRESSION_TYPE4: //ie. type4
                ds = new IntDataSource(am,false,true);
                ((IntDataSource*)ds)->setSingles(true);
                break;
        default :
                ds = new DataSource(am);
                break;
        }

        ds->printColumn(outSS);
        //cout << outSS.str();
        
        delete ds;
        
}

