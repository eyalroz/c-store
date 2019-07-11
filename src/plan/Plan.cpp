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
 *  Plan.cpp
 *  cstore
 *
 *  Created by Nga Tran on 3/30/05.
 *  
 *
 */

#include "Plan.h"


// Default
Plan::Plan()
{
	m_lpRootNode = NULL;
}

// Deallocate memory
Plan::~Plan()
{	
   Log::writeToLog("Plan", 10, "~Plan"); 
}

// Return the root node of the plan
Node* Plan::getRoot()
{
	return m_lpRootNode;
}

// Generate a plan
void Plan::makePlan(Query* lpQuery)
{

}

// run the plan
void Plan::run(bool rosOnly)
{	
	if (m_lpRootNode != NULL)
	{
		StopWatch stopWatch;
		stopWatch.start();
		
		m_lpRootNode->run(rosOnly);
		
		cout << "Query took: " << stopWatch.stop() << " ms" <<  endl;
	}
	else
	{
		cout << "No plan generated" <<  endl;
	}
}
		
// Show the plan
void Plan::show(bool rosOnly)
{
	if (m_lpRootNode != NULL)
	{
		try 
		{
			// Open file for writing generated C++ code of the plan
			ofstream fout(PLAN_C_CODE, ios::out);
			
			// Every node in a plan has an ID
			// Set start ID for node
			int nodeID = 1;
			
			// Generate plan in C++ code 
			m_lpRootNode->show(&nodeID, &fout, rosOnly);
			
			fout.close();
		}
		catch (std::exception&)
		{
			cout << "Cannot open " << PLAN_C_CODE << endl;
			cout << "No C++ code generated" << endl;
		}
	
		
	}
}

// Show tree presentation of the plan
void Plan::showTree()
{
	cout << "--------------- TREE PRESENTATION -------------------" << endl;
	if (m_lpRootNode != NULL)
	{
		m_lpRootNode->showTree("");
	}
	cout << "--------------- END (TREE PRESENTATION)-------------------"<< endl << endl;	
}


//---------------------------------------------------------------
// Memory management functions
		
	
// New a node and put it in the m_lpList
Plan* Plan::create()
{
	Log::writeToLog("Plan", 10, "Create without arguments");
		
	Plan* lpNode = new Plan();
	pushBack(lpNode);
	return lpNode;
}
