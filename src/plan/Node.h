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
 *  Node.h
 *  cstore
 *
 *  Created by Nga Tran on 3/17/05.
 *  
 *	This is a node in a plan tree
 *	A node can have no or one or two direct childen
 */
  
#ifndef _NODE_H_
#define _NODE_H_

#include <fstream>
#include <iostream>
#include <list>
#include <string>

#include "BaseNode.h"
#include "PlanConstants.h"
#include "../Operators/Operator.h"
#include "../Util/Log.h"


using namespace std;

class Node : public BaseNode
{
	protected:
		Operator* m_lpReturnOp;		// merged operator of ROS and WOS
		Operator* m_lpROSReturnOp;	// Returned operator for ROS
		Operator* m_lpWOSReturnOp;	// Returned operator for WOS
		
		int	m_iNodeID;				// ID of this node. This id is only used at showROS/WOS 
									// for the purpose of variable names
		
		bool m_bHasShown;		// will be set to TRUE (1) after being shown
		bool m_bROSHasShown;		// will be set to TRUE (1) after being shown
		bool m_bWOSHasShown;		// will be set to TRUE (1) after being shown
		bool m_bTreeHasShown;		// will be set to TRUE (1) after being shown
		
		int m_iPredicateType;		// no join, one join, join and other
	
	protected:
		// Node's constructor is protected because they should not be new directly.
		// A node can be new by using static "create" function
		// This is for the purpose of memory management
		Node();
		
	public:
		virtual ~Node();
		
		// Set predicate type
		virtual void setPredicateType(int iPredicateType);
		
		// Get predicate type
		virtual int getPredicateType();

		
		// Get node id
		virtual int getNodeID();
		
		// Set node id
		void setNodeID(int* lpNodeID);
		
		// Merge ROS and WOS operators
		virtual Operator* mergeROSandWOS();
		
		// Run the operator of this node
		virtual Operator* run(bool rosOnly = 1);	
		
		// Run ROS only
		virtual Operator* runROS();
		
		// Run WOS only
		virtual Operator* runWOS();
		
		// show what ROS operator this node will run
		virtual void show(int* lpNodeID, ofstream* lpOfstream, bool rosOnly = 1);
		
		// show what ROS operator this node will run
		virtual void showROS(int* lpNodeID, ofstream* lpOfstream);
		
		// show what WOS operator this node will run
		virtual void showWOS(int* lpNodeID, ofstream* lpOfstream);
		
		// show tree presentation
		virtual void showTree(string sTabs);
		
		// show merge operators
		virtual void showMerge(ofstream* lpOfstream);
		
		// Reset to rerun
		void reset();
	
	//---------------------------------------------------------------
	// Memory management functions
		
	public:
		
		// New a node and put it in the m_lpList
		static Node* create();
};

#endif // _NODE_H_
