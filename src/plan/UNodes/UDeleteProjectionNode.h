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
 *  UDeleteFromProjectionNode.h
 *  cstore
 *
 *  Created by Nga Tran on 6/23/05.
 *  
 *	Implementation of DELETE FROM projectionName WHERE ...
 */

#ifndef _UDELETEPROJECTIONNODE_H_
#define _UDELETEPROJECTIONNODE_H_

#include <string>
#include "../UNode.h"
#include "../PlanConstants.h"
#include "../../AM/WOSManager.h"
#include "../../Operators/DeleteOperator.h"
#include "../Nodes/SelectNode.h"

using namespace std;

class UDeleteProjectionNode : public UNode
{
	protected:
		DeleteOperator* m_lpDeleteOperator;
		bool			m_bDeleteOperatorHasShown;
		string			m_sProjection;
		bool			m_bIsSinglePredicate; // true if Where predicate is a single predicate
		
	protected:
		// Node's constructor is protected because they should not be new directly.
		// A node can be new by using static "create" function
		// This is for the purpose of memory management
		UDeleteProjectionNode();
		UDeleteProjectionNode(Node* lpChild, string sProjection, bool bIsSinglePredicate);
		
	public:
		virtual ~UDeleteProjectionNode();

		
		// Run ROS only
		Operator* runROS();
		
		// Run WOS only
		Operator* runWOS();
		
		// Merge ROS and WOS operators
		Operator* mergeROSandWOS();
		
		// Shows what operator this node will run
		void showROS(int* lpNodeID, ofstream* lpOfstream);
		
		// show what WOS operator this node will run
		void showWOS(int* lpNodeID, ofstream* lpOfstream);
		
		// show tree presentation
		void showTree(string sTabs);
		
		// show merge operators
		void showMerge(ofstream* lpOfstream);
		
	//---------------------------------------------------------------
	// Memory management functions
		
	public:		
		// New a node and put it in the m_lpList
		static UDeleteProjectionNode* create();
		static UDeleteProjectionNode* create(Node* lpChild, string sProjection, bool bIsSinglePredicate);
};

#endif //_UDELETEPROJECTIONNODE_H_

