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
#include "EHolder.h"

EHolder::EHolder() {
	m_lpExpression = NULL;
	m_sAlias = "";
}

EHolder::EHolder( Expression* e ) {
	m_lpExpression = e;
	m_sAlias = "";
}

EHolder::EHolder( Expression* e, string alias ) {
	m_lpExpression = e;
	m_sAlias = alias;
}

EHolder::~EHolder() 
{
	Log::writeToLog("EHolder", 10, "~EHolder");
}

void EHolder::setExpression( Expression* e ) {
	m_lpExpression = e;
}

void EHolder::setAlias( string alias ) {
	m_sAlias = alias;
}

Expression* EHolder::getExpression() {
	return m_lpExpression;
}

string EHolder::getAlias() {
	return m_sAlias;
}

string EHolder::toString() {
	if( m_sAlias.empty() ) {
		return m_lpExpression->toString();
	} else {
		return m_lpExpression->toString() + " AS " + m_sAlias;
	}
}

//---------------------------------------------------------------
// Memory management functions

// New a node and put it in the m_lpList
EHolder* EHolder::create()
{
	Log::writeToLog("EHolder", 0, "Create");
		
	EHolder* lpPObject = new EHolder();
	pushBack(lpPObject);
	return lpPObject;
}

// New a node and put it in the m_lpList
EHolder* EHolder::create(Expression* e)
{
	Log::writeToLog("EHolder", 0, "Create with arguments: Expression* e");
		
	EHolder* lpPObject = new EHolder(e);
	pushBack(lpPObject);
	return lpPObject;
}

// New a node and put it in the m_lpList
EHolder* EHolder::create(Expression* e, string alias)
{
	Log::writeToLog("EHolder", 0, "Create with arguments: Expression* e, string alias");
		
	EHolder* lpPObject = new EHolder(e, alias);
	pushBack(lpPObject);
	return lpPObject;
}
