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
#include "PosAndLM.h"

PosAndLM::PosAndLM() {
}


PosAndLM::~PosAndLM() { 
}

bool PosAndLM::run(Globals* g, const vector<string>& args) {
  bool success=true;

	if (args.size() != 6) {
		cerr << "Wrong number of arguments: need 6" << endl;
		throw UnexpectedException();
	}

  int shipnum = atoi(args[0].c_str());
  int linenum = atoi(args[1].c_str());
  string line_num_type = string(args[2]);
  bool aggregate = bool(atoi(args[3].c_str()));
  //bool back_to_disk = bool(atoi(args[4].c_str()));
  bool usePipelinedAlg = bool(atoi(args[4].c_str()));
  int testnum = atoi(args[5].c_str());

system("./flushCache");

  StopWatch stopWatch;
  stopWatch.start();

  ROSAM* am4 = new ROSAM("LMSHIPDATERLE", 1);
  RLEDataSource* shipdate = new RLEDataSource(am4, false);

  ROSAM* am = new ROSAM("LMSHIPDATERLE", 1);
  RLEDataSource* shipdate2 = new RLEDataSource(am, false);

//  ROSAM* am3 = new ROSAM("LMLINENUMINT", 1);
  
  DataSource* quant1;
  if (line_num_type == "T2DECOMP") {
  	// Quant1 is an int value source for type2 since posfiltering is slow
 	//quant1 = getLineNumDS("INT", back_to_disk); // true -> second copy
 	
    //Never go back to disk now
    quant1 = getLineNumDS("INT", 0); // true -> second copy
  } else {
    //quant1 = getLineNumDS(line_num_type, back_to_disk); // true -> second copy
    //Never go back to disk now
    quant1 = getLineNumDS(line_num_type, 0);
  }

//  ROSAM* am2 = new ROSAM("LMLINENUMT2Vals", 1);
  //Type2DecompDataSource* quant2=new Type2DecompDataSource(am2, false);
  DataSource* quant2 = getLineNumDS(line_num_type);
  
  
  Predicate* pred2 = new Predicate(Predicate::OP_LESS_THAN);
  shipdate->setPredicate(pred2);
  shipdate->changeRHSBinding(shipnum);
  
  Predicate* pred3 = new Predicate(Predicate::OP_LESS_THAN);
  quant2->setPredicate(pred3);
  quant2->changeRHSBinding(linenum);

  Operator* bcopy;
  PosAndOperator* band;
  if (usePipelinedAlg) {
    quant2->setPositionFilter(shipdate,0);
    bcopy = new BCopyOperator(quant2,  0, 2);
  }
  else {
    band = new PosAndOperator(shipdate, 0, quant2, 0);
    bcopy = new BCopyOperator(band,  0, 2);
  }

  shipdate2->setPositionFilter(bcopy,0);
  quant1->setPositionFilter(bcopy,1);

  char* temp8 = new char[128];
  sprintf(temp8, "test%dPosAndLMshipnum%dlinenum%d.out", testnum, shipnum, linenum);
  
  //SBlockPrinter* bPrint1 = NULL;
  BlockPrinter* bPrint2 = NULL;
  //Filter* filter;
  if (aggregate) {
	HashSum* hashAgg = new HashSum((Operator*) quant1, 0, (Operator*) shipdate2, 0);
 	hashAgg->setHashFunction(new IdentityHashFunction(11000));
  	hashAgg->setHashTableSize(11000);
  	Operator* srcs[1] = {hashAgg};

  	int numCols[1] = {2};
  	bPrint2=new BlockPrinter(srcs, numCols, 1, temp8);
	bPrint2->printColumns(true); // skip output
	printEndOfTest("PosAndLM", testnum, shipnum, linenum, line_num_type, aggregate, bPrint2->getNumPrinted(), stopWatch.stop());
  } else {
	Operator* srcs[2]={shipdate2,quant1};
	int numCols[2]={1,1};
//	filter = new Filter(srcs, 2, NULL, 0, NULL); 
//	bPrint1=new SBlockPrinter(filter, 2, temp8);
//	bPrint1->printColumns();
	//bPrint2 = new BlockPrinter(srcs, numCols, 2, temp8);
	//bPrint2->printColumns();


	TupleGenerator tg(srcs, numCols, 2, 2);
	SMultiBlock* s;
	SPair* sp;
	int nseen = 0;
	//int* temp;
	std::ofstream outstream;
	outstream.open(temp8);
	while ((s = static_cast<SMultiBlock*>(tg.getNextSValBlock(0)))) {
	  while(s->hasNext()) {
	    ++nseen;
	    sp = s->getNext();
	    //temp = (int*)(sp->value);
	    //for (int i=0; i < 1; i++) {
	    //outstream << temp[0] << ",";
	    //}
	    //outstream << temp[1] << endl;
	  }
	}
	outstream.close();

	//string reread = back_to_disk ? "reread" : "inmemory";
	string reread = usePipelinedAlg ? "pipelined" : "parallel";
	printEndOfTest("PosAndLM-" + reread, testnum, shipnum, linenum, line_num_type, aggregate, nseen, stopWatch.stop());
  }
  //bPrint->printColumns();
  //printEndOfTest("PosAndLM", testnum, shipnum, linenum, line_num_type, bPrint->getNumPrinted(), stopWatch.stop());
  
  return success;	
}
