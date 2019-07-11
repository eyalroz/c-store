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
 *  BDisjunctionNode.cpp
 *  cstore
 *
 *  Created by Nga Tran on 3/17/05.
 * 
 *
 */

#include "BDisjunctionNode.h"

#include <iostream>

BDisjunctionNode::BDisjunctionNode() : BNode()
{
}


BDisjunctionNode::BDisjunctionNode(Node* lpLeft, Node* lpRight) : BNode(lpLeft, lpRight)
{
	// Check if at least one of its children is a join node
	if (m_lpLeft != NULL)
	{
		if ( (m_lpLeft->getPredicateType() == PREDICATE_JOIN) || (m_lpLeft->getPredicateType() == PREDICATE_JOIN_AND_OTHER) )
		{
			setPredicateType(PREDICATE_JOIN_AND_OTHER);
			return;
		}		
	}
	if (m_lpRight != NULL)
	{
		if ( (m_lpRight->getPredicateType() == PREDICATE_JOIN) || (m_lpRight->getPredicateType() == PREDICATE_JOIN_AND_OTHER) )
		{
			setPredicateType(PREDICATE_JOIN_AND_OTHER);
		}		
	}
}

BDisjunctionNode::~BDisjunctionNode()
{
	 Log::writeToLog("BDisjunctionNode", 10, "~BDisjunctionNode"); 
	 
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

// Merge ROS and WOS operators
Operator* BDisjunctionNode::mergeROSandWOS()
{
	// Never used
	return BNode::mergeROSandWOS();
}

// Run ROS only
Operator* BDisjunctionNode::runROS()
{
	// The operator exists, only return it
	if (m_lpROSReturnOp != NULL)
	{
		return m_lpROSReturnOp;
	}
	
	// Generate the operator
	Operator*	lpLeft = NULL;
	Operator*	lpRight = NULL;
	
	if (m_lpLeft != NULL)
	{
		lpLeft = m_lpLeft->runROS();
	}
	
	if (m_lpRight != NULL)
	{
		lpRight = m_lpRight->runROS();
	}	
	
	//m_lpROSReturnOp = new BOrOperator(lpLeft, 0, lpRight, 0);
	m_lpROSReturnOp = new PosOrOperator(lpLeft, 0, lpRight, 0);
/*
	cout << endl << "//DISJUNCTION:" << endl;

	cout << endl << "	//Operator* lpAnd = new BOrOperator(lp_" << (int) lpLeft << ", 0, lp_" << (int) lpRight << ", 0); IS RUNNING" << endl;	
	m_lpROSReturnOp = new BOrOperator(lpLeft, 0, lpRight, 0);
	cout << "	Operator* lp_" << (int) m_lpROSReturnOp << " = new BOrOperator(lp_" << (int) lpLeft << ", 0, lp_" << (int) lpRight << ", 0);" << endl;
*/
	
	return m_lpROSReturnOp;
}

// Run WOS only
Operator* BDisjunctionNode::runWOS()
{
	// The operator exists, only return it
	if (m_lpWOSReturnOp != NULL)
	{
		return m_lpWOSReturnOp;
	}
	
	// Generate the operator
	Operator*	lpLeft = NULL;
	Operator*	lpRight = NULL;
	
	if (m_lpLeft != NULL)
	{
		lpLeft = m_lpLeft->runWOS();
	}
	
	if (m_lpRight != NULL)
	{
		lpRight = m_lpRight->runWOS();
	}	
	
	//m_lpWOSReturnOp = new BOrOperator(lpLeft, 0, lpRight, 0);
	m_lpWOSReturnOp = new PosOrOperator(lpLeft, 0, lpRight, 0);
	
	return m_lpWOSReturnOp;
}


// show what operator this node will run
void BDisjunctionNode::showROS(int* lpNodeID, ofstream* lpOfstream)
{
	// The operator exists
	if (m_bROSHasShown)
	{
		return;
	}
	
	//commonShowROS(lpNodeID, "BOrOperator", lpOfstream);
	commonShowROS(lpNodeID, "PosOrOperator", lpOfstream);
}

// show what operator this node will run
void BDisjunctionNode::showWOS(int* lpNodeID, ofstream* lpOfstream)
{
	// The operator exists
	if (m_bWOSHasShown)
	{
		return;
	}
	
	//commonShowWOS(lpNodeID, "BOrOperator", lpOfstream);
	commonShowWOS(lpNodeID, "PosOrOperator", lpOfstream);
}


// show tree presentation
void BDisjunctionNode::showTree(string sTabs)
{
	// The operator exists
	if (m_bTreeHasShown)
	{
		cout << endl;
		cout << sTabs << "DISJUNCTION - The operator was created before" << endl;
	
		return;
	}
	
	m_bTreeHasShown = 1;
	
	// Show its information first
	cout << endl;
	cout << sTabs << "DISJUNCTION" << endl;
	
	//cout << sTabs << "Operator: BOrOperator" << endl;
	cout << sTabs << "Operator: PosOrOperator" << endl;
	cout << sTabs << "	" << "Argument1: Operator1 (first below)" << endl;
	cout << sTabs << "	" << "Argument2: Column index of the argument1: " << endl;
	cout << sTabs << "	" << "Argument3: Operator2 (second below)" << endl;
	cout << sTabs << "	" << "Argument4: Column index of the argument3: " << endl;
		
	sTabs = sTabs + "	";
	// Show child's information
	if (m_lpLeft != NULL)
	{
		m_lpLeft->showTree(sTabs);
	}
	
	if (m_lpRight != NULL)
	{
		m_lpRight->showTree(sTabs);
	}
}


// show merge operators
void BDisjunctionNode::showMerge(ofstream* lpOfstream)
{
	BNode::showMerge(lpOfstream);
}


//---------------------------------------------------------------
// Memory management functions

// New a node and put it in the m_lpList
BDisjunctionNode* BDisjunctionNode::create()
{
	Log::writeToLog("BDisjunctionNode", 10, "Create without arguments");
		
	BDisjunctionNode* lpNode = new BDisjunctionNode();
	pushBack(lpNode);
	return lpNode;
}

// New a node and put it in the m_lpList
BDisjunctionNode* BDisjunctionNode::create(Node* lpLeft, Node* lpRight)
{
	Log::writeToLog("BDisjunctionNode", 10, "Create with 2 arguments: left node and right node");
		
	BDisjunctionNode* lpNode = new BDisjunctionNode(lpLeft, lpRight);
	pushBack(lpNode);
	return lpNode;
}
