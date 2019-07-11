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

  
#ifndef _OPT_AGG_SUM_H_
#define _OPT_AGG_SUM_H_
 
#include <string>

#include "Opt_Agg.h"
#include "Opt_Column.h"

using namespace std;
 
class Opt_Agg_Sum : public Opt_Agg {		
 public:
  // Default constructor
  Opt_Agg_Sum();
  
  // Provide column and table names
  Opt_Agg_Sum(string sColumnName, string sTableName);
  
  // Provide a column
  Opt_Agg_Sum(Opt_Object* ptrAgg);
  
  // Default Destructor
  virtual ~Opt_Agg_Sum();
  
  // Clone
  virtual Opt_Agg_Sum* clone();	
};

#endif // _OPT_AGG_SUM_H_
