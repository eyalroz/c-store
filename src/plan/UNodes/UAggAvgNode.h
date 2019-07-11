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
 *  UAggAvgNode.h
 *  cstore
 *
 *  Created by Nga Tran on 3/30/05.
 *  
 *	Avgerage node  without group by
 */

#ifndef _UAGGAVGNODE_H_
#define _UAGGAVGNODE_H_

#include <string>
#include "UAggNode.h"
#include "../../Operators/MergeSortedGroups.h"
#include "../../Operators/MergeOp.h"
#include "../../Operators/Operator.h"
#include "../PlanConstants.h"
#include "../../Operators/Aggregator.h"
#include "../../Operators/Average.h"


using namespace std;

class UAggAvgNode : public UAggNode
{
	protected:
		//MergeAvgOp*	m_lpMergeOp;

	protected:
		// Node's constructor is protected because they should not be new directly.
		// A node can be new by using static "create" function
		// This is for the purpose of memory management
		UAggAvgNode();
		UAggAvgNode(Node* lpAggCol, int iAggColIndex);
		
	public:
		virtual ~UAggAvgNode();
		
		// Merge ROS and WOS operators
		Operator* mergeROSandWOS();
		
		// Run ROS only
		Operator* runROS();
		
		// Run WOS only
		Operator* runWOS();	
		
		// show what operator this node will run
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
		static UAggAvgNode* create();
		static UAggAvgNode* create(Node* lpAggCol, int iAggColIndex);

};

#endif // _UAGGAVGNODE_H_



