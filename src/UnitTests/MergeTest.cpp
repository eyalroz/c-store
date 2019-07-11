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
#include "MergeTest.h"

MergeTest::MergeTest() {}

MergeTest::~MergeTest() {
}

bool MergeTest::run(Globals* g, const vector<string>&) {
  system("rm MergeSortedGroups.out");
  system("rm Union.out");
  mergeSortedGroupsTest();
  unionTest();
  
  cout << " " << endl;
  unionTest2();
  delete m_ws;
  delete m_rs;
  return true;
}

void MergeTest::mergeSortedGroupsTest() {
  createDataSources();

  cout << "Testing MergeSortedGroups... output in MergeSortedGroups.out\n";
  MergeSortedGroups *merge = new MergeSortedGroups(m_rs,  // RO segment
						   m_ws,  // WS segment
						   2,     // number of columns
						   0);    // index of merge col
  // must let the Merge operator know how to combine columns
  merge->setMergeOp(new MergeSumOp());

  Operator *srcs[1] = {merge};
  int nCols[1] = {2};

  BlockPrinter *bPrint = new BlockPrinter(srcs,
					  nCols,
					  1,
					  "MergeSortedGroups.out");
  bPrint->printColumns();
}

void MergeTest::unionTest() {
  createDataSources();
  
  cout << "Testing Union... output in Union.out\n";
  Union *merge = new Union(m_rs, m_ws, 2);

  Operator *srcs[1] = {merge};
  int nCols[1] = {2};

  BlockPrinter *bPrint = new BlockPrinter(srcs,
					  nCols,
					  1,
					  "Union.out");
  bPrint->printColumns();
}

void MergeTest::unionTest2() {
  system("rm Union2.out");
  createDataSources2();
  
  //system("rm Union2_ROS.out");
    
  /*
  Operator *ros_src[1] = {m_rs};
  int rosCols[1] = {1};

  BlockPrinter *rosPrint = new BlockPrinter(ros_src,
                                          rosCols,
                                          1,
                                          "Union2_ROS.out");
  rosPrint->printColumns();
  exit(1);
  */  
  
  cout << "Testing Union... output in Union2.out\n";
  Union *merge = new Union(m_rs, m_ws, 1);    //one column

  Operator *srcs[1] = {merge};
  int nCols[1] = {1};

  BlockPrinter *bPrint = new BlockPrinter(srcs,
                                          nCols,
                                          1,
                                          "Union2.out");
  bPrint->printColumnsWithPosition();
}
void MergeTest::createDataSources() {
  m_ws = new TestDataSrc(2);
  m_rs = new TestDataSrc(2);


  // WOS
  for (int i = 0; i < 10; i++) {
    BasicBlock *groupBlock = new BasicBlock(true, true, true);
    groupBlock->setValue(i, i);

    BasicBlock *valBlock = new BasicBlock(true, true, true);
    valBlock->setValue(3, i);

    m_ws->addBlock(groupBlock, 0);
    m_ws->addBlock(valBlock, 1);
  }

  for (int i = 10; i < 20; i++) {
    BasicBlock *groupBlock = new BasicBlock(true, true, true);
    groupBlock->setValue(i, i);
    
    BasicBlock *valBlock = new BasicBlock(true, true, true);
    valBlock->setValue(5, i);

    m_ws->addBlock(groupBlock, 0);
    m_ws->addBlock(valBlock, 1);
  }

  // ROS
  for (int i = 0; i < 10; i++) {
    BasicBlock *groupBlock = new BasicBlock(true, true, true);
    groupBlock->setValue(i, i);

    BasicBlock *valBlock = new BasicBlock(true, true, true);
    valBlock->setValue(7, i);

    m_rs->addBlock(groupBlock, 0);
    m_rs->addBlock(valBlock, 1);
  }

  for (int i = 20; i < 30; i++) {
    BasicBlock *groupBlock = new BasicBlock(true, true, true);
    groupBlock->setValue(i, i);
    
    BasicBlock *valBlock = new BasicBlock(true, true, true);
    valBlock->setValue(11, i);

    m_rs->addBlock(groupBlock, 0);
    m_rs->addBlock(valBlock, 1);
  }
}

void MergeTest::createDataSources2() {
  m_ws = new TestDataSrc(1);  //Basic
  m_rs = new TestDataSrc(1);  //RLE

  // WOS 7777733333555554444
  for (int i = 1; i <= 5; i++) {
    BasicBlock *valBlock = new BasicBlock(true, true, true);
    valBlock->setValue(7, i);
    m_ws->addBlock(valBlock, 0);
  }
  for (int i = 6; i <= 10; i++) {
    BasicBlock *valBlock = new BasicBlock(true, true, true);
    valBlock->setValue(3, i);
    m_ws->addBlock(valBlock, 0);
  }
  for (int i = 11; i <= 15; i++) {
    BasicBlock *valBlock = new BasicBlock(true, true, true);
    valBlock->setValue(5, i);
    m_ws->addBlock(valBlock, 0);
  }
  for (int i = 16; i <= 20; i++) {
    BasicBlock *valBlock = new BasicBlock(true, true, true);
    valBlock->setValue(4, i);
    m_ws->addBlock(valBlock, 0);
  }

  // ROS 1111122222333334444455555
  for (int i = 1; i < 5; i++) {
    int j = 1 + (i-1) * 5;
    RLEBlock* rleBlock = new RLEBlock(new RLETriple(new IntValPos(j, i), j, 5), true);
    m_rs->addBlock(rleBlock, 0);
  }  
  
}

