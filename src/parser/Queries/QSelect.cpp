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
 *  QSelect.cpp
 *  cstore
 *
 *  Created by Nga Tran on 6/20/05.
 *  Last modified by Tien Hoang on 06/30/2005
 *  
 *	Class for query: SELECT ... FROM ...
 */

#include "QSelect.h"

QSelect::QSelect() : Query()
{
	m_bIsNotDeletedAdded = false;
	m_selectExpressions = NULL;
	m_projections = NULL;
	m_lpWherePredicate = NULL;
	m_orderinglist = NULL;
	m_lpGroupCols = NULL;
	m_lpHavingPredicate = NULL;
}


QSelect::~QSelect()
{
	Log::writeToLog("QSelect", 10, "~QSelect");
}

void QSelect::setQueryComponents(ListSelections* ls, ListProjections* lp, BExpression* be1, ListCols* lc, BExpression* be2, ListOrdering* lo) {
  m_selectExpressions = ls;
  m_projections = lp;
  m_lpWherePredicate = be1;
  m_lpGroupCols = lc;
  m_lpHavingPredicate = be2;
  m_orderinglist = lo;
}

void QSelect::setSelections( ListSelections* expressions )
{
	m_selectExpressions = expressions;
}

void QSelect::addSelection( EHolder* expression )
{	
	if( expression->getExpression()->isAggregate() ) {
		m_selectExpressions->addAggregate( expression );
	} else {
		m_selectExpressions->addSelection( expression );
	}
}

ListSelections* QSelect::getSelections()
{
	return m_selectExpressions;
}

void QSelect::setProjections( ListProjections* projections )
{
  cout << " set projectsions " << endl << endl << endl;
	m_projections = projections;
}

void QSelect::addProjection( Projection* projection )
{
	m_projections->add( projection );
}

ListProjections* QSelect::getProjections()
{
	return m_projections;
}

void QSelect::setWherePredicate( BExpression* w ) {
	m_lpWherePredicate = w;
}

BExpression* QSelect::getWherePredicate() {
	return m_lpWherePredicate;
}

void QSelect::setOrdering( ListOrdering* l ) {
	m_orderinglist = l;
}

void QSelect::addOrderingCol( ColOrder* col ) {
	m_orderinglist->add( col );
}

ListOrdering*  QSelect::getOrdering() {
	return m_orderinglist;
}

void QSelect::setGroupCols( ListCols* cols )
{
  m_lpGroupCols = cols;
}

void QSelect::addGroupCol( EColumn* col )
{
  m_lpGroupCols->add( col );
}

ListCols* QSelect::getGroupCols()
{
  return m_lpGroupCols;
}

void QSelect::setHavingPredicate( BExpression* p ) {
  m_lpHavingPredicate = p;
}

BExpression* QSelect::getHavingPredicate() {
  return m_lpHavingPredicate;
}

// Add "AND projection.deleted <> 1" to where predicate
void QSelect::addNotDeletedPredicate()
{
	if (m_bIsNotDeletedAdded == true)
	{
		// Has been added
		return;
	}
	
	/*
	// Uncommnent later
	
	// Get deleleted column names for all projection from the catalog
	CatalogInstance* lpCatalog = NULL;
	lpCatalog = new CatalogInstance();
	
	if (lpCatalog == NULL)
	{
		cout << "Catalog not found" << endl;
		return;
	}
	
	if ( (m_lpWherePredicate!= NULL) && (m_lpWherePredicate->toString().compare("TRUE") == 0) )
	{
		delete m_lpWherePredicate;
		m_lpWherePredicate = NULL;
	}
	
	list<Projection*> projections = m_projections->getProjections();
	list<Projection*>::iterator lpProjection;
	for (lpProjection =  projections.begin(); lpProjection != projections.end(); lpProjection++)
	{
		string sDeletedColumn = "deleted__";
		// Catalog, later
		//sDeletedColumn =  lpCatalog->getDeletedColumnName( (*lpProjection)->get() );
	
		
		EColumn* lpDeletedColumn = new EColumn((*lpProjection)->get(), sDeletedColumn);
		// Store for deleting later
		m_deletionList.push_back(lpDeletedColumn);
	
		EInteger* lpValue = new EInteger(0);	// Deleted value
		// Store for deleting later
		m_deletionList.push_back(lpValue);
		
		BEq* lpPredicate = new BEq(lpDeletedColumn, lpValue);
		// Store for deleting later
		m_deletionList.push_back(lpPredicate);
		
		if (m_lpWherePredicate == NULL)
		{
			m_lpWherePredicate = lpPredicate;
		}
		else
		{
			m_lpWherePredicate = new BConjunction(m_lpWherePredicate, lpPredicate);
			// Store for deleting later
			m_deletionList.push_back(m_lpWherePredicate);
		}
	}
	
	delete lpCatalog;
	*/
	m_bIsNotDeletedAdded = true;
}


string QSelect::toString()
{
	string r = "";
	
	r = "SELECT ";
	ListSelections* l = getSelections();
	r += l->toString();

	ListProjections* p = getProjections();
	r += " FROM ";
	r += p->toString();

	BExpression* w = getWherePredicate();
	r += " WHERE ";
	r += w->toString();

	ListOrdering* order = getOrdering();
	r += order->toString();
	
	ListCols* group = getGroupCols();
	if (group != NULL) {
	  r += " GROUP BY ";
	  r += group->toString();
	}
	
	BExpression* having = getHavingPredicate();
	if (having != NULL) {
	  r += " HAVING ";
	  r += having->toString();
	}

	return r;
}

// Generate a query plan
void QSelect::makePlan()
{	
	m_lpPlan = SelectPlan::create();
	m_lpPlan->makePlan(this);
}

//---------------------------------------------------------------
// Memory management functions

// New a node and put it in the m_lpList
QSelect* QSelect::create()
{
	Log::writeToLog("QSelect", 0, "Create");
		
	QSelect* lpPObject = new QSelect();
	pushBack(lpPObject);
	return lpPObject;
}

