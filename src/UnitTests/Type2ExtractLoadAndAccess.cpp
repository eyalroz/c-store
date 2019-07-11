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
#include "Type2ExtractLoadAndAccess.h"

Type2ExtractLoadAndAccess::Type2ExtractLoadAndAccess() : UnitTest()
{
}

Type2ExtractLoadAndAccess::~Type2ExtractLoadAndAccess()
{
}


bool Type2ExtractLoadAndAccess::run(Globals* g, const vector<string>& args) {   	
  //system("rm Type2TestDAT*");

 	cout << endl << "Type2 Extracting and loading: ";   	
	bool success=true;
	string path=DELTAPOS_TEST;
	ColumnExtracter* ce = new ColumnExtracter(path, 1, g->force_rebuild);
	Type2Encoder* encoder=new Type2Encoder((Operator*) ce, 0, 29);
	Type2Decoder* decoder=new Type2Decoder(false, encoder->getValueIndex());
	PagePlacer* pagePlacer=new PagePlacer(encoder, decoder, 1, false);
	pagePlacer->placeColumn("Type2TestDATA", true, false);
	delete pagePlacer;
	delete decoder;
	delete ce;
	delete encoder;
	cout << "Complete" << endl;
	//system("mv Type2TestDATA* RuntimeData/");
	ROSAM* am;
	Type2DataSource* ds;
	Type2Block* blk;
	int numValues;
	Predicate* pred;
	//BasicBlock* basicBlk;
	
		
	cout << endl << "Type2DataSource Normal Mode W/out Predicate: " << endl;
	
	UncompressedDecoder* undec=new UncompressedDecoder(false, ValPos::INTTYPE);
	am = new ROSAM( "Type2TestDATAVals", 1);
	ds=new Type2DataSource(am, false, undec);
	numValues=0;
	blk=(Type2Block*) ds->getNextValBlock(0);
	cout << "(Value,Size): ";
	ValPos* p;
	while (blk!=NULL) {
	  cout << "(" << blk->getValue() << "," << blk->getSize() << "), " << endl;
	  numValues++;
	  while (blk->hasNext()) {
	    p = blk->getNext();
	    assert(p->type == ValPos::INTTYPE);
	    cout << "(" << *(int*)p->value << "," << p->position << "), ";
	  }
	  cout << endl;
	  blk=(Type2Block*) ds->getNextValBlock(0);
	}
	cout << endl;
	success&=test("numValues read: ", true, numValues, 4);
	delete ds;
	delete am;	
	
	cout << endl << "Type2DataSource Normal Mode W/ Predicate: " << endl;
	
	am = new ROSAM( "Type2TestDATAVals", 1);
	ds=new Type2DataSource(am, false, undec);
	pred=new Predicate(Predicate::OP_GREATER_THAN);
	ds->setPredicate(pred);
	ds->changeRHSBinding(2);
	numValues=0;
	blk=(Type2Block*) ds->getNextValBlock(0);
	cout << "(Value,Size): "; 
	while (blk!=NULL) {
		cout << "(" << blk->getValue() << "," << blk->getSize() << "), ";
		numValues++;
		blk=(Type2Block*) ds->getNextValBlock(0);
	}
	cout << endl;
	success&=test("numValues read: ", true, numValues, 2);
	delete ds;
	delete am;
	
	cout << endl << "Type2DataSource Pos Contig Mode W/out Predicate: " << endl;
	
	am = new ROSAM( "Type2TestDATAVals", 1);
	Type2DecompDataSource* ds2=new Type2DecompDataSource(am, false, undec);
	numValues=0;
	Block* mb=ds2->getNextValBlock(1);
	cout << "Values=";
	while (mb!=NULL) {
	  while (mb->hasNext()) {
	    cout << *(int*)mb->getNext()->value << ","; 
	    numValues++;
	  }
	  mb=ds2->getNextValBlock(1);
	}
	cout << endl;
	success&=test("numValues read: ", true, numValues, 16);
	delete ds2;
	delete am;
	
	cout << endl << "Type2DataSource Pos Contig Mode W/ Predicate: " << endl;
	
	am = new ROSAM( "Type2TestDATAVals", 1);
	ds2=new Type2DecompDataSource(am, false, undec);
	pred=new Predicate(Predicate::OP_GREATER_THAN);
	ds2->setPredicate(pred);
	ds2->changeRHSBinding(2);
	numValues=0;
	mb=ds2->getNextValBlock(1);
	cout << "Values=";
	while (mb!=NULL) {
	  while (mb->hasNext()) {
		cout << *(int*)mb->getNext()->value << ","; 
		numValues++;
	  }
	  mb=ds2->getNextValBlock(1);
	}
	cout << endl;
	success&=test("numValues read: ", true, numValues, 6);
	delete ds2;
	delete am;
	
	cout << endl << "Same Tests, a lot more data (300mb): " << endl;
	
 	cout << endl << "Type2 Extracting and loading: ";   	
	success=true;
	path=DELTAPOS_TEST_BIG;
	ce = new ColumnExtracter(path, 1, g->force_rebuild);
	encoder=new Type2Encoder((Operator*) ce, 0, 5);
	decoder=new Type2Decoder(false, encoder->getValueIndex());
	pagePlacer=new PagePlacer(encoder, decoder, 2, false);
	pagePlacer->placeColumn("Type2TestDATABIG", true, false);
	delete pagePlacer;
	delete decoder;
	
	//system("mv Type2TestDATABIG* RuntimeData/");

	
	cout << endl << "Type2DataSource Normal Mode W/out Predicate: " << endl;
	
	am = new ROSAM( "Type2TestDATABIGVals", 1);
	ds=new Type2DataSource(am, false, undec);
	numValues=0;
	blk=(Type2Block*) ds->getNextValBlock(0);
	cout << "(Value,Size): "; 
	while (blk!=NULL) {
		cout << "(" << blk->getValue() << "," << blk->getSize() << "), ";
		numValues++;
		blk=(Type2Block*) ds->getNextValBlock(0);
	}
	cout << endl;
	success&=test("numValues read: ", true, numValues, 6);
	delete ds;
	delete am;	
	
	cout << endl << "Type2DataSource Normal Mode W/ Predicate: " << endl;
	
	am = new ROSAM( "Type2TestDATABIGVals", 1);
	ds=new Type2DataSource(am, false, undec);
	pred=new Predicate(Predicate::OP_GREATER_THAN);
	ds->setPredicate(pred);
	ds->changeRHSBinding(2);
	numValues=0;
	blk=(Type2Block*) ds->getNextValBlock(0);

	while (blk!=NULL) {
		numValues++;
		blk=(Type2Block*) ds->getNextValBlock(0);
	}

	success&=test("numValues read: ", true, numValues, 3);
	delete ds;
	delete am;

	cout << endl << "Type2DataSource Pos Contig Mode W/out Predicate: " << endl;
	
	am = new ROSAM( "Type2TestDATABIGVals", 1);
	ds2=new Type2DecompDataSource(am, false, undec);
	numValues=0;
	mb=ds2->getNextValBlock(1);

	while (mb!=NULL) {
	  while (mb->hasNext()) {
	    mb->getNext();
		numValues++;
	  }
	  mb=ds2->getNextValBlock(1);
	}

	success&=test("numValues read: ", true, numValues, 1280000);
	delete ds2;
	delete am;
	
	cout << endl << "Type2DataSource Pos Contig Mode W/ Predicate: " << endl;
	
	am = new ROSAM( "Type2TestDATABIGVals", 1);
	ds2=new Type2DecompDataSource(am, false, undec);
	pred=new Predicate(Predicate::OP_GREATER_THAN);
	ds2->setPredicate(pred);
	ds2->changeRHSBinding(2);
	numValues=0;
	mb=ds2->getNextValBlock(1);

	while (mb!=NULL) {
	  while (mb->hasNext()) {
	    mb->getNext();
		numValues++;
	  }
	  mb=ds2->getNextValBlock(1);
	}
	success&=test("numValues read: ", true, numValues, 640000);
	delete ds2;
	delete am;
	
	delete undec;
	//system("rm Type2TestDAT*");	
	return success;
}
