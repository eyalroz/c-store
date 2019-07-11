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
#include "EDiv.h"

EDiv::EDiv() : EArithmetic()
{
	m_sOp = "/";
}


EDiv::EDiv(Expression* lpLeft, Expression* lpRight) : EArithmetic(lpLeft, lpRight, "/")
{
}


EDiv::~EDiv()
{
	Log::writeToLog("EDiv", 10, "~EDiv");
}

int EDiv::getType( ListProjections* lp ) {
	if( m_iType ) return m_iType;
	int lt = getLeft()->getType( lp );
	int rt = getRight()->getType( lp );
	if( (lt == TYPE_FLOAT || lt == TYPE_INTEGER) && (rt == TYPE_FLOAT || rt == TYPE_INTEGER) ) {
		m_iType = TYPE_FLOAT;
		return TYPE_FLOAT;
	} else {
		parser_error( "Invalid types for op /" );
		return 0;
	}
}

Node* EDiv::translate()
{
	return EArithmetic::translate();
}

// Translate the expression into a plan node
Node* EDiv::translate(bool bNewSelectNode, Node* lpAggNode, Node* lpGroupNode, int* lpCopyIndex)
{
	return EArithmetic::translate(bNewSelectNode, lpAggNode, lpGroupNode, lpCopyIndex);
}


//---------------------------------------------------------------
// Memory management functions

// New a node and put it in the m_lpList
EDiv* EDiv::create()
{
	Log::writeToLog("EDiv", 0, "Create");
		
	EDiv* lpPObject = new EDiv();
	pushBack(lpPObject);
	return lpPObject;
}


// New a node and put it in the m_lpList
EDiv* EDiv::create(Expression* lpLeft, Expression* lpRight)
{
	Log::writeToLog("EDiv", 0, "Create with argeuments: Expression* lpLeft, Expression* lpRight");
		
	EDiv* lpPObject = new EDiv(lpLeft, lpRight);
	pushBack(lpPObject);
	return lpPObject;
}


