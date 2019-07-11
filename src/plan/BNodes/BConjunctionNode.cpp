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
 *  BConjunctionNode.cpp
 *  cstore
 *
 *  Created by Nga Tran on 3/17/05.
 *  Copyright 2005 __MyCompanyName__. All rights reserved.
 *
 */

#include "BConjunctionNode.h"

#include <iostream>

BConjunctionNode::BConjunctionNode() : BNode()
{
}


BConjunctionNode::BConjunctionNode(Node* lpLeft, Node* lpRight) : BNode(lpLeft, lpRight)
{
	// Check if at least one of its children is a join node
	if ( (m_lpLeft != NULL) && (m_lpRight != NULL) )
	{
		// Join and another select predicate
		if ( (m_lpLeft->getPredicateType() == PREDICATE_JOIN) &&
			 (m_lpRight->getPredicateType() == PREDICATE_SELECT))
		{
		
			SelectNode* lpLeft = (SelectNode*) ((BNode*) m_lpLeft)->getLeft();
			lpLeft->checkAndSetPredicate((SelectNode*) m_lpRight);
			SelectNode* lpRight = (SelectNode*) ((BNode*) m_lpLeft)->getRight();
			lpRight->checkAndSetPredicate((SelectNode*) m_lpRight);
			
			// Set for replica
			((BJoinNode*) m_lpLeft)->checkAndSetPredicate((SelectNode*) m_lpRight);
			
			setPredicateType(PREDICATE_JOIN_AND_SELECT);			
			return;
		}
		
		if ( (m_lpLeft->getPredicateType() == PREDICATE_SELECT) &&
			 (m_lpRight->getPredicateType() == PREDICATE_JOIN))
		{
		
			SelectNode* lpLeft = (SelectNode*) ((BNode*) m_lpRight)->getLeft();
			lpLeft->checkAndSetPredicate((SelectNode*) m_lpLeft);
			SelectNode* lpRight = (SelectNode*) ((BNode*) m_lpRight)->getRight();
			lpRight->checkAndSetPredicate((SelectNode*) m_lpLeft);
			
			
			// Set for replica
			((BJoinNode*) m_lpRight)->checkAndSetPredicate((SelectNode*) m_lpLeft);
			
			setPredicateType(PREDICATE_JOIN_AND_SELECT);
			
			// Swap left and right
			Node* lpTmpNode = m_lpLeft;
			m_lpLeft = m_lpRight;
			m_lpRight = lpTmpNode;
			
			return;
		}
	}
	
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

BConjunctionNode::~BConjunctionNode()
{
	 Log::writeToLog("BConjunctionNode", 10, "~BConjunctionNode");
	 
	 if (m_lpROSReturnOp != NULL)
	{
	  //delete ( (BAndOperator*) m_lpROSReturnOp);
	  delete ( (PosAndOperator*) m_lpROSReturnOp);
	  m_lpROSReturnOp = NULL;
	}
	
	if (m_lpWOSReturnOp != NULL)
	{
	  //delete ( (BAndOperator*) m_lpWOSReturnOp);
	  delete ( (PosAndOperator*) m_lpWOSReturnOp);
	  m_lpWOSReturnOp = NULL;
	}
}

// Merge ROS and WOS operators
Operator* BConjunctionNode::mergeROSandWOS()
{
	// Never used
	return BNode::mergeROSandWOS();
}

// Run ROS only
Operator* BConjunctionNode::runROS()
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
	
	//m_lpROSReturnOp = new BAndOperator(lpLeft, 0, lpRight, 0);
	m_lpROSReturnOp = new PosAndOperator(lpLeft, 0, lpRight, 0);
	
	return m_lpROSReturnOp;
}

// Run WOS only
Operator* BConjunctionNode::runWOS()
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
	
	//m_lpWOSReturnOp = new BAndOperator(lpLeft, 0, lpRight, 0);
	m_lpWOSReturnOp = new PosAndOperator(lpLeft, 0, lpRight, 0);
		
	return m_lpWOSReturnOp;
}

// show what operator this node will run
void BConjunctionNode::showROS(int* lpNodeID, ofstream* lpOfstream)
{
	// The operator exists
	if (m_bROSHasShown)
	{
		return;
	}
	
	//commonShowROS(lpNodeID, "BAndOperator", lpOfstream);
	commonShowROS(lpNodeID, "PosAndOperator", lpOfstream);
}

// show what operator this node will run
void BConjunctionNode::showWOS(int* lpNodeID, ofstream* lpOfstream)
{
	// The operator exists
	if (m_bWOSHasShown)
	{
		return;
	}
	
	//commonShowWOS(lpNodeID, "BAndOperator", lpOfstream);
	commonShowWOS(lpNodeID, "PosAndOperator", lpOfstream);
}

// show tree presentation
void BConjunctionNode::showTree(string sTabs)
{
	// The operator exists
	if (m_bTreeHasShown)
	{
		cout << endl;
		cout << sTabs << "CONJUNCTION - The operator was created before" << endl;
		
		return;
	}
	
	m_bTreeHasShown = 1;
	
	// Show its information first
	cout << endl;
	cout << sTabs << "CONJUNCTION" << endl;
	
	//cout << sTabs << "Operator: BAndOperator" << endl;
	cout << sTabs << "Operator: PosAndOperator" << endl;
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
void BConjunctionNode::showMerge(ofstream* lpOfstream)
{
	BNode::showMerge(lpOfstream);
}


//---------------------------------------------------------------
// Memory management functions

// New a node and put it in the m_lpList
BConjunctionNode* BConjunctionNode::create()
{
	Log::writeToLog("BConjunctionNode", 10, "Create without arguments");
		
	BConjunctionNode* lpNode = new BConjunctionNode();
	pushBack(lpNode);
	return lpNode;
}

// New a node and put it in the m_lpList
BConjunctionNode* BConjunctionNode::create(Node* lpLeft, Node* lpRight)
{
	Log::writeToLog("BConjunctionNode", 10, "Create with 2 arguments: left node and right node");
		
	BConjunctionNode* lpNode = new BConjunctionNode(lpLeft, lpRight);
	pushBack(lpNode);
	return lpNode;
}
