/*
 *  Opt_Agg.cpp
 *  C-Store - Optimimzation
 *
 *  Created by Nga Tran on 11/01/05.
 * 
 *  Questions, ask Nga Tran at nga@brandeis.edu or Tien Hoang at hoangt@brandeis.edu
 *	
 *  This class is a base class for all aggregate
 */
 
#include "Opt_Agg.h"


// Default constructor
Opt_Agg::Opt_Agg()
{
	m_ptrAggExpression = NULL;
	m_sOp = "";
	m_sAlias = "";
}
		
// Provide column and table names
Opt_Agg::Opt_Agg(string sColumnName, string sTableName, string sOp)
{
	Opt_Agg();
	m_ptrAggExpression = new Opt_Column(sColumnName, sTableName);
	m_sOp = sOp;
	m_sAlias = "";
}

Opt_Agg::Opt_Agg(string sColumnName, string sAlias, string sTableName, string sOp) {
  m_ptrAggExpression = new Opt_Column(sColumnName, sTableName);
  m_sOp = sOp;
  m_sAlias = sAlias;
}
		
// Provide a column
Opt_Agg::Opt_Agg(Opt_Object* ptrAgg)
{
	m_ptrAggExpression = ptrAgg;
	m_sOp = "";
	m_sAlias = "";
}

// Provide a column
Opt_Agg::Opt_Agg(Opt_Object* ptrAgg, string sOp)
{
	m_ptrAggExpression = ptrAgg;
	m_sOp = sOp;
	m_sAlias = "";
}
		
// Default Destructor
Opt_Agg::~Opt_Agg()
{
}

// Clone
Opt_Agg* Opt_Agg::clone()
{
	Opt_Object*	ptrAgg = NULL;
	if (m_ptrAggExpression != NULL) ptrAgg = m_ptrAggExpression->clone();
	
	return new Opt_Agg(ptrAgg, m_sOp);
}

		
// Get and set functions
void Opt_Agg::setAggregate(Opt_Object*	ptrAgg)
{
	m_ptrAggExpression = ptrAgg;
}

Opt_Object* Opt_Agg::getAggregate()
{
	return m_ptrAggExpression;
}

void Opt_Agg::setOp(string sOp) {
  m_sOp = sOp;
}

string Opt_Agg::getOp() {
  return m_sOp;
}

void Opt_Agg::setAlias(string sAlias) {
  m_sAlias = sAlias;
}

string Opt_Agg::getAlias() {
  return m_sAlias;
}

// Return a string presentation for this aggregate
string  Opt_Agg::toStringNoTableDot() {
  string s = "";
  
  s = m_sOp + "(" + m_ptrAggExpression->toStringNoTableDot() + ")";
  if (m_sAlias.compare("") != 0) {
    s = s + " AS " + m_sAlias;
  }
  
  return s;
  //return Opt_Object::toString();
}

// Return a string presentation for this aggregate
string  Opt_Agg::toString() {
  string s = "";
  
  s = m_sOp + "(" + m_ptrAggExpression->toString() + ")";
  if (m_sAlias.compare("") != 0) {
    s = s + " AS " + m_sAlias;
  }
  
  return s;
  //return Opt_Object::toString();
}

// Get table name of this object
string Opt_Agg::getTableName()
{
	// For now only dealing with COLUMN
	return ((Opt_Column*) m_ptrAggExpression)->getTableName();
}

// Set table name of this object
void Opt_Agg::setTableName(string sTableName)
{
	((Opt_Column*) m_ptrAggExpression)->setTableName(sTableName);
}


// For now, aggregate is a column -> return column
Opt_Column* Opt_Agg::getColumn()
{
  Opt_Column* ptrCol = (Opt_Column*) m_ptrAggExpression;

  return ptrCol;
}
