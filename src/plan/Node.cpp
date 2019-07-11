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
 *  Node.cpp
 *  cstore
 *
 *  Created by Nga Tran on 3/17/05.
 *  
 *
 */

#include "Node.h"



// Default
Node::Node()
{
	m_lpReturnOp = NULL;
	m_lpROSReturnOp = NULL;
	m_lpWOSReturnOp = NULL;
	m_bROSHasShown = 0;
	m_bWOSHasShown = 0;
	m_bTreeHasShown = 0;
	m_bHasShown = 0;
	
	m_iNodeID = -1;
	m_iPredicateType = PREDICATE_NO_JOIN;
}



// Deallocate memory for its members
Node::~Node()
{
	Log::writeToLog("Node", 10, "~Node"); 
}


// Set predicate type
void Node::setPredicateType(int iPredicateType)
{
	m_iPredicateType = iPredicateType;
}
		
// Get predicate type
int Node::getPredicateType()
{
	return m_iPredicateType;
}


// Get node id
int Node::getNodeID()
{
	return m_iNodeID;
}

// Set node id
void Node::setNodeID(int* lpNodeID)
{
	if (m_iNodeID == -1)
	{
		// nodeID has not been set
		m_iNodeID = (*lpNodeID);
		(*lpNodeID)++;
	}
}



// Merge ROS and WOS operators
Operator* Node::mergeROSandWOS()
{	
	// Never used
	return m_lpReturnOp;
}


// Run the operator of this node
Operator* Node::run(bool rosOnly)
{
	return m_lpROSReturnOp;
}

// Run ROS only
Operator* Node::runROS()
{
	return m_lpROSReturnOp;
}

// Run WOS only
Operator* Node::runWOS()
{
	return m_lpWOSReturnOp;
}

// show what operator this node will run
void Node::show(int* lpNodeID, ofstream* lpOfstream, bool rosOnly)
{
}


// show what operator this node will run
void Node::showROS(int* lpNodeID, ofstream* lpOfstream)
{
}

// show what WOS operator this node will run
void Node::showWOS(int* lpNodeID, ofstream* lpOfstream)
{
}
		

// show tree presentation
void Node::showTree(string sTabs)
{
}

// Reset to rerun
void Node::reset()
{
  cout << " I am resetting " << endl;
	if (m_lpReturnOp != NULL)
	{
		delete m_lpReturnOp;
		m_lpReturnOp = NULL;
	}
	
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
	
	m_bROSHasShown = 0;
	m_bTreeHasShown = 0;
}

// show merge operators
void Node::showMerge(ofstream* lpOfstream)
{
	// Abstract method. Should never be called
	cout << "SHOW MERGE" << endl;
}


//---------------------------------------------------------------
// Memory management functions
		
// New a node and put it in the m_lpList
Node* Node::create()
{
	Log::writeToLog("Node", 0, "Create");
		
	Node* lpNode = new Node();
	pushBack(lpNode);
	return lpNode;
}
		
