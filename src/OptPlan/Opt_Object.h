/*
 *  Opt_Object.h
 *  C-Store - Optimimzation
 *
 *  Created by Nga Tran on 10/28/05.
 * 
 *  Questions, ask Nga Tran at nga@brandeis.edu or Tien Hoang at hoangt@brandeis.edu
 *	
 *  Base class for optimzed plan
 */
 
#ifndef _OPT_OBJECT_H_
#define _OPT_OBJECT_H_

#include <string>

using namespace std;

class Opt_Object {
 public:
  // Default constructor
  Opt_Object();
    
  // Default Destructor
  virtual ~Opt_Object();
  
  // Clone
  virtual Opt_Object* clone();
  
  // Clone with new table names, new table alias
  virtual Opt_Object* clone(string sTableName, string sTableAlias);

  // Return a string presentation for this aggregate
  virtual string toString();

  // Return a string presentation for this aggregate
  virtual string toStringNoTableDot();

  // Get table name of this object
  virtual string getTableName();
  
  // Set table name of this object
  virtual void setTableName(string sTableName);
  
  // Check if this object is a column
  virtual bool isColumn();
  
};

#endif // _OPT_OBJECT_H_
