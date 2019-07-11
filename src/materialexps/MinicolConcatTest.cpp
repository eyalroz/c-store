#include "MinicolConcatTest.h"

MinicolConcatTest::MinicolConcatTest()
{
}

MinicolConcatTest::~MinicolConcatTest()
{
}

bool MinicolConcatTest::run(Globals* g, const vector<string>& args) {
	cerr << "Starting MinicolConcatTest" << endl;
	IntDataSource* intDS = getUncompressedSuppkey();
	IntDataSource* intDS2 = getUncompressedSuppkey();
	
	BlockWithPosToMinicolConverter bpconv(intDS, "intDS");
	BlockWithPosToMinicolConverter bpconv2(intDS2, "intDS2");
	
//	BAndOperator band(&bpconv, 0, &bpconv2, 0);
//	PassthroughOperator band(&bpconv, &bpconv2);
	PosAndOperator band(&bpconv, 0, &bpconv2, 0);
	MinicolShimOperator* children[2];
	children[0] = &bpconv;
	children[1] = &bpconv2;
	
	MinicolShimOperator msop(&band, 0, children, 2, 2);
	
/*	Operator* bpconv_op = &bpconv;
	MinicolShimOperator* oparr[1];
	oparr[0] = &bpconv;
	
	cerr << "bpconv's addr is: " << &bpconv << endl;
	
	MinicolShimOperator msop(bpconv_op, 0, oparr, 1, 1);
	*/
	int count = 0;
	
	Block* b;
	Block* b2;
	ValPos *p1, *p2;
	while((b = msop.getNextValBlock(0)) != NULL) {
		b2 = msop.getNextValBlock(1);
		while(b->hasNext()) {
			p1 = b->getNext();
			p2 = b2->getNext();
			if (*p1 != p2 || p1->position != p2->position) {
				cerr << "Mismatch!" << endl;
				throw UnexpectedException();
			}
			//cerr << "Read: " << p1->value << " == " << p2->value << endl;
//			b->getNext();
			++count;
		}
	}
	cerr << "Finished reading a minicol block; read to date is of: " << count << endl;
	cerr << "Total read was of: " << count << endl;
	cerr << "Should be 59986052" << endl;
		
	
	/*
	Block* b;
	count = 0;
	while (b = intDS2->getNextValBlock(0)) {
		while (b->hasNext()) {
			b->getNext();
			++ count;
		}
	}
	cerr << "Actual value: " << count << endl;*/
	return true;
}
	
/*
bool MinicolConcatTest::run(Globals* g, const vector<string>& args) {
	cerr << "Starting MinicolConcatTest" << endl;
	//ROSAM* am = new ROSAM("D1_RLE_SHIPDATE",2);
	//RLEDataSource* rleDS = new RLEDataSource(am, true);
	//Operator* inputs = {rleDS}; 
	
	IntDataSource *intDS = getUncompressedSuppkey();
	Operator* inputs = {intDS};

	DataSource* ds = intDS;


	int* colidxs = {0};
	MinicolConcat* minicolConcat = new MinicolConcat(inputs, colidxs);
	Block* b;
	int count = 0;
	
	while ( (b = ds->getNextValBlock(0)) != NULL) {
		cerr << "Read block = " << b << endl << " size = " << b->getSize() <<
			" size in bits = " << b->getSizeInBits() << " splittable = " <<
			b->isSplittable() << endl;
		
		Block *b2 = b->split(142);
		while (b->hasNext()) {
			b->getNext();
			++count;
		}
		while(b2->hasNext()) {
			b2->getNext();
			++count;
		}
		break; // only do one block
	}
	cerr << "Count: " << count << endl;
	return true;	
}
*/

IntDataSource* MinicolConcatTest::getUncompressedSuppkey() {
	//ROSAM* am = new ROSAM("EM_UNCOMPRESSED_SUPPKEY", 2);
	//IntDataSource* ds = new IntDataSource(am, false, true); // is sorted, is ROS
//	ROSAM* am = new ROSAM("D2_UNCOMPRESSED_SUPPKEY", 1);
	ROSAM* am = new ROSAM("D1_UNCOMPRESSED_SUPPKEY", 1);
	IntDataSource* ds = new IntDataSource(am, false, true);
	//ds->setSingles(true);
	return ds;
}

#if 0
	ROSAM* am = new ROSAM("D1_RLE_SHIPDATE",2);
	RLEDataSource* rleDS = new RLEDataSource(am, true);
	
	IntDataSource *intDS = getUncompressedSuppkey();
	
	
	MinicolMaterializer* materializer = new MinicolMaterializer(intDS, rleDS);
	//MinicolMaterializer* materializer = new MinicolMaterializer(rleDS, intDS);
	
	materializer->updateMinicolBlock();

	Block* b;
	int sz = 0;
	while ( (b = materializer->getNextValBlock(0)) != NULL) {
		sz += b->getSize();
		cerr << "MinicolConcatTest: read block size = " << b->getSize() << " total now " << sz << endl;
		/*while(b->hasNext()) {
			cerr << b->getNext()->value << endl;
		}*/
	}
	cerr << "Total read was of: " << sz << endl;
#endif
