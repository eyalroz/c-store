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
 *  PlanInC.h
 *  cstore
 *
 *  Created by Nga Tran on 6/2/05.
 *  
 *	Test suit for generating plan in C++ code
 */

#include "PlanInC.h"

using namespace std;

PlanInC::PlanInC()
{
}

PlanInC::~PlanInC()
{
}

/* args isn't used here */
bool PlanInC::run(Globals* g, const vector<string>& args)
{
/*
	// this call is simply to instantiate the catalog
	CatalogInstance::getCatalog();
	
	cout << "ENTER A QUERY: " << endl;

  Parser* p = new Parser();
  int result = p->doParse();
  p->generatePlan();
  
  if( result == 0 ) {
    cout << "Success: Program parsed properly!\n" ;
    return 1;
  } else {
    cout << "Error: Program did not parse!\n";
    return 0;
  }
 */
 
	// this call is simply to instantiate the catalog
	cout << "Instatiating catalog. Please wait until you are asked to enter a query" << endl << endl;
	
	CatalogInstance::getCatalog();
	
	while(1)
	{
		Parser* p = new Parser();
		
		// Initialize un empty list of Node*
		BaseNode::initialize();
		
		// Initialize un empty list of PObject*
		PObject::initialize();
		
		cout << "Enter a query or q to quit: " << endl;		
		
			
		
		// Wait for a query and parse it
		int result = p->doParse();
		if( result == 0 ) 
		{
			cout << "The query parsed properly!\n" ;
				
			// run the query (= make plan + run the plan)
			system ("rm QueryX.out");
			p->generatePlan();
		} else {
			cout << "Error: Program did not parse!\n";
		}
		
		
		
		// Delete allocated base nodes (plan)
		BaseNode::uninitialize();
		
		// Delete allocated PObjects (parser)
		PObject::uninitialize();
		
		delete p;
	}
 
 
} 
