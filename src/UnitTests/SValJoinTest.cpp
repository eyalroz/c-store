#include "SValJoinTest.h"

SValJoinTest::SValJoinTest()
{
}

SValJoinTest::~SValJoinTest()
{
}

bool SValJoinTest::run(Globals* g, const vector<string>& args) {
	ROSAM* leftR = getROSAM();
	ROSAM* rightR = getROSAM();
	ROSAM* leftR2 = getROSAM();
	ROSAM* rightR2 = getROSAM();
	
	// bools are valSorted, isROS
	IntDataSource* leftIntSrc = new IntDataSource(leftR, true, true);
	IntDataSource* rightIntSrc = new IntDataSource(rightR, true, true);
	IntDataSource* leftIntSrc2 = new IntDataSource(leftR2, true, true);
	IntDataSource* rightIntSrc2 = new IntDataSource(rightR2, true, true);

	Operator* leftOps[] = {leftIntSrc, leftIntSrc2};
	Operator* rightOps[] = {rightIntSrc, rightIntSrc2};

	Concat* leftSrc = new Concat(leftOps, 2, 2);
	Concat* rightSrc = new Concat(rightOps, 2, 2);
	NLJoin* join = new NLJoin(leftSrc, 1, rightSrc, 0); // col indices are join cols
	TuplePrinter printer(join);
	//printer.printTuples(2); // 1->nattrs
	printer.tossTuples();
	return true;
}

ROSAM* SValJoinTest::getROSAM() {
	int nindices = 2;
	return new ROSAM("D2_UNCOMPRESSED_ORDERDATE", nindices);
	//return new ROSAM(DATAFILE, nindices);
}
