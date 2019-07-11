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
 *  SelectNode.h
 *  cstore
 *
 *  Created by Nga Tran on 3/17/05.
 *  
 *	This is a select node in a plan tree
 *	This node has no children
 *
 *	Similar to select operator in relation algebra, select is used to filter the tuples
 *	which satisfy the condition (predicate)
 */
#ifndef _SELECTNODE_H_
#define _SELECTNODE_H_

class UDeleteProjectionNode;

#include <string>
#include "../Node.h"
#include "../UNodes/UDeleteProjectionNode.h"
#include "../../common/Constants.h"
#include "../../AM/ROSAM.h"
#include "../../AM/WOSAM.h"
#include "../../Operators/Operator.h"
#include "../PlanConstants.h"
#include "../../common/Predicate.h"

// Constants for argument iJoinType
#define SELECT_NOT_FOR_JOIN		0
#define SELECT_FOR_JOIN			1
#define SELECT_FOR_AFTER_JOIN	2

using namespace std;

class SelectNode : public Node
{
	protected:
	
		bool	m_bHasPredicate;	// if false, all tuples are selected (no predicate/condition)
		int		m_iPredicateType;	// Type of predicate: -, >, <, ...
		string	m_sRHS;				// is used if m_bHasPredicate is true and m_iRHSType is STR_TYPE
		double	m_nRHS;				// is used if m_bHasPredicate is true and m_iRHSType is DBL_TYPE
		string	m_sColumn;			// Column applied for the selected
		int		m_iRHSType;			// Right hand side type: INT_TYPE, DBL_TYPE, STR_TYPE
		int		m_iRHS;				// is used if m_bHasPredicate is true and m_iRHSType is INT_TYPE
		string	m_sProjection;		// Projection name
		bool	m_bIsSorted;		// true if the column is sorted
		int		m_iEncodingType;	// encoding type of the column
		int		m_iJoinType;			// 0 if SELECT_NOT_FOR_JOIN
									// 1 if SELECT_FOR_JOIN
									// 2 if SELECT_FOR_AFTER_JOIN

		// Variables for the purpose of allocate/deallocate memory
		Predicate* m_lpROSPredicate;
		Predicate* m_lpWOSPredicate;
		
	protected:
		// Node's constructor is protected because they should not be new directly.
		// A node can be new by using static "create" function
		// This is for the purpose of memory management
		SelectNode();
		SelectNode(string sColumn, string sProjection);										// No predicate
		SelectNode(string sColumn, string sProjection, int iJoinType);					// For join			
		SelectNode(string sColumn, string sProjection, int iPredicateType, double nRHS);	// RHS is a double
		SelectNode(string sColumn, string sProjection, int iPredicateType, string sRHS);	// RHS is a string
		SelectNode(string sColumn, string sProjection, int iPredicateType, int sRHS);		// RHS is an integer

	public:
		virtual ~SelectNode();
		
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
		
		// Check and set predicate
		virtual void checkAndSetPredicate(SelectNode* lpNode);
		
		
	protected:
		
		// Get info from the catalog
		virtual void getCatalogInfo();
		
		// Get ORS dataSource for this column
		virtual void getROSDataSource();
		
		// Get WOS dataSource for this column
		virtual void getWOSDataSource();
		
	//---------------------------------------------------------------
	// Memory management functions
		
	public:		
		// New a node and put it in the m_lpList
		static SelectNode* create();
		static SelectNode* create(string sColumn, string sProjection);
		static SelectNode* create(string sColumn, string sProjection, int iJoinType);
		static SelectNode* create(string sColumn, string sProjection, int iPredicateType, double nRHS);
		static SelectNode* create(string sColumn, string sProjection, int iPredicateType, string sRHS);
		static SelectNode* create(string sColumn, string sProjection, int iPredicateType, int sRHS);
};


#endif // _SELECTNODE_H_

