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
/* ListProjections.cpp
 *
 * Original author: Adam Batkin
 * Last modified by: Tien Hoang on 06/29/2005
 * Email questions to tien@brandeis.edu
 *
 * Implementation for ListProjections class
*/

#include "ListProjections.h"
#include "../AM/CatalogInstance.h"
#include <iostream>

ListProjections::ListProjections() 
{
}

ListProjections::~ListProjections()
{
	Log::writeToLog("ListProjections", 10, "~ListProjections");
}

void ListProjections::add(Projection* lpProjection) {
  
  string projectionName = lpProjection->get();
  string projectionAlias = lpProjection->getAlias();
  int isNewProjection = 1;
  int isNewAlias = 1;
  
  if (projectionAlias == "")
    isNewAlias = 0;
  m_listProjections.push_back(lpProjection);

  if (isNewAlias) {
    if (m_projectionEnv.count(projectionAlias) > 0) {
      parser_error("Alias " + projectionAlias + " is used by more than one relation.");
    }
    if (m_projectionEnv.count(projectionName) > 0) {
      if (m_projectionEnv[projectionName]->getAlias() == "") {
	parser_error ("Relation " + projectionName + " is used multiple times without aliases.");
      }
      else {
	isNewProjection = 0;
	m_projectionEnv[projectionAlias] = lpProjection;
	
	Projection* lpProjection = Projection::create (projectionName, projectionAlias);
	m_projectionEnv[projectionName] = lpProjection; // map is one-to-one
      }
    }
    else {
      // isNewProjection = 1;
      m_projectionEnv[projectionAlias] = lpProjection;
    
	  Projection* lpProjection = Projection::create (projectionName, projectionAlias);
      m_projectionEnv[projectionName] = lpProjection;
    }
  }
  else {
    if (m_projectionEnv.count(projectionName) > 0) {
      parser_error("Relation " + projectionName + " is used multiple times without aliases.");
    }
    else {
      isNewProjection = 0;
      m_projectionEnv[projectionName] = lpProjection;
    }
  }
  
  CatalogInstance* lpCatalog = NULL;
  lpCatalog = CatalogInstance::getCatalog();
  if (lpCatalog == NULL)
    parser_error("Unable to obtain Catalog.");
  vector<string>* lpListColumnNames = lpCatalog->getColumnNames(projectionName);
  for (vector<string>::iterator i = lpListColumnNames->begin();
       i != lpListColumnNames->end(); i++) {
    string tupleName = (*i);
    // If there are two tables having the same column name, this column name can no longer be refered.
    if (m_tupleEnv.count(tupleName) > 0) {
      m_tupleEnv.erase(m_tupleEnv.find(tupleName));
    }
    else {
		TVariable* lpTVariable = TVariable::create(lpProjection, tupleName, lpCatalog->getColumnType(tupleName));
      m_tupleEnv[tupleName] = lpTVariable;
	}
    // If there is an alias for the current projection, the projection can only be refered through the alias, not through its original name.
    if (isNewAlias) {
		TVariable* lpTVariable = TVariable::create(lpProjection, tupleName, lpCatalog->getColumnType(tupleName));
      m_tupleEnv[projectionAlias + "." + tupleName] = lpTVariable;
    }
    else {
		TVariable* lpTVariable = TVariable::create(lpProjection, tupleName, lpCatalog->getColumnType(tupleName));
      m_tupleEnv[projectionName + "." + tupleName] = lpTVariable;
	}
  }
}

TVariable* ListProjections::getTuple(string projectionName, string tupleName ) {
  if (projectionName == "") {
    if (m_tupleEnv.count(tupleName)) {
      return m_tupleEnv[tupleName];
    }
    else
      parser_error("Could not find tuple " + tupleName);
  }
  else {
    if (m_tupleEnv.count(projectionName + "." + tupleName) > 0) {
      return m_tupleEnv[projectionName + "." + tupleName];
    }
    else {
      parser_error( "Tuple " + tupleName + " does not exist in relation (or alias) " + projectionName);
      return 0;
    }
  }
   return 0;
}

void ListProjections::dump_env() {
  cout << "Dumping tuple table..." << endl;
  for( map<string,TVariable*>::iterator i = m_tupleEnv.begin(); i != m_tupleEnv.end(); i++ ) {
    cout << "Name=" << (*i).first << ", Value=" << (*i).second << endl;
  }
  cout << "--------" << endl;
  cout << "Dumping projection list..." << endl;
  for( list<Projection*>::iterator i = m_listProjections.begin(); i != m_listProjections.end(); i++ ) {
    cout << (*i)->get() << endl;
  }
  cout << "--------" << endl;
  cout << "Dumping projection table..." << endl;
  for( map<string,Projection*>::iterator i = m_projectionEnv.begin(); i != m_projectionEnv.end(); i++ ) {
    cout << "Name=" << (*i).first << ", Value=" << (*i).second << endl;
  }
  cout << "--------" << endl;
}

list<Projection*> ListProjections::getProjections() {
  return m_listProjections;
}

void ListProjections::addTupleAlias( string pname, string tname, string alias ) {
  string name;
  if( pname == "" && m_tupleEnv.count( tname ) > 0 ) {
    name = tname;
  } else if( pname == "" ) {
    parser_error( "Unknown column: " + tname );
  } else if( m_tupleEnv.count( pname + "." + tname ) > 0 ) {
    name = pname + "." + tname;
  } else {
    parser_error( "Unknown column: " + pname + "." + tname );
  }
  m_tupleEnv[alias] = m_tupleEnv[name];
}

string ListProjections::toString() {
  if( m_listProjections.size() > 0 ) {
    string s;
    for( list<Projection*>::iterator i = m_listProjections.begin(); i != m_listProjections.end(); i++ ) {
      if( i != m_listProjections.begin() ) {
	s += ", ";
      }
      s += (*i)->toString();
    }
    return s;
  } else {
    return "__NO PROJECTIONS__";
  }
}

//---------------------------------------------------------------
// Memory management functions

// New a node and put it in the m_lpList
ListProjections* ListProjections::create()
{
	Log::writeToLog("ListProjections", 0, "Create");
		
	ListProjections* lpPObject = new ListProjections();
	pushBack(lpPObject);
	return lpPObject;
}
