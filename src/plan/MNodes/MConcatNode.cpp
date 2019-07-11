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
 *  MConcatNode.cpp
 *  cstore
 *
 *  Created by Nga Tran on 3/31/05.
 *  
 *	Concat Node which concats many operators
 */

#include "MConcatNode.h"

#include <iostream>


MConcatNode::MConcatNode() : MNode()
{
}

MConcatNode::MConcatNode(list<Node*> lpChildren) : MNode(lpChildren)
{
}

// Deallocate memory for its members
MConcatNode::~MConcatNode()
{
	 Log::writeToLog("MConcatNode", 10, "~MConcatNode");  
}

// Merge ROS and WOS operators
Operator* MConcatNode::mergeROSandWOS()
{
	// Never used
	return MNode::mergeROSandWOS();
}


// Run ROS only
Operator* MConcatNode::runROS()
{
	if (m_lpROSReturnOp != NULL)
	{
		return m_lpROSReturnOp;
	}
	
	if (m_children.size() > 0)
	{
		// will be different later
		m_lpROSReturnOp = (m_children.front())->runROS();
	}
	
	return m_lpROSReturnOp;
}

// Run WOS only
Operator* MConcatNode::runWOS()
{
	if (m_lpWOSReturnOp != NULL)
	{
		return m_lpWOSReturnOp;
	}
	
	if (m_children.size() > 0)
	{
		// will be different later
		m_lpWOSReturnOp = (m_children.front())->runWOS();
	}
	
	return m_lpWOSReturnOp;
}

// show what operator this node will run
void MConcatNode::showROS(int* lpNodeID, ofstream* lpOfstream)
{
	// The operator exists
	if (m_bROSHasShown)
	{		
		return;
	}
	
	m_bROSHasShown = 1;
	
	// Set nodeID to have different variable name for this node
	setNodeID(lpNodeID);
	
	if (m_children.size() > 0)
	{
		// will be different later
		(m_children.front())->showROS(lpNodeID, lpOfstream);
	}
	
	cout << "CONCAT:" << endl;
	cout << "	Later...................." << endl;
}

// show what operator this node will run
void MConcatNode::showWOS(int* lpNodeID, ofstream* lpOfstream)
{
	// The operator exists
	if (m_bWOSHasShown)
	{		
		return;
	}
	
	m_bWOSHasShown = 1;
	
	// Set nodeID to have different variable name for this node
	setNodeID(lpNodeID);
	
	if (m_children.size() > 0)
	{
		// will be different later
		(m_children.front())->showWOS(lpNodeID, lpOfstream);
	}
	
	cout << "CONCAT:" << endl;
	cout << "	Later...................." << endl;
}


// show merge operators
void MConcatNode::showMerge(ofstream* lpOfstream)
{
	MNode::showMerge(lpOfstream);
}

// show tree presentation
void MConcatNode::showTree(string sTabs)
{
	MNode::showTree(sTabs);
}


//---------------------------------------------------------------
// Memory management functions

// New a node and put it in the m_lpList
MConcatNode* MConcatNode::create()
{
	Log::writeToLog("MConcatNode", 10, "Create without arguments");
		
	MConcatNode* lpNode = new MConcatNode();
	pushBack(lpNode);
	return lpNode;
}

// New a node and put it in the m_lpList
MConcatNode* MConcatNode::create(list<Node*>	lpChildren)
{
	Log::writeToLog("MConcatNode", 10, "Create with 1 argument: list of child nodes");
		
	MConcatNode* lpNode = new MConcatNode(lpChildren);
	pushBack(lpNode);
	return lpNode;
}


