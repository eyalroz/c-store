#include "EMTest.h"

TupwidthTest::TupwidthTest()
{
}

TupwidthTest::~TupwidthTest()
{
}

bool TupwidthTest::run(Globals* g, const vector<string>& args) {
	makeData();
	doTest(1);
	/*for (int i = 1; i <= NSOURCES; ++i) {
		system("./flushCache");
		doTest(i);
	}*/
	return true;
}

void TupwidthTest::doTest(int ntouse) {
	
	cerr << "Starting new TupwidthTest with ntouse = " << ntouse << endl;

	StopWatch stopWatch;
 	stopWatch.start();

	ROSAM** rosams = new ROSAM*[NSOURCES];
	IntDataSource** dataSources = new IntDataSource*[NSOURCES];
	for (int i = 0; i < NSOURCES; i++) {
		rosams[i] = getROSAM(i);
		dataSources[i] = new IntDataSource(rosams[i], true, true);
	}

	Concat concat((Operator**)dataSources, NSOURCES, ntouse); // total inputs, num to use
	
	// STUFF FOR TESTING TUPLEPOSGET
	/*IntDataSource* newCol = new IntDataSource(getROSAM(0), true, true);
	TuplePosGet* tpget = new TuplePosGet(&concat, newCol); 
	TuplePrinter tp(tpget);
	*/
			
	// NORMAL CODE
	TuplePrinter tp(&concat);
	
	cerr << "Go for toss" << endl;
	//tp.printTuples(ntouse);
	//tp.printTuples(2);
	//tp.tossTuples();
	
	cout << "EMTest with ntouse = " << ntouse 
		<< " took " << stopWatch.stop() << " ms." << endl;
	
	for (int i = 0; i < NSOURCES; i++) {
		delete dataSources[i];
		delete rosams[i]; // DataSource destructor does this?
	}
	delete[] rosams;
	delete[] dataSources;
}

ROSAM* TupwidthTest::getROSAM(int i) {
	int nindices = 2;
	char filename[100];
	sprintf(filename, "D2_UNCOMPRESSED_ORDERDATE.%d", i);
	cerr << "Open: " << filename << endl;
	//return new ROSAM(filename, nindices);
//	return new ROSAM("D2_UNCOMPRESSED_ORDERDATE", nindices);
	return new ROSAM(DATAFILE, nindices);
}

void TupwidthTest::makeData() {
    cerr << "Generating TupwidthTest data" << endl;
    ColumnExtracter* ce = new ColumnExtracter(RAW_DATAFILE,
					      1,                 // column index
					      true); // force rebuild flag
    IntEncoder* encoder = new IntEncoder((Operator*) ce, // data source
					 0,              // column index
					 8*PAGE_SIZE);   // buffer size in bits
    IntDecoder* decoder = new IntDecoder(true);         // value sorted

    PagePlacer* pagePlacer = new PagePlacer(encoder,
					    decoder, 
					    2,       // num indexes
					    true);  // position primary ??? (ignored)
    pagePlacer->placeColumn(DATAFILE, // name
			    false,          // split on value
			    true);          // value sorted

    delete pagePlacer;
    delete ce;
    delete encoder;
    delete decoder;
    
    cerr << "Done generating data" << endl;
}
