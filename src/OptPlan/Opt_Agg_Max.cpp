/*
 *  Opt_Agg_Max.h
 *  C-Store - Optimimzation
 *
 *  Created by Nga Tran on 11/01/05.
 * 
 *  Questions, ask Nga Tran at nga@brandeis.edu or Tien Hoang at hoangt@brandeis.edu
 *	
 *  This class implements max aggregate
 */

#include "Opt_Agg_Max.h"

// Default constructor
Opt_Agg_Max::Opt_Agg_Max() : Opt_Agg()
{
}
		
// Provide column and table names
Opt_Agg_Max::Opt_Agg_Max(string sColumnName, string sTableName) : Opt_Agg(sColumnName, sTableName, "max")
{
}
		
// Provide a column
Opt_Agg_Max::Opt_Agg_Max(Opt_Object* ptrAgg) : Opt_Agg(ptrAgg, "max")
{
}
		
// Default Destructor
Opt_Agg_Max::~Opt_Agg_Max()
{
}

// Clone
Opt_Agg_Max* Opt_Agg_Max::clone()
{
	Opt_Object*	ptrAgg = NULL;
	if (m_ptrAggExpression != NULL) ptrAgg = m_ptrAggExpression->clone();
	
	return new Opt_Agg_Max(ptrAgg);
}

