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
 *  BAggNode.cpp
 *  cstore
 *
 *  Created by Nga Tran on 3/30/05.
 *  
 *	An aggregate node  with group by
 */

#include "BAggNode.h"

#include <iostream>


// Default
BAggNode::BAggNode() : BNode()
{
	m_iAggColIndex = 0;
	m_iGroupColIndex = 0;
	m_bUnsortedGroupBy = false;
	m_iHashTableSize = 150000;
	m_lpROSIdHashFunction = NULL;
	m_lpWOSIdHashFunction = NULL;
}


// Initialize with column to be aggregated
BAggNode::BAggNode(Node* lpAggCol, int iAggColIndex, Node* lpAggGroup, int iGroupColIndex) : BNode(lpAggGroup, lpAggCol)
{
	m_iAggColIndex = iAggColIndex;
	m_iGroupColIndex = iGroupColIndex;
	m_bUnsortedGroupBy = false;
	m_lpROSIdHashFunction = NULL;
	m_lpWOSIdHashFunction = NULL;
}


// Deallocate memory
BAggNode::~BAggNode()
{
	 Log::writeToLog("BAggNode", 10, "~BAggNode"); 
}


void BAggNode::setAggColIndex(int iAggColIndex)
{
	m_iAggColIndex = iAggColIndex;
}

void BAggNode::setGroupColIndex(int iGroupColIndex)
{
	m_iGroupColIndex = iGroupColIndex;
}


void BAggNode::setUnsortedGroupBy(int iHashTableSize)
{
	m_bUnsortedGroupBy = true;
	m_iHashTableSize = iHashTableSize;
}

// Merge ROS and WOS operators
Operator* BAggNode::mergeROSandWOS()
{
	return BNode::mergeROSandWOS();
}

// Run the operators for this node
Operator* BAggNode::run(bool rosOnly)
{
	return BNode::run(rosOnly);
}

// Run ROS only
Operator* BAggNode::runROS()
{
	return BNode::runROS();
}

// Run WOS only
Operator* BAggNode::runWOS()
{
	return BNode::runWOS();
}


// show what operator this node will run
void BAggNode::showROS(int* lpNodeID, ofstream* lpOfstream)
{
	BNode::showROS(lpNodeID, lpOfstream);
}

// show what WOS operator this node will run
void BAggNode::showWOS(int* lpNodeID, ofstream* lpOfstream)
{
	BNode::showWOS(lpNodeID, lpOfstream);
}

// show tree presentation
void BAggNode::showTree(string sTabs)
{
	BNode::showTree(sTabs);
}

// show merge operators
void BAggNode::showMerge(ofstream* lpOfstream)
{
	BNode::showMerge(lpOfstream);
}

// Common fucntions called in showROS
void BAggNode::commonShowROS(int* lpNodeID, string sOperator, ofstream* lpOfstream)
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
	
	commonShowROSWOS(sOperator, "ROS", lpOfstream);
}


// Common fucntions called in showROS
void BAggNode::commonShowWOS(int* lpNodeID, string sOperator, ofstream* lpOfstream)
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
	
	commonShowROSWOS(sOperator, "WOS", lpOfstream);
}


// Common functions called in showROS and WOS
void BAggNode::commonShowROSWOS(string sOperator, string sPointerType, ofstream* lpOfstream)
{	
	char sNodeID[15];	
	sprintf(sNodeID, "%d", m_iNodeID);
	
	char sLeftNodeID[15];	
	sprintf(sLeftNodeID, "%d", m_lpLeft->getNodeID());
	
	char sRightNodeID[15];	
	sprintf(sRightNodeID, "%d", m_lpRight->getNodeID());
	
	char sAggColIndex[15];	
	sprintf(sAggColIndex, "%d", m_iAggColIndex);
	
	char sGroupColIndex[15];	
	sprintf(sGroupColIndex, "%d", m_iGroupColIndex);
	
	string sOps = "//OPERATOR: " + sOperator + "\n";
	
	if (m_bUnsortedGroupBy)
	{
		// Unsorted groupy column, use hash aggregate
		char sHashTableSize[15];	
		sprintf(sHashTableSize, "%d", m_iHashTableSize);
		
		//m_lpROSReturnOp = new HashXXX(lpRight, m_iAggColIndex, lpLeft, m_iGroupColIndex);
		sOps = sOps + "	Operator* lp_" + sPointerType + string(sNodeID);
		sOps = sOps + " = new Hash" + sOperator + "(lp_" + sPointerType + string(sRightNodeID) + ", " + sAggColIndex; 
		sOps = sOps  + ", lp_" + sPointerType + string(sLeftNodeID) + ", " + sGroupColIndex + ");\n";
		
		//m_lpROSReturnOp->setHashFunction(new IdentityHashFunction(m_iHashTableSize));
		sOps = sOps + "	((HashAggregator*) lp_" + sPointerType + string(sNodeID) + ")";
		sOps = sOps + "->setHashFunction(new IdentityHashFunction(";
		sOps = sOps + sHashTableSize + "));\n";
		
		//m_lpROSReturnOp->setHashTableSize(m_iHashTableSize);
		sOps = sOps + "	((HashAggregator*) lp_" + sPointerType + string(sNodeID) + ")";
		sOps = sOps + "->setHashTableSize(" + sHashTableSize + ");\n";
	}
	else
	{
		// m_lpROSReturnOp = new XXX(lpRight, m_iAggColIndex, lpLeft, m_iGroupColIndex);
		sOps = sOps + "	Operator* lp_" + sPointerType + string(sNodeID);
		sOps = sOps + " = new " + sOperator + "(lp_" + sPointerType + string(sRightNodeID) + ", " + sAggColIndex; 
		sOps = sOps  + ", lp_" + sPointerType + string(sLeftNodeID) + ", " + sGroupColIndex + ");\n";
	}
	
	lpOfstream->write(sOps.c_str(), sOps.length());
}


// Common fucntions called in showMerge
void BAggNode::commonShowMerge(string sOperator, ofstream* lpOfstream)
{
	char sNodeID[15];	
	sprintf(sNodeID, "%d", m_iNodeID);
	
	string sOps = "//MERGE SORTED GROUP:\n";
	sOps = sOps + "	Operator* lp_Merge" + string(sNodeID) + " = new MergeSortedGroups(lp_ROS" + string(sNodeID) + ", lp_WOS" + string(sNodeID) + ", 2, 0); \n";
	sOps = sOps + "	((MergeSortedGroups*) lp_Merge" + string(sNodeID) + ")->setMergeOp(new Merge" + sOperator + "Op());\n";
	
	lpOfstream->write(sOps.c_str(), sOps.length());
}


// common show tree presentation
void BAggNode::commonShowTree(string sOperator, string sTabs)
{	
	cout << sTabs << "Operator: " << sOperator << endl;
	cout << sTabs << "	" << "Argument1: Operator for the aggregate (second below)" << endl;
	cout << sTabs << "	" << "Argument2: Column index of the argument1: " << m_iAggColIndex << endl;
	cout << sTabs << "	" << "Argument3: Operator for the group (first below)" << endl;
	cout << sTabs << "	" << "Argument4: Column index of the argument3: " << m_iGroupColIndex << endl;
}

//---------------------------------------------------------------
// Memory management functions

// New a node and put it in the m_lpList
BAggNode* BAggNode::create()
{
	Log::writeToLog("BAggNode", 10, "Create without arguments");
		
	BAggNode* lpNode = new BAggNode();
	pushBack(lpNode);
	return lpNode;
}

// New a node and put it in the m_lpList
BAggNode* BAggNode::create(Node* lpAggCol, int iAggColIndex, Node* lpAggGroup, int iGroupColIndex)
{
	Log::writeToLog("BAggNode", 10, "Create with arguments: Node* lpAggCol, int iAggColIndex, Node* lpAggGroup, int iGroupColIndex");
		
	BAggNode* lpNode = new BAggNode(lpAggCol, iAggColIndex, lpAggGroup, iGroupColIndex);
	pushBack(lpNode);
	return lpNode;
}

