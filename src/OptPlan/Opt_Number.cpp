/*
 *  Opt_Number.cpp
 *  C-Store - Optimimzation
 *
 *  Created by Nga Tran on 11/01/05.
 * 
 *  Questions, ask Nga Tran at nga@brandeis.edu or Tien Hoang at hoangt@brandeis.edu
 *	
 *  This class implements a number
 */

#include "Opt_Number.h"

// Default constructor
Opt_Number::Opt_Number()
{
}

// Default Destructor
Opt_Number::~Opt_Number()
{
}

// Clone
Opt_Number* Opt_Number::clone()
{
	return new Opt_Number();
}


// Clone with new table names, new table alias
Opt_Number* Opt_Number::clone(string sTableName, string sTableAlias)
{
	return new Opt_Number();
}
