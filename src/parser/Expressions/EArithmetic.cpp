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
#include "EArithmetic.h"

EArithmetic::EArithmetic(): Expression()
{
	m_lpLeft = NULL;
	m_lpRight = NULL;
}


EArithmetic::EArithmetic(Expression* lpLeft, Expression* lpRight, string op) : Expression()
{
  m_lpLeft = lpLeft;
  m_lpRight = lpRight;
  m_sOp = op;  
}

EArithmetic::~EArithmetic()
{
	Log::writeToLog("EArithmetic", 10, "~EArithmetic");
}

void EArithmetic::setLeft(Expression* lpLeft)
{
   m_lpLeft = lpLeft;
}

void EArithmetic::setRight(Expression* lpRight)
{
  m_lpRight = lpRight;
}

void EArithmetic::set(Expression* lpLeft, Expression* lpRight, string op)
{
  m_lpLeft = lpLeft;
  m_lpRight = lpRight;
  m_sOp = op; 
}

int EArithmetic::getType( ListProjections* lp ) {
	if( m_iType ) return m_iType;
	int lt = getLeft()->getType( lp );
	int rt = getRight()->getType( lp );
	if( lt == TYPE_FLOAT && rt == TYPE_FLOAT  ) {
		m_iType = TYPE_FLOAT;
		return TYPE_FLOAT;
	} else if( lt == TYPE_INTEGER && rt == TYPE_INTEGER ) {
		m_iType = TYPE_INTEGER;
		return TYPE_INTEGER;
	} else if( (lt == TYPE_FLOAT && rt == TYPE_INTEGER) || (lt == TYPE_INTEGER && rt == TYPE_FLOAT) ) {
		m_iType = TYPE_FLOAT;
		return TYPE_FLOAT;
	} else {
		parser_error( "Invalid types for op" + m_sOp );
		return 0;
	}
}

Expression*	EArithmetic::getLeft()
{
  return m_lpLeft;
}

Expression*	EArithmetic::getRight()
{
  return m_lpRight;
}

string          EArithmetic::toString()
{
  return "(" + m_lpLeft->toString() + m_sOp+m_lpRight->toString() + ")";
}

// Translate the expression into a plan node
Node* EArithmetic::translate()
{
	return Expression::translate();
}

// Translate the expression into a plan node
Node* EArithmetic::translate(bool bNewSelectNode, Node* lpAggNode, Node* lpGroupNode, int* lpCopyIndex)
{
	return Expression::translate(bNewSelectNode, lpAggNode, lpGroupNode, lpCopyIndex);
}

//---------------------------------------------------------------
// Memory management functions

// New a node and put it in the m_lpList
EArithmetic* EArithmetic::create()
{
	Log::writeToLog("EArithmetic", 0, "Create");
		
	EArithmetic* lpPObject = new EArithmetic();
	pushBack(lpPObject);
	return lpPObject;
}


// New a node and put it in the m_lpList
EArithmetic* EArithmetic::create(Expression* lpLeft, Expression* lpRight, string op)
{
	Log::writeToLog("EArithmetic", 0, "Create: Expression* lpLeft, Expression* lpRight, string op");
		
	EArithmetic* lpPObject = new EArithmetic(lpLeft, lpRight, op);
	pushBack(lpPObject);
	return lpPObject;
}

