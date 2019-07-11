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
#include "PosBlockTest.h"
// Tests all Position Blocks (RLE, Delta, and Bit)
// Tests Writers and Blocks
// Problems: migfer@mit.edu

PosBlockTest::PosBlockTest()
{
}

PosBlockTest::~PosBlockTest()
{
}

bool PosBlockTest::run(Globals* g, const vector<string>& args) {
	bool success=true;
	//runTest(WriterType type_, unsigned int bfrSizeInBits_, unsigned int deltaSize_, unsigned int numValues_, bool sequential_)

	cout << "\t> Delta, 50 bit buffer (151 w/ header), 4 bit field, 50 sequential positions" << endl;
	success&=runTest(PosBlockTest::DELTA, 151, 4, 50, true);

	cout << "\t> Delta, 50 bit buffer (151 w/ header), 4 bit field, 50 non sequential positions" << endl;
	success&=runTest(PosBlockTest::DELTA, 151, 4, 50, false);

	cout << "\t> RLE sequential, 50 positions" << endl;
	success&=runTest(PosBlockTest::RLE, 0, 0, 20, true);

	cout << "\t> RLE not sequential, 50 positions" << endl;
	success&=runTest(PosBlockTest::RLE, 0, 0, 50, false);

	return success;
}

bool PosBlockTest::runTest(WriterType type_, unsigned int bfrSizeInBits_, unsigned int deltaSize_, unsigned int numValues_, bool sequential_) {
	bool success=true;
	// First generate the data
	TestDataSrc* testSrc=new TestDataSrc();
	
	unsigned int pos[numValues_+1];
	for (unsigned int i=1; i<=numValues_; i++) {
		if (sequential_) {
			pos[i]=i;
		}
		else {
			pos[i]=i*10+(rand()&0x9);
		}
		BasicBlock* bBlock=new BasicBlock((type_==RLE), true, true);
		bBlock->setValue(rand()%1000, pos[i]);
		testSrc->addBlock(bBlock);
	}	
	
	
	Writer* writer=NULL;
	
	if (type_==PosBlockTest::RLE) {
		writer=new PosRLEWriter(testSrc, 0);
	}
	else {
		writer=new PosDeltaWriter(testSrc, 0, bfrSizeInBits_, deltaSize_);
	}
	
	// now run it through writer
	unsigned int count=1;
	unsigned int numBlocks=0;
	bool blockSuccess=true;
	PosBlock* block=writer->getNextPosBlock(0);
	while (block!=NULL) {
		//cout << "New Block" <<  endl;
		while (block->hasNext()) {
			unsigned int currPos=block->getNext();
			//cout << "Positions Read: " << currPos << " E is " << pos[count] << endl;
			blockSuccess&=(currPos==pos[count]);
			count++;
		}
		numBlocks++;
		block=writer->getNextPosBlock(0);
	}
	if (!blockSuccess)
		cout << "\t \t Error positions did not match those coded in Block" << endl;
	success&=blockSuccess;
	
	if ((count-1)!=numValues_) {
		cout << "\t \t Error: not all positions were coded" << endl;
		success=false;
	}
	
	if (success)
		cout << "\t \t Successful: num blocks coded was: " << numBlocks << endl;
	
	
	return success;
}
