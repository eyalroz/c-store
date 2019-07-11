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
 *  UNegationNode.cpp
 *  cstore
 *
 *  Created by Nga Tran on 3/17/05.
 *  Copyright 2005 __MyCompanyName__. All rights reserved.
 *
 */

#include "UNegationNode.h"

#include <iostream>

UNegationNode::UNegationNode(): UNode()
{
}


UNegationNode::UNegationNode(Node* lpChild) : UNode(lpChild)
{
}

UNegationNode::~UNegationNode()
{
	 Log::writeToLog("UNegationNode", 10, "~UNegationNode");
	 
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


// Run ROS only
Operator* UNegationNode::runROS()
{
	// The operator exists, only return it
	if (m_lpROSReturnOp != NULL)
	{
		return m_lpROSReturnOp;
	}
	
	Operator* lpOp = NULL;
	if (m_lpChild != NULL)
	{
		lpOp = m_lpChild->runROS();
	}
	
	if (lpOp != NULL)
	{
		m_lpROSReturnOp = new BNotOperator(lpOp, 0);
	}
	 
	return m_lpROSReturnOp;
}

// Run WOS only
Operator* UNegationNode::runWOS()
{
	// The operator exists, only return it
	if (m_lpWOSReturnOp != NULL)
	{
		return m_lpWOSReturnOp;
	}
	
	Operator* lpOp = NULL;
	if (m_lpChild != NULL)
	{
		lpOp = m_lpChild->runWOS();
	}
	
	if (lpOp != NULL)
	{
		m_lpWOSReturnOp = new BNotOperator(lpOp, 0);
	}
	 
	return m_lpWOSReturnOp;
}

// Merge ROS and WOS operators
Operator* UNegationNode::mergeROSandWOS()
{
	// Never used
	return UNode::mergeROSandWOS();
}

// Shows what operators this node will run
void UNegationNode::showROS(int* lpNodeID, ofstream* lpOfstream)
{
	// The operator exists
	if (m_bROSHasShown)
	{
		return;
	}
	
	commonShowROS(lpNodeID, "BNotOperator", lpOfstream);
	
	// Show: "BNotOperator* lpBNot = new BNotOperator(m_lpChild->run(), 0);"
	if (m_lpChild != NULL)
	{
		char sNodeID[15];	
		sprintf(sNodeID, "%d", m_iNodeID);
		
		char sChildNodeID[15];	
		sprintf(sChildNodeID, "%d", m_lpChild->getNodeID());
		
		string sOps = "//NEGATION:\n";
		sOps = sOps + "	Operator* lp_ROS" + sNodeID + " = new BNotOperator(lp_ROS" + sChildNodeID + ", 0);\n";
		
		lpOfstream->write(sOps.c_str(), sOps.length());
	}
}


// Shows what operators this node will run
void UNegationNode::showWOS(int* lpNodeID, ofstream* lpOfstream)
{
	// The operator exists
	if (m_bWOSHasShown)
	{
		return;
	}
	
	commonShowWOS(lpNodeID, "BNotOperator", lpOfstream);
	
	// Show: "BNotOperator* lpBNot = new BNotOperator(m_lpChild->run(), 0);"
	if (m_lpChild != NULL)
	{
		char sNodeID[15];	
		sprintf(sNodeID, "%d", m_iNodeID);
		
		char sChildNodeID[15];	
		sprintf(sChildNodeID, "%d", m_lpChild->getNodeID());
		
		string sOps = "//NEGATION:\n";
		sOps = sOps + "	Operator* lp_WOS" + sNodeID + " = new BNotOperator(lp_WOS" + sChildNodeID + ", 0);\n";
		
		lpOfstream->write(sOps.c_str(), sOps.length());
	}
}


// show tree presentation
void UNegationNode::showTree(string sTabs)
{
	// The operator exists
	if (m_bTreeHasShown)
	{
		cout << endl;
		cout << sTabs << "NEGATION - The operator was created before" << endl;
		
		return;
	}
	
	m_bTreeHasShown = 1;
	
	// Show its information first
	cout << endl;
	cout << sTabs << "NEGATION OPERATOR" << endl;
	
	cout << sTabs << "Operator: BNotOperator" << endl;
	cout << sTabs << "	" << "Argument1: Operator (below)" << endl;
	cout << sTabs << "	" << "Argument2: Column Index of the operator: 0" << endl;
		
	// Show child's information
	if (m_lpChild != NULL)
	{
		sTabs = sTabs + "	";
		m_lpChild->showTree(sTabs);
	}
}

// show merge operators
void UNegationNode::showMerge(ofstream* lpOfstream)
{
	UNode::showMerge(lpOfstream);
}

//---------------------------------------------------------------
// Memory management functions
		
	
// New a node and put it in the m_lpList
UNegationNode* UNegationNode::create()
{
	Log::writeToLog("UNegationNode", 10, "Create without arguments");
		
	UNegationNode* lpNode = new UNegationNode();
	pushBack(lpNode);
	return lpNode;
}

UNegationNode* UNegationNode::create(Node* lpChild)
{
	Log::writeToLog("UNegationNode", 10, "Create with 1 argument: child node");
		
	UNegationNode* lpNode = new UNegationNode(lpChild);
	pushBack(lpNode);
	return lpNode;
}
