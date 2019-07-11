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
/**
 * BNotOpTest: tests for BNotOperator
 *
 * Author: cstore-dev
 * 
 */
#include "BNotOpTest.h"
#include <iostream>
#include <algorithm>  // random_shuffle(), sort(), reverse() ...
#include <numeric>    // accumulate()
#include <iterator>   // ostream_iterator

using namespace std;

BNotOpTest::BNotOpTest() {}

BNotOpTest::~BNotOpTest() {}

/* args isn't used here */
bool BNotOpTest::run(Globals* g, const vector<string>& args) {
	
	Log::writeToLog("BNotOpTest", 10, "BNOT testSrc...");
	StopWatch* stopWatch = new StopWatch();
	stopWatch->start();

	Log::writeToLog("BNotOpTest", 10, "BNotOpTest starting...");
	bool success = true;
	
	//  postion 1-30
	//  1-5  11-15  21-25...
	Log::writeToLog("BNotOpTest", 10, "1-5 11-15...");
	success &= runTestCase1(0, 0);	//RLE
	success &= runTestCase2(0, 0);	//Basic		
	
	//  position 1-42
	//  1-7  15-22  29-35...
	Log::writeToLog("BNotOpTest", 10, "1-7 15-22...");
	success &= runTestCase1(2, 0);	//RLE
	success &= runTestCase2(2, 0);	//Basic
	
	//  position 1-42
	//  5-10	18-24	32-38...
	Log::writeToLog("BNotOpTest", 10, "5-10 18-24 32-38...");
	success &= runTestCase1(2, 3);	//RLE
	success &= runTestCase2(2, 3);	//Basic
	
	cout << "BNotOpTest took: " << stopWatch->stop() << " ms" <<  endl;
	delete stopWatch;
	
	return success;	
}

//PosRLEBlock
bool BNotOpTest::runTestCase1(unsigned int inputRunLengthDiff_, unsigned int inputStartPosDiff_)
{ 
	const unsigned int numPosBlocks = 3;
	const unsigned int runLength = 5;
	const unsigned int jumpFactor = 2;
		
	TestDataSrc* testSrc=new TestDataSrc();

	//generate the data
	unsigned int inputRunLengthDiff;
	unsigned int inputStartPosDiff;
	inputRunLengthDiff = inputRunLengthDiff_;
	inputStartPosDiff = inputStartPosDiff_;
	unsigned int numOfPos = numPosBlocks*(runLength+inputRunLengthDiff)*jumpFactor;
		
	unsigned int* pos = new unsigned int[numOfPos];
	unsigned int blockIndex = 0;	//zero-based
					 
	//testSrc
	unsigned int k = runLength+inputRunLengthDiff;	//repition
	for (unsigned int i=1; i<=numOfPos; i++) {
		pos[i] = 0;
		unsigned int j = i % ((runLength+inputRunLengthDiff)*jumpFactor);
		if ((j == 1) && ((i+inputStartPosDiff+k-1)<numOfPos)) {
			PosRLEBlock* bBlock = new PosRLEBlock();
			RLETriple* rt = new RLETriple();
			rt = bBlock->getTriple();
			rt->startPos = i+inputStartPosDiff;
			rt->reps = k;
			//rt->setTriple(new IntValPos(i+inputStartPosDiff, 1), i+inputStartPosDiff, k);
			bBlock->setTriple(rt);
			testSrc->addBlock(bBlock);
		}
		if ((j > inputStartPosDiff) && (j <= (k+inputStartPosDiff))) {
			pos[i] = 1;
		}	
	}
	testSrc->setLastPosition((int)numOfPos);
	
	/*
	for (unsigned int i=1; i<=numOfPos; ++i) {
		cout << "position: " << i << " " << pos[i] << " " << endl; 
	};
	*/
		
	BNotOperator* bnotop = new BNotOperator(testSrc, 0);
	blockIndex = 0;
	unsigned int posInARun = 0;
	
	bool success_ = true;
	PosBlock* block = bnotop->getNextPosBlock();		
	while (block != NULL) {
		//cout << "Block: " << blockIndex << endl;
		Log::writeToLog("BNotOpTest", 10, "Block: ", blockIndex);
		posInARun = 0;
		while (block->hasNext()) {
			unsigned int currPos=block->getNext();
			//cout << "postion: " << currPos << ' ' << pos[currPos] << endl;
			success_ &= ((pos[currPos] == 0));
			if (!success_) {
				cout << "\t \t BNot Operation failed" << endl;
				return success_;
			}
			posInARun++;
		}
		blockIndex++;
		block=bnotop->getNextPosBlock();
	}
		
	delete testSrc;
	delete [] pos;
		
	return success_;
}

//PosBasicBlock
bool BNotOpTest::runTestCase2(unsigned int inputRunLengthDiff_, unsigned int inputStartPosDiff_)
{ 
	const unsigned int numPosBlocks = 3;
	const unsigned int runLength = 5;
	const unsigned int jumpFactor = 2;
		
	TestDataSrc* testSrc=new TestDataSrc();

	//generate the data
	unsigned int inputRunLengthDiff;
	unsigned int inputStartPosDiff;
	inputRunLengthDiff = inputRunLengthDiff_;
	inputStartPosDiff = inputStartPosDiff_;
	unsigned int numOfPos = numPosBlocks*(runLength+inputRunLengthDiff)*jumpFactor;
		
	unsigned int* pos = new unsigned int[numOfPos];
	unsigned int blockIndex = 0;	//zero-based
					 
	//testSrc
	unsigned int k = runLength+inputRunLengthDiff;	//repition
	for (unsigned int i=1; i<=numOfPos; i++) {
		pos[i] = 0;
		unsigned int j = i % ((runLength+inputRunLengthDiff)*jumpFactor);
		if ((j > inputStartPosDiff) && (j <= (k+inputStartPosDiff))) {
			PosBasicBlock* bBlock = new PosBasicBlock(true);
			bBlock->setPosition(i);
			testSrc->addBlock(bBlock);
			pos[i] = 1;
		}	
	}
	testSrc->setLastPosition((int)numOfPos);	
  /*
	for (unsigned int i=1; i<=numOfPos; ++i) {
		cout << "position: " << i << " " << pos[i] << " " << endl; 
	};
  */
	
	BNotOperator* bnotop = new BNotOperator(testSrc, 0);
	blockIndex = 0;
	unsigned int posInARun = 0;
	
	bool success_ = true;
	PosBlock* block = bnotop->getNextPosBlock();
	while (block != NULL) {
		//cout << "Block: " << blockIndex << endl;
		Log::writeToLog("BNotOpTest", 10, "Block: ", blockIndex);
		posInARun = 0;
		while (block->hasNext()) {
			unsigned int currPos=block->getNext();
			//cout << "postion: " << currPos << ' ' << pos[currPos] << endl;
			success_ &= ((pos[currPos] == 0));
			if (!success_) {
				//cout << "\t \t BNot Operation failed" << endl;
				return success_;
			}
			posInARun++;
		}
		blockIndex++;
		block=bnotop->getNextPosBlock();
	}
	
	delete testSrc;
	delete [] pos;
		
	return success_;

}
