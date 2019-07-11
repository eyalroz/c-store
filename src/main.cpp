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
#include "main.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include "Util/StringUtil.h"

#define DEFAULT_TEST_CONF "test.cnf"
#define DEFAULT_GLOB_CONF "global.cnf"
#define SEPARATOR '\t'

main::main()
{
}

main::~main()
{
}

void runTest(string, string,Globals*, char*, char*);
void initAllTestSuites(map<string, UnitTest*> &testSuites);

int main(int argc, char **argv) {
	using namespace std;
	Log::logInit();
	string testconf;
	string globalconf;
	bool useOwn = true;
	Globals* g = new Globals();
	char* arg1 = NULL;
	char* arg2 = NULL;

	system("cat license.txt");

	testconf = DEFAULT_TEST_CONF;
	globalconf = DEFAULT_GLOB_CONF;
	if(argc >=2){
	  useOwn = (!(argv[1][0] == '0'));
	  if (useOwn) {
	    if (argc >= 3) {
	      arg1 = argv[2];
	      if (argc >=4)
		arg2 = argv[3];
	    }
	  }
	  else {
	    assert(argc>=4);
	    testconf = argv[2];
	    globalconf = argv[3];
	    if (argc >= 5) {
	      arg1 = argv[4];
	      if (argc >=6)
		arg2 = argv[5];
	    }
	  }
	}
	//cout << "args: " << arg1 << " " <<  arg2 << endl;
	runTest(testconf, globalconf, g, arg1, arg2);

	Log::logDestroy();
	delete g;
	return 0;
}

void runTest(string testConfigFile, string globConfigFile,Globals* g, char* arg1, char* arg2){
  bool success = true;
  map<string, UnitTest*> testSuites;

  g->build_tables = true;
  g->force_rebuild = false;


  ifstream* infile1 = new ifstream(globConfigFile.c_str());
  string line1;
  string whitespace1 = "\t ";
  while(getline((*infile1), line1)){ 

    //use # indicate comment for test config File
    line1 = StringUtil::ltrim(line1,whitespace1);
    if(line1[0] != '#' && line1.size()>1){
      //according to global config file
      //First token is the variable name
      //second token is the value

      string varName = StringUtil::extractDelimitedSubstring(SEPARATOR, line1, 1);
      string varValue = StringUtil::extractDelimitedSubstring(SEPARATOR, line1, 2);
      cout << varName << "=" << varValue << endl;
      if (strcmp(varName.c_str(), "BUILD_TABLES") == 0)
	g->build_tables=atoi(varValue.c_str());
      if (strcmp(varName.c_str(), "FORCE_REBUILD") == 0)
	g->force_rebuild=atoi(varValue.c_str());
    }
  }

  delete infile1;
  initAllTestSuites(testSuites);

  ifstream* infile = new ifstream(testConfigFile.c_str());
  string line;
  string whitespace = "\t ";
  while(getline((*infile), line)){ 

    //use # indicate comment for test config File
    line = StringUtil::ltrim(line,whitespace);

    //cout << " LINE " << line << " size " << line.size() << endl;
    if(line[0] != '#' && line.size()>1){
      //according to test config file
      //First token is the test suites name
      //second token is the number of arguments
      //following is all the arguments

      string suiteName = StringUtil::extractDelimitedSubstring(SEPARATOR, line, 1);
      string numArgStr = StringUtil::extractDelimitedSubstring(SEPARATOR, line, 2);
      int numArg = atoi(numArgStr.c_str());
      cout << suiteName << " " << numArg << endl;

      vector<string> args;
      for (int i=1; i <=numArg; i++){
	string stemp = StringUtil::extractDelimitedSubstring(SEPARATOR, line, i+2);
	if (stemp[0] == '$') {
	  if (stemp[1] =='1') {
	    if (arg1==NULL) {
	      cout << "Expected argument but not given in command line" << endl;
	      exit(1);
	    }
	    stemp=arg1;
	  }
	  if (stemp[1] =='2') {
	    if (arg2==NULL) {
	      cout << "Expected argument but not given in command line" << endl;
	      exit(1);
	    }
	    stemp=arg2;
	  }
	}
	args.push_back(stemp);
      }

      cout << "-->>> Run test suite: " << suiteName << "\t[num of args:] " << numArg ;
      for(unsigned int i=0; i<args.size(); i++){
	cout << "\t[arg" << i+1 << ":] " << args[i]; 
      }
      cout << endl;

      if(testSuites[suiteName] != NULL){
	success &=testSuites[suiteName]->run(g, args);
      }else{
	cout << "No such unit test been intialized!" <<endl;
      }
    }
  }

  delete infile;

  if (success) 
    cout << endl << "SUCCESSFULL ON ALL TESTS" << endl;
  else
    cout << endl << "FAILED SOME TESTS" << endl;

  for (map<string, UnitTest*>::iterator iterPos=testSuites.begin(); iterPos!=testSuites.end(); ++iterPos)
    delete iterPos->second;
  
}


void initAllTestSuites(map<string, UnitTest*> &testSuites){
#ifndef MINICSTORE
  testSuites["QueryTest"] = new QueryTest();
  testSuites["PlanData"] = new PlanData();  
  testSuites["PlanDataFullScale"] = new PlanDataFullScale();  
  testSuites["OptTest"] = new OptTest();
  testSuites["OptPlanTest"] = new OptPlanTest();
  testSuites["OptTestOneChoice"] = new OptTestOneChoice();
  testSuites["Plans"] = new Plans();
  testSuites["PlanInC"] = new PlanInC();
  testSuites["BitReaderWriter"]= new BitReaderWriter();
  testSuites["RLEBlockTest"]=new RLEBlockTest();
  testSuites["RLEEncodeDecode"]=new RLEEncodeDecode();
  testSuites["BDBReadWrite"] = new BDBReadWrite();
  testSuites["RLEWriterTest"]=new RLEWriterTest();
  testSuites["TmMergerTest"] = new TmMergerTest();
  testSuites["RLEExtractLoadAndAccess"]=new RLEExtractLoadAndAccess();
  testSuites["Query1"]=new Query1();
  testSuites["BCopyOpTest"]=new BCopyOpTest();
  testSuites["BAndOpTest"]=new BAndOpTest();
  testSuites["BAndQueryTest"]=new BAndQueryTest();
  testSuites["BOrOpTest"]=new BOrOpTest();
  testSuites["BNotOpTest"]=new BNotOpTest();
  testSuites["BNotQueryTest"]=new BNotQueryTest();
  testSuites["MultipleColumnsTest"]=new MultipleColumnsTest();
  testSuites["SpillMapTest"]=new SpillMapTest();
  testSuites["DeltaPosWriterTest"]=new DeltaPosWriterTest();
  testSuites["Type2WriterTest"]=new Type2WriterTest();
  testSuites["RLEEncodeDecodeII"]=new RLEEncodeDecodeII();
  testSuites["IntReaderWriterTest"]=new IntReaderWriterTest();
  testSuites["IntEncodeDecodeTest"]=new IntEncodeDecodeTest();
  testSuites["IntExtractAndLoad"]=new IntExtractAndLoad();
  testSuites["MultiLoadAndAccess"]=new MultiLoadAndAccess();
  testSuites["DeltaPosExtractLoadAndAccess"]=new DeltaPosExtractLoadAndAccess();
  testSuites["Type2ExtractLoadAndAccess"]=new Type2ExtractLoadAndAccess();
  
  testSuites["LZEncodeDecode"]=new LZEncodeDecode();
  testSuites["LZED"] = new LZED();
  testSuites["NSEncodeDecode"]=new NSEncodeDecode();
  testSuites["LZvsINT"]=new LZvsINT();
  testSuites["DictExtractLoad"]=new DictExtractLoad();
  
  testSuites["LoadTPCHData"]=new LoadTPCHData();
  testSuites["GenerateBDBQueryData"]=new GenerateBDBQueryData();
  testSuites["Queries"]=new Queries();
  testSuites["Query100"] = new Query100();
  testSuites["Query2"] = new Query2();
  testSuites["Query3"] = new Query3();
  testSuites["Query4"] = new Query4();
  testSuites["Query4Prime"] = new Query4Prime();
  testSuites["Query5"] = new Query5();
  testSuites["Query6"] = new Query6();
  testSuites["Query7"] = new Query7();
  
  testSuites["Query1U"] = new Query1U();
  testSuites["Query2U"] = new Query2U();
  testSuites["Query3U"] = new Query3U();
  testSuites["Query4U"] = new Query4U();
  testSuites["Query5U"] = new Query5U();
  testSuites["Query6U"] = new Query6U();
  testSuites["Query7U"] = new Query7U();
  
  testSuites["Query1WSRS"] = new Query1WSRS();
  testSuites["Query2WSRS"] = new Query2WSRS();
  testSuites["Query3WSRS"] = new Query3WSRS();
  
  testSuites["NSDataMaker"]= new NSDataMaker();
  testSuites["DeltaPosDataMaker"]= new DeltaPosDataMaker();
  testSuites["LZDataMaker"] = new LZDataMaker();
  testSuites["DictDataMaker"] = new DictDataMaker();
  testSuites["DictCPUDataMaker"] = new DictCPUDataMaker();
  
  testSuites["RLE2DataMaker"] = new RLE2DataMaker();
  testSuites["Int2DataMaker"]= new Int2DataMaker();
  testSuites["NS2DataMaker"]= new NS2DataMaker();
  testSuites["DeltaPos2DataMaker"]= new DeltaPosDataMaker();
  testSuites["LZ2DataMaker"] = new LZ2DataMaker();
  testSuites["Dict2DataMaker"] = new Dict2DataMaker();
  testSuites["DictCPU2DataMaker"] = new DictCPU2DataMaker();
  
  testSuites["JoinExpDataMaker"] = new JoinExpDataMaker();
  testSuites["JoinExpDataRunner"] = new JoinExpDataRunner();
  testSuites["JoinExpRLEDataRunner"] = new JoinExpRLEDataRunner();
  testSuites["JoinExpDictCPUDataRunner"] = new JoinExpDictCPUDataRunner();
  testSuites["JoinExpType2DataRunner"] = new JoinExpType2DataRunner();
  testSuites["SelExpDataMaker"] = new SelExpDataMaker();
  testSuites["SelExpDataRunner"] = new SelExpDataRunner();
  testSuites["SelExpRLEDataRunner"] = new SelExpRLEDataRunner();
  testSuites["SelExpType2DataRunner"] = new SelExpType2DataRunner();
  testSuites["SelExpDictCPUDataRunner"] = new SelExpDictCPUDataRunner();
  
  testSuites["NSDataRunner"] = new NSDataRunner();    
  testSuites["DeltaPosDataRunner"]= new DeltaPosDataRunner();
  testSuites["LZDataRunner"]= new LZDataRunner();
  testSuites["DictDataRunner"]= new DictDataRunner();
  testSuites["DictCPUDataRunner"]= new DictCPUDataRunner();
  testSuites["DictShortCutDataRunner"]= new DictShortCutDataRunner();
  
  testSuites["RLE2DataRunner"] = new RLE2DataRunner();
  testSuites["Int2DataRunner"]= new Int2DataRunner();
  testSuites["NS2DataRunner"] = new NS2DataRunner();    
  //testSuites["DeltaPos2DataRunner"]= new DeltaPosDataRunner();
  testSuites["LZ2DataRunner"]= new LZ2DataRunner();
  testSuites["Dict2DataRunner"]= new Dict2DataRunner();
  testSuites["DictCPU2DataRunner"]= new DictCPU2DataRunner();
  
  testSuites["PosBlockTest"]=new PosBlockTest();
  
  testSuites["BitProfileTest"]=new BitProfileTest();
  
  testSuites["HashMapTest"] = new HashMapTest();
  
  testSuites["MergeTest"] = new MergeTest();
  testSuites["JoinTest"] = new JoinTest();
  
  testSuites["WOSTest"] = new WOSTest();
  testSuites["WOSTest2"] = new WOSTest2();
  testSuites["DummyTest"] = new DummyTest();
  testSuites["DVTest"] = new DVTest();
  testSuites["CatalogTest"] = new CatalogTest();
  
  testSuites["BDBPerformanceProfile"] = new BDBPerformanceProfile();
  
  testSuites["InsertOpTest"]=new InsertOpTest();
  testSuites["DeleteOpTest"]=new DeleteOpTest();
  testSuites["SelectDV"]=new SelectDV();
  testSuites["DJD_Data"]=new DJD_Data();
  testSuites["DJD_QueryAny"]=new DJD_QueryAny();
  testSuites["BAndRLERLE"]=new BAndRLERLE();
  testSuites["MinicolConcatTest"]=new MinicolConcatTest();
  testSuites["PosAndLM"]=new PosAndLM();
  testSuites["PosAndMC"]=new PosAndMC();
  testSuites["PosAndEM"]=new PosAndEM();
  testSuites["LMDataMaker"]=new LMDataMaker();
  testSuites["T2Test"]=new T2Test();
  testSuites["JoinLM"]=new JoinLM();
  testSuites["JoinEM"]=new JoinEM();
  testSuites["JoinMC"]=new JoinMC();
#endif
  testSuites["SelExpDM"]=new SelExpDM();
  testSuites["SelExpDR"]=new SelExpDR();
  testSuites["StrSelExpDM"]=new StrSelExpDM();
  testSuites["StrSelExpDR"]=new StrSelExpDR();
  testSuites["QueryAny"]=new QueryAny();
  testSuites["ProjMaker"]=new ProjMaker();
  testSuites["RLEDataMaker"] = new RLEDataMaker();
  testSuites["RLEDataRunner"] = new RLEDataRunner();
  testSuites["Type2DataMaker"]= new Type2DataMaker();
  testSuites["Type2DataRunner"]= new Type2DataRunner();
  testSuites["IntDataMaker"]= new IntDataMaker();
  testSuites["IntDataRunner"]= new IntDataRunner();

}
