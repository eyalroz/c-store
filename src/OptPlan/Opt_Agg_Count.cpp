/*
 *  Opt_Agg_Count.h
 *  C-Store - Optimimzation
 *
 *  Created by Nga Tran on 11/01/05.
 * 
 *  Questions, ask Nga Tran at nga@brandeis.edu or Tien Hoang at hoangt@brandeis.edu
 *	
 *  This class implements count aggregate
 */

#include "Opt_Agg_Count.h"

// Default constructor
Opt_Agg_Count::Opt_Agg_Count() : Opt_Agg()
{
}
		
// Provide column and table names
Opt_Agg_Count::Opt_Agg_Count(string sColumnName, string sTableName) : Opt_Agg(sColumnName, sTableName, "count")
{
}
		
// Provide a column
Opt_Agg_Count::Opt_Agg_Count(Opt_Object* ptrAgg) : Opt_Agg(ptrAgg, "count")
{
}
		
// Default Destructor
Opt_Agg_Count::~Opt_Agg_Count()
{
}


// Clone
Opt_Agg_Count* Opt_Agg_Count::clone()
{
	Opt_Object*	ptrAgg = NULL;
	if (m_ptrAggExpression != NULL) ptrAgg = m_ptrAggExpression->clone();
	
	return new Opt_Agg_Count(ptrAgg);
}



