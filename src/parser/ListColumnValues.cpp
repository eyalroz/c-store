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
 *  ListColumnValues.h
 *  cstore
 *
 *  Created by Nga Tran on 6/3/05.
 *  
 *	This is a list of column names and their values
 *      This is used for inserting a row of a projection
 */

#include "ListColumnValues.h"

ListColumnValues::ListColumnValues() 
{
	m_list_projections = NULL;
}

ListColumnValues::~ListColumnValues() 
{  
	Log::writeToLog("ListColumnValues", 10, "~ListColumnValues");
}


void ListColumnValues::add(EColumn* lpColumn, Expression* lpValue)
{
	m_listColumns.push_back(lpColumn);
	m_listValues.push_back(lpValue);
}


void ListColumnValues:: setColumns(ListCols* lpColumns)
{
  m_listColumns = lpColumns->get();
}


void ListColumnValues::setValues(ListExps* lpValues)
{
  m_listValues = lpValues->get();
}

void ListColumnValues::typeCheck(Projection* lpProjection) {
  if (m_listColumns.size() != m_listValues.size())
    parser_error("Invalid INSERT statement: sizes are not equal.");
  CatalogInstance* lpCatalog = NULL;
  lpCatalog = CatalogInstance::getCatalog();
  if (lpCatalog == NULL)
    parser_error("Unable to create Catalog.");

  m_list_projections = ListProjections::create();
  m_list_projections->add(lpProjection);
  list<EColumn*>::iterator i = m_listColumns.begin();
  list<Expression*>::iterator j = m_listValues.begin();

  for (; i != m_listColumns.end();) {
    EColumn* column = *i;
    Expression* expression = *j;
    int column_type = lpCatalog->getColumnType(column->getName());
    int value_type = expression->getType(m_list_projections);
    if (column_type != value_type)
      parser_error("Invalid INSERT statement: types are not equivalent.");
    i++; j++;
  }
}

list<EColumn*> ListColumnValues::getColumns()
{
    return m_listColumns;
}


list<Expression*> ListColumnValues::getValues()
{
    return m_listValues;
}


void ListColumnValues::getColumnValues(list<EColumn*> *lpColumns, list<Expression*> *lpValues)
{
   (*lpColumns) = m_listColumns;
   (*lpValues) = m_listValues;
}


// Show list of:   column   value \n
string	ListColumnValues::toString()
{
   if (m_listColumns.size() != m_listValues.size() )
   {
     return "__Invalid columns and values__";
   }

   if (m_listColumns.size() == 0)
   {
     return "__NO COLUMNS__";
   }

   int length = m_listColumns.size();
   list<EColumn*>::iterator lpColumn = m_listColumns.begin();
   list<Expression*>::iterator lpValue = m_listValues.begin();
   string s;

   for (int i = 0; i < length; i++)
   {
     s = (*lpColumn)->toString() + "   " + (*lpValue)->toString() + "\n";
   }

   return s;
}

//---------------------------------------------------------------
// Memory management functions

// New a node and put it in the m_lpList
ListColumnValues* ListColumnValues::create()
{
	Log::writeToLog("ListColumnValues", 0, "Create");
		
	ListColumnValues* lpPObject = new ListColumnValues();
	pushBack(lpPObject);
	return lpPObject;
}
