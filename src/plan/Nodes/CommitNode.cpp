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
 *  CommitNode.cpp
 *  cstore
 *
 *  Created by Nga Tran on 6/6/05.
 *  
 *
 */

#include "CommitNode.h"

CommitNode::CommitNode() : Node()
{
}

CommitNode::~CommitNode()
{
	 Log::writeToLog("CommitNode", 10, "~CommitNode");  
}
		
// Run ROS only
Operator* CommitNode::runROS()
{
	if (m_lpROSReturnOp != NULL)
	{
		return m_lpROSReturnOp;
	}
	
	// TODO .....
	cout << "COMMIT has not implemented yet" << endl;
	return m_lpROSReturnOp;
}

		
// Run WOS only
Operator* CommitNode::runWOS()
{
	if (m_lpWOSReturnOp != NULL)
	{
		return m_lpWOSReturnOp;
	}
	
	// TODO .....
	cout << "COMMIT has not implemented yet" << endl;	
	return m_lpWOSReturnOp;
}

		
// show what operator this node will run
void CommitNode::showROS(int* lpNodeID, ofstream* lpOfstream)
{
	// The operator exists
	if (m_bROSHasShown)
	{
		return;
	}
	
	m_bROSHasShown = 1;
	
	string sOps = "	//COMMIT has not implemented yet\n";
	lpOfstream->write(sOps.c_str(), sOps.length());
}

		
// show what WOS operator this node will run
void CommitNode::showWOS(int* lpNodeID, ofstream* lpOfstream)
{
	// The operator exists
	if (m_bWOSHasShown)
	{
		return;
	}
	
	m_bWOSHasShown = 1;
	
	string sOps = "	//COMMIT has not implemented yet\n";
	lpOfstream->write(sOps.c_str(), sOps.length());
}

		
// show tree presentation
void CommitNode::showTree(string sTabs)
{
	// The operator exists
	if (m_bTreeHasShown)
	{
		cout << endl;
		cout << sTabs << "COMMIT - The operator was created before" << endl;
		
		return;
	}
	
	m_bTreeHasShown = 1;
	
	cout << sTabs << "COMMIT has not implemented yet" << endl;	
}

//---------------------------------------------------------------
// Memory management functions

// New a node and put it in the m_lpList
CommitNode* CommitNode::create()
{
	Log::writeToLog("CommitNode", 10, "Create without arguments");
		
	CommitNode* lpNode = new CommitNode();
	pushBack(lpNode);
	return lpNode;
}
