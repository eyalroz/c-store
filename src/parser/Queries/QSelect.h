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
 *  QSelect.h
 *  cstore
 *
 *  Created by Nga Tran on 6/20/05.
 *  
 *	Class for query: SELECT ... FROM ...
 */

#ifndef _QSELECT_H_
#define _QSELECT_H_

class SelectPlan;

#include <string>

#include "../Query.h"
#include "../../plan/Plans/SelectPlan.h"
#include "../ListCols.h"
#include "../Expression.h"
#include "../Expressions/EColumn.h"
#include "../BExpression.h"

using namespace std;

class QSelect : public Query
{
	
	protected:
		
		// PARSER VARIABLES 
		// TO DO: change variables' names

		ListSelections*		m_selectExpressions;
		ListProjections*	m_projections;
		BExpression*		m_lpWherePredicate;
		ListOrdering*		m_orderinglist;
		bool				m_bIsNotDeletedAdded; // Check if "AND deleted <> 1" has been added
		// merging with QSelectGroupBy
		ListCols*		m_lpGroupCols;
		BExpression*		m_lpHavingPredicate;

   protected:
		// PObject's constructor is protected because they should not be new directly.
		// A node can be new by using static "create" function
		// This is for the purpose of memory management
		QSelect();
		
	public:
		virtual ~QSelect();
		
		void setQueryComponents(ListSelections* ls, ListProjections* lp, BExpression* be1, ListCols* lc, BExpression* be2, ListOrdering* lo);

		// PARSER METHODS
		
		void setSelections( ListSelections* expressions );
		void addSelection( EHolder* expression );
		ListSelections*  getSelections();

		void setProjections( ListProjections* projections );
		void addProjection( Projection* projection );
		ListProjections*  getProjections();

		void setWherePredicate( BExpression* w );
		BExpression* getWherePredicate();

		void setOrdering( ListOrdering* l );
		void addOrderingCol( ColOrder* col );
		ListOrdering*  getOrdering();
		
		void setGroupCols( ListCols* cols );
		void addGroupCol( EColumn* col );
		ListCols* getGroupCols();
		
		void setHavingPredicate( BExpression* p );
		BExpression* getHavingPredicate();

		// Add "AND projection.deleted <> 1" to where predicate
		void addNotDeletedPredicate();
		
		string toString();
		
		// PLAN METHODS
	
		// Generate a plan
		void makePlan();
		
	//---------------------------------------------------------------
	// Memory management functions
		
	public:
		// New a PObject and put it in the m_lpList
		static QSelect* create();
};

#endif // _QSELECT_H_

