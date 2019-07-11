#ifndef EMTEST_H_
#define EMTEST_H_

#include "UnitTest.h"
#include "../AM/AM.h"
#include "../AM/ROSAM.h"
#include "../Paths.h"
#include "../Operators/EMTuplePrinter.h"
#include "../Operators/Concat.h"
#include "../Operators/Operator.h"
#include "../Wrappers/IntDataSource.h"
#include "../common/Util.h"
#include "../Operators/ColumnExtracter.h"
#include "../Operators/Sum.h"
#include "../Wrappers/Encoder/IntEncoder.h"
#include "../Wrappers/Encoder/RLEEncoder.h"
#include "../Wrappers/Decoder/IntDecoder.h"
#include "../Wrappers/Decoder/RLEDecoder.h"
#include "../Wrappers/RLEDataSource.h"
#include "../AM/PagePlacer.h"
//#include "../Operators/TuplePosGet.h"

#define RAW_DATAFILE "../dsmdata/emtest.in"
#define DATAFILE "../dsmdata/emtest.data"

class TupwidthTest : public UnitTest
{
public:
	TupwidthTest();
	virtual ~TupwidthTest();
	bool run(Globals* g, const vector<string>& args);
private:
	static const int NSOURCES = 5;
	void doTest(int ntouse);
	void makeData();
	ROSAM* getROSAM(int i);
};

#endif /*EMTEST_H_*/
