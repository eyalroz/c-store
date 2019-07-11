/*
 *  Opt_Number_Integer.h
 *  C-Store - Optimimzation
 *
 *  Created by Nga Tran on 11/01/05.
 * 
 *  Questions, ask Nga Tran at nga@brandeis.edu or Tien Hoang at hoangt@brandeis.edu
 *	
 *  This class implements a float number
 */

  
#ifndef _OPT_NUMBER_INTEGER_H_
#define _OPT_NUMBER_INTEGER_H_
 
#include <string>

#include "Opt_Number.h"

using namespace std;
 
class Opt_Number_Integer : public Opt_Number {	
 protected:
  int m_iNumber;
  
 public:
  // Default constructor
  Opt_Number_Integer();
  Opt_Number_Integer(int dNumber);
  
// Default Destructor
  virtual ~Opt_Number_Integer();
  
  // Clone
  virtual Opt_Number_Integer* clone();
  
  // Clone with new table names, new table alias
  virtual Opt_Number_Integer* clone(string sTableName, string sTableAlias);
  
  // Get and set functions
  void set(int dNumber);
		int get();
};

#endif // _OPT_NUMBER_INTEGER_H_
