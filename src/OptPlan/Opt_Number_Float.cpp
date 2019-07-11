/*
 *  Opt_Number_Float.h
 *  C-Store - Optimimzation
 *
 *  Created by Nga Tran on 11/01/05.
 * 
 *  Questions, ask Nga Tran at nga@brandeis.edu or Tien Hoang at hoangt@brandeis.edu
 *	
 *  This class implements a float number
 */

#include "Opt_Number_Float.h"

// Default constructor
Opt_Number_Float::Opt_Number_Float()
{
	Opt_Number_Float(0);
}

Opt_Number_Float::Opt_Number_Float(double dNumber)
{
	 m_dNumber = dNumber;
}

// Default Destructor
Opt_Number_Float::~Opt_Number_Float()
{
}

// Clone
Opt_Number_Float* Opt_Number_Float::clone()
{
	return new Opt_Number_Float(m_dNumber);
}


// Clone with new table names, new table alias
Opt_Number_Float* Opt_Number_Float::clone(string sTableName, string sTableAlias)
{
	return new Opt_Number_Float(m_dNumber);
}
		
// Get and set functions
void Opt_Number_Float::set(double dNumber)
{
	 m_dNumber = dNumber;
}
		
double Opt_Number_Float::get()
{
	return m_dNumber;
}

