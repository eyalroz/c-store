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
#include "IntExtractAndLoad.h"

IntExtractAndLoad::IntExtractAndLoad() : UnitTest()
{
}

IntExtractAndLoad::~IntExtractAndLoad()
{
}

bool IntExtractAndLoad::run(Globals* g, const vector<string>& args) {
  //system("rm IntExtractAndLoadDAT*");	
	bool success=true;

	string path=SHIPDATE_MINI;
	ColumnExtracter* ce = new ColumnExtracter(path, 1, g->force_rebuild);
	IntEncoder* encoder=new IntEncoder((Operator*) ce, 0, 8*PAGE_SIZE);
	IntDecoder* decoder=new IntDecoder(true);
	PagePlacer* pagePlacer=new PagePlacer(encoder, decoder, 2, true);
	pagePlacer->placeColumn("IntExtractAndLoadDATA", false, true);

	delete pagePlacer;
	system("mv IntExtractAndLoadDATA* " RUNTIME_DATA);
	int count=0;
	int total=0;
	int prevVal=-1;

	
	ROSAM* am = new ROSAM( "IntExtractAndLoadDATA" , 2);
	IntDataSource* ds=new IntDataSource(am, true, true);


	Block* blk=ds->getNextValBlock(0);
	
	while (blk!=NULL) {
	  while (blk->hasNext()) {
	    ValPos* vp = blk->getNext();
	    assert(vp->type == ValPos::INTTYPE);
	    int currVal=*(int*)vp->value;
	    if (prevVal!=currVal)  {
	      if (count%10==0) cout  << endl << "Read unique value: ";
	      cout << currVal << ", ";
	      count++;
	      prevVal=currVal;
	    }
	  }
	  blk=ds->getNextValBlock(0);
	  total++;
	}
	cout << endl;
	cout << "Total number of blocks read: " << total << endl;
	success&=test("# Distinct Values read: ", true, count, 31);
	delete ds;
	delete am;
	
	cout << "### Testing DataSource with Predicates ###" << endl;
	// Column: starts at 8036, ends at 8066
	
	am = new ROSAM( "IntExtractAndLoadDATA" , 2);
	ds=new IntDataSource(am, true, true);
	Predicate* pred=new Predicate(Predicate::OP_GREATER_THAN);
	ds->setPredicate(pred);
	ds->changeRHSBinding(8045);
	count=0;
	total=0;
	prevVal=-1;
	blk=ds->getNextValBlock(0);
	while (blk!=NULL) {
	  while (blk->hasNext()) {
	    ValPos* vp = blk->getNext();
	    assert(vp->type == ValPos::INTTYPE);
	    int currVal=*(int*)vp->value;
	    if (prevVal!=currVal)  {
	      if (count%10==0) cout  << endl << "Read uniques value: ";
	      cout << currVal << ", ";
	      count++;
	      prevVal=currVal;
	    }
	  }	  
	  blk=ds->getNextValBlock(0);
	  total++;
	  
	}
	cout << endl;
	cout << "Total number of blocks read: " << total << endl;
	success&=test("# Distinct Values read: ", true, count, 21);
	
	delete ds;
	delete pred;
	delete am;
	
	am = new ROSAM( "IntExtractAndLoadDATA" , 2);
	ds=new IntDataSource(am, false, true);
	TestDataSrc* dataSrc=new TestDataSrc();
	dataSrc->addBlock(new PosRLEBlock(new RLETriple(new IntValPos(30000,1),30000,10000)));
//	dataSrc->addBlock(new PosRLEBlock(new RLETriple(1,45000,1000)));
	dataSrc->addBlock(new PosRLEBlock(new RLETriple(new IntValPos(70000,1),70000,1000)));
	ds->setPositionFilter(dataSrc, 0);
	count=0;
	total=1;
	prevVal=-1;
	blk=ds->getNextValBlock(0);
	while (blk!=NULL) {
	  while (blk->hasNext()) {
	    ValPos* vp = blk->getNext();
	    assert(vp->type == ValPos::INTTYPE);
	    int currVal=*(int*)vp->value;
	    if (prevVal!=currVal)  {
	      if (count%10==0) cout  << endl << "Read uniques value: ";
	      cout << currVal << ", ";
	      count++;
	      prevVal=currVal;
	    }
	  }
	  blk=ds->getNextValBlock(0);
	  total++;
	}
	cout << endl;
	cout << "Total number of blocks read: " << total << endl;
	success&=test("# Distinct Values read: ", true, count, 5);
	
	
	//system ("rm IntExtractAndLoadDAT*");
	return success;
	
}
