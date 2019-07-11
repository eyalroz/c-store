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
#include "BLt.h"

BLt::BLt() : BComparison()
{
	m_sOp = "<";
}


BLt::BLt(Expression* lpLeft, Expression* lpRight) : BComparison(lpLeft, lpRight, "<")
{
}


BLt::~BLt()
{
	Log::writeToLog("BLt", 10, "~BLt");
}

// Translate the expression into a SelectNode for the operator plan
Node* BLt::translate(StringNodeMap* lpMapColumns)
{
	return commonTranslate(Predicate::OP_LESS_THAN, lpMapColumns);
}


//---------------------------------------------------------------
// Memory management functions

// New a node and put it in the m_lpList
BLt* BLt::create()
{
	Log::writeToLog("BLt", 0, "Create");
		
	BLt* lpPObject = new BLt();
	pushBack(lpPObject);
	return lpPObject;
}


// New a node and put it in the m_lpList
BLt* BLt::create(Expression* lpLeft, Expression* lpRight)
{
	Log::writeToLog("BLt", 0, "Create with argeuments: Expression* lpLeft, Expression* lpRight");
		
	BLt* lpPObject = new BLt(lpLeft, lpRight);
	pushBack(lpPObject);
	return lpPObject;
}

