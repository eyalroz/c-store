/*
 *  Opt_Object.cpp
 *  C-Store - Optimimzation
 *
 *  Created by Nga Tran on 10/28/05.
 * 
 *  Questions, ask Nga Tran at nga@brandeis.edu or Tien Hoang at hoangt@brandeis.edu
 *	
 *  Base class for optimzed plan
 */

#include "Opt_Object.h"

// Default constructor
Opt_Object::Opt_Object(){}
		
// Default Destructor
Opt_Object::~Opt_Object(){}

// Clone
Opt_Object* Opt_Object::clone()
{
	return new Opt_Object();
}

// Clone with new table names, new table alias
Opt_Object* Opt_Object::clone(string sTableName, string sTableAlias)
{
	return new Opt_Object();
}

// Return a string presentation for this aggregate
string  Opt_Object::toStringNoTableDot() {
  return "";
}


// Return a string presentation for this aggregate
string  Opt_Object::toString() {
  return "";
}

// Get table name of this object
string  Opt_Object::getTableName()
{
	return "";
}

// Set table name of this object
void Opt_Object::setTableName(string sTableName)
{
}
  
// Check if this object is a column
bool Opt_Object::isColumn()
{
	return 0; // false
}
