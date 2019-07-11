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
 *  MNode.cpp
 *  cstore
 *
 *  Created by Nga Tran on 3/31/05.
 *  
 *	This is a multi-children node in a plan tree
 */

#include "MNode.h"

#include <iostream>


MNode::MNode() : Node()
{
}

MNode::MNode(list<Node*> children) : Node()
{
	m_children = children;
}

// Deallocate memory for its members
MNode::~MNode()
{
	Log::writeToLog("MNode", 10, "~MNode"); 
}

// Add a child node
void MNode::add(Node* lpChild)
{
	m_children.push_back(lpChild);
}


// Add a list of child nodes
void MNode::add(list<Node*> children)
{
	list<Node*>::iterator it;
	for (it = children.begin(); it != children.end(); it++)
	{
		Node* lpChild = (*it);
		add(lpChild);
	}
}
		

// 
list<Node*> MNode::getChildren()
{
	return m_children;
}

// Merge ROS and WOS operators
Operator* MNode::mergeROSandWOS()
{
	// Never used
	return Node::mergeROSandWOS();
}


// Run the operator of this node
Operator* MNode::run(bool rosOnly)
{
 	// Run all child nodes first
	list<Node*>::iterator it;
	for (it = m_children.begin(); it != m_children.end(); it++)
	{
		Node* lpNode = (*it);
		if (lpNode != NULL)
		{
			// ROS
			Operator* lpOpROS = NULL;
			lpOpROS = lpNode->runROS();
			
			//srcsROS[i] = lpOpROS;
			cout << "	ROS " << lpNode->getNodeID() << " done"<< endl;

			if (!rosOnly)
			  {			
			    // WOS
			    Operator* lpOpWOS = NULL;
			    lpOpWOS = lpNode->runWOS();
			
			    //srcsWOS[i] = lpOpWOS;
			    cout << "	WOS " << lpNode->getNodeID() << " done"<< endl;
			  }
		}
	}
    

	return m_lpReturnOp;
}


// Run ROS only
Operator* MNode::runROS()
{
	//return Node::runROS();
	
	// Run all child nodes first
	list<Node*>::iterator it;
	for (it = m_children.begin(); it != m_children.end(); it++)
	{
		Node* lpNode = (*it);
		if (lpNode != NULL)
		{
			// ROS
			Operator* lpOpROS = NULL;
			lpOpROS = lpNode->runROS();
		}
	}
	
	cout << "	ROS done"<< endl;
	
	return m_lpROSReturnOp;
}

// Run WOS only
Operator* MNode::runWOS()
{
	//return Node::runWOS();
	
	// Run all child nodes first
	list<Node*>::iterator it;
	for (it = m_children.begin(); it != m_children.end(); it++)
	{
		Node* lpNode = (*it);
		if (lpNode != NULL)
		{
			// ROS
			Operator* lpOpROS = NULL;
			lpOpROS = lpNode->runWOS();
		}
	}
	
	cout << "	WOS done"<< endl;
	
	return m_lpWOSReturnOp;
}

// show what operator this node will run
void MNode::show(int* lpNodeID, ofstream* lpOfstream, bool rosOnly)
{
	//Node::show(lpNodeID, lpOfstream);
	
	// Set nodeID to have different variable name for this node
	setNodeID(lpNodeID);
	
	// Run all child nodes first
	int numCols = m_children.size();
	
	char sTmp[15];	
	sprintf(sTmp, "%d", numCols);
	
	list<Node*>::iterator it;
	for (it = m_children.begin(); it != m_children.end(); it++)
	{
		Node* lpNode = (*it);
		if (lpNode != NULL)
		{
			string sOps = "\n//ROS OPERATORS:\n";
			lpOfstream->write(sOps.c_str(), sOps.length());
			lpNode->showROS(lpNodeID, lpOfstream);
			
			if (!rosOnly)
			  {
			    sOps = "\n//WOS OPERATORS:\n";
			    lpOfstream->write(sOps.c_str(), sOps.length());
			    lpNode->showWOS(lpNodeID, lpOfstream);
			  }
		}
	}
}

// show what operator this node will run
void MNode::showROS(int* lpNodeID, ofstream* lpOfstream)
{
	// The operator exists
	if (m_bROSHasShown)
	{
		return;
	}
	
	m_bROSHasShown = 1;
	
	// Set nodeID to have different variable name for this node
	setNodeID(lpNodeID);
					
	// Run all child nodes first
	list<Node*>::iterator it;
	for (it = m_children.begin(); it != m_children.end(); it++)
	{
		Node* lpNode = (*it);
		if (lpNode != NULL)
		{
			lpNode->showROS(lpNodeID, lpOfstream);
		}
	}
}


// show what WOS operator this node will run
void MNode::showWOS(int* lpNodeID, ofstream* lpOfstream)
{
	// The operator exists
	if (m_bWOSHasShown)
	{
		return;
	}
	
	m_bWOSHasShown = 1;
		
	// Run all child nodes first
	list<Node*>::iterator it;
	for (it = m_children.begin(); it != m_children.end(); it++)
	{
		Node* lpNode = (*it);
		if (lpNode != NULL)
		{
			lpNode->showWOS(lpNodeID, lpOfstream);
		}
	}
}


// show tree presentation
void MNode::showTree(string sTabs)
{
	// The operator exists
	if (m_bTreeHasShown)
	{
		cout << endl;
		cout << sTabs << "The operator was created before" << endl;
		return;
	}

	int iNumNode = m_children.size();
	cout << sTabs << "Number of child nodes: " << iNumNode << endl;
		
	// Show all child nodes' information
	sTabs = sTabs + "	";
	list<Node*>::iterator it;
	for (it = m_children.begin(); it != m_children.end(); it++)
	{
		Node* lpNode = (*it);
		if (lpNode != NULL)
		{
			lpNode->showTree(sTabs);
		}
	}
	
	m_bTreeHasShown = 1;
}

// show merge operators
void MNode::showMerge(ofstream* lpOfstream)
{
	Node::showMerge(lpOfstream);
}

//---------------------------------------------------------------
// Memory management functions

// New a node and put it in the m_lpList
MNode* MNode::create()
{
	Log::writeToLog("MNode", 10, "Create without arguments");
		
	MNode* lpNode = new MNode();
	pushBack(lpNode);
	return lpNode;
}

// New a node and put it in the m_lpList
MNode* MNode::create(list<Node*>	lpChildren)
{
	Log::writeToLog("MNode", 10, "Create with 1 argument: list of child nodes");
		
	MNode* lpNode = new MNode(lpChildren);
	pushBack(lpNode);
	return lpNode;
}




