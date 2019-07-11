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
/*
 *  QDelete.cpp
 *  cstore
 *
 *  Created by Nga Tran on 6/21/05.
 * 
 *  Class for query DELETE FROM ...
 */

#include "QDelete.h"

QDelete::QDelete() : Query()
{
	m_lpProjection = NULL;
	m_lpWherePredicate = NULL;
}


QDelete::~QDelete()
{
	Log::writeToLog("QDelete", 10, "~QDelete");
}

void QDelete::setProjection(Projection* lpProjection) {
  m_lpProjection = lpProjection;
}

Projection* QDelete::getProjection() {
  return m_lpProjection;
}


void QDelete::setWherePredicate( BExpression* w ) {
	m_lpWherePredicate = w;
}

BExpression* QDelete::getWherePredicate() {
	return m_lpWherePredicate;
}


string QDelete::toString()
{
	string r = "";

	r = "DELETE FROM ";

	Projection* p = getProjection();
	r += p->toString();

	BExpression* w = getWherePredicate();
	r += " WHERE ";
	r += w->toString();

	return r;
}



// Generate a query plan
void QDelete::makePlan()
{
	m_lpPlan = DeletePlan::create();
	m_lpPlan->makePlan(this);
}

//---------------------------------------------------------------
// Memory management functions

// New a node and put it in the m_lpList
QDelete* QDelete::create()
{
	Log::writeToLog("QDelete", 0, "Create");
		
	QDelete* lpPObject = new QDelete();
	pushBack(lpPObject);
	return lpPObject;
}
