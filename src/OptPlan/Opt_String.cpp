/*
 *  Opt_String.h
 *  C-Store - Optimimzation
 *
 *  Created by Nga Tran on 11/01/05.
 * 
 *  Questions, ask Nga Tran at nga@brandeis.edu or Tien Hoang at hoangt@brandeis.edu
 *	
 *  This class implements a float number
 */

#include "Opt_String.h"

// Default constructor
Opt_String::Opt_String()
{
	Opt_String(0);
}

Opt_String::Opt_String(string sValue)
{
	 m_sValue = sValue;
}

// Default Destructor
Opt_String::~Opt_String()
{
}

// Clone
Opt_String* Opt_String::clone()
{
	return new Opt_String(m_sValue);
}

// Clone with new table names, new table alias
Opt_String* Opt_String::clone(string sTableName, string sTableAlias)
{
	return new Opt_String(m_sValue);
}
		
// Get and set functions
void Opt_String::set(string sValue)
{
	 m_sValue = sValue;
}
		
string Opt_String::get()
{
	return m_sValue;
}

// Return a string presentation
string Opt_String::toStringNoTableDot()
{
   return m_sValue;
}

string Opt_String::toString() {
  return m_sValue;
}
