#include "T2Test.h"

T2Test::T2Test()
{
}

T2Test::~T2Test()
{
}

bool T2Test::run(Globals* g, const vector<string>& args) {
  bool success=true;

  //int shipnum = atoi(args[0].c_str());
  //int testnum = atoi(args[1].c_str());

//	system("./flushCache");

  StopWatch stopWatch;
  stopWatch.start();



  //ROSAM* am2 = new ROSAM("LMQUANTITYT2Vals", 1);
  //ROSAM* am2 = new ROSAM("LMLINENUMT2Vals", 1);
  //Type2DecompDataSource* quant2=new Type2DecompDataSource(am2, false);
//	Type2DataSource* quant2 = new Type2DataSource(am2, false);
  
  ROSAM* am3 = new ROSAM("LMLINENUMT2Vals", 1);
  UncompressedDecoder* dec = new UncompressedDecoder(false, ValPos::INTTYPE);
  Type2DecompDataSource* quant3=new Type2DecompDataSource(am3, false, dec);
//	Type2DataSource* quant3 = new Type2DataSource(am3, false);
  
  
  // Predicate on quant2
  //Predicate* pred3 = new Predicate(Predicate::OP_LESS_THAN);
  //quant2->setPredicate(pred3);
  //quant2->changeRHSBinding(4);
  
  Predicate* pred4 = new Predicate(Predicate::OP_LESS_THAN);
  quant3->setPredicate(pred4);
  quant3->changeRHSBinding(7);
 
//  PosOrOperator por(quant2, 0, quant3, 0);
  //PosAndOperator pand(quant2, 0, quant3, 0);
  
  /*
  // BlockPrinter
  Operator* srcs[1]={quant2};
  int numCols[1]={1};
  char* temp6 = new char[30];
  sprintf(temp6, "T2Test");
  char* temp8 = new char[30];
  sprintf(temp8, "test%d%s.out", testnum,temp6);
  BlockPrinter* bPrint=new BlockPrinter(srcs, numCols, 1, temp8);
  bPrint->printColumns();*/
  int count = 0;
  Block* b;
  //while(b = por.getNextPosBlock(0)) {
  while((b = quant3->getNextValBlock(0))) {
 // while(b = quant3->getNextPosBlock(0)) {
  	while(b->hasNext()) {
  		ValPos* p = b->getNext();
  		assert(p != NULL);
		//assume int type for now ... fix later
		assert(p->type == ValPos::INTTYPE);
  		int v = *(int*)p->value;
  		if (v >= 7) {
  			cerr << "bad v = " << v << endl;
  			throw UnexpectedException();
  		}
  		++count;
  	}
  }
  cerr << "Read " << count << " wanted  57845879" << endl;  	
  
  cout << "Query took: " << stopWatch.stop() << " ms" <<  endl;
  
  return success;	
}
