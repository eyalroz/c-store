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
#include "ListCols.h"

ListCols::ListCols() {
}

ListCols::~ListCols()
{
	Log::writeToLog("ListCols", 10, "~ListCols");
}

void ListCols::add(EColumn* e) {
	m_listCols.push_back( e );
}

void ListCols::typeCheck( ListProjections* lp, ListSelections* ls ) {
  //list<EHolder*> aggs = ls->getAggregates();
  /*  TO DO: make sure that the column name is in the list of all selections
  string selection_list = "";
  if (ls != NULL) {
    list<EHolder*> eholder_list = ls->getSelection();
  }
  */
  string to_check = "";
  for( list<EColumn*>::iterator i = m_listCols.begin(); i != m_listCols.end(); i++ ) {
    EColumn* col = *i;
    string name = col->getName();
    if (to_check.find(name) != string::npos)
      parser_error(name + " field cannot not be repeated in Insert query.");
    else
      to_check.append(name);
    col->getType(lp);

    /*
    EColumn* col = *i;
    col->getType( lp );
    string name = col->getName();
    for( list<EHolder*>::iterator j = aggs.begin(); j != aggs.end(); j++ ) {
      EAgg* e = dynamic_cast<EAgg*>((*j)->getExpression());
      EColumn* icol = e->getRight();
      string iname = icol->getName();
      if( name.compare( iname ) == 0 ) {
	parser_error( "Column " + name + " can not be used in an aggregate function and in the GROUP BY clause" );
      }
    }
    */
  }
}

string	ListCols::toString() {
	if( m_listCols.size() > 0 ) {
		string s;
		for( list<EColumn*>::iterator i = m_listCols.begin(); i != m_listCols.end(); i++ ) {
			if( i != m_listCols.begin() ) {
				s += ", ";
			}
			s += (*i)->toString();
		}
		return s;
	} else {
		return "__NO COLUMNS__";
	}
}

list<EColumn*> ListCols::get()
{
  return m_listCols;
}

void ListCols::isValid(ListSelections* ls, ListProjections* lp) {
  string sAllPossibleNames = "";

  // All the aliases that are in aggregates
  list<EHolder*> listAggregates = ls->getAggregates();
  for (list<EHolder*>::iterator i = listAggregates.begin();
       i != listAggregates.end(); i++) {
    string sAlias = (*i)->getAlias();
    if (sAlias.compare("") != 0) {
      sAllPossibleNames += sAlias;
    }
  }

  // All the columns' names of all the projections
  CatalogInstance* ptrCatalog = NULL;
  ptrCatalog = CatalogInstance::getCatalog();
  if (ptrCatalog == NULL) {
    parser_error("ListOrdering.cpp: Unable to obtain catalog\n");
  }
  list<Projection*> listProjections = lp->getProjections();
  for (list<Projection*>::iterator i = listProjections.begin();
       i != listProjections.end(); i++) {
    string sProjectionName = (*i)->get();
    vector<string>* ptrColumnNames = ptrCatalog->getColumnNames(sProjectionName);
    for (vector<string>::iterator j = ptrColumnNames->begin();
         j != ptrColumnNames->end(); j++) {
      sAllPossibleNames += (*j);
    }
  }

  // For each group by item, check if it's in the sAllPossibleNames
  for (list<EColumn*>::iterator i = m_listCols.begin();
       i != m_listCols.end(); i++) {
    string sColumnName = (*i)->getName();
    unsigned int iLocation = sAllPossibleNames.find(sColumnName, 0);
    if (iLocation == string::npos) {
      parser_error("Invalid GROUP BY clause: could not find tuple " + sColumnName);
    }
  }
}

//---------------------------------------------------------------
// Memory management functions

// New a node and put it in the m_lpList
ListCols* ListCols::create()
{
	Log::writeToLog("ListCols", 0, "Create");
		
	ListCols* lpPObject = new ListCols();
	pushBack(lpPObject);
	return lpPObject;
}
