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
 *  UNode.cpp
 *  cstore
 *
 *  Created by Nga Tran on 3/17/05.
 *  
 *
 */

#include "UNode.h"

#include <iostream>

UNode::UNode() : Node()
{
	m_lpChild = NULL;
}

UNode::UNode(Node* lpChild) : Node()
{
	m_lpChild = lpChild;
}

// Deallocate memory for its members
UNode::~UNode()
{
	 Log::writeToLog("UNode", 10, "~UNode"); 
}

// Get node id
int UNode::getNodeID()
{
	return Node::getNodeID();
}

void UNode::setChild(Node* lpChild)
{
	m_lpChild = lpChild;
}

Node* UNode::getChild()
{
	return m_lpChild;
}

// Merge ROS and WOS operators
Operator* UNode::mergeROSandWOS()
{
	// Never used
	return Node::mergeROSandWOS();
}


// Run the operator of this node
Operator* UNode::run(bool rosOnly)
{
	return Node::run(rosOnly);
}

// Run ROS only
Operator* UNode::runROS()
{
	return Node::runROS();
}

// Run WOS only
Operator* UNode::runWOS()
{
	return Node::runWOS();
}

// show what operator this node will run
void UNode::showROS(int* lpNodeID, ofstream* lpOfstream)
{
	Node::showROS(lpNodeID, lpOfstream);
}

// show what WOS operator this node will run
void UNode::showWOS(int* lpNodeID, ofstream* lpOfstream)
{
	Node::showWOS(lpNodeID, lpOfstream);
}


// show tree presentation
void UNode::showTree(string sTabs)
{
	Node::showTree(sTabs);
}

// show merge operators
void UNode::showMerge(ofstream* lpOfstream)
{
	Node::showMerge(lpOfstream);
}

// Common fucntions called in showROS
void UNode::commonShowROS(int* lpNodeID, string sOperator, ofstream* lpOfstream)
{
	m_bROSHasShown = 1;
	
	// Set nodeID to have different variable name for this node
	setNodeID(lpNodeID);
	
	if (m_lpChild != NULL)
	{
		m_lpChild->showROS(lpNodeID, lpOfstream);
	}
}


// Common fucntions called in showWOS
void UNode::commonShowWOS(int* lpNodeID, string sOperator, ofstream* lpOfstream)
{
	m_bWOSHasShown = 1;
	
	// Set nodeID to have different variable name for this node
	setNodeID(lpNodeID);
	
	if (m_lpChild != NULL)
	{
		m_lpChild->showWOS(lpNodeID, lpOfstream);
	}
}

//---------------------------------------------------------------
// Memory management functions
		
	
// New a node and put it in the m_lpList
UNode* UNode::create()
{
	Log::writeToLog("UNode", 10, "Create without arguments");
		
	UNode* lpNode = new UNode();
	pushBack(lpNode);
	return lpNode;
}

UNode* UNode::create(Node* lpChild)
{
	Log::writeToLog("UNode", 10, "Create with 1 argument: child node");
		
	UNode* lpNode = new UNode(lpChild);
	pushBack(lpNode);
	return lpNode;
}

