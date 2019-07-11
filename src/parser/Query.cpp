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
#include "Query.h"

Query::Query()
{
	m_lpPlan = NULL;
}

Query::~Query()
{
	Log::writeToLog("Query", 10, "~Query");
}

string Query::toString()
{
	return "";
}

// Generate a query plan
void Query::makePlan()
{
}
		
// run the query plan
void Query::run()
{
	if (m_lpPlan != NULL)
	{
		m_lpPlan->run();
	}
	else
	{
		cout << "No plan generated" << endl;
	}

}
		
// Show the query plan
void Query::show()
{
	if (m_lpPlan != NULL)
	{
		m_lpPlan->show();
	}
	else
	{
		cout << "No plan generated" << endl;
	}
}
		
// Show tree presentation of the query plan
void Query::showTree()
{
	if (m_lpPlan != NULL)
	{
		m_lpPlan->showTree();
	}
	else
	{
		cout << "No plan generated" << endl;
	}
}
		
//---------------------------------------------------------------
// Memory management functions

// New a node and put it in the m_lpList
Query* Query::create()
{
	Log::writeToLog("Query", 0, "Create");
		
	Query* lpPObject = new Query();
	pushBack(lpPObject);
	return lpPObject;
}
