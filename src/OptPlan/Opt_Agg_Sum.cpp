/*
 *  Opt_Agg_Sum.h
 *  C-Store - Optimimzation
 *
 *  Created by Nga Tran on 11/01/05.
 * 
 *  Questions, ask Nga Tran at nga@brandeis.edu or Tien Hoang at hoangt@brandeis.edu
 *	
 *  This class implements sum aggregate
 */

#include "Opt_Agg_Sum.h"

// Default constructor
Opt_Agg_Sum::Opt_Agg_Sum() : Opt_Agg()
{
}
		
// Provide column and table names
Opt_Agg_Sum::Opt_Agg_Sum(string sColumnName, string sTableName) : Opt_Agg(sColumnName, sTableName, "sum")
{
}
		
// Provide a column
Opt_Agg_Sum::Opt_Agg_Sum(Opt_Object* ptrAgg) : Opt_Agg(ptrAgg, "sum")
{
}
		
// Default Destructor
Opt_Agg_Sum::~Opt_Agg_Sum()
{
}

// Clone
Opt_Agg_Sum* Opt_Agg_Sum::clone()
{
	Opt_Object*	ptrAgg = NULL;
	if (m_ptrAggExpression != NULL) ptrAgg = m_ptrAggExpression->clone();
	
	return new Opt_Agg_Sum(ptrAgg);
}


