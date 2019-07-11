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
#include<iostream>
#include "../constants.h"
#include "EAgg.h"

EAgg::EAgg(): Expression()
{
	m_lpAggNode = NULL;
	m_lpSelectNode = NULL;
	m_lpRight = NULL;
}


EAgg::EAgg(EColumn* lpRight, string op) : Expression()
{
  m_lpRight = lpRight;
  m_sOp = op;  
  m_lpAggNode = NULL;
  m_lpSelectNode = NULL;
}

EAgg::~EAgg()
{
	Log::writeToLog("EAgg", 10, "~EAgg");
}

void EAgg::setRight(EColumn* lpRight)
{
  m_lpRight = lpRight;
}

void EAgg::set(EColumn* lpRight, string op)
{
  m_lpRight = lpRight;
  m_sOp = op; 
}

EColumn*	EAgg::getRight()
{
  return m_lpRight;
}

string EAgg::getOp() {
  return m_sOp;
}

int EAgg::getType( ListProjections* lp ) {
	if( !m_lpRight ) parser_error( "AGGREGATE(*) only allowed for COUNT" );
	if( m_iType ) return m_iType;
	int rtype = m_lpRight->getType( lp );
	if( rtype == TYPE_INTEGER || rtype == TYPE_FLOAT ) {
		m_iType = rtype;
		return m_iType;
	} else {
		parser_error( "Non-numeric type for aggregate function " + m_sOp );
	}
}

int EAgg::isAggregate() {
	return 1;
}

string EAgg::toString()
{
	if( m_lpRight ) {
		return m_sOp + '(' + m_lpRight->toString() + ')';
	} else {
		return m_sOp + "(*)";
	}
}

// Translate the expression into a plan node
Node* EAgg::translate()
{
	return Expression::translate();
}

// Translate the expression into a plan node
Node* EAgg::translate(bool bNewSelectNode, Node* lpNode, Node* lpGroupNode, int* lpCopyIndex)
{
	return Expression::translate(bNewSelectNode, lpNode, lpGroupNode, lpCopyIndex);
}

// Get real aggregate node
Node* EAgg::getAggNode(bool bNewSelectNode, Node* lpNode, int* lpCopyIndex)
{	
	if (!bNewSelectNode)
	{
		// Node available, do not create a new one
		m_lpAggNode = lpNode;
	}
	else
	{
		// Get projection name				
		string sProjection = m_lpRight->getProjection();
				
		// Create a new select node
		m_lpSelectNode = SelectNode::create(m_lpRight->toString(), sProjection);
		
		// Create project node
		if (lpNode != NULL)
		{
			// Has copy node
			m_lpAggNode = BProjectNode::create(m_lpSelectNode, lpNode, (*lpCopyIndex));
			(*lpCopyIndex)++;
		}
		else
		{
			// No copy node
			m_lpAggNode = UProjectNode::create(m_lpSelectNode);
		}
	}
	
	return m_lpAggNode;

}

//---------------------------------------------------------------
// Memory management functions

// New a node and put it in the m_lpList
EAgg* EAgg::create()
{
	Log::writeToLog("EAgg", 0, "Create");
		
	EAgg* lpPObject = new EAgg();
	pushBack(lpPObject);
	return lpPObject;
}


// New a node and put it in the m_lpList
EAgg* EAgg::create(EColumn* lpRight, string op)
{
	Log::writeToLog("EAgg", 0, "Create with argeuments: EColumn* lpRight, string op");
		
	EAgg* lpPObject = new EAgg(lpRight, op);
	pushBack(lpPObject);
	return lpPObject;
}
