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
#include "DeltaPosWriterTest.h"

DeltaPosWriterTest::DeltaPosWriterTest()
{
}

DeltaPosWriterTest::~DeltaPosWriterTest()
{
	
}


bool DeltaPosWriterTest::run(Globals* g, const vector<string>& args) {
	bool success=true;
	
	cout << endl << "****DeltaPos Two Value test: " << endl << endl;
	//success&=twoValTest();
	
	cout << endl << "****DeltaPos Large Delta test: " << endl << endl;
	//success&=largeDeltaTest();
	
	cout << endl << "****DeltaPos Large Delta test: " << endl << endl;
	success&=randomDeltaTest();
	
	return success; 
}

// This test encodes many positions for 3 values, with more than one page for each
bool DeltaPosWriterTest::randomDeltaTest() {
	bool success=true;
	TestDataSrc* src=new TestDataSrc();
	unsigned int position[2000][3];
	position[0][0]=500;
	position[0][1]=1000;
	position[0][2]=1500;
	
	for (int val=0; val<3; val++) {
		for (int i=1; i<2000; i++) {
			position[i][val]=rand()%1024+position[i-1][val]+1;
		}
	}
	for (int i=0; i<2000; i++) {
		for (int val=0; val<3; val++) {
			BasicBlock* blck=new BasicBlock(false, true, true);
			blck->setValue(new IntValPos(position[i][val], val));
			src->addBlock(blck);
		}	
	}
	
	DeltaPosWriter* writer=new DeltaPosWriter(src, 0, 1024, 5);
	DeltaPosBlock* block=(DeltaPosBlock*) writer->getNextValBlock(0);
	ValPos* pair=NULL;
	bool blockSuccess=true;
	int numBlocks=0;	
	int pos[3]={0,0,0};
	while (block!=NULL) {
		numBlocks++;
		int val=block->getValue();
		while (block->hasNext()) {
			pair=block->getNext();
			// cout << "Read: " << pair->value << "," << pair->position << " Should be: " << position[pos[val]][val] << endl;
			blockSuccess&=(pair->position==position[pos[val]][val]);
			pos[val]++;
		}
		block=(DeltaPosBlock*) writer->getNextValBlock(0);	
	}
	if (blockSuccess) 
		cout << "Blocks matched random position vectors" <<  endl;
	else 
		cout << "Blocks did NOT matched random position vectors" <<  endl;
	success&=blockSuccess;
	cout << "Read a total of: " << numBlocks << " blocks" << endl;
	success&=test("Number of positions read for val=0", true, pos[0], 2000);
	success&=test("Number of positions read for val=1", true, pos[1], 2000);
	success&=test("Number of positions read for val=2", true, pos[2], 2000);	
	return success;
}

bool DeltaPosWriterTest::largeDeltaTest() {
	bool success=true;
	TestDataSrc* src=new TestDataSrc();
	int val=5;
	for (int i=0; i<1000; (i%2==0) ? i+=201 : i++) {
		BasicBlock* blck=new BasicBlock(false, true, true);
		blck->setValue(new IntValPos(i, val));
		src->addBlock(blck);
	}
	
	DeltaPosWriter* writer=new DeltaPosWriter(src, 0, 500, 5);
	DeltaPosBlock* block=(DeltaPosBlock*) writer->getNextValBlock(0);	
	bool blockSuccess=true;
	for (unsigned int i=0; i<1000; (i%2==0) ? i+=201 : i++) {
		ValPos* pair=block->getNext();		
		blockSuccess&=(pair->position==i);
	}
	
	success&=test("Test of positions (mixed small/large deltas)", blockSuccess, 0, 0);
	return success;
}
bool DeltaPosWriterTest::twoValTest() {
	bool success=true;
	
	TestDataSrc* src=new TestDataSrc();
	int val=5;
	for (int i=0; i<10; i++) {
		BasicBlock* blck=new BasicBlock(false, true, true);
		blck->setValue(new IntValPos(i, val));
		src->addBlock(blck);
	}
	val++;
	for (int i=10; i<15; i++) {
		BasicBlock* blck=new BasicBlock(false, true, true);
		blck->setValue(new IntValPos(i, val));
		src->addBlock(blck);
	}


	DeltaPosWriter* writer=new DeltaPosWriter(src, 0, 500, 5);
	DeltaPosBlock* block=(DeltaPosBlock*) writer->getNextValBlock(0);	
	success&=test("Block value: ", true, block->getValue(), 5);

	bool blockSuccess=true;
	for (unsigned int i=0; i<10; i++) {
		ValPos* pair=block->getNext();		
		blockSuccess&=(pair->position==i);
	}
	success&=test("Test of positions in value=5", blockSuccess, 0, 0);

	block=(DeltaPosBlock*) writer->getNextValBlock(0);	
	success&=test("Block value: ", true, block->getValue(), 6);
	blockSuccess=true;
	for (unsigned int i=10; i<15; i++) {
		ValPos* pair=block->getNext();		
		blockSuccess&=(pair->position==i);
	}
	success&=test("Test of positions in value=6", blockSuccess, 0, 0);
	return success;
}


bool DeltaPosWriterTest::test(char* msg_, int retBool_, int val_, int exp_) {
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
