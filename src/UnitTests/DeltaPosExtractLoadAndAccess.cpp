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
#include "DeltaPosExtractLoadAndAccess.h"

DeltaPosExtractLoadAndAccess::DeltaPosExtractLoadAndAccess() : UnitTest()
{
}

DeltaPosExtractLoadAndAccess::~DeltaPosExtractLoadAndAccess()
{
}


bool DeltaPosExtractLoadAndAccess::run(Globals* g, const vector<string>& args) {   	
	system("rm DeltaPosTestDAT*");

 	cout << endl << "DeltaPos Extracting and loading: ";   	
	bool success=true;
	string path=DELTAPOS_TEST;
	ColumnExtracter* ce = new ColumnExtracter(path, 1, g->force_rebuild);
	DeltaPosEncoder* encoder=new DeltaPosEncoder((Operator*) ce, 0, 29);
	DeltaPosDecoder* decoder=new DeltaPosDecoder(false);
	PagePlacer* pagePlacer=new PagePlacer(encoder, decoder, 1, false);
	pagePlacer->placeColumn("DeltaPosTestDATA", true, false);
	delete pagePlacer;
	delete decoder;
	delete ce;
	delete encoder;
	cout << "Complete" << endl;
	system("mv DeltaPosTestDATA* " RUNTIME_DATA);
	ROSAM* am;
	DeltaPosDataSource* ds;
	DeltaPosBlock* blk;
	int numValues;
	Predicate* pred;
	BasicBlock* basicBlk;
	
	
	cout << endl << "DeltaPosDataSource Normal Mode W/out Predicate: " << endl;
	
	am = new ROSAM( "DeltaPosTestDATAVals", 1);
	ds=new DeltaPosDataSource(am, false);
	numValues=0;
	blk=(DeltaPosBlock*) ds->getNextValBlock(0);
	cout << "(Value,Size): "; 
	while (blk!=NULL) {
	  //while (blk->hasNext()) {
	    cout << "(" << blk->getNext()->value << "," << blk->getSize() << "), ";
	    numValues++;
	    //}
	  blk=(DeltaPosBlock*) ds->getNextValBlock(0);
	}
	cout << endl;
	success&=test("numValues read: ", true, numValues, 4);
	delete ds;
	delete am;	
	
	cout << endl << "DeltaPosDataSource Normal Mode W/ Predicate: " << endl;
	
	am = new ROSAM( "DeltaPosTestDATAVals", 1);
	ds=new DeltaPosDataSource(am, false);
	pred=new Predicate(Predicate::OP_GREATER_THAN);
	ds->setPredicate(pred);
	ds->changeRHSBinding(2);
	numValues=0;
	blk=(DeltaPosBlock*) ds->getNextValBlock(0);
	cout << "(Value,Size): "; 
	while (blk!=NULL) {
		cout << "(" << blk->getValue() << "," << blk->getSize() << "), ";
		numValues++;
		blk=(DeltaPosBlock*) ds->getNextValBlock(0);
	}
	cout << endl;
	success&=test("numValues read: ", true, numValues, 2);
	delete ds;
	delete am;

	cout << endl << "DeltaPosDataSource Pos Contig Mode W/out Predicate: " << endl;
	
	am = new ROSAM( "DeltaPosTestDATAVals", 1);
	ds=new DeltaPosDataSource(am, false);
	numValues=0;
	basicBlk=(BasicBlock*) ds->getNextValBlock(1);
	cout << "Values=";
	while (basicBlk!=NULL) {
		cout << basicBlk->getValue() << ","; 
		numValues++;
		basicBlk=(BasicBlock*) ds->getNextValBlock(1);
	}
	cout << endl;
	success&=test("numValues read: ", true, numValues, 16);
	delete ds;
	delete am;
	
	cout << endl << "DeltaPosDataSource Pos Contig Mode W/ Predicate: " << endl;
	
	am = new ROSAM( "DeltaPosTestDATAVals", 1);
	ds=new DeltaPosDataSource(am, false);
	pred=new Predicate(Predicate::OP_GREATER_THAN);
	ds->setPredicate(pred);
	ds->changeRHSBinding(2);
	numValues=0;
	basicBlk=(BasicBlock*) ds->getNextValBlock(1);
	cout << "Values=";
	while (basicBlk!=NULL) {
		cout << basicBlk->getValue() << ","; 
		numValues++;
		basicBlk=(BasicBlock*) ds->getNextValBlock(1);
	}
	cout << endl;
	success&=test("numValues read: ", true, numValues, 6);
	delete ds;
	delete am;
	
	cout << endl << "Same Tests, a lot more data (300mb): " << endl;
	
 	cout << endl << "DeltaPos Extracting and loading: ";   	
	success=true;
	path=DELTAPOS_TEST_BIG;
	ce = new ColumnExtracter(path, 1, g->force_rebuild);
	encoder=new DeltaPosEncoder((Operator*) ce, 0, 5);
	decoder=new DeltaPosDecoder(false);
	pagePlacer=new PagePlacer(encoder, decoder, 2, false);
	pagePlacer->placeColumn("DeltaPosTestDATABIG", true, false);
	delete pagePlacer;
	delete decoder;
	
	system("mv DeltaPosTestDATABIG* " RUNTIME_DATA);

	
	cout << endl << "DeltaPosDataSource Normal Mode W/out Predicate: " << endl;
	
	am = new ROSAM( "DeltaPosTestDATABIGVals", 1);
	ds=new DeltaPosDataSource(am, false);
	numValues=0;
	blk=(DeltaPosBlock*) ds->getNextValBlock(0);
	cout << "(Value,Size): "; 
	while (blk!=NULL) {
		cout << "(" << blk->getValue() << "," << blk->getSize() << "), ";
		numValues++;
		blk=(DeltaPosBlock*) ds->getNextValBlock(0);
	}
	cout << endl;
	success&=test("numValues read: ", true, numValues, 16);
	delete ds;
	delete am;	
	
	cout << endl << "DeltaPosDataSource Normal Mode W/ Predicate: " << endl;
	
	am = new ROSAM( "DeltaPosTestDATABIGVals", 1);
	ds=new DeltaPosDataSource(am, false);
	pred=new Predicate(Predicate::OP_GREATER_THAN);
	ds->setPredicate(pred);
	ds->changeRHSBinding(2);
	numValues=0;
	blk=(DeltaPosBlock*) ds->getNextValBlock(0);

	while (blk!=NULL) {
		numValues++;
		blk=(DeltaPosBlock*) ds->getNextValBlock(0);
	}

	success&=test("numValues read: ", true, numValues, 8);
	delete ds;
	delete am;

	cout << endl << "DeltaPosDataSource Pos Contig Mode W/out Predicate: " << endl;
	
	am = new ROSAM( "DeltaPosTestDATABIGVals", 1);
	ds=new DeltaPosDataSource(am, false);
	numValues=0;
	basicBlk=(BasicBlock*) ds->getNextValBlock(1);

	while (basicBlk!=NULL) {

		numValues++;
		basicBlk=(BasicBlock*) ds->getNextValBlock(1);
	}

	success&=test("numValues read: ", true, numValues, 1280000);
	delete ds;
	delete am;
	
	cout << endl << "DeltaPosDataSource Pos Contig Mode W/ Predicate: " << endl;
	
	am = new ROSAM( "DeltaPosTestDATABIGVals", 1);
	ds=new DeltaPosDataSource(am, false);
	pred=new Predicate(Predicate::OP_GREATER_THAN);
	ds->setPredicate(pred);
	ds->changeRHSBinding(2);
	numValues=0;
	basicBlk=(BasicBlock*) ds->getNextValBlock(1);

	while (basicBlk!=NULL) {
		numValues++;
		basicBlk=(BasicBlock*) ds->getNextValBlock(1);
	}
	success&=test("numValues read: ", true, numValues, 640000);
	delete ds;
	delete am;
	
	system("rm DeltaPosTestDAT*");	
	return success;
}
