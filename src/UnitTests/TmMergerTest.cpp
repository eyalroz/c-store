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
//TmMergerTest.cpp

//Author: Xuedong Chen
/**
 *this test suite is for Tuple Mover Merger
 *create a ROS projection and have them stored in BDB type1,2,4 encoded
 *create a fake WOS for that projection and create a WosTupleIter from it
 *loop by WosTupleIter and merge WOS tuples into RS
 *retrive saved ROS merged projection and check the result.
 */

#include "TmMergerTest.h"
#define SEPARATOR ' '

using namespace std;

/*Testing program for TupleMoverMerger
 *it first create a set of WOS tuples and put them 
 *in a vector and make a Tuple iterator from it
 *which is called testTupleIter. 
 *This part is purely for testing purpose and
 *as the WOS is fully implemented, this tuple iterator
 *will be constructed in different way.

 *Second, it create a AM array based pregenerated file. a vector for column type 

 *Third, in order to match the BDB database file name with column name
 *an array of strings is created explicitly and get passed to TupleMoverMerger
 *ctor along with the projection.

 *Then the whole merge action is triggered by doMerger() call.
 *After that you will see result which is printed page by page, column by column.
 *
 *Todo: 
 *1.Currently, only type1 (RLE) column is handled, thus need to work on other types
 *2.Use WSMV instead.
 */

TmMergerTest::TmMergerTest()
{
  outBDBName = OUTBDBNAME;
}

TmMergerTest::~TmMergerTest() {
	
}

bool TmMergerTest::run(Globals* g, const vector<string>& args) {
        int verify=0;
        int lastSortKeyCol =0;
        int tableReady = 0;

        if(args.size() == 1){
                string projName = args[0];
                CatalogInstance *ci = CatalogInstance::getCatalog();
                
                StopWatch * timer = new StopWatch();
                timer->start();

                cout << projName;
                TMMerger tmm(ci,projName);
                tmm.doMerge();
                
                timer->stop();
                cout <<endl;
                tmm.printProfile();
                return true;
        }

        if(args.size() >=5){
                m_fileName = args[0];
                m_col = atoi(args[1].c_str());
                
                
                lastSortKeyCol = atoi(args[2].c_str());

                verify = atoi(args[3].c_str());  /*1 mean verify, 0 mean no*/
                tableReady = atoi(args[4].c_str());/*1 mean bdb table already generated*/
                
                if(m_col <=0){
                        cout << "column number should be greater or equal to 1" <<endl;
                        return false;
                }
                
        }else{
                cout << "Inproper number of parameters" << endl;
                return false;
        }
        cout << "FileName: " << m_fileName << "  column: " << m_col << endl;
        
	bool success=true;

	string bdbTmpPath = BDBTMPTABLEPATH;

        int numCol =m_col;
        stringstream wosFileName;
	wosFileName << m_fileName << ".wos";

        if(tableReady != 1){

                //clean up test directory anyway, in case last run abort 
                system(("rm -f " + bdbTmpPath + "*TM*").c_str());
                system(("mkdir -p " + bdbTmpPath).c_str());
        }                


        //have to generate for each run
        const char* tt = wosFileName.str().c_str();

        ifstream* infile = new ifstream(tt);
        string line;
        string tmpWOSSeg = bdbTmpPath+"projRWSeg*";
        system(("rm -rf " + tmpWOSSeg).c_str());
        ROSWOSSegment *rwseg = new ROSWOSSegment(bdbTmpPath+"projRWSeg", numCol);
        
        int storageKey =1;
        while(getline((*infile), line)){ 
                char *tuple = new char[(numCol+1)*sizeof(int)];
                memcpy(tuple, &storageKey, sizeof(int));
                for(int i=1; i<= numCol; i++){
                        string tempLine = line;
                        string tempStr = extractDelimitedSubstring(SEPARATOR, tempLine, i);
                        int val = atoi(tempStr.c_str());
                        memcpy(tuple+i*sizeof(int), &val, sizeof(int));
                }
                rwseg->appendTuple(tuple);
                //                delete [] tuple;
                storageKey++;
        }
        infile->close();

        WosTupleIter testTupleIter(rwseg, numCol);

        vector<string> colNames;
        vector<int> colTypes;
        ROSAM* ams[numCol];

        for(int i=1; i<=numCol; i++){

                stringstream SS;
                SS << bdbTmpPath << outBDBName <<i;

                string stt = SS.str();
                if(i==1){
                        colTypes.push_back(COMPRESSION_TYPE1);
                        if(tableReady!=1){
                                success&=extractAndLoad(i,COMPRESSION_TYPE1,m_fileName+".ros", stt, g);
                        }
                }else{
                        colTypes.push_back(COMPRESSION_TYPE4);
                        if(tableReady!=1){
                                success&=extractAndLoad(i,COMPRESSION_TYPE4,m_fileName+".ros", stt, g);
                        }
                }
                stringstream tmpSS;
                tmpSS << outBDBName <<i;

                if(i==1){
                        ams[i-1] = new ROSAM(tmpSS.str(),2);
                }else{
                        ams[i-1] = new ROSAM(tmpSS.str(),1);
                }
                
                colNames.push_back(tmpSS.str());
                
                
        }

        StopWatch * timer = new StopWatch();
        timer->start();
	TMMerger tmm(ams,lastSortKeyCol,colTypes, &testTupleIter, &colNames);
	tmm.doMerge();

        timer->stop();
        cout <<endl;
        tmm.printProfile();
        
        for(int i=0; i<numCol; i++){
                delete ams[i];
        }
        if(verify==1){
                for(int i=0; i< numCol && success; i++){
                        stringstream outSS; 
                        tmm.printMergedCol(i,outSS);  //it print to stringstream outSS of TMMerger

                        stringstream SS;
                        SS << bdbTmpPath << outBDBName << (i+1);

                        ROSAM* mo_am;
                        DataSource* mo_ds;
                        stringstream mo_ss;
                        
                        if(i==0){
                                  if(tableReady!=1){
                                          success&=extractAndLoad(i+1,COMPRESSION_TYPE1,m_fileName+".mo", SS.str()+".mo", g);
                                  }
                                mo_am = new ROSAM(outBDBName+"1.mo", 2);
        
                                mo_ds = new RLEDataSource(mo_am, true);
                        }else{
                                if(tableReady!=1){
                                        success&=extractAndLoad(i+1,COMPRESSION_TYPE4,m_fileName+".mo", SS.str()+".mo", g);
                                }
                                stringstream tmpSS;
                                tmpSS << outBDBName <<i+1;
                                
                                mo_am = new ROSAM(tmpSS.str()+".mo", 1);
                                //last flag tells isROS and primary index is for Position
                                mo_ds = new  IntDataSource(mo_am, false, true); 
                                ((IntDataSource*)mo_ds)->setSingles(true);
                        }

                        mo_ds->printColumn(mo_ss);
                        
                        success&=(outSS.str()==mo_ss.str());	
                        if(!success){
                                ofstream out1;
                                stringstream tmpStr1, tmpStr2;
                                tmpStr1 << bdbTmpPath << "TM_output_" << i+1;

                                out1.open(tmpStr1.str().c_str(),ios::out);
                                out1 << outSS.str();
                                out1.close();

                                tmpStr2 << bdbTmpPath << "CORRECT_output_" << i+1;
                                out1.open(tmpStr2.str().c_str(),ios::out);
                                out1 << mo_ss.str();
                                out1.close();
                                /*
                                cout << ">>>>TM output>>>>>>> " << outSS.str().substr(1,200) << endl;
                                cout << "<<<< Correct <<<<<<< " << mo_ss.str().substr(1,200) << endl;
                                */
                        }

                        delete mo_ds;
                        delete mo_am;
                }
        }
       
        //delete rwseg;
          
	return success;
}



bool TmMergerTest::extractAndLoad(int idx,int type, string srcFile, string bdbTableFile, Globals* g) {
	ColumnExtracter* ce = new ColumnExtracter(srcFile, idx, g->force_rebuild);
        Encoder* encoder;
        Decoder* decoder;
        PagePlacer* pagePlacer;

        system(("rm -f " + bdbTableFile).c_str());
        if(type == COMPRESSION_TYPE1){
                encoder=new RLEEncoder((Operator*) ce, 0, 8*PAGE_SIZE, (byte) INT_VALUE_TYPE, (short) 16, (unsigned int) 27, (unsigned int) 16);
                decoder=new RLEDecoder(true);
                pagePlacer=new PagePlacer(encoder, decoder, 2, false);
                pagePlacer->placeColumn(bdbTableFile, false, true);

                delete pagePlacer;
                
        }else if(type==COMPRESSION_TYPE4){
                encoder=new IntEncoder((Operator*) ce, 0, 8*PAGE_SIZE);
                decoder=new IntDecoder(false);
                pagePlacer=new PagePlacer(encoder, decoder, 1, true);
                pagePlacer->placeColumn(bdbTableFile, false, false);

                delete pagePlacer;
                
        }

	return true;
}


string TmMergerTest::extractDelimitedSubstring( char c, string str, int index )
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




