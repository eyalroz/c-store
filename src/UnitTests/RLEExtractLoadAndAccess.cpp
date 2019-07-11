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
#include "RLEExtractLoadAndAccess.h"

RLEExtractLoadAndAccess::RLEExtractLoadAndAccess()
{
}

RLEExtractLoadAndAccess::~RLEExtractLoadAndAccess() {
	
}

bool RLEExtractLoadAndAccess::run(Globals* g, const vector<string>& args) {
	bool success=true;

	string path=RLE_EXTRACT_LOAD_AND_ACCESS_DATA_PATH;
	ColumnExtracter* ce = new ColumnExtracter(path, 1, g->force_rebuild);
	RLEEncoder* encoder=new RLEEncoder((Operator*) ce, 0, 8*PAGE_SIZE, (byte) INT_VALUE_TYPE, (short) 16, (unsigned int) 27, (unsigned int) 16);
	RLEDecoder* decoder=new RLEDecoder(true);
	PagePlacer* pagePlacer=new PagePlacer(encoder, decoder, 2, false);
	pagePlacer->placeColumn("RLEExtractLoadAndAccessDATA", false, true);
	delete pagePlacer;
	system("mv RLEExtractLoadAndAccessDATA* " RUNTIME_DATA);
	ROSAM* am = new ROSAM( "RLEExtractLoadAndAccessDATA" , 2);
	RLEDataSource* ds=new RLEDataSource(am,true);
	int count=0;
	Block* blk=ds->getNextValBlock(0);
	while (blk!=NULL) {
//		RLETriple* trip=((RLEBlock*) blk)->getTriple();
//		cout << "Triple #" << count << " val=" << trip->value
//									<< " \tstPos=" << trip->startPos
//									<< " \tReps=" << trip->reps << endl;
		count++;
		blk=ds->getNextValBlock(0);
	}
	success&=test("# Triples read: ", true, count, 31);
	delete ds;
	delete am;

		
	cout << "### Testing DataSource with Predicates ###" << endl;
	// Column: starts at 8036, ends at 8066	
	am = new ROSAM( "RLEExtractLoadAndAccessDATA" , 2);
	ds=new RLEDataSource(am,true);
	Predicate* pred=new Predicate(Predicate::OP_GREATER_THAN);
	ds->setPredicate(pred);
	ds->changeRHSBinding(8045);
	count=0;
	blk=ds->getNextValBlock(0);

	while (blk!=NULL) {
		//RLETriple* trip=((RLEBlock*) blk)->getTriple();
		//cout << "Triple #" << count << " val=" << trip->value
		//								<< " \tstPos=" << trip->startPos
		//								<< " \tReps=" << trip->reps << endl;
		count++;
		blk=ds->getNextValBlock(0);
		
	}
	success&=test("# Triples read: ", true, count, 21);

	delete ds;
	delete am;
	delete pred;
	
	cout << "### Testing DataSource with PosFilter ###" << endl;
	// Column: starts at 8036, ends at 8066
	
	TestDataSrc* dataSrc=new TestDataSrc();
	dataSrc->addBlock(new PosRLEBlock(new RLETriple(new IntValPos(3000,1),30000,10000)));
	dataSrc->addBlock(new PosRLEBlock(new RLETriple(new IntValPos(45000,1),45000,1000)));
	// For lshipdatemini, I know this matches for 5 triples

	
	
	am = new ROSAM( "RLEExtractLoadAndAccessDATA" , 2);
	ds=new RLEDataSource(am,true);
	ds->setPositionFilter(dataSrc, 0);
	count=0;
	blk=ds->getNextValBlock(0);
	while (blk!=NULL) {
	  //RLETriple* trip=((RLEBlock*) blk)->getTriple();
	  //assert(trip->value->type == ValPos::INTTYPE);
	  //cout << "Triple #" << count << " val=" << *(int*)trip->value->value << " \tstPos=" << trip->startPos << " \tReps=" << trip->reps << " \tEndPos=" << trip->startPos+trip->reps-1 << endl;
		count++;
		blk=ds->getNextValBlock(0);
	}
	success&=test("# Triples read: ", true, count, 5);


	delete ds;
	//delete am;
	delete dataSrc;


	cout << "### Testing PosBlocks on DataSource with Predicates ###" << endl;
	// Column: starts at 8036, ends at 8066	
	am = new ROSAM( "RLEExtractLoadAndAccessDATA" , 2);
	ds=new RLEDataSource(am,true);
	pred=new Predicate(Predicate::OP_GREATER_THAN);
	ds->setPredicate(pred);
	ds->changeRHSBinding(8045);
	count=0;
	PosBlock* posBlk=ds->getNextPosBlock(0);
	while (posBlk!=NULL) {
		//RLETriple* trip=((PosRLEBlock*) posBlk)->getTriple();
		//cout << "Triple #" << count << " val=" << trip->value
		//								<< " \tstPos=" << trip->startPos
		//								<< " \tReps=" << trip->reps << endl;
		count++;
		posBlk=ds->getNextPosBlock(0);
		
	}
	success&=test("# Triples read: ", true, count, 1);

	delete ds;
	delete pred;
	//system ("rm RLEExtractLoadAndAccessDATA*");	
	return success;	
}

bool RLEExtractLoadAndAccess::test(char* msg_, int retBool_, int val_, int exp_) {
	using namespace std;
	if (retBool_) {
		cout << msg_ << " X: " << val_ << "\tE[X]: " << exp_ << " \t";
		if (val_==exp_) {
			cout << "SUCCESS" << endl;
			return true;
		}
		else {
			cout << "FAILED" << endl;
			return false;
		}
	}
	else {
		cout << "FAILED (function return failed)" <<  endl;
		return false;
	}	
}


