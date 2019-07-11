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
/*
 *  UAggSumNode.cpp
 *  cstore
 *
 *  Created by Nga Tran on 3/30/05.
 *  
 *	Sum node  without group by
 */

#include "UAggSumNode.h"

#include <iostream>


// Default
UAggSumNode::UAggSumNode() : UAggNode()
{
	m_lpMergeOp = NULL;
}


// Initialize with column to be aggregated
UAggSumNode::UAggSumNode(Node* lpAggCol, int iAggColIndex) : UAggNode(lpAggCol, iAggColIndex)
{
	m_lpMergeOp = NULL;
}


// Deallocate memory
UAggSumNode::~UAggSumNode()
{
	 Log::writeToLog("UAggSumNode", 10, "~UAggSumNode");
	 
	 if (m_lpROSReturnOp != NULL)
	 {
		delete m_lpROSReturnOp;
		m_lpROSReturnOp = NULL;
	 }
	 
	 if (m_lpWOSReturnOp != NULL)
	 {
		delete m_lpWOSReturnOp;
		m_lpWOSReturnOp = NULL;
	 }
	 
	 if (m_lpMergeOp != NULL)
	 {
		delete m_lpMergeOp;
		m_lpMergeOp = NULL;
	 }

}

// Merge ROS and WOS operators
Operator* UAggSumNode::mergeROSandWOS()
{
	bool bGoOn = 0;
	bGoOn = commonMergeRosAndWos();
	
	if (bGoOn == 1)
	{
		m_lpMergeOp = new MergeSumOp();
		((MergeSortedGroups*) m_lpReturnOp)->setMergeOp(m_lpMergeOp);
	}
	
	return m_lpReturnOp;
}

// Run ROS only
Operator* UAggSumNode::runROS()
{
	// The operator exists, only return it
	if (m_lpROSReturnOp != NULL)
	{
		return m_lpROSReturnOp;
	}
	
	if (m_lpChild != NULL)
	{
		Operator* lpOp = NULL;
		lpOp = m_lpChild->runROS();
		
		if (lpOp != NULL)
		{
			m_lpROSReturnOp = new Sum(lpOp, m_iAggColIndex);
		}
	}
		
	return m_lpROSReturnOp;
}

// Run WOS only
Operator* UAggSumNode::runWOS()
{
	// The operator exists, only return it
	if (m_lpWOSReturnOp != NULL)
	{
		return m_lpWOSReturnOp;
	}
	
	if (m_lpChild != NULL)
	{
		Operator* lpOp = NULL;
		lpOp = m_lpChild->runWOS();
		
		if (lpOp != NULL)
		{
			m_lpWOSReturnOp = new Sum(lpOp, m_iAggColIndex);
		}
	}
		
	return m_lpWOSReturnOp;
}

// show what operator this node will run
void UAggSumNode::showROS(int* lpNodeID, ofstream* lpOfstream)
{
	// The operator exists
	if (m_bROSHasShown)
	{
		return;
	}
	
	commonShowROS(lpNodeID, "Sum", lpOfstream);
}

// show what operator this node will run
void UAggSumNode::showWOS(int* lpNodeID, ofstream* lpOfstream)
{
	// The operator exists
	if (m_bWOSHasShown)
	{
		return;
	}
	
	commonShowWOS(lpNodeID, "Sum", lpOfstream);
}

// show tree presentation
void UAggSumNode::showTree(string sTabs)
{
	// The operator exists
	if (m_bTreeHasShown)
	{
		cout << endl;
		cout << sTabs << "SUM AGGREGATE - The operator was created before" << endl;
		
		return;
	}
	
	m_bTreeHasShown = 1;
	
	// Show its information first
	cout << endl;
	cout << sTabs << "SUM AGGREGATE" << endl;
	
	cout << sTabs << "Operator: Sum" << endl;
	cout << sTabs << "	" << "Argument1: Operator (below)" << endl;
	cout << sTabs << "	" << "Argument2: Column index of the argument1: " << m_iAggColIndex << endl;
		
	// Show child's information
	if (m_lpChild != NULL)
	{
		sTabs = sTabs + "	";
		m_lpChild->showTree(sTabs);
	}
}

// show merge operators
void UAggSumNode::showMerge(ofstream* lpOfstream)
{
	commonShowMerge("Sum", lpOfstream);
}

//---------------------------------------------------------------
// Memory management functions
		
	
// New a node and put it in the m_lpList
UAggSumNode* UAggSumNode::create()
{
	Log::writeToLog("UAggSumNode", 10, "Create without arguments");
		
	UAggSumNode* lpNode = new UAggSumNode();
	pushBack(lpNode);
	return lpNode;
}

UAggSumNode* UAggSumNode::create(Node* lpAggCol, int iAggColIndex)
{
	Log::writeToLog("UAggSumNode", 10, "Create with arguments: Node* lpAggCol, int iAggColIndex");
		
	UAggSumNode* lpNode = new UAggSumNode(lpAggCol, iAggColIndex);
	pushBack(lpNode);
	return lpNode;
}

