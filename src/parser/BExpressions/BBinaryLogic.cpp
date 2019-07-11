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
#include "BBinaryLogic.h"

BBinaryLogic::BBinaryLogic() : BExpression()
{
	m_lpLeft = NULL;
	m_lpRight = NULL;
}

BBinaryLogic::BBinaryLogic( BExpression* lpLeft, BExpression* lpRight, string op ) : BExpression()
{
	m_lpLeft = lpLeft;
	m_lpRight = lpRight;
	m_sOp = op;
}

BBinaryLogic::~BBinaryLogic()
{
	Log::writeToLog("BBinaryLogic", 10, "~BBinaryLogic");
}

void BBinaryLogic::typeCheck( ListProjections* lp ) {
	m_lpLeft->typeCheck( lp );
	m_lpRight->typeCheck( lp );
}

void BBinaryLogic::setLeft( BExpression* lpLeft )
{
	m_lpLeft = lpLeft;
}

void BBinaryLogic::setRight( BExpression* lpRight )
{
	m_lpRight = lpRight;
}

BExpression* BBinaryLogic::getLeft()
{
	return m_lpLeft;
}

BExpression* BBinaryLogic::getRight()
{
	return m_lpRight;
}

string BBinaryLogic::getOp() {
  return m_sOp;
}

Node* BBinaryLogic::translate(StringNodeMap* lpMapColumns)
{
	return BExpression::translate(lpMapColumns);
}

string BBinaryLogic::toString()
{
	return m_lpLeft->toString() + " " + m_sOp + " " + m_lpRight->toString();
}


//---------------------------------------------------------------
// Memory management functions

// New a node and put it in the m_lpList
BBinaryLogic* BBinaryLogic::create()
{
	Log::writeToLog("BBinaryLogic", 0, "Create");
		
	BBinaryLogic* lpPObject = new BBinaryLogic();
	pushBack(lpPObject);
	return lpPObject;
}


// New a node and put it in the m_lpList
BBinaryLogic* BBinaryLogic::create(BExpression* lpLeft, BExpression* lpRight, string op)
{
	Log::writeToLog("BBinaryLogic", 0, "Create with argeuments: BExpression* lpLeft, BExpression* lpRight, string op");
		
	BBinaryLogic* lpPObject = new BBinaryLogic(lpLeft, lpRight, op);
	pushBack(lpPObject);
	return lpPObject;
}

