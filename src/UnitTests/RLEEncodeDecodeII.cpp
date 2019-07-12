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
#include "RLEEncodeDecodeII.h"

RLEEncodeDecodeII::RLEEncodeDecodeII()
{}
RLEEncodeDecodeII::~RLEEncodeDecodeII()
{}


bool RLEEncodeDecodeII::run(Globals* g, const vector<string>& args) {
	using namespace std;
	bool success=true;
	cout << "*** RLE Encoding and Decoding simple test ***" << endl;
	
	cout << "--- Encoding page with the encoder - 1086 bits ---" << endl;
	cout << "                                   - type   =1" << endl
		 << "                                   - size   =4" << endl
		 << "                                   - reps   =5" << endl;
		 
	TestDataSrc* dataSrc=new TestDataSrc();
	dataSrc->addBlock(new RLEBlock(new RLETriple(new IntValPos(4,1),4,5), true));

	RLEEncoderII* encoder=new RLEEncoderII(dataSrc,0,1086, 1, 4, 5);


	cout << "-I  : " << " Encoding triple (1, 4, 5)" << endl;
	RLEDecoderII* decoder=new RLEDecoderII(encoder->getPage(), true);		
		  		   		 
	success&=test("Start Int \t", true, *(int*)decoder->getStartVal()->value, 1);
	success&=test("Start Pos \t", true, decoder->getStartPos(), 4);
	success&=test("End Int \t", true, *(int*)decoder->getEndVal()->value, 1);
	success&=test("End Pos \t", true, decoder->getEndPos(), 8);

	byte* pagePtr=encoder->getPage();
	delete decoder;
	delete encoder;
	delete pagePtr;	
	delete dataSrc;	
	cout << "*** RLE Encoding and Decoding randomized test ***" << endl;	
	
	dataSrc=new TestDataSrc();
	unsigned int val=0;
	unsigned int lastVal=0;
	unsigned int currentPos=0;
	unsigned int oldPos=0;
	unsigned int reps=2;
	unsigned int oldReps=2;
	int numTriplesWritten=0;
	int values[2500];
	unsigned int starts[2500];
	unsigned int repits[2500];
	for (int i=0; i<2000; i++) {
		dataSrc->addBlock(new RLEBlock(new RLETriple(new IntValPos(currentPos, val),currentPos,reps),false));
		values[numTriplesWritten]=val;
		starts[numTriplesWritten]=currentPos;
		repits[numTriplesWritten]=reps;		
		lastVal=val;
		oldPos=currentPos;
		oldReps=reps;

		val=rand()&0xF;
		if (val==lastVal) {
			val++;
			val=val&0xF;
		}
		currentPos+=reps;
		reps=((rand())&0x3F);
		if (reps==0)
			reps=1;
		numTriplesWritten++;
	}
	cout << "--- Encoding page with the encoder - 10862 bits ---" << endl;
	cout << "                                   - type   =1" << endl
		 << "                                   - size   =4" << endl
		 << "                                   - reps   =6" << endl;


	encoder=new RLEEncoderII(dataSrc,0,10862, 1, 4, 6);
		 
	byte* page=encoder->getPage();					
	decoder=new RLEDecoderII(page, true);		
	
	bool triplesSuccess=true;
	int counter=0;


	bool flag=true;
	cout << "Testing all triples match on decoding: ";
	RLEBlock*	block=((RLEBlock*) decoder->getNextBlock());
	RLETriple* triple;
	if (block==NULL) flag=false; 
	else triple=block->getTriple();
	while (flag) {
	  assert(triple->value->type == ValPos::INTTYPE);
		triplesSuccess&=(values[counter]==*(int*)triple->value->value);
		triplesSuccess&=(starts[counter]==triple->startPos);
		triplesSuccess&=(repits[counter]==triple->reps);
		
		//cout << "Was: (" << triple->value << "," << triple->startPos << "," << triple->reps
		//	 << ") E: (" << values[counter] << "," << starts[counter] << "," << repits[counter] << endl;
		counter++;
		block=((RLEBlock*) decoder->getNextBlock());
		if (block==NULL) flag=false; 
		else triple=block->getTriple();
	}
	if (triplesSuccess) cout << "Matched on all " << counter << " triples" << endl;
	else cout << "Failed to match some/all of " << counter << " triples" << endl;

	test("Triple's fields", triplesSuccess, 0, 0);
	success&=triplesSuccess;
	int expNumTriples=(10862-32-16-64)/(4+6);
	success&=test("NumTriples\t",true, counter, (10862-32-16-64)/(4+6));
	success&=test("Start Int \t",true, *(int*)decoder->getStartVal()->value, 0);
	success&=test("Start Pos \t",true, decoder->getStartPos(), 0);
	success&=test("End Int   \t",true, *(int*)decoder->getEndVal()->value, values[expNumTriples-1]);
	success&=test("End Pos   \t",true, decoder->getEndPos(), starts[expNumTriples]-1);
	
	return success;
}


bool RLEEncodeDecodeII::test(const char* msg_, int retBool_, int val_, int exp_) {
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

