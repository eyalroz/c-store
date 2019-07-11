/*
 *  Opt_Table.h
 *  C-Store - Optimimzation
 *
 *  Created by Nga Tran on 10/28/05.
 * 
 *  Questions, ask Nga Tran at nga@brandeis.edu or Tien Hoang at hoangt@brandeis.edu
 *	
 *  This class implements a table in a query_graph.
 *	A table has one of two roles: FACT or DIMENSION
 *	A table has a list of its attributes and their roles in the query
 */

#ifndef _OPT_TABLE_H_
#define _OPT_TABLE_H_

#include <iostream>
#include <list>
#include <string>

#include "Opt_Object.h"
#include "Opt_Column.h"
#include "Opt_Agg.h"
#include "Opt_Join.h"
#include "Opt_Comparison.h"
#include "Opt_Comparison_And.h"

#define TABLE_ROLE_FACT		1
#define TABLE_ROLE_DIMENSION	2

using namespace std;

class Opt_Table: Opt_Object {
 protected:
  // Member declaration
  string  m_sName;  // Table name
  string  m_sAlias; // Alias
  int m_iRole;	    // Table role: TABLE_ROLE_FACT or TABLE_ROLE_DIMENSION (default)
  long m_lRowNum;   // Number of rows of this table  
  long m_lCardinality; // cardinality of this table in the query (given selection predicates)

//  list<Opt_Column*>  mColumnList;	// List of attributes of the table apppearing in the query

  list<Opt_Column*>	mSelectionList;
  list<Opt_Agg*>	mAggregateList;
  list<Opt_Join*>	mJoinList;
  list<Opt_Comparison*> mNoneJoinList;
  Opt_Comparison*	m_ptrNoneJoinPredicates;
  
		
 public:
  // Default constructor
  Opt_Table();
  
  Opt_Table(string sName);
  Opt_Table(string sName, string sAlias);
  
  // Default Destructor
  virtual ~Opt_Table();
  
  string getName();
  string getAlias();
  int getRole();
  long getRowNum();
  long getCardinality();
  long double getSelectivity();

  list<Opt_Column*> getSelectionList();
  list<Opt_Agg*> getAggregateList();
  list<Opt_Join*> getJoinList();
  list<Opt_Comparison*> getNoneJoinList();
  Opt_Comparison* getNoneJoin();

  //  list<Opt_Column*> getColumns();
  
  // Set table name of this object
  virtual void setTableName(string sTableName);
  
  void setName(string sName);
  void setAlias(string sAlias);
  void setRole(int iRole);
  void setRowNum(long lRowNum);
  void setCardinality(long lCardinality);
 
  void addSelection(Opt_Column* ptrColumn);
  void addAggregate(Opt_Agg* ptrAgg);
  void addJoin(Opt_Join* ptrJoin);
  void addNoneJoin(Opt_Comparison* ptrComparison);
  void setNoneJoinPredicate(Opt_Comparison* ptrComparison);

  void createNoneJoinPredicates();
  string toString();
  // Return a string presentation
  virtual string toStringNoTableDot();

  string printNoneJoin();

  // Half Clone: Cloned table only has the same role, name and m_ptrNoneJoinPredicates
  virtual Opt_Table* cloneForReplica(string sTableName, string sTableAlias);
  
  // Half Clone: Cloned table only has the same role, name, mSelectionList and m_ptrNoneJoinPredicates
  virtual Opt_Table* cloneHalf();

   // CLone this table
  // Also in this clone table mark snowflake join for joins if they point to the same table as ptrNonSnowflakeJoin but go from different table
  virtual Opt_Table* clone(Opt_Join* ptrJoin);

 // CLone this table
  virtual Opt_Table* clone();
  
  // Check if this is a fact table
  bool isFactTable();
};

#endif // _OPT_TABLE_H_

