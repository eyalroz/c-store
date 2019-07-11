/*
 *  Opt_Number_Float.h
 *  C-Store - Optimimzation
 *
 *  Created by Nga Tran on 11/01/05.
 * 
 *  Questions, ask Nga Tran at nga@brandeis.edu or Tien Hoang at hoangt@brandeis.edu
 *	
 *  This class implements a float number
 */

  
#ifndef _OPT_NUMBER_FLOAT_H_
#define _OPT_NUMBER_FLOAT_H_
 
#include <string>

#include "Opt_Number.h"

using namespace std;
 
class Opt_Number_Float : public Opt_Number {	
 protected:
  double m_dNumber;
  
 public:
  // Default constructor
  Opt_Number_Float();
  Opt_Number_Float(double dNumber);
    
  // Default Destructor
  virtual ~Opt_Number_Float();
  
  // Clone
  virtual Opt_Number_Float* clone();
  
  // Clone with new table names, new table alias
  virtual Opt_Number_Float* clone(string sTableName, string sTableAlias);

  // Get and set functions
  void set(double dNumber);
  double get();
};

#endif // _OPT_NUMBER_FLOAT_H_
