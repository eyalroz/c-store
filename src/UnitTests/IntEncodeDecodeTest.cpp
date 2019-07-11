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
#include "IntEncodeDecodeTest.h"

IntEncodeDecodeTest::IntEncodeDecodeTest()
{
}

IntEncodeDecodeTest::~IntEncodeDecodeTest()
{
}

bool IntEncodeDecodeTest::run(Globals* g, const vector<string>& args) {
	bool success=true;
	cout << "IntEncodeDecodeTest: Simple two value test" << endl;
	TestDataSrc* dataSrc=new TestDataSrc();
	BasicBlock* currBlock=new BasicBlock(false, true, true);
	currBlock->setValue(4,6);
	dataSrc->addBlock(currBlock);
	
	currBlock=new BasicBlock(false, true, true);
	currBlock->setValue(10,7);
	dataSrc->addBlock(currBlock);
	
	IntEncoder* encoder=new IntEncoder(dataSrc, 0, 800);
	IntDecoder* decoder=new IntDecoder(encoder->getPage(), false);
	
	success&=test("Start Int: ", true, *(int*)decoder->getStartVal()->value, 4);
	success&=test("Start Pos: ", true, decoder->getStartPos(), 6);
	success&=test("End Int: ", true, *(int*)decoder->getEndVal()->value, 10);
	success&=test("End Pos: ", true, decoder->getEndPos(), 7);
	success&=test("Skip to non existing", false==decoder->skipToPos(2), 0, 0);
	delete dataSrc;
	delete encoder;
	delete decoder;
	
	cout << "Int: Randomized Multi Page Encoding and Decoding test" << endl;	
	dataSrc=new TestDataSrc();
	unsigned int val=0;
	int numIntsWritten=0;
	int values[100000];
	unsigned int startPos=421;
	unsigned int currPos=startPos;
	
	for (int i=0; i<3000; i++) {
		BasicBlock* currBlock=new BasicBlock(false, true, true);
		currBlock->setValue(val, currPos);
		dataSrc->addBlock(currBlock);
		values[numIntsWritten]=val;
		val=rand();
		currPos++;
		numIntsWritten++;
	}
	
	encoder=new IntEncoder(dataSrc, 0, 8*PAGE_SIZE);
	byte* page=encoder->getPage();
	decoder=new IntDecoder(false);
	if (page==NULL) cout << "Error page was NULL" << endl;
	decoder->setBuffer(page);

	bool valueSuccess=true;
	unsigned int counter=0;
	int numPages=0;
	currBlock=(BasicBlock*) decoder->getNextBlock();	
	while (page!=NULL) {
		while (currBlock!=NULL) {
			ValPos* pair=currBlock->getNext();
			assert(pair->type ==ValPos::INTTYPE);
			valueSuccess&=(values[counter]==*(int*)pair->value);
			// cout << "Got: " << pair->value << "\t\t E: " <<  values[counter] << endl;
			valueSuccess&=(startPos+counter==pair->position);
			counter++;
			currBlock=(BasicBlock*) decoder->getNextBlock();
		}
		page=encoder->getPage();
		if (page!=NULL) decoder->setBuffer(page);
		numPages++;
		currBlock=(BasicBlock*) decoder->getNextBlock();
	}
	cout << "Testing all ints match on decoding: ";
	if (valueSuccess) cout << "Matched on all " << counter << " values" << endl;
	else cout << "Failed to match some/all of " << counter << " values" << endl;
	cout << "Num pages encoded/decoded: " << numPages << endl;

	success&=valueSuccess;

	return success;
}
