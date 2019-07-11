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
 *  MDeleteProjectionNode.cpp
 *  cstore
 *
 *  Created by Nga Tran on 6/9/05.
 * 
 *	Node to do delete a projection
 */

#include "MDeleteProjectionNode.h"

MDeleteProjectionNode::MDeleteProjectionNode() : MNode()
{
}


MDeleteProjectionNode::MDeleteProjectionNode(list<Node*> lpChildren) : MNode(lpChildren)
{
}		


MDeleteProjectionNode::~MDeleteProjectionNode()
{
	  Log::writeToLog("MDeleteProjectionNode", 10, "~MDeleteProjectionNode"); 
}
	
		
// show tree presentation
void MDeleteProjectionNode::showTree(string sTabs)
{
	// Show its information first
	cout << endl;
	cout << sTabs << "DELETE from all columns of the projection" << endl;
	
	MNode::showTree(sTabs);
}


//---------------------------------------------------------------
// Memory management functions

// New a node and put it in the m_lpList
MDeleteProjectionNode* MDeleteProjectionNode::create()
{
	Log::writeToLog("MDeleteProjectionNode", 10, "Create without arguments");
		
	MDeleteProjectionNode* lpNode = new MDeleteProjectionNode();
	pushBack(lpNode);
	return lpNode;
}

// New a node and put it in the m_lpList
MDeleteProjectionNode* MDeleteProjectionNode::create(list<Node*>	lpChildren)
{
	Log::writeToLog("MDeleteProjectionNode", 10, "Create with 1 argument: list of child nodes");
		
	MDeleteProjectionNode* lpNode = new MDeleteProjectionNode(lpChildren);
	pushBack(lpNode);
	return lpNode;
}

