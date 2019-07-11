/*
 *  Opt_Number.h
 *  C-Store - Optimimzation
 *
 *  Created by Nga Tran on 11/01/05.
 * 
 *  Questions, ask Nga Tran at nga@brandeis.edu or Tien Hoang at hoangt@brandeis.edu
 *	
 *  This class implements a number
 */

  
#ifndef _OPT_NUMBER_H_
#define _OPT_NUMBER_H_
 
#include <string>

#include "Opt_Object.h"

using namespace std;
 
class Opt_Number : public Opt_Object {		
 public:
  // Default constructor
  Opt_Number();
  
  // Default Destructor
  virtual ~Opt_Number();
  
  // Clone
  virtual Opt_Number* clone();

  // Clone with new table names, new table alias
  virtual Opt_Number* clone(string sTableName, string sTableAlias);
};

#endif // _OPT_NUMBER_H_
