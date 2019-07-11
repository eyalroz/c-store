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
 * BOrOpTest: tests for BOrOperator
 *   ( Note: BOrOperator takes two datasources of bit-string/bit-vector(both should be position block) and
 *   generates position blocks of the "OR" result of the input datasources.
 *   datasource1: 0000111110000
 *   datasource2: 1100100111011
 *   BOrOperator of datasource1 and datasource2: 1100111111011 )
 *
 * Author: cstore-dev
 * 
 */
#include "BOrOpTest.h"
#include <iostream>
#include <algorithm>  // random_shuffle(), sort(), reverse() ...
#include <numeric>    // accumulate()
#include <iterator>   // ostream_iterator

using namespace std;


BOrOpTest::BOrOpTest() {}

BOrOpTest::~BOrOpTest() {}

/* args isn't used here */
bool BOrOpTest::run(Globals* g, const vector<string>& args) {
	
	Log::writeToLog("BOrOpTest", 10, "testSrc  BOr  testSrc2...");
	StopWatch* stopWatch = new StopWatch();
	stopWatch->start();

	Log::writeToLog("BOrOpTest", 10, "BOrOpTest starting...");
	bool success = true;
	
  //  left range = right range
	//  left	0-3  10-13  20-23  30-33...
	//  right 	0-3  10-13  20-23  30-33...	
	Log::writeToLog("BOrOpTest", 10, "left range = right range");
  success &= runTestCase1(0, 0, 0, 0);  
  success &= runTestCase2(0, 0, 0, 0); 
  
  //  left range > right range
  //  left  0-6  16-22  32-38...
  //  right 0-3  10-13  20-23  30-33... 
  Log::writeToLog("BOrOpTest", 10, "left range > right range");
  success &= runTestCase1(3, 0, 0, 0);
  success &= runTestCase2(3, 0, 0, 0);
  
  //  left range < right range
  //  left  0-3  10-13  20-23  30-33...
  //  right 0-6  10-16  20-26  30-36...
  Log::writeToLog("BOrOpTest", 10, "left range < right range");
  success &= runTestCase1(0, 3, 0, 0);
  success &= runTestCase2(0, 3, 0, 0);
      
  
  //  left range contains right range
  //  left  0-6  10-16  20-26  30-36...
  //  right 2-5  12-15  22-25  32-35...
  Log::writeToLog("BOrOpTest", 10, "left range contains right range");
  success &= runTestCase1(3, 0, 0, 2);  
  success &= runTestCase2(3, 0, 0, 2);  
	  
  //  right range contains left range

	// left:   3-7, 13-17, 23-27, 33-37
	//  right: 0-5, 14-19, 28-33, 42-47
	Log::writeToLog("BOrOpTest", 10, "right range contains left range");
  success &= runTestCase1(0, 2, 3, 0);
  success &= runTestCase2(0, 2, 3, 0);
        
  cout << "BOrOpTest took: " << stopWatch->stop() << " ms" <<  endl;
	delete stopWatch;
	
	return success;	
}

bool BOrOpTest::runTestCase1(unsigned int leftRunLengthDiff_, unsigned int rightRunLengthDiff_, unsigned int leftStartPosDiff_, unsigned int rightStartPosDiff_)
{ 
	const unsigned int numPosBlocks = 1000;
	const unsigned int runLength = 5;
	const unsigned int jumpFactor = 2;
		
	TestDataSrc* testSrc=new TestDataSrc();
	TestDataSrc* testSrc2=new TestDataSrc();

	//generate the data
	unsigned int leftRunLengthDiff, rightRunLengthDiff;
	unsigned int leftStartPosDiff, rightStartPosDiff;
	leftRunLengthDiff = leftRunLengthDiff_;
	rightRunLengthDiff = rightRunLengthDiff_;
	leftStartPosDiff = leftStartPosDiff_;
	rightStartPosDiff = rightStartPosDiff_;
	unsigned int numOfPosR = numPosBlocks*(runLength+rightRunLengthDiff)*jumpFactor;
	unsigned int numOfPosL = numPosBlocks*(runLength+leftRunLengthDiff)*jumpFactor;
	unsigned int numOfPos = (numOfPosL > numOfPosR ? numOfPosL : numOfPosR);
		
	unsigned int* pos = new unsigned int[numOfPos];
	unsigned int* pos2 = new unsigned int[numOfPos];
	unsigned int blockIndex = 0;	//zero-based
					 
	//testSrc
	unsigned int k = runLength+leftRunLengthDiff-1;	//repition
	for (unsigned int i=0; i<numOfPos; i++) {
		pos[i] = 0;
		unsigned int j = i % ((runLength+leftRunLengthDiff)*jumpFactor);
		if ((j == 0) && ((i+leftStartPosDiff+k-1)<numOfPos)) {
			PosRLEBlock* bBlock = new PosRLEBlock();
			RLETriple* rt = new RLETriple();
			rt = bBlock->getTriple();
			rt->startPos = i+leftStartPosDiff;
			rt->reps = k;
			//rt->setTriple(new IntValPos(i+leftStartPosDiff,1), i+leftStartPosDiff, k);
			bBlock->setTriple(rt);
			testSrc->addBlock(bBlock);
		}
		if ((j >= leftStartPosDiff) && (j <= (k+leftStartPosDiff))) {
			pos[i] = 1;
		}	
	}
		
	//testSrc2
	k = runLength-1+rightRunLengthDiff;	//repition
	for (unsigned int i=0; i<numOfPos; i++) {
		pos2[i] = 0;
		unsigned int j = i % ((runLength+rightRunLengthDiff)*jumpFactor);
		if ((j == 0) && ((i+rightStartPosDiff+k-1)<numOfPos)) {
			PosRLEBlock* bBlock = new PosRLEBlock();
			RLETriple* rt = new RLETriple();
			rt = bBlock->getTriple();
			rt->startPos = i+rightStartPosDiff;
			rt->reps = k;
			//rt->setTriple(new IntValPos(i+rightStartPosDiff,1), i+rightStartPosDiff, k);
			bBlock->setTriple(rt);
			testSrc2->addBlock(bBlock);
		}
		if ((j >= rightStartPosDiff) && (j < (k + rightStartPosDiff))) {
			pos2[i] = 1;
		}
	}
	
	/* debug			
	for (unsigned int i=0; i<numOfPos; ++i) {
		cout << "position: " << i << " " << pos[i] << " " << pos2[i] << endl; 
	};
	*/
		
	BOrOperator* borop = new BOrOperator(testSrc, 0, testSrc2, 0);	
	blockIndex = 0;
	unsigned int posInARun = 0;
	
	bool success_ = true;
	PosBlock* block = borop->getNextPosBlock();
		
	while (block != NULL) {
		// cout << "Block: " << blockIndex << endl;
		Log::writeToLog("BOrOpTest", 10, "Block: ", blockIndex);
		posInARun = 0;
		while (block->hasNext()) {
			unsigned int currPos=block->getNext();
			// cout << "postion: " << currPos << ' ' << pos[currPos] << ' ' << pos2[currPos] << endl;
			success_ &= ((pos[currPos] == 1) | (pos2[currPos] == 1));
			if (!success_) {
				cout << "\t \t BOr Operation failed" << endl;
				return success_;
			}
			posInARun++;
		}
		blockIndex++;
		block=borop->getNextPosBlock();
	}
		
	delete testSrc;
	delete testSrc2;
	delete [] pos;
	delete [] pos2;
		
	return success_;
}

//left: RLE     right:Basic
bool BOrOpTest::runTestCase2(unsigned int leftRunLengthDiff_, unsigned int rightRunLengthDiff_, unsigned int leftStartPosDiff_, unsigned int rightStartPosDiff_)
{ 
  const unsigned int numPosBlocks = 10;
  const unsigned int runLength = 5;
  const unsigned int jumpFactor = 2;
    
  TestDataSrc* testSrc=new TestDataSrc();
  TestDataSrc* testSrc2=new TestDataSrc();

  //generate the data
  unsigned int leftRunLengthDiff, rightRunLengthDiff;
  unsigned int leftStartPosDiff, rightStartPosDiff;
  leftRunLengthDiff = leftRunLengthDiff_;
  rightRunLengthDiff = rightRunLengthDiff_;
  leftStartPosDiff = leftStartPosDiff_;
  rightStartPosDiff = rightStartPosDiff_;
  unsigned int numOfPosR = numPosBlocks*(runLength+rightRunLengthDiff)*jumpFactor;
  unsigned int numOfPosL = numPosBlocks*(runLength+leftRunLengthDiff)*jumpFactor;
  unsigned int numOfPos = (numOfPosL > numOfPosR ? numOfPosL : numOfPosR);
    
  unsigned int* pos = new unsigned int[numOfPos];
  unsigned int* pos2 = new unsigned int[numOfPos];
  unsigned int blockIndex = 0;  //zero-based
           
  //testSrc
  unsigned int k = runLength+leftRunLengthDiff-1; //repition
  for (unsigned int i=1; i<=numOfPos; i++) {
    pos[i] = 0;
    unsigned int j = i % ((runLength+leftRunLengthDiff)*jumpFactor);
    if ((j == 1) && ((i+leftStartPosDiff+k-1)<=numOfPos)) {
      PosRLEBlock* bBlock = new PosRLEBlock();
      RLETriple* rt = new RLETriple();
      rt = bBlock->getTriple();
      rt->startPos = i+leftStartPosDiff;
      rt->reps = k;
      //rt->setTriple(new IntValPos(i+leftStartPosDiff,1), i+leftStartPosDiff, k);
      bBlock->setTriple(rt);
      testSrc->addBlock(bBlock);
    }
    if ((j > leftStartPosDiff) && (j <= (k+leftStartPosDiff))) {
      pos[i] = 1;
    } 
  }
    
  //testSrc2
  k = runLength-1+rightRunLengthDiff; //repition
  for (unsigned int i=1; i<=numOfPos; i++) {
    pos2[i] = 0;
    unsigned int j = i % ((runLength+rightRunLengthDiff)*jumpFactor);
    if ((j > rightStartPosDiff) && (j <= (k+rightStartPosDiff))) {
      PosBasicBlock* bBlock = new PosBasicBlock(true);
      bBlock->setPosition(i);
      testSrc2->addBlock(bBlock);
      pos2[i] = 1;
    } 
  } 
    
  // debug
  /*  
  blockIndex = 0;
  PosBlock* pb = testSrc->getNextPosBlock(0);
  while (pb != NULL) {
  cout << "Block: " << blockIndex << endl;
  while (pb->hasNext()) {
  unsigned int currPos=pb->getNext();
  cout << "TestSrc postion: " << currPos << ' ' << pos[currPos] << ' ' << pos2[currPos] << endl;
  }
  blockIndex++;
  pb=testSrc->getNextPosBlock(0);
  }
  
  cout << endl;
  blockIndex = 0;
  pb = testSrc2->getNextPosBlock(0);
  while (pb != NULL) {
  cout << "Block: " << blockIndex << endl;
  while (pb->hasNext()) {
  unsigned int currPos=pb->getNext();
  cout << "TestSrc2 postion: " << currPos << ' ' << pos[currPos] << ' ' << pos2[currPos] << endl;
  }
  blockIndex++;
  pb=testSrc2->getNextPosBlock(0);
  }
  
  for (unsigned int i=1; i<=numOfPos; ++i) {
  cout << "position: " << i << " " << pos[i] << " " << pos2[i] << endl; 
  };
  */      
  //
  BOrOperator* bop = new BOrOperator(testSrc, 0, testSrc2, 0);  
  blockIndex = 0;
  unsigned int posInARun = 0;
  
  bool success_ = true;
  PosBlock* block = bop->getNextPosBlock();
    
  while (block != NULL) {
    // cout << "Block: " << blockIndex << endl;
    Log::writeToLog("BOrOpTest", 10, "Block: ", blockIndex);
    posInARun = 0;
    while (block->hasNext()) {
      unsigned int currPos=block->getNext();
      //cout << "output postion: " << currPos << ' ' << pos[currPos] << ' ' << pos2[currPos] << endl;
      success_ &= ((pos[currPos] == 1) | (pos2[currPos] == 1));
      if (!success_) {
        cout << "\t \t BOr Operation failed" << endl;
        return success_;
      }
      posInARun++;
    }
    blockIndex++;
    block=bop->getNextPosBlock();
  }
    
  delete testSrc;
  delete testSrc2;
  delete [] pos;
  delete [] pos2;
    
  return success_;
}
