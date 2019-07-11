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
 *  BJoinNode.h
 *  cstore
 *
 *  Created by Nga Tran on 3/23/05.
 * 
 *  Join node
 *  Join is a doing differnetly from other nodes because ROS and WOS have to join together
 */
 
#ifndef _BJOINNODE_H_
#define _BJOINNODE_H_

#include <string>

#include "../BNode.h"
#include "../../Operators/Operator.h"
#include "../../Operators/NLJoin.h"
#include "../PlanConstants.h"
#include "../../common/Predicate.h"
#include "../../parser/Expressions/EColumn.h"
#include "../Nodes/SelectNode.h"
#include "../Util/JoinOutputs.h"

using namespace std;

class BJoinNode : public BNode
{
	protected:
		SelectNode*	m_lpLeftReplica;	// 4 joins so datacource replica is needed
		SelectNode*	m_lpRightReplica;	// 4 joins so datacource replica is needed
		
		int		m_iComparisonType;
		bool	m_bIsRun;
		
		JoinOutputs*	m_lpJoinOutputs; // Four output joins
		
		Operator*      m_lpLeftROS;
		Operator*      m_lpRightROS;
		
		
	protected:
		// Node's constructor is protected because they should not be new directly.
		// A node can be new by using static "create" function
		// This is for the purpose of memory management
		BJoinNode();
		//BJoinNode(Node* lpleft, Node* lpRight, int iComparisonType);
		BJoinNode(EColumn* lpLeft, EColumn* lpRight, int iComparisonType);
		
	public:
		virtual ~BJoinNode();
		
		// Get 4 join outputs
		virtual JoinOutputs* getJoinOutputs();
		
		// Run 
		virtual Operator* run(bool rosOnly = 1);
		
		// show tree presentation
		virtual void show(int* lpNodeID, ofstream* lpOfstream, bool rosOnly = 1);
		
		// show tree presentation
		virtual void showTree(string sTabs);
		
		// Check and set predicate
		void checkAndSetPredicate(SelectNode* lpNode);
		
	//---------------------------------------------------------------
	// Memory management functions
		
	public:		
		// New a node and put it in the m_lpList
		static BJoinNode* create();
		static BJoinNode* create(EColumn* lpLeft, EColumn* lpRight, int iComparisonType);

};

#endif // _BJOINNODE_H_

