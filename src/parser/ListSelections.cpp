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
#include<iostream>
#include "ListSelections.h"

ListSelections::ListSelections() {
	m_iSelectAll = 0;
}

ListSelections::~ListSelections() 
{
		
		Log::writeToLog("ListSelections", 10, "~ListSelections");
}

void ListSelections::addSelection(EHolder* e) {
	m_listSelections.push_back( e );
}

void ListSelections::addAggregate(EHolder* e) {
	m_listAggregates.push_back( e );
}

void ListSelections::setProjectionList( ListProjections* l ) {

  m_lpListProjections = l;
  CatalogInstance* lpCatalog = NULL;
  lpCatalog = CatalogInstance::getCatalog(); //new CatalogInstance();
  if (lpCatalog == NULL)
    parser_error("Unable to create Catalog.");

  if( m_iSelectAll ) {
    m_iSelectAll = 0;
    list<Projection*> lp = m_lpListProjections->getProjections();
    for( list<Projection*>::iterator i = lp.begin(); i != lp.end(); i++ ) {
      Projection* p = (*i);
      string name = p->getAlias();
      if( name == "" ) {
	name = p->get();
      }
      
      vector<string>* lpListColumnNames = lpCatalog->getColumnNames(p->get());
      for(vector<string>::iterator i = lpListColumnNames->begin();
	  i != lpListColumnNames->end(); ++i) {
			EColumn* col = EColumn::create(name, *i);
			EHolder* lpHolder = EHolder::create(col);
			addSelection(lpHolder);
      }
    }
  }
}

void ListSelections::setSelectAll() {
	m_iSelectAll = 1;
}

void ListSelections::typeCheck() {
	for( list<EHolder*>::iterator i = m_listSelections.begin(); i != m_listSelections.end(); i++ ) {
		EHolder* e = (*i);
		Expression* exp = e->getExpression();
		exp->getType( m_lpListProjections );
	}
	for( list<EHolder*>::iterator i = m_listAggregates.begin(); i != m_listAggregates.end(); i++ ) {
		EHolder* e = (*i);
		Expression* exp = e->getExpression();
		exp->getType( m_lpListProjections );
	}
}

list<EHolder*> ListSelections::getSelections() {
	return m_listSelections;
}

list<EHolder*> ListSelections::getAggregates() {
	return m_listAggregates;
}

int ListSelections::getSelectAll() {
	return m_iSelectAll;
}

string	ListSelections::toString() {
	if( m_iSelectAll ) {
		return "*";
	}
	
	string rv;
	if( m_listSelections.size() > 0 ) {
		for( list<EHolder*>::iterator i = m_listSelections.begin(); i != m_listSelections.end(); i++ ) {
			if( i != m_listSelections.begin() ) {
				rv += ", ";
			}
			EHolder* e = (*i);
			rv += e->toString();
		}
		if( m_listAggregates.size() > 0 ) {
			rv += ", ";
		}
	}

	if( m_listAggregates.size() > 0 ) {
		for( list<EHolder*>::iterator i = m_listAggregates.begin(); i != m_listAggregates.end(); i++ ) {
			if( i != m_listAggregates.begin() ) {
				rv += ", ";
			}
			EHolder* e = (*i);
			rv += e->toString();
		}
	}

	return rv;
}


//---------------------------------------------------------------
// Memory management functions

// New a node and put it in the m_lpList
ListSelections* ListSelections::create()
{
	Log::writeToLog("ListSelections", 0, "Create");
		
	ListSelections* lpPObject = new ListSelections();
	pushBack(lpPObject);
	return lpPObject;
}
