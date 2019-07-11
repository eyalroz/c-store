/*
 *  Opt_Number_Integer.h
 *  C-Store - Optimimzation
 *
 *  Created by Nga Tran on 11/01/05.
 * 
 *  Questions, ask Nga Tran at nga@brandeis.edu or Tien Hoang at hoangt@brandeis.edu
 *	
 *  This class implements a float number
 */

#include "Opt_Number_Integer.h"

// Default constructor
Opt_Number_Integer::Opt_Number_Integer()
{
	Opt_Number_Integer(0);
}

Opt_Number_Integer::Opt_Number_Integer(int iNumber)
{
	 m_iNumber = iNumber;
}

  
// Default Destructor
Opt_Number_Integer::~Opt_Number_Integer()
{
}

// Clone
Opt_Number_Integer* Opt_Number_Integer::clone()
{
	return new Opt_Number_Integer(m_iNumber);
}

// Clone with new table names, new table alias
Opt_Number_Integer* Opt_Number_Integer::clone(string sTableName, string sTableAlias)
{
	return new Opt_Number_Integer(m_iNumber);
}
		
// Get and set functions
void Opt_Number_Integer::set(int iNumber)
{
	 m_iNumber = iNumber;
}
		
int Opt_Number_Integer::get()
{
	return m_iNumber;
}

