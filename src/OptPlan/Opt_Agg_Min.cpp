/*
 *  Opt_Agg_Min.h
 *  C-Store - Optimimzation
 *
 *  Created by Nga Tran on 11/01/05.
 * 
 *  Questions, ask Nga Tran at nga@brandeis.edu or Tien Hoang at hoangt@brandeis.edu
 *	
 *  This class implements min aggregate
 */

#include "Opt_Agg_Min.h"

// Default constructor
Opt_Agg_Min::Opt_Agg_Min() : Opt_Agg()
{
}
		
// Provide column and table names
Opt_Agg_Min::Opt_Agg_Min(string sColumnName, string sTableName) : Opt_Agg(sColumnName, sTableName, "min")
{
}
		
// Provide a column
Opt_Agg_Min::Opt_Agg_Min(Opt_Object* ptrAgg) : Opt_Agg(ptrAgg, "min")
{
}
		
// Default Destructor
Opt_Agg_Min::~Opt_Agg_Min()
{
}

// Clone
Opt_Agg_Min* Opt_Agg_Min::clone()
{
	Opt_Object*	ptrAgg = NULL;
	if (m_ptrAggExpression != NULL) ptrAgg = m_ptrAggExpression->clone();
	
	return new Opt_Agg_Min(ptrAgg);
}

