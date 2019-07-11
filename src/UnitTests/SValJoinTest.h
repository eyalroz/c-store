#ifndef SVALJOINTEST_H_
#define SVALJOINTEST_H_

#include "UnitTest.h"
#include "../AM/AM.h"
#include "../AM/ROSAM.h"
#include "../Paths.h"
#include "../Operators/EMTuplePrinter.h"
#include "../Operators/Concat.h"
#include "../Operators/Operator.h"
#include "../Operators/NLJoin.h"
#include "../Wrappers/IntDataSource.h"
#include "../common/Util.h"

#define DATAFILE "../dsmdata/emtest.data"

class SValJoinTest : public UnitTest
{
public:
	SValJoinTest();
	virtual ~SValJoinTest();
	bool run(Globals* g, const vector<string>& args);
protected:
	ROSAM* getROSAM();
};

#endif /*SVALJOINTEST_H_*/
