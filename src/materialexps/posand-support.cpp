#include "posand-support.h"

DataSource* getLineNumIntDS(bool second_copy) {
	ROSAM* am = second_copy ? new ROSAM("LMLINENUMINT2", 1) : new ROSAM("LMLINENUMINT", 1);
	return new IntDataSource(am, false,true);
}

DataSource* getLineNumRLEDS(bool second_copy) {
	ROSAM* am = second_copy ? new ROSAM("LMLINENUMRLE2", 1) : new ROSAM("LMLINENUMRLE", 1);
	return new RLEDataSource(am, false);
}

DataSource* getLineNumT2DecompDS(bool second_copy) {
	ROSAM* am = second_copy ? new ROSAM("LMLINENUMT2Vals-2", 1) : new ROSAM("LMLINENUMT2Vals", 1);
  	return new Type2DecompDataSource(am, false, new UncompressedDecoder(false, ValPos::INTTYPE));
}

DataSource* getLineNumT2PosDS(bool second_copy) {
	ROSAM* am = second_copy ? new ROSAM("LMLINENUMT2Vals-2", 1) : new ROSAM("LMLINENUMT2Vals", 1);
  	return new Type2DataSource(am, false, new UncompressedDecoder(false, ValPos::INTTYPE));
}

DataSource* getLineNumDS(string type, bool second_copy) {
	if (type == "RLE") {
		return getLineNumRLEDS(second_copy);
	} else if (type == "INT") {
		return getLineNumIntDS(second_copy);
	} else if (type == "T2POS") {
		return getLineNumT2PosDS(second_copy);
	} else if (type == "T2DECOMP") {
		return getLineNumT2DecompDS(second_copy);
	} else {
		throw UnexpectedException("bad type");
	}
}

void printEndOfTest(string name, int testnum, int shipnum, int linenum, string line_num_type, bool aggregate, int nseen, double time) {	
  cout << name << " test " << testnum << " shipnum < " << shipnum << " linenum < " << linenum 
  	<< " type " << line_num_type << " aggregate " << aggregate << " took " << time << " ms read " 
  	<< nseen << " selectivity = " << nseen / 59986052.0 <<  endl;
}
