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
 *  BAggCountNode.h
 *  cstore
 *
 *  Created by Nga Tran on 3/30/05.
 *  
 *	Count node  with group by
 */


#ifndef _BAGGCOUNTNODE_H_
#define _BAGGCOUNTNODE_H_

#include <string>
#include "BAggNode.h"
#include "../../Operators/MergeSortedGroups.h"
#include "../../Operators/MergeOp.h"
#include "../../Operators/Operator.h"
#include "../PlanConstants.h"
#include "../../Operators/Aggregator.h"
#include "../../Operators/Count.h"

using namespace std;

class BAggCountNode : public BAggNode
{
	protected:
		MergeCountOp*	m_lpMergeOp;

	protected:
		// Node's constructor is protected because they should not be new directly.
		// A node can be new by using static "create" function
		// This is for the purpose of memory management
		BAggCountNode();
		BAggCountNode(Node* lpAggCol, int iAggColIndex, Node* lpAggGroup, int iGroupColIndex);
		
	public:
		virtual ~BAggCountNode();
		
		// Merge ROS and WOS operators
		virtual Operator* mergeROSandWOS();
		
		// Run ROS only
		virtual Operator* runROS();
		
		// Run WOS only
		virtual Operator* runWOS();
		
		// show what operator this node will run
		virtual void showROS(int* lpNodeID, ofstream* lpOfstream);
		
		// show what WOS operator this node will run
		virtual void showWOS(int* lpNodeID, ofstream* lpOfstream);
	
		// show tree presentation
		virtual void showTree(string sTabs);
		
		// show merge operators
		virtual void showMerge(ofstream* lpOfstream);
		
		
	//---------------------------------------------------------------
	// Memory management functions
		
	public:		
		// New a node and put it in the m_lpList
		static BAggCountNode* create();
		static BAggCountNode* create(Node* lpAggCol, int iAggColIndex, Node* lpAggGroup, int iGroupColIndex);

};

#endif // _BAGGCOUNTNODE_H_


