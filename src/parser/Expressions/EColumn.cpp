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
#include "EColumn.h"


EColumn::EColumn(string column): Expression()
{
	m_bIsColumn = 1;	// is a EColumn
	m_sColumnName = column;  
	m_sProjection = "";
	m_lpTuple = NULL;
}

EColumn::EColumn(string projection, string column) {
	m_bIsColumn = 1;
	m_sColumnName = column;
	m_sProjection = projection;
	m_lpTuple = NULL;
}

EColumn::~EColumn()
{
	Log::writeToLog("EColumn", 10, "~EColumn");
}

TVariable* EColumn::getTupleVar() {
	return m_lpTuple;
}

string EColumn::getName() {
	return m_sColumnName;
}

int EColumn::getType( ListProjections* lp ) {
  if(m_iType) return m_iType;
  m_lpTuple = lp->getTuple(m_sProjection, m_sColumnName);
  m_iType = m_lpTuple->getType();
  return m_iType;
}


string	EColumn::getProjection()
{
	if ( m_sProjection.compare("") != 0)
	{
		return m_sProjection;
	}
	
	if (getTupleVar() != NULL) {
	  return getTupleVar()->getProjection()->get();
	}
	else return "";
	
	//return getTupleVar()->getProjection()->get();
}

string	EColumn::toString()
{
  return m_sColumnName;
}

Node* EColumn::translate()
{
	return Expression::translate();
}

// Translate the expression into a plan node
Node* EColumn::translate(bool bNewSelectNode, Node* lpAggNode, Node* lpGroupNode, int* lpCopyIndex)
{
	return Expression::translate(bNewSelectNode, lpAggNode, lpGroupNode, lpCopyIndex);
}

//---------------------------------------------------------------
// Memory management functions

// New a node and put it in the m_lpList
EColumn* EColumn::create(string column)
{
	Log::writeToLog("EColumn", 0, "Create: string column");
		
	EColumn* lpPObject = new EColumn(column);
	pushBack(lpPObject);
	return lpPObject;
}


// New a node and put it in the m_lpList
EColumn* EColumn::create(string projection, string column)
{
	Log::writeToLog("EColumn", 0, "Create: string projection, string column");
		
	EColumn* lpPObject = new EColumn(projection, column);
	pushBack(lpPObject);
	return lpPObject;
}

