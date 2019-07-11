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
#include "PosAndEM.h"
#include "posand-support.h"

PosAndEM::PosAndEM() {
}


PosAndEM::~PosAndEM() {
}

bool PosAndEM::run(Globals* g, const vector<string>& args) {
  bool success=true;

	if (args.size() != 6) {
		cerr << "Wrong number of arguments: need 6" << endl;
		throw UnexpectedException();
	}

  int shipnum = atoi(args[0].c_str());
  int linenum = atoi(args[1].c_str());
  string line_num_type = string(args[2]);
  bool aggregate = bool(atoi(args[3].c_str()));
  //bool smart = bool(atoi(args[4].c_str()));
  bool smart = true;
  bool parallel = bool(atoi(args[4].c_str()));
  int testnum = atoi(args[5].c_str());

	system("./flushCache");

  StopWatch stopWatch;
  stopWatch.start();

  ROSAM* am = new ROSAM("LMSHIPDATERLE", 1);
  RLEDataSource* shipdate2 = new RLEDataSource(am, false);

  DataSource* quant1 = getLineNumDS(line_num_type);
  
  Predicate* pred2 = new Predicate(Predicate::OP_LESS_THAN);
  pred2->setRHS(shipnum);
  
  Predicate* pred3 = new Predicate(Predicate::OP_LESS_THAN);
  pred3->setRHS(linenum);

  Operator* srcs[2]={shipdate2,quant1};
  Predicate* preds[2]={pred2,pred3};
  int indexes[2]={0,1};
  Filter* filter = new Filter(srcs, 2, preds, 2, indexes);

  // For smart filtering
  if (smart) {
  	filter->makeSmart();
  }

  if (parallel)
    filter->setDoParallel();
  
  EMHashSum* emha = NULL;
  if (aggregate) {
  	emha = new EMHashSum(filter, 0, 1, 2);
  	emha->setHashFunction(new IdentityHashFunction(11000));
  	emha->setHashTableSize(11000);
  }
  char* temp8 = new char[128];
  sprintf(temp8, "test%dPosAndEMshipnum%dlinenum%d.out", testnum, shipnum, linenum);
  //BlockPrinter* bPrint=new BlockPrinter(srcs, numCols, 2, temp8);
  //bPrint->printColumns();

  
  SBlock* sb;
  SPair* p;
  int nseen = 0;
  //int* temp;
  std::ofstream outstream;
  outstream.open(temp8);
  Operator* tupSrc = aggregate ? static_cast<Operator*>(emha) : static_cast<Operator*>(filter);


  //SBlockPrinter* bPrint1 = NULL;
  //bPrint1=new SBlockPrinter(tupSrc, 3, temp8);
  //bPrint1->printColumns();
  while ((sb = tupSrc->getNextSValBlock(0))) {
    while (sb->hasNext()) {
      p = sb->getNext();
      //temp = (int*)(p->value);
      //for (int i=0; i < 1; i++) {
      //outstream << temp[1] << ",";
      //}
      //outstream << temp[2] << endl;
      ++nseen;
    }
  }
  outstream.close();
  string smartstr = parallel ? "parallel" : "pipelined";
  printEndOfTest("PosAndEM-" + smartstr, testnum, shipnum, linenum, line_num_type, aggregate, nseen, stopWatch.stop());
  return success;	
}
