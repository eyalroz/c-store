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
 */
#include "JoinLM.h"

JoinLM::JoinLM() {
}


JoinLM::~JoinLM() { 
}

bool JoinLM::run(Globals* g, const vector<string>& args) {
  bool success=true;

  int testnum = atoi(args[0].c_str());
  int custnum = atoi(args[1].c_str());/*
  string line_num_type = string(args[2]);
  bool aggregate = bool(atoi(args[3].c_str()));*/

system("./flushCache");

  StopWatch stopWatch;
  stopWatch.start();

  ROSAM* am4 = new ROSAM("LMOCUSTKEYRLE", 1);
  RLEDataSource* ocustkey = new RLEDataSource(am4, false);

  ROSAM* am = new ROSAM("LMNATCODEINT", 1);
  IntDataSource* cnatcode = new IntDataSource(am, false,true);

  ROSAM* am3 = new ROSAM("LMORDERDATEINT", 1);
  IntDataSource* oorderdate = new IntDataSource(am3, false,true);

  ROSAM* am2 = new ROSAM("LMCCUSTKEYINT", 1);
  IntDataSource* ccustkey = new IntDataSource(am2, false,true);

  Predicate* pred3 = new Predicate(Predicate::OP_LESS_THAN);
  ocustkey->setPredicate(pred3);
  ocustkey->changeRHSBinding(custnum);
  NLJoin* join1 = new NLJoin(ocustkey, 0, ccustkey, 0);
  oorderdate->setPositionFilter(join1,0);
  cnatcode->setPositionFilter(join1,1);
  cnatcode->setSingles(true);
  join1->doLM();

  //PosBlock* a = join1->getNextPosBlock(0);
  //PosBlock* b = join1->getNextPosBlock(1);
    /*DataSource* quant1;
  if (line_num_type == "T2DECOMP") {
  	// Quant1 is an int value source for type2 since posfiltering is slow
 	quant1 = getLineNumDS("INT", true); // true -> second copy
  } else {
  	quant1 = getLineNumDS(line_num_type, true); // true -> second copy
	}*/

//  ROSAM* am2 = new ROSAM("LMLINENUMT2Vals", 1);
  //Type2DecompDataSource* quant2=new Type2DecompDataSource(am2, false);
  //DataSource* quant2 = getLineNumDS(line_num_type);
  
  
  /*Predicate* pred2 = new Predicate(Predicate::OP_LESS_THAN);
  shipdate->setPredicate(pred2);
  shipdate->changeRHSBinding(shipnum);
  
  Predicate* pred3 = new Predicate(Predicate::OP_LESS_THAN);
  quant2->setPredicate(pred3);
  quant2->changeRHSBinding(linenum);
  
  PosAndOperator* band = new PosAndOperator(shipdate, 0, quant2, 0);

  Operator* bcopy = new BCopyOperator(band,  0, 2);

  shipdate2->setPositionFilter(bcopy,0);
  quant1->setPositionFilter(bcopy,1);*/

	char* temp8 = new char[128];
	sprintf(temp8, "test%dJoinLM.out", testnum);
	BlockPrinter* bPrint = NULL;
  	Operator* srcs[2] = {oorderdate,cnatcode};

  	int numCols[2] = {1,1};
  	bPrint=new BlockPrinter(srcs, numCols, 2, temp8);
	bPrint->printColumns();
	cout << "Join LM took: " << stopWatch.stop() << " ms" <<  endl;

	/*
  SBlockPrinter* bPrint1 = NULL;
  BlockPrinter* bPrint2 = NULL;
  Filter* filter;
  if (aggregate) {
	HashSum* hashAgg = new HashSum((Operator*) quant1, 0, (Operator*) shipdate2, 0);
 	hashAgg->setHashFunction(new IdentityHashFunction(11000));
  	hashAgg->setHashTableSize(11000);
  	Operator* srcs[1] = {hashAgg};

  	int numCols[1] = {2};
  	bPrint2=new BlockPrinter(srcs, numCols, 1, temp8);
	bPrint2->printColumns();
	printEndOfTest("JoinLM", testnum, shipnum, linenum, line_num_type, aggregate, bPrint2->getNumPrinted(), stopWatch.stop());
  } else {
	Operator* srcs[2]={shipdate2,quant1};
	int numCols[2]={1,1};
	filter = new Filter(srcs, 2, NULL, 0, NULL); 
	bPrint1=new SBlockPrinter(filter, 2, temp8);
	bPrint1->printColumns();
	//bPrint2 = new BlockPrinter(srcs, numCols, 2, temp8);
	//bPrint2->printColumns();
	printEndOfTest("JoinLM", testnum, shipnum, linenum, line_num_type, aggregate, bPrint1->getNumPrinted(), stopWatch.stop());
	}*/
  //bPrint->printColumns();
  //printEndOfTest("JoinLM", testnum, shipnum, linenum, line_num_type, bPrint->getNumPrinted(), stopWatch.stop());
  
  return success;	
}
