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
 *  BProjectNode.cpp
 *  cstore
 *
 *  Created by Nga Tran on 3/31/05.
 *
 *	Project Node which has 2 children: SelectNode and UCopyNode
 */

#include "BProjectNode.h"

#include <iostream>


// Default
BProjectNode::BProjectNode() : BNode()
{
	m_iIndex = 0;
}


// Initialize with column to be aggregated
BProjectNode::BProjectNode(Node* lpLeft, Node* lpRight, int iIndex) : BNode(lpLeft, lpRight)
{
	m_iIndex = iIndex;
}


// Deallocate memory
BProjectNode::~BProjectNode()
{
	 Log::writeToLog("BProjectNode", 10, "~BProjectNode");
	 
	 if (m_lpReturnOp != NULL)
	{
		delete m_lpReturnOp;
		m_lpReturnOp = NULL;
	}
}


// Get node id
int BProjectNode::getNodeID()
{
	// Special node ID, no treturn its ID bu
	// Return its child ID (SELECT ID)
	return m_lpLeft->getNodeID();
}

void BProjectNode::setIndex(int iIndex)
{
	m_iIndex = iIndex;
}


// Merge ROS and WOS operators
Operator* BProjectNode::mergeROSandWOS()
{
	// Union
	if (m_lpReturnOp != NULL)
	{
		// Has been merged
		return m_lpReturnOp;
	}
	
	if ( (m_lpROSReturnOp != NULL) && (m_lpWOSReturnOp != NULL) )
	{
		// Merge them
		m_lpReturnOp = new Union(m_lpROSReturnOp, m_lpWOSReturnOp, 1);	
		return m_lpReturnOp;
	}
	
	if (m_lpROSReturnOp != NULL)
	{
		return m_lpROSReturnOp;
	}
	
	if (m_lpWOSReturnOp != NULL)
	{
		return m_lpWOSReturnOp;
	}
	
	return m_lpReturnOp;
}


// Run ROS only
Operator* BProjectNode::runROS()
{
	// The operator exists, only return it
	if (m_lpROSReturnOp != NULL)
	{
		return m_lpROSReturnOp;
	}
	
	// Right (UCopyNode) must run first
	Operator* lpRight = NULL;
	if (m_lpRight != NULL)
	{
		lpRight = m_lpRight->runROS();
	}
	
	//cout << endl << "//PROJECT:" << endl;
	if (m_lpLeft != NULL)
	{
		m_lpROSReturnOp = m_lpLeft->runROS();
	}
	
	if ( (m_lpROSReturnOp != NULL) && (lpRight != NULL) )
	{
		((DataSource*) m_lpROSReturnOp)->setPositionFilter(lpRight, m_iIndex);	
	}
	
	return m_lpROSReturnOp;
}

// Run WOS only
Operator* BProjectNode::runWOS()
{
	// The operator exists, only return it
	if (m_lpWOSReturnOp != NULL)
	{
		return m_lpWOSReturnOp;
	}
	
	// Right (UCopyNode) must run first
	Operator* lpRight = NULL;
	if (m_lpRight != NULL)
	{
		lpRight = m_lpRight->runWOS();
	}
	
	if (m_lpLeft != NULL)
	{
		m_lpWOSReturnOp = m_lpLeft->runWOS();
	}
	
	if ( (m_lpWOSReturnOp != NULL) && (lpRight != NULL) )
	{
		((DataSource*) m_lpWOSReturnOp)->setPositionFilter(lpRight, m_iIndex);	
	}
	return m_lpWOSReturnOp;
}

// show what operator this node will run
void BProjectNode::showROS(int* lpNodeID, ofstream* lpOfstream)
{
	// The operator exists
	if (m_bROSHasShown)
	{
		return;
	}
	
	m_bROSHasShown = 1;
	
	// Set nodeID to have different variable name for this node
	setNodeID(lpNodeID);

	// Right (UCopyNode) must run first
	if (m_lpRight != NULL)
	{
		m_lpRight->showROS(lpNodeID, lpOfstream);
	}
			
	string sOps = "//PROJECT:\n";
	lpOfstream->write(sOps.c_str(), sOps.length());
	
	// Select info
	if (m_lpLeft != NULL)
	{
		m_lpLeft->showROS(lpNodeID, lpOfstream);
	}

	char sLeft[15];	
	sprintf(sLeft, "%d", m_lpLeft->getNodeID());
	
	char sRight[15];	
	sprintf(sRight, "%d", m_lpRight->getNodeID());
	
	char sIndex[15];	
	sprintf(sIndex, "%d", m_iIndex);
	
	sOps = "	((DataSource*) lp_ROS" + string(sLeft) + ")->setPositionFilter(lp_ROS" + string(sRight) + ", " + sIndex + ");\n";
	lpOfstream->write(sOps.c_str(), sOps.length());
}

// show what operator this node will run
void BProjectNode::showWOS(int* lpNodeID, ofstream* lpOfstream)
{
	// The operator exists
	if (m_bWOSHasShown)
	{
		return;
	}
	
	m_bWOSHasShown = 1;
	
	// Set nodeID to have different variable name for this node
	setNodeID(lpNodeID);

	// Right (UCopyNode) must run first
	if (m_lpRight != NULL)
	{
		m_lpRight->showWOS(lpNodeID, lpOfstream);
	}
	
	string sOps = "//PROJECT:\n";
	lpOfstream->write(sOps.c_str(), sOps.length());
	
	// Select info		
	if (m_lpLeft != NULL)
	{
		m_lpLeft->showWOS(lpNodeID, lpOfstream);
	}

	char sLeft[15];	
	sprintf(sLeft, "%d", m_lpLeft->getNodeID());
	
	char sRight[15];	
	sprintf(sRight, "%d", m_lpRight->getNodeID());
	
	char sIndex[15];	
	sprintf(sIndex, "%d", m_iIndex);

	sOps = "	((DataSource*) lp_WOS" + string(sLeft) + ")->setPositionFilter(lp_WOS" + string(sRight) + ", " + sIndex + ");\n";
	
	lpOfstream->write(sOps.c_str(), sOps.length());
}

// show tree presentation
void BProjectNode::showTree(string sTabs)
{
	// The operator exists
	if (m_bTreeHasShown)
	{
		cout << endl;
		cout << sTabs << "PROJECT - The operator was created before" << endl;
	
		return;
	}
	
	m_bTreeHasShown = 1;
	
	// Show its information first
	cout << endl;
	cout << sTabs << "PROJECT" << endl;
	
	if (m_lpLeft != NULL)
	{
		sTabs = sTabs + "	";
		m_lpLeft->showTree(sTabs);
		
		cout << endl;
		cout << sTabs << "POSITION FILTER" << endl;
		cout << sTabs << "setPositionFilter" << endl;
		cout << sTabs << "	" << "Argument1: Operator (below)" << endl;
		cout << sTabs << "	" << "Argument2: Col Index of that operator: " << m_iIndex << endl;		
	}
		
	// Show child's information (UCopyNode)
	if (m_lpRight != NULL)
	{
		sTabs = sTabs + "	";
		m_lpRight->showTree(sTabs);
	}
}

// show merge operators
void BProjectNode::showMerge(ofstream* lpOfstream)
{
	char sNodeID[15];	
	sprintf(sNodeID, "%d", m_iNodeID);
	
	char sChildNodeID[15];	
	sprintf(sChildNodeID, "%d", m_lpLeft->getNodeID());
	
	string sOps = "//UNION:\n";
	// Use child node ID to set name for this merge operator
	sOps = sOps + "	Operator* lp_Merge" + string(sChildNodeID) + " = new Union(lp_ROS" + string(sChildNodeID) + ", lp_WOS" + string(sChildNodeID) + ", 1); \n";
	
	lpOfstream->write(sOps.c_str(), sOps.length());
}


//---------------------------------------------------------------
// Memory management functions

// New a node and put it in the m_lpList
BProjectNode* BProjectNode::create()
{
	Log::writeToLog("BProjectNode", 10, "Create without arguments");
		
	BProjectNode* lpNode = new BProjectNode();
	pushBack(lpNode);
	return lpNode;
}

// New a node and put it in the m_lpList
BProjectNode* BProjectNode::create(Node* lpLeft, Node* lpRight, int iIndex)
{
	Log::writeToLog("BProjectNode", 10, "Create with 2 arguments: left node and right node");
		
	BProjectNode* lpNode = new BProjectNode(lpLeft, lpRight, iIndex);
	pushBack(lpNode);
	return lpNode;
}

