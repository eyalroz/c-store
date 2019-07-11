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
 *  UUAggNode.cpp
 *  cstore
 *
 *  Created by Nga Tran on 3/30/05.
 *  
 *
 */

#include "UAggNode.h"

#include <iostream>


// Default
UAggNode::UAggNode() : UNode()
{
	m_iAggColIndex = 0;
}


// Initialize with column to be aggregated
UAggNode::UAggNode(Node* lpAggCol, int iAggColIndex) : UNode(lpAggCol)
{
	m_iAggColIndex = iAggColIndex;
}


// Deallocate memory
UAggNode::~UAggNode()
{
	 Log::writeToLog("UAggNode", 10, "~UAggNode");
	 
	 if(m_lpReturnOp  != NULL)
	 {
		delete m_lpReturnOp;
		m_lpReturnOp = NULL;
	 }
}


void UAggNode::setAggColIndex(int iAggColIndex)
{
	m_iAggColIndex = iAggColIndex;
}

// Merge ROS and WOS operators
Operator* UAggNode::mergeROSandWOS()
{
	// Never used
	return UNode::mergeROSandWOS();
}

// Run the operators for this node
Operator* UAggNode::run(bool rosOnly)
{
	return UNode::run(rosOnly);
}

// Run ROS only
Operator* UAggNode::runROS()
{
	return UNode::runROS();
}

// Run WOS only
Operator* UAggNode::runWOS()
{
	return UNode::runWOS();
}

// show what operator this node will run
void UAggNode::showROS(int* lpNodeID, ofstream* lpOfstream)
{
	UNode::showROS(lpNodeID, lpOfstream);
}

// show what WOS operator this node will run
void UAggNode::showWOS(int* lpNodeID, ofstream* lpOfstream)
{
	UNode::showWOS(lpNodeID, lpOfstream);
}

// show tree presentation
void UAggNode::showTree(string sTabs)
{
	UNode::showTree(sTabs);
}

// show merge operators
void UAggNode::showMerge(ofstream* lpOfstream)
{
	UNode::showMerge(lpOfstream);
}

// Common functions called in showROS
void UAggNode::commonShowROS(int* lpNodeID, string sOperator, ofstream* lpOfstream)
{
	UNode::commonShowROS(lpNodeID, sOperator, lpOfstream);
	
	commonShowROSWOS(sOperator, "ROS", lpOfstream);
}

// Common functions called in showWOS
void UAggNode::commonShowWOS(int* lpNodeID, string sOperator, ofstream* lpOfstream)
{
	UNode::commonShowWOS(lpNodeID, sOperator, lpOfstream);
	
	commonShowROSWOS(sOperator, "WOS", lpOfstream);
}

// Common functions called in showWOS and ROS
void UAggNode::commonShowROSWOS(string sOperator, string sPointerTyper, ofstream* lpOfstream)
{	
	char sNodeID[15];	
	sprintf(sNodeID, "%d", m_iNodeID);
		
	char sChildNodeID[15];	
	sprintf(sChildNodeID, "%d", m_lpChild->getNodeID());
	
	char sAggColIndex[15];	
	sprintf(sAggColIndex, "%d", m_iAggColIndex);
	
	string sOps = "//OPERATOR: " + sOperator+ "\n";
	sOps = sOps + "	Operator* lp_" + sPointerTyper + sNodeID + " = new " + sOperator;
	sOps = sOps + "(lp_" + sPointerTyper + sChildNodeID + ", " +  sAggColIndex + ");\n";
		
	lpOfstream->write(sOps.c_str(), sOps.length());
}

// Common fucntions called in showMerge
void UAggNode::commonShowMerge(string sOperator, ofstream* lpOfstream)
{
	char sNodeID[15];	
	sprintf(sNodeID, "%d", m_iNodeID);
	
	string sOps = "//MERGE SORTED GROUP:\n";
	sOps = sOps + "	Operator* lp_Merge" + string(sNodeID) + " = new MergeSortedGroups(lp_ROS" + string(sNodeID) + ", lp_WOS" + string(sNodeID) + ", 1, -1); \n";
	sOps = sOps + "	((MergeSortedGroups*) lp_Merge" + string(sNodeID) + ")->setMergeOp(new Merge" + sOperator + "Op());\n";

	lpOfstream->write(sOps.c_str(), sOps.length());
}

// Common functions calles in mergeRosAndWos
bool UAggNode::commonMergeRosAndWos()
{
	bool  bGoOn = 0;
	if (m_lpReturnOp != NULL)
	{
		// Has been merged
		return bGoOn;
	}
	
	if ( (m_lpROSReturnOp == NULL) || (m_lpWOSReturnOp == NULL) )
	{
		cout << "ERROR: Invalid ros or wos. The results won't be correct." << endl;
		return bGoOn;
	}
		
	// Merge them
	m_lpReturnOp = new MergeSortedGroups(	m_lpROSReturnOp,
											m_lpWOSReturnOp,
                                               1,      // num columns
											   -1);     // merge by col index
												
	bGoOn = 1;
	return bGoOn;
}

//---------------------------------------------------------------
// Memory management functions
		
	
// New a node and put it in the m_lpList
UAggNode* UAggNode::create()
{
	Log::writeToLog("UAggNode", 10, "Create without arguments");
		
	UAggNode* lpNode = new UAggNode();
	pushBack(lpNode);
	return lpNode;
}

UAggNode* UAggNode::create(Node* lpAggCol, int iAggColIndex)
{
	Log::writeToLog("UAggNode", 10, "Create with arguments: Node* lpAggCol, int iAggColIndex");
		
	UAggNode* lpNode = new UAggNode(lpAggCol, iAggColIndex);
	pushBack(lpNode);
	return lpNode;
}


