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
 *  UCopyNode.cpp
 *  cstore
 *
 *  Created by Nga Tran on 3/17/05.
 *  
 *  Operator for outputing many columns
 */

#include "UCopyNode.h"

#include <iostream>


// Default
UCopyNode::UCopyNode() : UNode()
{
	m_iNumNodes = 0;
	m_iColIndex = 0;
}


// Child is a Node
UCopyNode::UCopyNode(Node* lpChild, int iColIndex, int iNumNodes) : UNode(lpChild)
{
	m_iNumNodes = iNumNodes;
	m_iColIndex = iColIndex;
}


// Deallocate memory
UCopyNode::~UCopyNode()
{
	 Log::writeToLog("UCopyNode", 10, "~UCopyNode"); 
	 
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
}


// Set number of node to copied
void UCopyNode::setNumNodes(int iNumNodes)
{
	m_iNumNodes = iNumNodes;
}

// Set Column index
void UCopyNode::setColIndex(int iColIndex)
{
	m_iColIndex = iColIndex;
}

// Merge ROS and WOS operators
Operator* UCopyNode::mergeROSandWOS()
{
	// Never used
	return UNode::mergeROSandWOS();
}

// Run ROS only
Operator* UCopyNode::runROS()
{
	// The operator exists, only return it
	if (m_lpROSReturnOp != NULL)
	{
		return m_lpROSReturnOp;
	}
	
	// Generate the operator
	Operator* lpOp =  NULL;
	if (m_lpChild != NULL)
	{
		lpOp = m_lpChild->runROS();
	}
	
	if (lpOp != NULL)
	{
		m_lpROSReturnOp = new BCopyOperator(lpOp, m_iColIndex, m_iNumNodes);
	}
	
	return m_lpROSReturnOp;
}

// Run WOS only
Operator* UCopyNode::runWOS()
{
	// The operator exists, only return it
	if (m_lpWOSReturnOp != NULL)
	{
		return m_lpWOSReturnOp;
	}
	
	// Generate the operator
	Operator* lpOp =  NULL;
	if (m_lpChild != NULL)
	{
		lpOp = m_lpChild->runWOS();
	}
	
	if (lpOp != NULL)
	{
		m_lpWOSReturnOp = new BCopyOperator(lpOp, m_iColIndex, m_iNumNodes);
	}
	
	return m_lpWOSReturnOp;
}

// Show the operator of this node
void UCopyNode::showROS(int* lpNodeID, ofstream* lpOfstream)
{
	// The operator exists
	if (m_bROSHasShown)
	{
		return;
	}
	
	commonShowROS(lpNodeID, "BCopyOperator", lpOfstream);
	
	// Show: "BCopyOperator* lpCopy = new BCopyOperator(m_lpChild->run(), m_iColIndex, m_iNumNodes);"
	char colIndex[10];
	sprintf(colIndex, "%d", m_iColIndex);
	char num[10];
	sprintf(num, "%d", m_iNumNodes);
	
	
	char sNodeID[15];	
	sprintf(sNodeID, "%d", m_iNodeID);
		
	char sChildNodeID[15];	
	sprintf(sChildNodeID, "%d", m_lpChild->getNodeID());
		
	string sOps = "//COPY:\n";
	sOps = sOps + "	Operator* lp_ROS" + sNodeID + " = new BCopyOperator(lp_ROS" + sChildNodeID + ",  ";
	sOps = sOps + string(colIndex) + ", " + string(num) + ");\n";

	lpOfstream->write(sOps.c_str(), sOps.length());
}

void UCopyNode::showWOS(int* lpNodeID, ofstream* lpOfstream)
{
	// The operator exists
	if (m_bWOSHasShown)
	{
		return;
	}
	
	commonShowWOS(lpNodeID, "BCopyOperator", lpOfstream);
	
	// Show: "BCopyOperator* lpCopy = new BCopyOperator(m_lpChild->run(), m_iColIndex, m_iNumNodes);"
	char colIndex[10];
	sprintf(colIndex, "%d", m_iColIndex);
	char num[10];
	sprintf(num, "%d", m_iNumNodes);
	
	
	char sNodeID[15];	
	sprintf(sNodeID, "%d", m_iNodeID);
		
	char sChildNodeID[15];	
	sprintf(sChildNodeID, "%d", m_lpChild->getNodeID());
		
	string sOps = "//COPY:\n";
	sOps = sOps + "	Operator* lp_WOS" + sNodeID + " = new BCopyOperator(lp_WOS" + sChildNodeID + ",  ";
	sOps = sOps + string(colIndex) + ", " + string(num) + ");\n";

	lpOfstream->write(sOps.c_str(), sOps.length());
}



// show tree presentation
void UCopyNode::showTree(string sTabs)
{
	// The operator exists
	if (m_bTreeHasShown)
	{
		cout << endl;
		cout << sTabs << "COPY - The operator was created before" << endl;
		
		return;
	}
	
	m_bTreeHasShown = 1;
	
	// Show its information first
	cout << endl;
	cout << sTabs << "COPY OPERATOR" << endl;
	
	// Show: "BCopyOperator* lpCopy = new BCopyOperator(m_lpChild->run(), m_iColIndex, m_iNumNodes);"
	char colIndex[10];
	sprintf(colIndex, "%d", m_iColIndex);
	char num[10];
	sprintf(num, "%d", m_iNumNodes);
	
	cout << sTabs << "Operator: BCopyOperator" << endl;
	cout << sTabs << "	" << "Argument1: Operator (below)" << endl;
	cout << sTabs << "	" << "Argument2: Column Index of the operator: " << string(colIndex) << endl;
	cout << sTabs << "	" << "Argument3: Number of parent nodes: " << string(num) << endl;
		
	// Show child's information
	if (m_lpChild != NULL)
	{
		sTabs = sTabs + "	";
		m_lpChild->showTree(sTabs);
	}
}

// show merge operators
void UCopyNode::showMerge(ofstream* lpOfstream)
{
	UNode::showMerge(lpOfstream);
}

//---------------------------------------------------------------
// Memory management functions
		
	
// New a node and put it in the m_lpList
UCopyNode* UCopyNode::create()
{
	Log::writeToLog("UCopyNode", 10, "Create without arguments");
		
	UCopyNode* lpNode = new UCopyNode();
	pushBack(lpNode);
	return lpNode;
}

UCopyNode* UCopyNode::create(Node* lpChild, int iColIndex, int iNumNodes)
{
	Log::writeToLog("UCopyNode", 10, "Create with arguments: Node* lpChild, int iColIndex, int iNumNodes");
		
	UCopyNode* lpNode = new UCopyNode(lpChild, iColIndex, iNumNodes);
	pushBack(lpNode);
	return lpNode;
}
