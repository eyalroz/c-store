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
#include "BNegation.h"

BNegation::BNegation() : BExpression()
{
	m_lpRight = NULL;
}

BNegation::BNegation( BExpression* lpRight) : BExpression()
{
	m_lpRight = lpRight;
}

BNegation::~BNegation()
{
	Log::writeToLog("BNegation", 10, "~BNegation");
}

void BNegation::setRight(BExpression* lpRight)
{
	m_lpRight = lpRight;
}

BExpression* BNegation::getRight()
{
	return m_lpRight;
}

void BNegation::typeCheck( ListProjections* lp ) {
	m_lpRight->typeCheck( lp );
}

string BNegation::toString()
{
	return "not " + m_lpRight->toString();
}

Node* BNegation::translate(StringNodeMap* lpMapColumns)
{
	//return BExpression::translate(lpMapColumns);
	if (m_lpRight != NULL)
	{
		Node* lpchildNode = NULL;
		lpchildNode = m_lpRight->translate(lpMapColumns);
		if (lpchildNode != NULL)
		{
			m_lpNode = UNegationNode::create(lpchildNode);
		}
	}
	
	return m_lpNode;
}

//---------------------------------------------------------------
// Memory management functions

// New a node and put it in the m_lpList
BNegation* BNegation::create()
{
	Log::writeToLog("BNegation", 0, "Create");
		
	BNegation* lpPObject = new BNegation();
	pushBack(lpPObject);
	return lpPObject;
}


// New a node and put it in the m_lpList
BNegation* BNegation::create(BExpression* lpRight)
{
	Log::writeToLog("BNegation", 0, "Create with argeuments: BExpression* lpRight");
		
	BNegation* lpPObject = new BNegation(lpRight);
	pushBack(lpPObject);
	return lpPObject;
}


