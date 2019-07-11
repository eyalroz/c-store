/*
 *  Opt_Comparison.h
 *  C-Store - Optimimzation
 *
 *  Created by Nga Tran on 11/01/05.
 * 
 *  Questions, ask Nga Tran at nga@brandeis.edu or Tien Hoang at hoangt@brandeis.edu
 *	
 *  This class implements a join
 */

#include "Opt_Comparison.h"

// Default constructor
Opt_Comparison::Opt_Comparison() {
  m_ptrLeft = NULL;
  m_ptrRight = NULL;
  m_sOp = "";
}

// Provide column and table names
Opt_Comparison::Opt_Comparison(Opt_Object* ptrLeft, Opt_Object* ptrRight, string sOp) {
  m_ptrLeft = ptrLeft;
  m_ptrRight = ptrRight;
  m_sOp = sOp;
}


// Default Destructor
Opt_Comparison::~Opt_Comparison() {}
		
void Opt_Comparison::setLeft(Opt_Object* ptrLeft)
{
	m_ptrLeft = ptrLeft;
}

// Clone
Opt_Comparison* Opt_Comparison::clone()
{
	Opt_Object* ptrLeft = NULL;
	Opt_Object* ptrRight = NULL;
	
	if (m_ptrLeft != NULL) ptrLeft = m_ptrLeft->clone();
	if (m_ptrRight != NULL) ptrRight = m_ptrRight->clone();
	
	return new Opt_Comparison(ptrLeft, ptrRight, m_sOp);
}

// Clone with new table names, new table alias
Opt_Comparison* Opt_Comparison::clone(string sTableName, string sTableAlias)
{
	Opt_Object* ptrLeft = NULL;
	Opt_Object* ptrRight = NULL;
	
	if (m_ptrLeft != NULL) ptrLeft = m_ptrLeft->clone(sTableName, sTableAlias);
	if (m_ptrRight != NULL) ptrRight = m_ptrRight->clone(sTableName, sTableAlias);
	
	return new Opt_Comparison(ptrLeft, ptrRight, m_sOp);  
}

Opt_Object* Opt_Comparison::getLeft()
{
	return m_ptrLeft;
}

		
void Opt_Comparison::setRight(Opt_Object* ptrRight)
{
	m_ptrRight = ptrRight;
}

Opt_Object* Opt_Comparison::getRight()
{
	return m_ptrRight;
}

void Opt_Comparison::setOp(string sOp) {
  m_sOp = sOp;
}

string Opt_Comparison::getOp() {
  return m_sOp;
}

string Opt_Comparison::toStringNoTableDot() {
  string s = "";
  
  s = m_ptrLeft->toStringNoTableDot() + " " + m_sOp + " " + m_ptrRight->toStringNoTableDot();

  return s;
}


string Opt_Comparison::toString() {
  string s = "";
  
  s = m_ptrLeft->toString() + " " + m_sOp + " " + m_ptrRight->toString();

  return s;
}
