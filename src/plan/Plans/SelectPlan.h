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
 *  SelectPlan.h
 *  cstore
 *
 *  Created by Nga Tran on 6/20/05.
 *  
 *	Plan for select query
 */

#ifndef _SELECTPLAN_H_
#define _SELECTPLAN_H_

class QSelect;

#include <vector>

#include "../Plan.h"
#include "../PlanConstants.h"
#include "../BNodes/BAggNode.h"
#include "../BNodes/BConjunctionNode.h"
#include "../MNodes/MResultNode.h"
#include "../SNodes/SProjectUnionNode.h"
#include "../UNodes/UCopyNode.h"
#include "../../parser/Queries/QSelect.h"

class SelectPlan : public Plan
{
	protected:
		// Node of where predicate in which there is only one attribute (column)
		Node*	m_lpPredicate;
		
		// Store for deallocating later
		vector<Node*> m_deletionList;

	protected:
		// Node's constructor is protected because they should not be new directly.
		// A node can be new by using static "create" function
		// This is for the purpose of memory management
		SelectPlan();
		
	public:
		virtual ~SelectPlan();
		
		// Generate a plan
		void makePlan(Query* lpQuery);
		
	protected:
		// Parse the query (SELECT) into specific lists
		void parseQuery(
			Query* lpQuery, list<EHolder*> *lpSelections, list<EHolder*> *lpAggregates,
			ListProjections** lpProjections, BExpression** lpWherePredicate);

		// Show query
		void showQuery(
			list<EHolder*> selections, list<EHolder*> aggregates,
			ListProjections* lpProjections, BExpression* lpWherePredicate);
			
		// Create copy for masking later
		Node* createCopyNode(
			Node* lpChild, bool* bNeedPositionFilter, StringNodeMap* lpMapColumns,
			list<EHolder*> selections, list<EHolder*> aggregates);

		// Create selection nodes
		Node* createSelectionNodes(Node* lpChild, bool bNeedPositionFilter, list<EHolder*> selections,
			Node** lpFirstProject, Node** lpGoup, StringNodeMap mapColumns, int* lpCopyIndex);
			
		// Create selection nodes for join predicate
		Node* createSelectionNodesForJoin(Node* lpChild, list<EHolder*> selections);
		
		// Create aggregate nodes
		Node* createAggregateNodes(Node* lpChild,  bool bNeedPositionFilter, list<EHolder*> selections,
			 Node* lpFirstProject, Node* lpGroup, StringNodeMap mapColumns, int* lpCopyIndex, 
			 list<EHolder*> aggregates, string sFirstProjection);

		// process having conditions
		// ...
	
		// sort
		// ...

		// generate root node
		void createRoot(Node* lpSelections, Node* lpAggregates);
		
	//---------------------------------------------------------------
	// Memory management functions
		
	public:		
		// New a node and put it in the m_lpList
		static SelectPlan* create();
};


#endif //  _SELECTPLAN_H_
