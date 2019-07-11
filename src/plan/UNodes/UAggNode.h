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
 *  UUAggNode.h
 *  cstore
 *
 *  Created by Nga Tran on 3/30/05.
 * 
 *	An aggregate node  without group by
 */

#ifndef _UAGGNODE_H_
#define _UAGGNODE_H_

#include <string>
#include "../UNode.h"
#include "../../Operators/MergeSortedGroups.h"
#include "../../Operators/MergeOp.h"
#include "../../Operators/Operator.h"
#include "../PlanConstants.h"
#include "../../Operators/Aggregator.h"

using namespace std;

class UAggNode : public UNode
{
	protected:
		int	m_iAggColIndex;		// Index of that column  ???

	protected:
		// Node's constructor is protected because they should not be new directly.
		// A node can be new by using static "create" function
		// This is for the purpose of memory management
		UAggNode();
		UAggNode(Node* lpAggCol, int iAggColIndex);
		
	public:
		virtual ~UAggNode();
		
		void setAggColIndex(int iAggColIndex);
		
		// Merge ROS and WOS operators
		virtual Operator* mergeROSandWOS();
		
		// Run the operators for this node		
		virtual Operator* run(bool rosOnly = 1);
		
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
		
	protected:
		// Common fucntions called in showROS
		void commonShowROS(int* lpNodeID, string sOperator, ofstream* lpOfstream);
		
		// Common fucntions called in showWOS
		void commonShowWOS(int* lpNodeID, string sOperator, ofstream* lpOfstream);
		
		// Common functions called in showWOS and ROS
		void commonShowROSWOS(string sOperator, string sPointerTyper, ofstream* lpOfstream);
		
		// Common fucntions called in showMerge
		void commonShowMerge(string sOperator, ofstream* lpOfstream);
		
		// Common functions calles in mergeRosAndWos
		// Return:
		//	. 0: Has been merged or error -> stop merging
		//	. 1: Go on to do other merging methods
		bool commonMergeRosAndWos();
	//---------------------------------------------------------------
	// Memory management functions
		
	public:		
		// New a node and put it in the m_lpList
		static UAggNode* create();
		static UAggNode* create(Node* lpAggCol, int iAggColIndex);
};

#endif // _UAGGNODE_H_
