/*
 *  Opt_Comparison_Gt.h
 *  C-Store - Optimimzation
 *
 *  Created by Nga Tran on 11/01/05.
 * 
 *  Questions, ask Nga Tran at nga@brandeis.edu or Tien Hoang at hoangt@brandeis.edu
 *	
 *  This class implements a equal comparison
 */

  
#ifndef _OPT_COMPARISON_GT_H_
#define _OPT_COMPARISON_GT_H_
 
#include <string>

#include "Opt_Object.h"
#include "Opt_Comparison.h"

using namespace std;
 
class Opt_Comparison_Gt : public Opt_Comparison {				
 public:
  // Default constructor
  Opt_Comparison_Gt();
  
  // Provide column and table names
  Opt_Comparison_Gt(Opt_Object* ptrLeft, Opt_Object* ptrRight);
  
  // Default Destructor
  virtual ~Opt_Comparison_Gt();

  // Clone
  virtual Opt_Comparison_Gt* clone();

  // Clone with new table names, new table alias
  virtual Opt_Comparison_Gt* clone(string sTableName, string sTableAlias);
};

#endif // _OPT_COMPARISON_GT_H_
