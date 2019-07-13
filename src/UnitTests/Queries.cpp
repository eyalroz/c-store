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
#include "Queries.h"

Queries::Queries()
{
}

Queries::~Queries()
{
}


bool Queries::run(Globals* g, const vector<string>& args) {

 if(args.size() < 1){
    cout << "Queries::run: Improper argument" <<endl;
    return false;
  }

  //be careful: if args[0] isn't an integer string
  //atoi will return 0. So recommend having num_ >=1
  //as validation check. 
 
  int num_ = atoi(args[0].c_str());

	cout << "Running Query2: " << endl;
	DataSource* ds1=NULL;
	DataSource* ds2=NULL;
	string queryName="Q2";
	string name1;
	string name2;
	ROSAM* am1=NULL;
	ROSAM* am2=NULL;
	if (num_>=4) {
		name1="RleShipdate.BDB";
		am1 = new ROSAM( name1 , 2);
		ds1=new RLEDataSource(am1,true);
	}
	else if (num_>=8) {
		name1="RleIIShipdate.BDB";
		am1 = new ROSAM( name1 , 2);
		ds1=new RLEIIDataSource(am1,true);
	}
	else if (num_>=16) {
		name1="NSShipdate.BDB";
		am1 = new ROSAM( name1 , 2);
		ds1=new NSDataSource(am1, true);
	}
	else if (num_>=32) {
		name1="LZShipdate.BDB";
		am1 = new ROSAM( name1 , 2);
		ds1=new LZDataSource(am1, true);
	}
	else if (num_>=64) {
		name1="IntShipdate.BDB";
		am1 = new ROSAM( name1 , 2);
		ds1=new IntDataSource(am1, false, true);	
	}		
	
	if ((num_&0x03)==0) {
		name2="NSSuppkey.BDB";
		am2 = new ROSAM( name2 , 2);
		ds2=new NSDataSource(am2, false);	
	} 
	else if	((num_&0x03)==1) {
		name2="LZSuppkey.BDB";
		am2 = new ROSAM( name2 , 2);
		ds2=new LZDataSource(am2, false);			
	}
	else if ((num_&0x03)==2) {
		name2="IntSuppkey.BDB";
		am2 = new ROSAM( name2 , 2);
		ds2=new IntDataSource(am2, false, true);					
	}
	
	StopWatch stopWatch;
  	stopWatch.start();
  	Predicate* pred=new Predicate(Predicate::OP_EQUAL);
  	ds1->setPredicate(pred);
  	ds1->changeRHSBinding(9000);
	Count* agg = new Count((Operator*) ds1, 0, (Operator*) ds2, 0);
  	Operator* srcs[1]={agg};
  	int numCols[1]={2};
  	string outFileName=(queryName+name1+name2+".out");
   	BlockPrinter* bPrint=new BlockPrinter(srcs, numCols, 1, (char*) outFileName.c_str());
   	bPrint->printColumns();
	cout << queryName << " " << name1 << " " << name2 << " Took" << stopWatch.stop() << " ms" <<  endl;
	return true;	
}
