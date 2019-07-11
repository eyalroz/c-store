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
 *  BaseBaseNode.h
 *  cstore
 *
 *  Created by Nga Tran on 7/7/05.
 *  
 *	Base node for memory management
 */

#include "BaseNode.h"

// Default
BaseNode::BaseNode()
{
}



// Deallocate memory for its members
BaseNode::~BaseNode()
{
	Log::writeToLog("BaseNode", 10, "~BaseNode"); 
}


// Memory management functions

list<BaseNode*>* BaseNode::m_lpList = NULL;


// Create an empty list (m_lpList) to keep track allocated nodes
// If the list is available, do nothing
void BaseNode::initialize()
{
	Log::writeToLog("BaseNode", 0, "Initialize");

	if ( m_lpList == NULL )
	{
		m_lpList = new list<BaseNode*>();
	}
}
		
// New a node and put it in the m_lpList
BaseNode* BaseNode::create()
{
	Log::writeToLog("BaseNode", 0, "Create");
		
	BaseNode* lpBaseNode = new BaseNode();
	pushBack(lpBaseNode);
	return lpBaseNode;
}
		
// Put a node in the m_lpList
void BaseNode::pushBack(BaseNode* lpBaseNode)
{
	Log::writeToLog("BaseNode", 10, "PushBack");
	
	m_lpList->push_back(lpBaseNode);
}
		
// Deallocate memory for all nodes in m_lpList and empty that list
void BaseNode::uninitialize()
{
	Log::writeToLog("BaseNode", 10, "Uninitialize");
	
	if ( m_lpList != NULL )
	{
		string s = "	List size: " + m_lpList->size();		
	    Log::writeToLog("BaseNode", 10, s);	

		for (list<BaseNode*>::iterator it = m_lpList->begin(); it != m_lpList->end(); it++)
		{
			BaseNode* lpBaseNode = (*it);
			delete lpBaseNode;
		}
		m_lpList->clear();
		
		delete m_lpList;
		m_lpList = NULL;
	}
	
	Log::writeToLog("BaseNode", 10, "Uninitialize: Done");	
}


