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
 *  SProjectUnionNode.h
 *  cstore
 *
 *  Created by Nga Tran on 6/22/05.
 *  
 *	Projection Node which has to use Union 
 *
 *	There will be 2 datasources for the projection which will be used to setPositionFilter
 *	for the joins and then union together
 */

#ifndef _SPROJECTUNIONNODE_H_
#define _SPROJECTUNIONNODE_H_

#include <string>
#include <list>

#include "../Node.h"
#include "../Nodes/SelectNode.h"
#include "../BNodes/BJoinNode.h"
#include "../Util/JoinOutputs.h"
#include "../PlanConstants.h"
#include "../../Operators/Union.h"
#include "../../Operators/Operator.h"
#include "../../common/DataSource.h"
#include "../../parser/Expressions/EColumn.h"


using namespace std;

class SProjectUnionNode: public Node
{
	protected:
		Node*	m_lpJoin;	
		Node*	m_lpSelect1;	// m_lpSelect1 and m_lpSelect2 will produce datasources of the same column
		Node*	m_lpSelect2;
		string	m_sProjection;	// Projection name of the column of m_lpSelect1 and m_lpSelect2
		
	protected:
		// Node's constructor is protected because they should not be new directly.
		// A node can be new by using static "create" function
		// This is for the purpose of memory management
		SProjectUnionNode();
		SProjectUnionNode(Node* lpJoin, EColumn* lpSelectionColumn);
		
	public:
		virtual ~SProjectUnionNode();
		
		// Run ROS
		Operator* runROS();
		
		// Run WOS
		Operator* runWOS();
		
		// Merge ROS and WOS operators
		Operator* mergeROSandWOS();
		
		// show what operators this node will run
		//void show(int* lpNodeID, ofstream* lpOfstream);
		
		// show what ROS operators this node will run
		void showROS(int* lpNodeID, ofstream* lpOfstream);
		
		// show what WOS operators this node will run
		void showWOS(int* lpNodeID, ofstream* lpOfstream);
		
		// show merge operators
		void showMerge(ofstream* lpOfstream);
		
		// show tree presentation
		void showTree(string sTabs);
		
	//---------------------------------------------------------------
	// Memory management functions
		
	public:		
		// New a node and put it in the m_lpList
		static SProjectUnionNode* create();
		static SProjectUnionNode* create(Node* lpJoin, EColumn* lpSelectionColumn);
};

#endif //_SPROJECTUNIONNODE_H_
