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
#include "LZEncodeDecode.h"


LZEncodeDecode::LZEncodeDecode()
{
}

LZEncodeDecode::~LZEncodeDecode()
{
}

bool LZEncodeDecode::run(Globals* g, const vector<string>& args) {
	bool success=true;
	cout << "LZEncodeDecodeTest: Simple two value test" << endl;
	TestDataSrc* dataSrc;
	BasicBlock* currBlock;
	byte* page;
	
	LZEncoder* encoder;
	LZDecoder* decoder;
	/*
	  dataSrc=new TestDataSrc();

	currBlock=new BasicBlock(false, true, true);
	currBlock->setValue(4,6);
	dataSrc->addBlock(currBlock);
	
	currBlock=new BasicBlock(false, true, true);
	currBlock->setValue(10,7);
	dataSrc->addBlock(currBlock);
	
	encoder=new LZEncoder(dataSrc, 0, 100000/sizeof(int));
	decoder=new LZDecoder(false);
		
	page=encoder->getPage();
	decoder->setBuffer(page); 
	
	success&=test("Start Int: ", true, decoder->getStartInt(), 4);
	success&=test("Start Pos: ", true, decoder->getStartPos(), 6);
	success&=test("End Int: ", true, decoder->getEndInt(), 10);
	success&=test("End Pos: ", true, decoder->getEndPos(), 7);
	success&=test("Skip to non existing", false==decoder->skipToPos(2), 0, 0);
	delete dataSrc;
	delete encoder;
	delete decoder;
	*/
		cout << "Int: Randomized Multi Page Encoding and Decoding test" << endl;	
	dataSrc=new TestDataSrc();
	unsigned int val=1;
	int numIntsWritten=0;
	int values[100000];
	unsigned int startPos=1;
	unsigned int currPos=startPos;
	
	for (int i=0; i<2000; i++) {
		BasicBlock* currBlock=new BasicBlock(false, true, true);
		currBlock->setValue(val, currPos);
		dataSrc->addBlock(currBlock);
		values[numIntsWritten]=val;
		if (i < 10)
		  cout << "val is: " << val  << endl;
		val=rand()%100;
		currPos++;
		numIntsWritten++;
	}
	
	
	encoder=new LZEncoder(dataSrc, 0, 65000/sizeof(int));
	decoder=new LZDecoder(false);

	
	page=encoder->getPage();
	decoder->setBuffer(page);

	bool valueSuccess=true;
	int counter=0;
	int numPages=0;
	int diffCount=0;
	currBlock=(BasicBlock*) decoder->getNextBlock();	
	while (page!=NULL) {
		while (currBlock!=NULL) {
			ValPos* pair=currBlock->getNext();
			assert(pair->type == ValPos::INTTYPE);
			valueSuccess&=(values[counter]==*(int*)pair->value);
			if ((!valueSuccess) && (diffCount<12)) {
				cout << "Count:" << counter << " G: " << *(int*)pair->value << "\t E: " <<  values[counter] << endl;
				diffCount++;
			}
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
