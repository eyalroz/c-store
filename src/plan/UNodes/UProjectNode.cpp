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
 *  UProjectNode.cpp
 *  cstore
 *
 *  Created by Nga Tran on 3/30/05.
 *
 *	Project Node which has 1 child: SelectNode
 */

#include "UProjectNode.h"

#include <iostream>


// Default
UProjectNode::UProjectNode() : UNode()
{
}


// Initialize with column to be aggregated
UProjectNode::UProjectNode(Node* lpChild) : UNode(lpChild)
{
}


// Deallocate memory
UProjectNode::~UProjectNode()
{
	 Log::writeToLog("UProjectNode", 10, "~UProjectNode");
	 
	 if (m_lpReturnOp != NULL)
	 {
		delete m_lpReturnOp;
		m_lpReturnOp = NULL;
	 }
}


// Get node id
int UProjectNode::getNodeID()
{
	// Special node ID, no treturn its ID bu
	// Return its child ID (SELECT ID)
	return m_lpChild->getNodeID();
}


// Merge ROS and WOS operators
Operator* UProjectNode::mergeROSandWOS()
{
	// Union
	if (m_lpReturnOp != NULL)
	{
		// Has been merged
		return m_lpReturnOp;
	}
	
	if ( (m_lpROSReturnOp == NULL) || (m_lpWOSReturnOp == NULL) )
	{
		cout << "ERROR: Invalid ros or wos. The results won't be correct." << endl;
		return m_lpReturnOp;
	}
	
	// Merge them
	m_lpReturnOp = new Union(m_lpROSReturnOp, m_lpWOSReturnOp, 1);	
	return m_lpReturnOp;
	
}

// Run ROS only
Operator* UProjectNode::runROS()
{
	// The operator exists, only return it
	if (m_lpROSReturnOp != NULL)
	{
		return m_lpROSReturnOp;
	}
	
	//cout << endl << "//PROJECT:" << endl;
	if (m_lpChild != NULL)
	{
		m_lpROSReturnOp = m_lpChild->runROS();
	}
		
	return m_lpROSReturnOp;
}

// Run WOS only
Operator* UProjectNode::runWOS()
{
	// The operator exists, only return it
	if (m_lpWOSReturnOp != NULL)
	{
		return m_lpWOSReturnOp;
	}
	
	if (m_lpChild != NULL)
	{
		m_lpWOSReturnOp = m_lpChild->runWOS();
	}
		
	return m_lpWOSReturnOp;
}

// show what operator this node will run
void UProjectNode::showROS(int* lpNodeID, ofstream* lpOfstream)
{
	// The operator exists
	if (m_bROSHasShown)
	{
		return;
	}
	
	m_bROSHasShown = 1;
	
	// Set nodeID to have different variable name for this node
	setNodeID(lpNodeID);
	
	if (m_lpChild != NULL)
	{
		string sOps = "//PROJECT:\n";
		lpOfstream->write(sOps.c_str(), sOps.length());
		
		m_lpChild->showROS(lpNodeID, lpOfstream);
	}
}

// show what operator this node will run
void UProjectNode::showWOS(int* lpNodeID, ofstream* lpOfstream)
{
	// The operator exists
	if (m_bWOSHasShown)
	{
		return;
	}
	
	m_bWOSHasShown = 1;
	
	// Set nodeID to have different variable name for this node
	setNodeID(lpNodeID);
	
	if (m_lpChild != NULL)
	{
		string sOps = "//PROJECT:\n";
		lpOfstream->write(sOps.c_str(), sOps.length());
		
		m_lpChild->showWOS(lpNodeID, lpOfstream);
	}
}




// show tree presentation
void UProjectNode::showTree(string sTabs)
{
	// The operator exists
	if (m_bTreeHasShown)
	{
		cout << endl;
		cout << sTabs << "PROJECT - The operator was created before" << endl;
		
		
		return;
	}
	
	m_bTreeHasShown = 1;
		
	// Show child's information
	if (m_lpChild != NULL)
	{
		// Show its information first
		cout << endl;
		cout << sTabs << "PROJECT" << endl;
	
		sTabs = sTabs + "	";
	
		m_lpChild->showTree(sTabs);
	}
}


// show merge operators
void UProjectNode::showMerge(ofstream* lpOfstream)
{
	//cout << "UNION:" << endl;
	//cout << "	Operator* lpMerge = new Union(ROS Operator, WOS Operator, 1);" << endl;
	
	char sNodeID[15];	
	sprintf(sNodeID, "%d", m_iNodeID);
	
	char sChidNodeID[15];	
	sprintf(sChidNodeID, "%d", m_lpChild->getNodeID());
	
	string sOps = "//UNION:\n";
	//sOps = sOps + "	Operator* lp_Merge" + string(sNodeID) + " = new Union(lp_ROS" + string(sChidNodeID) + ", lp_WOS" + string(sChidNodeID) + ", 1); \n";
	// Use child node ID to set name for this merge operator
	sOps = sOps + "	Operator* lp_Merge" + string(sChidNodeID) + " = new Union(lp_ROS" + string(sChidNodeID) + ", lp_WOS" + string(sChidNodeID) + ", 1); \n";

	lpOfstream->write(sOps.c_str(), sOps.length());

}


//---------------------------------------------------------------
// Memory management functions
		
	
// New a node and put it in the m_lpList
UProjectNode* UProjectNode::create()
{
	Log::writeToLog("UProjectNode", 10, "Create without arguments");
		
	UProjectNode* lpNode = new UProjectNode();
	pushBack(lpNode);
	return lpNode;
}

UProjectNode* UProjectNode::create(Node* lpChild)
{
	Log::writeToLog("UNode", 10, "Create with 1 argument: child node");
		
	UProjectNode* lpNode = new UProjectNode(lpChild);
	pushBack(lpNode);
	return lpNode;
}

