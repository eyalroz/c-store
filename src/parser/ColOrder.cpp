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
#include "ColOrder.h"

ColOrder::ColOrder() {
	m_lpCol = NULL;
}

ColOrder::ColOrder( EColumn* col, int asc_desc ) {
	m_lpCol = col;
	m_iAscDesc = asc_desc;
}

ColOrder::~ColOrder()
{
	Log::writeToLog("ColOrder", 10, "~ColOrder");
}

void ColOrder::setColumn( EColumn* col ) {
	m_lpCol = col;
}

void ColOrder::setOrder( int asc_desc ) {
	m_iAscDesc = asc_desc;
}
	
void ColOrder::typeCheck( ListProjections* lp ) {
	m_lpCol->getType( lp );
}

EColumn* ColOrder::getColumn() {
	return m_lpCol;
}

int ColOrder::getOrder() {
	return m_iAscDesc;
}

string ColOrder::toString() {
	std::string colstring = getColumn()->toString();
	std::string ordering = (getOrder()? " DESC" : " ASC");
	return colstring + ordering;
}

//---------------------------------------------------------------
// Memory management functions

// New a node and put it in the m_lpList
ColOrder* ColOrder::create()
{
	Log::writeToLog("ColOrder", 0, "Create");
		
	ColOrder* lpPObject = new ColOrder();
	pushBack(lpPObject);
	return lpPObject;
}

// New a node and put it in the m_lpList
ColOrder* ColOrder::create(EColumn* col, int asc_desc)
{
	Log::writeToLog("ColOrder", 0, "Create with arguments: EColumn* col, int asc_desc");
		
	ColOrder* lpPObject = new ColOrder(col, asc_desc);
	pushBack(lpPObject);
	return lpPObject;
}

