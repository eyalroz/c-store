#include "Opt_Comparison_And.h"

// Default constructor
Opt_Comparison_And::Opt_Comparison_And() : Opt_Comparison() {}

Opt_Comparison_And::Opt_Comparison_And(Opt_Object* ptrLeft, Opt_Object* ptrRight)
  : Opt_Comparison(ptrLeft, ptrRight, "and") {}

Opt_Comparison_And::~Opt_Comparison_And() {}

// Clone
Opt_Comparison_And* Opt_Comparison_And::clone()
{
	Opt_Object* ptrLeft = NULL;
	Opt_Object* ptrRight = NULL;
	
	if (m_ptrLeft != NULL) ptrLeft = m_ptrLeft->clone();
	if (m_ptrRight != NULL) ptrRight = m_ptrRight->clone();
	
	return new Opt_Comparison_And(ptrLeft, ptrRight);
}

// Clone with new table names, new table alias
Opt_Comparison_And* Opt_Comparison_And::clone(string sTableName, string sTableAlias)
{
	Opt_Object* ptrLeft = NULL;
	Opt_Object* ptrRight = NULL;
	
	if (m_ptrLeft != NULL) ptrLeft = m_ptrLeft->clone(sTableName, sTableAlias);
	if (m_ptrRight != NULL) ptrRight = m_ptrRight->clone(sTableName, sTableAlias);
	
	return new Opt_Comparison_And(ptrLeft, ptrRight);
}
