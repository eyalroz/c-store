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
 *  QInsert.cpp
 *  cstore
 *
 *  Created by Nga Tran on 6/20/05.
 *  
 *  Class for query: INSERT INTO ...
 */

#include "QInsert.h"

QInsert::QInsert() : Query()
{
	m_lpProjection = NULL;
	m_lpSelectQuery = NULL;
}


QInsert::~QInsert()
{
	Log::writeToLog("QInsert", 10, "~QInsert");
}

void QInsert::setColumnValues( ListColumnValues* lpColumnValues )
{
	m_lpColumnValues = lpColumnValues;
}

void QInsert::addColumnValue( EColumn* lpColumn, Expression* lpValue)
{
	m_lpColumnValues->add(lpColumn, lpValue);
}

ListColumnValues*  QInsert::getColumnValues()
{
	return m_lpColumnValues;
}

void QInsert::setProjection(Projection* lpProjection) {
  m_lpProjection = lpProjection;
}

Projection* QInsert::getProjection() {
  return m_lpProjection;
}

void QInsert::setSelectQuery(Query* lpQuery)
{
	m_lpSelectQuery = lpQuery;
}

Query* QInsert::getSelectQuery()
{
	return m_lpSelectQuery;
}

string QInsert::toString()
{
	string r = "";


	r = "INSERT INTO ";

	Projection* p = getProjection();
	r += p->toString();
		
	// Column names
	r += "(";

	list<EColumn*> columnList = m_lpColumnValues->getColumns();
	list<EColumn*>::iterator lpColumn;
	for (lpColumn = columnList.begin(); lpColumn != columnList.end(); lpColumn++)
	{
		if (lpColumn != columnList.begin())
		{
			r += ", ";
		}
			
		r += (*lpColumn)->toString(); 
	}
	r += ")";
		
	// Values
	r += " VALUES(";
	list<Expression*> valueList = m_lpColumnValues->getValues();
	list<Expression*>::iterator lpValue;
	for (lpValue = valueList.begin(); lpValue != valueList.end(); lpValue++)
	{
		if (lpValue != valueList.begin())
		{
			r += ", ";
		}
			
		r += (*lpValue)->toString(); 
	}
	r += ")";

	return r;
}

// Generate a query plan
void QInsert::makePlan()
{	
	m_lpPlan = InsertPlan::create();
	m_lpPlan->makePlan(this);
}


//---------------------------------------------------------------
// Memory management functions

// New a node and put it in the m_lpList
QInsert* QInsert::create()
{
	Log::writeToLog("QInsert", 0, "Create");
		
	QInsert* lpPObject = new QInsert();
	pushBack(lpPObject);
	return lpPObject;
}

