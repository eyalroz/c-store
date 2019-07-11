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
#include "BComparison.h"

#include <iostream>


BComparison::BComparison() : BExpression()
{
	m_lpLeft = NULL;
	m_lpRight = NULL;
}

BComparison::BComparison( Expression* lpLeft, Expression* lpRight, string op ) : BExpression()
{
	m_lpLeft = lpLeft;
	m_lpRight = lpRight;
	m_sOp = op;
}

BComparison::~BComparison()
{
	Log::writeToLog("BComparison", 10, "~BComparison");
}

void BComparison::setLeft( Expression* lpLeft )
{
	m_lpLeft = lpLeft;
}

void BComparison::setRight( Expression* lpRight )
{
	m_lpRight = lpRight;
}

Expression* BComparison::getLeft()
{
	return m_lpLeft;
}

Expression* BComparison::getRight()
{
	return m_lpRight;
}

string BComparison::getOp() {
  return m_sOp;
}

void BComparison::typeCheck( ListProjections* lp ) {
	int lt = m_lpLeft->getType( lp );
	int rt = m_lpRight->getType( lp );
	if( lt != rt ) {
		parser_error( "Comparison between different types not allowed" );
	}
	if( lt == TYPE_INTEGER || lt == TYPE_FLOAT ) return;
	if( lt == TYPE_STRING && m_sOp == "=" ) return;
	if( lt == TYPE_STRING && m_sOp == "<>" ) return;
	parser_error( "Invalid comparison using operator " + m_sOp );
}

Node* BComparison::translate(StringNodeMap* lpMapColumns)
{
	return BExpression::translate(lpMapColumns);
}

string BComparison::toString()
{
	return "(" + m_lpLeft->toString() + " " + m_sOp + " " + m_lpRight->toString() + ")";
}

// Common translation for all BComparison nodes
Node* BComparison::commonTranslate(int iComparisonType, StringNodeMap* lpMapColumns)
{	
	// Check attributes on left and right hand sides
	
	// A join
	if ( (m_lpLeft->isColumn()) && (m_lpRight->isColumn()) )
	{		
		m_lpNode = BJoinNode::create((EColumn*) m_lpLeft,  (EColumn*)m_lpRight, iComparisonType);
		
		// Store this node
		(*lpMapColumns)[m_lpLeft->toString()] = m_lpNode;
		(*lpMapColumns)[m_lpRight->toString()] = m_lpNode;
		
		return m_lpNode;
	}
	
	// A normal condition
	// Column 
	if ( ( (m_lpLeft->isColumn()) && (!m_lpRight->isColumn()) ) || // Form: columnName > 15
		 ( (!m_lpLeft->isColumn()) && (m_lpRight->isColumn()) ) )  // Form: 15 < columnName
	{		
		string sColumn;
		string sProjection;
		string sRHS;
		
		/////
		// Switch to the unique form: columnName (LHS) > 15 (RHS)
		Expression* lpRHS = NULL;
		//////
		
		if (m_lpLeft->isColumn())
		{
			sColumn = ((EColumn*) m_lpLeft)->toString();
			sProjection = ((EColumn*)m_lpLeft)->getProjection();
			sRHS = m_lpRight->toString();
			
			////////
			lpRHS = m_lpRight;
			///////
		}
		else
		{
			sColumn = ((EColumn*) m_lpRight)->toString();
			sProjection = ((EColumn*)m_lpRight)->getProjection();
			sRHS = m_lpLeft->toString();
			
			////////
			lpRHS = m_lpLeft;
			///////
		}
		
		
		/////////
		int iRHS = 0;
		double dRHS = 0;
		int t = lpRHS->getType(NULL);
		if (t == TYPE_INTEGER)
		{
			iRHS = ((EInteger*) lpRHS)->get();
			m_lpNode = SelectNode::create(sColumn, sProjection, iComparisonType, iRHS);
		}
		else if (t == TYPE_FLOAT)
		{
			dRHS = ((EFloat*) lpRHS)->get();
			m_lpNode = SelectNode::create(sColumn, sProjection, iComparisonType, dRHS);
		}
		else // String
		{
			sRHS = lpRHS->toString();
			m_lpNode = SelectNode::create(sColumn, sProjection, iComparisonType, sRHS);
		}

		
		// Store this node
		(*lpMapColumns)[sColumn] = m_lpNode;
	}
	
	return m_lpNode;

}

//---------------------------------------------------------------
// Memory management functions

// New a node and put it in the m_lpList
BComparison* BComparison::create()
{
	Log::writeToLog("BComparison", 0, "Create");
		
	BComparison* lpPObject = new BComparison();
	pushBack(lpPObject);
	return lpPObject;
}


// New a node and put it in the m_lpList
BComparison* BComparison::create(Expression* lpLeft, Expression* lpRight, string op)
{
	Log::writeToLog("", 0, "Create with argeuments: Expression* lpLeft, Expression* lpRight, string op");
		
	BComparison* lpPObject = new BComparison(lpLeft, lpRight, op);
	pushBack(lpPObject);
	return lpPObject;
}

