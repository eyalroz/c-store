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
#include "NSEncodeDecode.h"

NSEncodeDecode::NSEncodeDecode()
{
}

NSEncodeDecode::~NSEncodeDecode()
{
}

bool NSEncodeDecode::run(Globals* g, const vector<string>& args) {
	bool success=true;
	cout << "NSEncodeDecode: Simple two value test" << endl;
	TestDataSrc* dataSrc=new TestDataSrc();
	BasicBlock* currBlock=new BasicBlock(false, true, true);
	currBlock->setValue(4,6);
	dataSrc->addBlock(currBlock);

	currBlock=new BasicBlock(false, true, true);
	currBlock->setValue(5,7);
	dataSrc->addBlock(currBlock);

	currBlock=new BasicBlock(false, true, true);
	currBlock->setValue(6,8);
	dataSrc->addBlock(currBlock);

	currBlock=new BasicBlock(false, true, true);
	currBlock->setValue(7,8);
	dataSrc->addBlock(currBlock);

	
	currBlock=new BasicBlock(false, true, true);
	currBlock->setValue(10,10);
	dataSrc->addBlock(currBlock);
	
	NSEncoder* encoder=new NSEncoder(dataSrc, 0);
	NSDecoder* decoder=new NSDecoder(false);
	decoder->setBuffer(encoder->getPage());
	success&=test("Start Int: ", true, *(int*)decoder->getStartVal()->value, 4);
	success&=test("Start Pos: ", true, decoder->getStartPos(), 6);
	success&=test("End Int: ", true, *(int*)decoder->getEndVal()->value, 10);
	success&=test("End Pos: ", true, decoder->getEndPos(), 10);
	success&=test("Skip to non existing", NULL==decoder->skipToBlock(5), 0, 0);
	
	
	currBlock=(BasicBlock*) decoder->getNextBlock();
	while (currBlock!=NULL) {
		ValPos* pair=currBlock->getNext();
		assert(pair->type == ValPos::INTTYPE);
		cout << "(" << *(int*)pair->value << "," <<  pair->position << ")" << endl;
		currBlock=(BasicBlock*) decoder->getNextBlock();
	}

	
	delete dataSrc;
	delete encoder;
	delete decoder;
	
	

	cout << "NS: Randomized Multi Page Encoding and Decoding test" << endl;	
	dataSrc=new TestDataSrc();
	unsigned int val=341;
	int numIntsWritten=0;
	int values[PAGE_SIZE];
	unsigned int startPos=421;
	unsigned int currPos=startPos;
	
	for (int i=0; i<PAGE_SIZE; i++) {
		BasicBlock* currBlock=new BasicBlock(false, true, true);
		currBlock->setValue(val, currPos);
		dataSrc->addBlock(currBlock);
		values[numIntsWritten]=val;
		val=rand();
		currPos++;
		numIntsWritten++;
	}

	encoder=new NSEncoder(dataSrc, 0);
	byte* page=encoder->getPage();
	decoder=new NSDecoder(page, false);

	bool valueSuccess=true;
	unsigned int counter=0;
	int numMisses=0;
	int numPages=0;
	Block* currB=decoder->getNextBlock();	
	while (page!=NULL) {
		while (currB!=NULL) {
		  ValPos* pair;
		  while (pair=currB->getNext()) {
		    assert(pair->type == ValPos::INTTYPE);
		    valueSuccess&=(values[counter]==*(int*)pair->value);
		    valueSuccess&=(startPos+counter==pair->position);
		    if (((values[counter]!=*(int*)pair->value) || (startPos+counter!=pair->position)) && (numMisses<10)) {
		      cout << "G: (" << *(int*)pair->value << ","<< pair->position << ")"
			   << "\t E: (" <<  values[counter] << "," << startPos+counter << ")" << endl;
		      numMisses++;
		    }
		    counter++;
		  }
		  currB=decoder->getNextBlock();
		}
		page=encoder->getPage();
		if (page!=NULL) decoder->setBuffer(page);
		numPages++;
		currB=(BasicBlock*) decoder->getNextBlock();
	}
	cout << "Testing all ints match on decoding: ";
	if (valueSuccess) cout << "Matched on all " << counter << " values" << endl;
	else cout << "Failed to match some/all of " << counter << " values" << endl;
	cout << "Num pages encoded/decoded: " << numPages << endl;

	success&=valueSuccess;
	

	return success;	
}
