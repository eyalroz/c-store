/*
 *  Opt_Agg.h
 *  C-Store - Optimimzation
 *
 *  Created by Nga Tran on 11/01/05.
 * 
 *  Questions, ask Nga Tran at nga@brandeis.edu or Tien Hoang at hoangt@brandeis.edu
 *	
 *  This class is a base class for all aggregate
 */
 
  
#ifndef _OPT_AGG_H_
#define _OPT_AGG_H_
 
#include <string>

#include "Opt_Object.h"
#include "Opt_Column.h"

using namespace std;
 
class Opt_Agg : public Opt_Object {		
 protected:
  // Member declaration
  Opt_Object*	m_ptrAggExpression; // Column or expression to be aggregated
									// NGA: But for now, only column is working
  string m_sOp;
  string m_sAlias;

 public:
  // Default constructor
  Opt_Agg();
  
  // Provide column and table names
  Opt_Agg(string sColumnName, string sTableName, string sOp);

  // Provide aggregate, its alias, table name, and type of operation
  Opt_Agg(string sColumnName, string sAlias, string sTableName, string sOp);
  
  // Provide a column
  Opt_Agg(Opt_Object* ptrAgg);
  
  // Provide a column
  Opt_Agg(Opt_Object* ptrAgg, string sOp);
  
  // Default Destructor
  virtual ~Opt_Agg();
  
  // Clone
  virtual Opt_Agg* clone();
  
  // Get and set functions
  void setAggregate(Opt_Object*	ptrAgg);
  Opt_Object* getAggregate();	

  void setOp(string sOp);
  string getOp();
  
  void setAlias(string sAlias);
  string getAlias();

  // Return a string presentation for this aggregate
  virtual string toString();

  // Return a string presentation for this aggregate
  virtual string toStringNoTableDot();
  
  // Get table name of this object
  virtual string getTableName();
  
  // Set table name of this object
  virtual void setTableName(string sTableName);

  // For now, aggregate is a column -> return column
  Opt_Column* getColumn();
};

#endif // _OPT_AGG_H_
