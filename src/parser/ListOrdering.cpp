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
#include "ListOrdering.h"

ListOrdering::ListOrdering() {
}

ListOrdering::~ListOrdering()
{
	Log::writeToLog("ListOrdering", 10, "~ListOrdering");
}

void ListOrdering::add(ColOrder* c) {
	m_listOrdering.push_back( c );
}

void ListOrdering::typeCheck( ListProjections* lp ) {
	for( list<ColOrder*>::iterator i = m_listOrdering.begin(); i != m_listOrdering.end(); i++ ) {
		ColOrder* c = (*i);
		c->typeCheck( lp );
	}
}

list<ColOrder*> ListOrdering::getListOrdering() {
  return m_listOrdering;
}

string	ListOrdering::toString() {
	if( m_listOrdering.size() > 0 ) {
		string s = " ORDER BY ";
		for( list<ColOrder*>::iterator i = m_listOrdering.begin(); i != m_listOrdering.end(); i++ ) {
			if( i != m_listOrdering.begin() ) {
				s += ", ";
			}
			s += (*i)->toString();
		}
		return s;
	} else {
		return "";
	}
}

void ListOrdering::isValid(ListSelections* ls, ListProjections* lp) {
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

  // For each order by item, check if it's in the sAllPossibleNames
  for (list<ColOrder*>::iterator i = m_listOrdering.begin();
       i != m_listOrdering.end(); i++) {
    EColumn* ptrColumn = (*i)->getColumn();
    string sColumnName = ptrColumn->getName();
    unsigned int iLocation = sAllPossibleNames.find(sColumnName, 0);
    if (iLocation == string::npos) {
      parser_error("Invalid ORDER BY clause: could not find tuple " + sColumnName);
    }
  }
}

//---------------------------------------------------------------
// Memory management functions

// New a node and put it in the m_lpList
ListOrdering* ListOrdering::create()
{
	Log::writeToLog("ListOrdering", 0, "Create");
		
	ListOrdering* lpPObject = new ListOrdering();
	pushBack(lpPObject);
	return lpPObject;
}
