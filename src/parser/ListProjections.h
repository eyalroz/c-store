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
/* ListProjections.h
 * 
 * Author: Adam Batkin
 * Last modified by: Tien Hoang on 06/29/2005
 *
 * Class for holding a list of projections in FROM line of a SELECT query
 */

#ifndef _LISTPROJECTIONS_H_
#define _LISTPROJECTIONS_H_

#include <string>
#include <list>
#include <map>
#include <utility>

#include "PObject.h"
#include "Projection.h"
#include "constants.h"

using namespace std;

// Variable
class ListProjections : public PObject
{
   protected:
	list<Projection*> m_listProjections;
	map<string, TVariable*> m_tupleEnv;
	map<string, Projection*> m_projectionEnv;
	//string m_projEnv;
	//string m_tupleEnv;
       
   protected:
		// PObject's constructor is protected because they should not be new directly.
		// A node can be new by using static "create" function
		// This is for the purpose of memory management
		ListProjections();
		
	public:
		virtual ~ListProjections();
		TVariable* getTuple(string projectionName, string tupleName );
		void dump_env();
		void	add(Projection* e);
		void    addTupleAlias( string pname, string tname, string alias );
		list<Projection*> getProjections();
		string	toString();
		
	//---------------------------------------------------------------
	// Memory management functions
		
	public:
		// New a PObject and put it in the m_lpList
		static ListProjections* create();
};

#endif // _LISTPROJECTIONS_H_
