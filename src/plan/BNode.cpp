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
 *  BNode.cpp
 *  cstore
 *
 *  Created by Nga Tran on 3/17/05.
 *  Copyright 2005 __MyCompanyName__. All rights reserved.
 *
 */

#include "BNode.h"


#include <iostream>


BNode::BNode() : Node()
{
	m_lpLeft = NULL;
	m_lpRight = NULL;
}

BNode::BNode(Node* lpLeft, Node* lpRight) : Node()
{
	m_lpLeft = lpLeft;
	m_lpRight = lpRight;
}

// Deallocate memory for its members
BNode::~BNode()
{
	Log::writeToLog("BNode", 10, "~BNode");  
}


// Get node id
int BNode::getNodeID()
{
	return Node::getNodeID();
}


void BNode::setChildren(Node* lpLeft, Node* lpRight)
{
	m_lpLeft = lpLeft;
	m_lpRight = lpRight;
}

void BNode::setLeft(Node* lpLeft)
{
	m_lpLeft = lpLeft;
}

void BNode::setRight(Node* lpRight)
{
	m_lpRight = lpRight;
}

Node* BNode::getLeft()
{
	return m_lpLeft;
}

Node* BNode::getRight()
{
	return m_lpRight;
}

// Merge ROS and WOS operators
Operator* BNode::mergeROSandWOS()
{
	// Never used
	return Node::mergeROSandWOS();
}


// Run the operator of this node
Operator* BNode::run(bool rosOnly)
{
	return Node::run(rosOnly);
}


// Run ROS only
Operator* BNode::runROS()
{
	return Node::runROS();
}

// Run WOS only
Operator* BNode::runWOS()
{
	return Node::runWOS();
}

// show what operator this node will run
void BNode::showROS(int* lpNodeID, ofstream* lpOfstream)
{
	Node::showROS(lpNodeID, lpOfstream);
}

// show what WOS operator this node will run
void BNode::showWOS(int* lpNodeID, ofstream* lpOfstream)
{
	Node::showWOS(lpNodeID, lpOfstream);
}

// show tree presentation
void BNode::showTree(string sTabs)
{
	Node::showTree(sTabs);
}

// show merge operators
void BNode::showMerge(ofstream* lpOfstream)
{
	Node::showMerge(lpOfstream);
}

// Common fucntions called in showROS
void BNode::commonShowROS(int* lpNodeID, string sOperator, ofstream* lpOfstream)
{
	m_bROSHasShown = 1;
	
	// Set nodeID to have different variable name for this node
	setNodeID(lpNodeID);
	
	if (m_lpLeft != NULL)
	{
		m_lpLeft->showROS(lpNodeID, lpOfstream);
	}
	
	if (m_lpRight != NULL)
	{
		m_lpRight->showROS(lpNodeID, lpOfstream);
	}
	
	char sNodeID[15];	
	sprintf(sNodeID, "%d", m_iNodeID);
	
	char sLeftNodeID[15];	
	sprintf(sLeftNodeID, "%d", m_lpLeft->getNodeID());
	
	char sRightNodeID[15];	
	sprintf(sRightNodeID, "%d", m_lpRight->getNodeID());
	
	string sOps = "//OPERATOR: " + sOperator + "\n";
	sOps = sOps + "	Operator* lp_ROS" + string(sNodeID);
	sOps = sOps + " = new " + sOperator + "(lp_ROS" + string(sLeftNodeID) + ", 0, lp_ROS" + string(sRightNodeID) + ", 0);\n";	
	
	lpOfstream->write(sOps.c_str(), sOps.length());
}


// Common fucntions called in showROS
void BNode::commonShowWOS(int* lpNodeID, string sOperator, ofstream* lpOfstream)
{
	m_bWOSHasShown = 1;
	
	// Set nodeID to have different variable name for this node
	setNodeID(lpNodeID);
	
	if (m_lpLeft != NULL)
	{
		m_lpLeft->showWOS(lpNodeID, lpOfstream);
	}
	
	if (m_lpRight != NULL)
	{
		m_lpRight->showWOS(lpNodeID, lpOfstream);
	}
	
	char sNodeID[15];	
	sprintf(sNodeID, "%d", m_iNodeID);
	
	char sLeftNodeID[15];	
	sprintf(sLeftNodeID, "%d", m_lpLeft->getNodeID());
	
	char sRightNodeID[15];	
	sprintf(sRightNodeID, "%d", m_lpRight->getNodeID());
	
	string sOps = "//OPERATOR: " + sOperator + "\n";
	sOps = sOps + "	Operator* lp_WOS" + string(sNodeID);
	sOps = sOps + " = new " + sOperator + "(lp_WOS" + string(sLeftNodeID) + ", 0, lp_WOS" + string(sRightNodeID) + ", 0);\n";	
	
	lpOfstream->write(sOps.c_str(), sOps.length());
}

//---------------------------------------------------------------
// Memory management functions

// New a node and put it in the m_lpList
BNode* BNode::create()
{
	Log::writeToLog("BNode", 10, "Create without arguments");
		
	BNode* lpNode = new BNode();
	pushBack(lpNode);
	return lpNode;
}

// New a node and put it in the m_lpList
BNode* BNode::create(Node* lpLeft, Node* lpRight)
{
	Log::writeToLog("BNode", 10, "Create with 2 arguments: left node and right node");
		
	BNode* lpNode = new BNode(lpLeft, lpRight);
	pushBack(lpNode);
	return lpNode;
}

