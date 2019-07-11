/*
 *  Opt_Join.h
 *  C-Store - Optimimzation
 *
 *  Created by Nga Tran on 11/01/05.
 * 
 *  Questions, ask Nga Tran at nga@brandeis.edu or Tien Hoang at hoangt@brandeis.edu
 *	
 *  This class implements a join
 */

  
#ifndef _OPT_JOIN_H_
#define _OPT_JOIN_H_
 
#include <string>
#include <iostream>

#include "Opt_Object.h"
#include "Opt_Column.h"

using namespace std;
 
class Opt_Join : public Opt_Object {		
 protected:
  // Member declaration
  Opt_Column*	m_ptrFromColumn;
  Opt_Column*	m_ptrToColumn;
  
  bool			m_bIsTraversed;		// true (1) if this join was traveresed 
  bool     m_bIsSnowflakeJoinPredicate;         // true (1) if this join is not a join in one snowflake
  
 public:
  // Default constructor
  Opt_Join();
  
  // Provide column and table names
  Opt_Join(Opt_Column* ptrFromColumn, Opt_Column* ptrToColumn);
  
  // Default Destructor
  virtual ~Opt_Join();

  // Clone
  virtual Opt_Join* clone();

  // Clone
 // Also in  mark snowflake join for this join if it points to the same table as ptrNonSnowflakeJoin but goes from different table
  virtual Opt_Join* clone(Opt_Join* ptrNonSnowflakeJoin);
  
  void setFromColumn(Opt_Column* ptrFromColumn);
  Opt_Column* getFromColumn();
  
  void setToColumn(Opt_Column* ptrToColumn);
  Opt_Column* getToColumn();

  string toString();

  // Return a string presentation for this aggregate
  virtual string toStringNoTableDot();
  
  // Mark this join traversed
  void setTraversed();

  // UNmark this join traversed
  void resetTraversed();
  
  // check if this join traversed
  bool isTraversed();

  // Mark this join is a join between 2 snowfalkes
  void setSnowflakesJoin(); 

  // check if this is a snowflake join
  bool isSnowflakesJoin();
};

#endif // _OPT_JOIN_H_
