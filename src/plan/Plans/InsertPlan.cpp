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
 *  InsertPlan.cpp
 *  cstore
 *
 *  Created by Nga Tran on 6/20/05.
 *  
 *	Plan for insert query
 */

#include "InsertPlan.h"

InsertPlan::InsertPlan() : Plan()
{
	m_lpSelectPlan = NULL;
}

InsertPlan::~InsertPlan()
{
	Log::writeToLog("InsertPlan", 10, "~InsertPlan"); 
}

// Generate a plan
void InsertPlan::makePlan(Query* lpQuery)
{
	Projection* lpProjection = NULL;
	list<EColumn*> columns;
	list<Expression*> values;
	Query* lpSelectQuery = NULL;
	
	parseQuery(lpQuery, &lpProjection, &columns, &values, &lpSelectQuery);
	
	if (columns.size() == 0)
	{
		// nothing to insert
		return;
	}
	
	 if (columns.size() == values.size())
	 {
		// Insert enterd values
		
		// Create insert root node
		m_lpRootNode = InsertNode::create(lpProjection, columns, values);
	 }
	 else if (values.size() == 0)
	 {
		// Insert from a select query
		
		if (lpSelectQuery == NULL)
		{
			cout << "ERROR: Select query is NULL. Making insert plan failed" << endl;
			return;
		}
		
		// Make plan for select query
		m_lpSelectPlan = SelectPlan::create();
		m_lpSelectPlan->makePlan(lpSelectQuery);
		
		// Get root node of the select plan tree
		Node* lpSelectPlanRoot = NULL;
		lpSelectPlanRoot = m_lpSelectPlan->getRoot();
		
		if (lpSelectPlanRoot == NULL)
		{
			cout << "ERROR: Cannot create plan for the select query. Making insert plan fails" << endl;
			return;
		}
		
		// Provide top level datasources (columns selected in select) of the select plan to delete node
		m_lpRootNode = MInsertProjectionNode::create( ((MResultNode*)lpSelectPlanRoot)->getChildren(), lpProjection, columns);

	 }

}


// Parse INSERT query into specific lists
void InsertPlan::parseQuery(
			Query* lpQuery, Projection** lpProjection, list<EColumn*> *lpColumns,
			list<Expression*> *lpValues, Query** lpSelectQuery)
{
	// Get the projection	
	(*lpProjection) = ( (QInsert*) lpQuery)->getProjection();	
	
	ListColumnValues* lpColumnValues = NULL;
	lpColumnValues = ( (QInsert*) lpQuery)->getColumnValues();
	if (lpColumnValues != NULL)
	{
		// Get column names
		(*lpColumns) = lpColumnValues->getColumns();
	
		// Get values to be inserted
		(*lpValues) = lpColumnValues->getValues();
		
		if ((*lpValues).size() == 0)
		{
			// No values, get data from select query
			(*lpSelectQuery) = ((QInsert*) lpQuery)->getSelectQuery();
		}
	}
}

//---------------------------------------------------------------
// Memory management functions
		
	
// New a node and put it in the m_lpList
InsertPlan* InsertPlan::create()
{
	Log::writeToLog("InsertPlan", 10, "Create without arguments");
		
	InsertPlan* lpNode = new InsertPlan();
	pushBack(lpNode);
	return lpNode;
}
