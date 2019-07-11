#include "Opt_Comparison_Or.h"

// Default constructor                                                                                 
Opt_Comparison_Or::Opt_Comparison_Or() : Opt_Comparison() {}

Opt_Comparison_Or::Opt_Comparison_Or(Opt_Object* ptrLeft, Opt_Object* ptrRight)
  : Opt_Comparison(ptrLeft, ptrRight, "or") {}

Opt_Comparison_Or::~Opt_Comparison_Or() {}

// Clone
Opt_Comparison_Or* Opt_Comparison_Or::clone()
{
	Opt_Object* ptrLeft = NULL;
	Opt_Object* ptrRight = NULL;
	
	if (m_ptrLeft != NULL) ptrLeft = m_ptrLeft->clone();
	if (m_ptrRight != NULL) ptrRight = m_ptrRight->clone();
	
	return new Opt_Comparison_Or(ptrLeft, ptrRight);
}


// Clone with new table names, new table alias
Opt_Comparison_Or* Opt_Comparison_Or::clone(string sTableName, string sTableAlias)
{
	Opt_Object* ptrLeft = NULL;
	Opt_Object* ptrRight = NULL;
	
	if (m_ptrLeft != NULL) ptrLeft = m_ptrLeft->clone(sTableName, sTableAlias);
	if (m_ptrRight != NULL) ptrRight = m_ptrRight->clone(sTableName, sTableAlias);
	
	return new Opt_Comparison_Or(ptrLeft, ptrRight);
}
