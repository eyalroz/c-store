/*
 *  Opt_Agg_Avg.h
 *  C-Store - Optimimzation
 *
 *  Created by Nga Tran on 11/01/05.
 * 
 *  Questions, ask Nga Tran at nga@brandeis.edu or Tien Hoang at hoangt@brandeis.edu
 *	
 *  This class implements avg aggregate
 */

#include "Opt_Agg_Avg.h"

// Default constructor
Opt_Agg_Avg::Opt_Agg_Avg() : Opt_Agg()
{
}
		
// Provide column and table names
Opt_Agg_Avg::Opt_Agg_Avg(string sColumnName, string sTableName) : Opt_Agg(sColumnName, sTableName, "avg")
{
}
		
// Provide a column
Opt_Agg_Avg::Opt_Agg_Avg(Opt_Object* ptrAgg) : Opt_Agg(ptrAgg, "avg")
{
}
		
// Default Destructor
Opt_Agg_Avg::~Opt_Agg_Avg()
{
}

// Clone
Opt_Agg_Avg* Opt_Agg_Avg::clone()
{
	Opt_Object*	ptrAgg = NULL;
	if (m_ptrAggExpression != NULL) ptrAgg = m_ptrAggExpression->clone();
	
	return new Opt_Agg_Avg(ptrAgg);
}
