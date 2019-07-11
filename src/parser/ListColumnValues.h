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
 *  ListColumn.h.h
 *  cstore
 *
 *  Created by Nga Tran on 6/3/05.
 *  
 *	This is a list of column names and their values
 *      This is used for inserting a row of a projection
 */

#ifndef _LISTCOLUMNVALUES_H_
#define _LISTCOLUMNVALUES_H_

#include <string>
#include <list>

#include "Expression.h"
#include "ListCols.h"
#include "ListExps.h"
#include "Expressions/EColumn.h"
#include "Expressions/EString.h"
#include "Expressions/ENumbers/EFloat.h"
#include "Expressions/ENumbers/EInteger.h"

using namespace std;

class ListColumnValues : public PObject
{
   protected:
  list<EColumn*> m_listColumns;
  list<Expression*> m_listValues;
  
	// For deallocating
	ListProjections* m_list_projections;
  
       
   protected:
		// PObject's constructor is protected because they should not be new directly.
		// A node can be new by using static "create" function
		// This is for the purpose of memory management
		ListColumnValues();
	
	public:
		virtual ~ListColumnValues();
	
		void	add(EColumn* lpColumn, Expression* lpValue);
		void    setColumns(ListCols* lpColumns);
		void    setValues(ListExps* lpValues);
		void    typeCheck(Projection* lpProjection);

		list<EColumn*> getColumns();
		list<Expression*> getValues();
		void getColumnValues(list<EColumn*> *lpColumns, list<Expression*> *lpValues);

		string	toString();
		
	//---------------------------------------------------------------
	// Memory management functions
		
	public:
		// New a PObject and put it in the m_lpList
		static ListColumnValues* create();
};

#endif // _LISTCOLUMNVALUES_H_
