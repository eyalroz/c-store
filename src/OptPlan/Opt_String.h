/*
 *  Opt_String.h
 *  C-Store - Optimimzation
 *
 *  Created by Nga Tran on 11/01/05.
 * 
 *  Questions, ask Nga Tran at nga@brandeis.edu or Tien Hoang at hoangt@brandeis.edu
 *	
 *  This class implements a string
 */

  
#ifndef _OPT_STRING_H_
#define _OPT_STRING_H_
 
#include <string>

#include "Opt_Number.h"

using namespace std;

class Opt_String : public Opt_Number {	
 protected:
  string m_sValue;
  
 public:
  // Default constructor
  Opt_String();
  Opt_String(string sValue);
    
  // Default Destructor
  virtual ~Opt_String();
  
  // Clone
  virtual Opt_String* clone();
  
  // Clone with new table names, new table alias
  virtual Opt_String* clone(string sTableName, string sTableAlias);

  // Get and set functions
  void set(string sValue);
  string get();

  string toString();

  // Return a string presentation for this aggregate
  virtual string toStringNoTableDot();

};

#endif // _OPT_STRING_H_
