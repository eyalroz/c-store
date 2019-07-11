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
#include "BConjunction.h"

BConjunction::BConjunction() : BBinaryLogic()
{
	m_sOp = "and";
}


BConjunction::BConjunction(BExpression* lpLeft, BExpression* lpRight) : BBinaryLogic(lpLeft, lpRight, "and")
{
}


BConjunction::~BConjunction()
{
	Log::writeToLog("BConjunction", 10, "~BConjunction");
}

// Translate the expression into a SelectNode for the operator plan
Node* BConjunction::translate(StringNodeMap* lpMapColumns)
{
	Node* lpLeft = NULL;
	Node* lpRight = NULL;
	
	lpLeft = m_lpLeft->translate(lpMapColumns);
	lpRight = m_lpRight->translate(lpMapColumns);
	
	// No valid children
	if ( (lpLeft == NULL) || (lpRight == NULL) )
	{
		return NULL;
	}
	
	m_lpNode = BConjunctionNode::create(lpLeft, lpRight);

	return m_lpNode;
}

//---------------------------------------------------------------
// Memory management functions

// New a node and put it in the m_lpList
BConjunction* BConjunction::create()
{
	Log::writeToLog("BConjunction", 0, "Create");
		
	BConjunction* lpPObject = new BConjunction();
	pushBack(lpPObject);
	return lpPObject;
}


// New a node and put it in the m_lpList
BConjunction* BConjunction::create(BExpression* lpLeft, BExpression* lpRight)
{
	Log::writeToLog("BConjunction", 0, "Create with argeuments: BExpression* lpLeft, BExpression* lpRight");
		
	BConjunction* lpPObject = new BConjunction(lpLeft, lpRight);
	pushBack(lpPObject);
	return lpPObject;
}

