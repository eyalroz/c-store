#include "PosAndMC.h"

PosAndMC::PosAndMC()
{
}

PosAndMC::~PosAndMC()
{
}

bool PosAndMC::run(Globals* g, const vector<string>& args) {
  bool success=true;

	if (args.size() != 6) {
		cerr << "Wrong number of arguments: need 5" << endl;
		throw UnexpectedException();
	}

  int shipnum = atoi(args[0].c_str());
  int linenum = atoi(args[1].c_str());
  string line_num_type = string(args[2]);
  bool aggregate = bool(atoi(args[3].c_str()));
  bool usePipelinedAlg = bool(atoi(args[4].c_str()));
  int testnum = atoi(args[5].c_str());

	system("./flushCache");

  StopWatch stopWatch;
  stopWatch.start();

  ROSAM* am4 = new ROSAM("LMSHIPDATERLE", 1);
  RLEDataSource* shipdate = new RLEDataSource(am4, false);
  
 // ROSAM* am2 = new ROSAM("LMLINENUMT2Vals", 1);
 // Type2DecompDataSource* quant2=new Type2DecompDataSource(am2, false);

  DataSource* quant2;
  //if (!usePipelinedAlg)
    quant2 = getLineNumDS(line_num_type);
    /*else {
    if (line_num_type == "T2DECOMP")
      quant2 = getLineNumDS("T2POS");
    else
      quant2 = getLineNumDS(line_num_type);
      }*/

  DataSource* quant1;
  if (line_num_type == "T2DECOMP") {
    // Quant1 is an int value source for type2 since posfiltering is slow
    quant1 = getLineNumDS("INT", 0); // true -> second copy
  }

  // Predicate on shipdate
  Predicate* pred2 = new Predicate(Predicate::OP_LESS_THAN);
  shipdate->setPredicate(pred2);
  shipdate->changeRHSBinding(shipnum);
  
  // Predicate on quant2
  Predicate* pred3 = new Predicate(Predicate::OP_LESS_THAN);
  quant2->setPredicate(pred3);
  quant2->changeRHSBinding(linenum);
  
  // Converters to adapt ship and quant2 to produce MinicolBlocks
  BlockWithPosToMinicolConverter shipconv(shipdate, "shipdate");
//  BlockWithPosToMinicolConverter quant2conv(quant2, "quant2");
 
  MinicolShimOperator* quant2conv;
  if (line_num_type == "T2DECOMP") {
    if (!usePipelinedAlg)
      quant2conv = new Type2BlockToMinicolConverter(quant2, "quant2");
    else
      quant2conv = new BlockWithPosToMinicolConverter(quant1, "quant1");
  } else if (line_num_type == "RLE") {
    quant2conv = new BlockWithPosToMinicolConverter(quant2, "quant2");
  } else if (line_num_type == "INT") {
    quant2conv = new BlockWithPosToMinicolConverter(quant2, "quant2");
  }
  else throw new UnexpectedException("Not supposed to happen");
  
  MinicolShimOperator* mso;
  PosAndOperator* band;
  if (usePipelinedAlg) {
    if (line_num_type == "T2DECOMP") {
      band = new PosAndOperator(&shipconv, 0, quant2, 0);
      quant1->setPositionFilter(band,0);
    }
    else {
      quant2->setPositionFilter(&shipconv, 0);
    }
      MinicolShimOperator* children[2];
      children[0] = quant2conv;
      children[1] = &shipconv;
    mso = new MinicolShimOperator(NULL, 0, children, 2, 2);
  }
  else {
    // PosAnd
    band = new PosAndOperator(&shipconv, 0, quant2conv, 0);
    
    // MinicolShimOperator above PosAnd
    MinicolShimOperator* children[2];
    children[0] = &shipconv;
    children[1] = quant2conv;
    mso = new MinicolShimOperator(band, 0, children, 2, 2); // op, op_pos_col, children, nchildren, outwidth
  }  

   char* temp8 = new char[128];
   sprintf(temp8, "test%dPosAndMCshipnum%dlinenum%d.out", testnum, shipnum, linenum);  
   //SBlockPrinter* bPrint1 = NULL;
   BlockPrinter* bPrint2 = NULL;
   //Filter* filter;
  if (aggregate) {
  	HashSum* hashAgg;
	if (usePipelinedAlg)
	  hashAgg = new HashSum(mso, 0, mso, 1);
	else
	  hashAgg = new HashSum(mso, 1, mso, 0);
 	hashAgg->setHashFunction(new IdentityHashFunction(11000));
  	hashAgg->setHashTableSize(11000);
  	Operator* srcs[1] = {hashAgg};
  	int numCols[1] = {2};
  	bPrint2=new BlockPrinter(srcs, numCols, 1, temp8);
	//filter = new Filter(srcs, 1, NULL, 0, NULL); 
	//bPrint=new SBlockPrinter(filter, 2, temp8);
	bPrint2->printColumns(true); // skip output
	string reread = usePipelinedAlg ? "pipelined" : "parallel";
	printEndOfTest("PosAndMC-" + reread, testnum, shipnum, linenum, line_num_type, aggregate, bPrint2->getNumPrinted(), stopWatch.stop());
  } else { 
	Operator* srcs[1]={mso};
	int numCols[1]={2};
	
/*
	filter = new Filter(srcs, 1, NULL, 0, NULL); 
	bPrint1=new SBlockPrinter(filter, 2, temp8);
	bPrint1->printColumns();
	printEndOfTest("PosAndMC", testnum, shipnum, linenum, line_num_type, aggregate, bPrint1->getNumPrinted(), stopWatch.stop());
*/


	TupleGenerator tg(srcs, numCols, 1, 2);
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
	    /*temp = (int*)(sp->value);
	    if (usePipelinedAlg) {
	      for (int i=0; i < 1; i++) {
	    outstream << temp[1] << ",";
	    }
	    outstream << temp[0] << endl;
	    }
	    else {
	    for (int i=0; i < 1; i++) {
	    outstream << temp[0] << ",";
	    }
	    outstream << temp[1] << endl;
	    }*/
	  }
	}
	outstream.close();
	string reread = usePipelinedAlg ? "pipelined" : "parallel";
	printEndOfTest("PosAndMC-" + reread, testnum, shipnum, linenum, line_num_type, aggregate, nseen, stopWatch.stop());
  }
  
  return success;	
}
