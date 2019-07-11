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
#include "TVariable.h"

TVariable::TVariable()
{
    m_sName = "__NOT SET__";
}

TVariable::TVariable(Projection* proj, string name)
{
  m_projection = proj;
  m_sName = name;
}

TVariable::TVariable(Projection* proj, string name, int type) {
	m_projection = proj;
	m_sName = name;
	m_iType = type;
}
	

TVariable::~TVariable()
{
	Log::writeToLog("Tavriable", 10, "~TVariable");
}


void	TVariable::set(string name)
{
  m_sName = name;  
}

string	TVariable::get()
{
  return m_sName;
}

void TVariable::setType( int type ) {
	m_iType = type;
}

int TVariable::getType() {
	return m_iType;
}

Projection* TVariable::getProjection() {
	return m_projection;
}

string	TVariable::toString()
{

   return m_sName;
}


//---------------------------------------------------------------
// Memory management functions

// New a node and put it in the m_lpList
TVariable* TVariable::create()
{
	Log::writeToLog("TVariable", 0, "Create");
		
	TVariable* lpPObject = new TVariable();
	pushBack(lpPObject);
	return lpPObject;
}

// New a node and put it in the m_lpList
TVariable* TVariable::create(Projection* proj, string name)
{
	Log::writeToLog("TVariable", 0, "Create with arguments: Projection* proj, string name");
		
	TVariable* lpPObject = new TVariable(proj, name);
	pushBack(lpPObject);
	return lpPObject;
}

// New a node and put it in the m_lpList
TVariable* TVariable::create(Projection* proj, string name, int type)
{
	Log::writeToLog("TVariable", 0, "Create with arguments: Projection* proj, string name, int type");
		
	TVariable* lpPObject = new TVariable(proj, name, type);
	pushBack(lpPObject);
	return lpPObject;
}


