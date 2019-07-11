/*
 *  Opt_Column.h
 *  C-Store - Optimimzation
 *
 *  Created by Nga Tran on 10/28/05.
 * 
 *  Questions, ask Nga Tran at nga@brandeis.edu or Tien Hoang at hoangt@brandeis.edu
 *	
 *  This class implements an attribute of a table
 */
 
  
#ifndef _OPT_COLUMN_H_
#define _OPT_COLUMN_H_
 
#include <string>

#include "Opt_Object.h"

using namespace std;

class Opt_Column : public Opt_Object {
 protected:
  // Member declaration
  string	m_sColumnName;	
  string	m_sTableName;
  string	m_sColumnAlias;	
  string	m_sTableAlias;

 public:
  // Default constructor
  Opt_Column();
		
  // Provide column and table names
  Opt_Column(string sColumnName, string sTableName);
 
  // Provide column and table names and column alias
  Opt_Column(string sColumnName, string sTableName, string sColumnAlias);;
   
  // Provide column and table names and their alias
  Opt_Column(string sColumnName, string sTableName, string sColumnAlias, string sTableAlias);
  
  // Default Destructor
  virtual ~Opt_Column();
  
  // Clone
  virtual Opt_Column* clone();
 
  // Clone with new table names, new table alias
  virtual Opt_Column* clone(string sTableName, string sTableAlias);
  
  // Get and set functions
  void setColumnName(string sColumnName);
  string getColumnName();
  
  void setTableName(string sTableName);
  string getTableName();
  
  void setColumnAlias(string sColumnAlias);
  string getColumnAlias();
  
  void setTableAlias(string sTableAlias);
  string getTableAlias();

  string toString();

  // Return a string presentation for this aggregate
  virtual string toStringNoTableDot();

  // Check if this object is a column
  virtual bool isColumn();
};

#endif // _OPT_COLUMN_H_
