/*
 *  Opt_QueryGraph.h
 *  C-Store - Optimimzation
 *
 *  Created by Nga Tran on 10/28/05.
 * 
 *  Questions, ask Nga Tran at nga@brandeis.edu or Tien Hoang at hoangt@brandeis.edu
 *	
 *  This class implements a query_graph.
 *	Graph will be a list of tables. The links between tables are kept int the attributes of the tables
 *
 *  Opt_QueryGraph is used to store 2 kinds of queries:
 *   . Input query: a representation of any given SQL query. This Opt_QueryGraph can have many FACT tables.
 *     In this case, the name (identification) of this QueryGraph is not necessary
 *   . Snowflake query: a representation of a snowflake query, in which there is only ONE FACT table.
 *     In this case, the name (identification) of this QueryGraph is also the name of the FACT table
 */

#ifndef _OPT_QUEYGRAPH_H_
#define _OPT_QUEYGRAPH_H_

#include <list>
#include <map>
#include <string>

#include "Opt_Object.h"
#include "Opt_Column.h"
#include "Opt_Table.h"
#include "Opt_String.h"
#include "Opt_Plan.h"

#include "Opt_Agg.h"
#include "Opt_Agg_Avg.h"
#include "Opt_Agg_Count.h"
#include "Opt_Agg_Max.h"
#include "Opt_Agg_Min.h"
#include "Opt_Agg_Sum.h"
#include "Opt_Comparison_And.h"
#include "Opt_Comparison_Or.h"

#include "../parser/Queries/QSelect.h"
#include "../parser/Projection.h"
#include "../parser/EHolder.h"
#include "../parser/BExpression.h"
#include "../parser/Expressions/EAgg/EAggAvg.h"
#include "../parser/Expressions/EAgg/EAggCount.h"
#include "../parser/Expressions/EAgg/EAggMax.h"
#include "../parser/Expressions/EAgg/EAggMin.h"
#include "../parser/Expressions/EAgg/EAggSum.h"
#include "../parser/ListOrdering.h"
#include "../parser/ColOrder.h"

class Opt_Plan;

#define NOT_QUERY_NAME ""

using namespace std;

typedef struct ReplicaTable
{
  string   m_sName;
  int  m_iNumReplica;
}  ReplicaTable;

typedef struct CheckedReplicaTable
{
  string  m_sTableName;
  int    m_bIsChecked;
} CheckedReplicaTable;

class Opt_QueryGraph: Opt_Object
{
 protected:
  list<string> mFactTableList;
  list<Opt_Table*>  mTableList;	// List of tables apppearing in the query
  list<Opt_Column*> mGroupByList;
  //list<Opt_Column*> mOrderByList;
  list<Opt_Object*> mOrderByList;
  Opt_Comparison* m_ptrHavingPredicate;
  list<ReplicaTable*> mReplicaTableList;
  list<CheckedReplicaTable*> mCheckReplicaTableList;
  map<string, string> mManyToManyJoinList;

  // Name of this queryGraph. It is only applicable for snowflake query (query which has only one fact table)
  string m_sQueryName;
  int m_nCountStar; // 1 if this Opt_QueryGraph contains a count (*), 0 otherwise
      
 public:
  // Default constructor
  Opt_QueryGraph();
  
  // Constructor with query name
  Opt_QueryGraph(string sQueryname);

  // Debug constructor
  Opt_QueryGraph(list<Opt_Table*>  tableList, list<Opt_Column*> groupByList, 
		 list<Opt_Object*> orderByList, Opt_Comparison* ptrHavingPredicate);

  Opt_QueryGraph(Query* ptrQuery);
  
  // Default Destructor
  virtual ~Opt_QueryGraph();
    
  list<string> getFactTableList();
  
  // dealing with user's input for factables
  void addFactTable(string sFactTable);
  void setFactTable();
  void resetFactTable();

  // Get a list of joins between 2 plans
  list<Opt_Join*> getJoinsForPlans(Opt_Plan* ptrLeftPlan, Opt_Plan* ptrRightPlan);

  // Get information for final clauses
  void getSelectInfo(list<Opt_Column*> *selections, list<Opt_Agg*> *aggregates, 
		    list<Opt_Column*> *groupBys, list<Opt_Object*> *orderBys);

  // set many to many join
  void addManyToManyJoin(string sFromTable, string sToTable);
  bool isManyToManyJoin(string sFromTable, string sToTable);

  void setCountStar(int nCountStar);
  int isCountStar();

  bool isFactTable(string sTableName);

  list<Opt_Table*> getTableList();
  list<Opt_Column*> getGroupByList();
  list<Opt_Object*> getOrderByList();
  Opt_Comparison* getHavingPredicate();
  
  string getSQLQuery();
  string getSQLQueryWithHint();
  string toString();

  // Return a string presentation for this aggregate
  virtual string toStringNoTableDot();
  
  // Add a table
  void addTable(Opt_Table* ptrTable);

  // Add an order by object
  void addOrderByObject(Opt_Object* ptrObject);

  // Add an group by column
  void addGroupByColumn(Opt_Column* ptrCol);
  
  // Add a column into the selection list
  // return true (1) if the column is added
  // Query name is also returned along
  bool addSelectionColumn(Opt_Column* ptrColumn, string* ptrQueryName);

  // clone the join and add it to this graph if its two join tables belong to this graph
  // return true (1) if the join is added
  bool addJoin(Opt_Join* ptrJoin);

  // Get the queryGraph name
  string getQueryName();
  
  // Set the queryGraph name
  void setQueryName(string sQueryName);
  
  // Check if a table is a fact table in the graph
  bool isFactTableInTable(string sTableName);

   // Return table of a tablename
  Opt_Table* getTable(string sTableName);
  
  // Check if a table is in the graph
  bool isInGraph(string sTableName);

  // Set number of rows and cardinality for a table
  void setTableInfo(string sTableName, long lRowNum, long lCardinality);
  
  // Check if a table is in the graph. If yes, return the query name as well
  bool isInGraph(string sTableName, string* ptrQueryName);

  // Get all joins that have not marked snowflake join poiting to ptrTable
  list<Opt_Join*> getJoinsTo(Opt_Table* ptrTargetTable);

  // Get all joins that have not marked snowflake join poiting to ptrTable
  list<Opt_Join*> getJoinsTo(string sTableName);

  // Get all tables that link to a given table
  list<string> getStringTablesTo(string sTableName);

  // Get all tables that link to a given table
  list<Opt_Table*> getPointerTablesTo(string sTableName);

  // Get all tables that are linked from a given table
  list<string> getTablesFrom(string sTableName);

  // CLone this graph
  // Also in this clone grpah mark snowflake join for joins if they point to the same table as ptrNonSnowflakeJoin but go from different table
  // If ptrJoin == NULL, 100% clone
  Opt_QueryGraph* clone(Opt_Join* ptrJoin);

  // Get name of the replica table
  string addReplicaTableName(string tableName);

  // Return replica if possible
  // A replica is a equivalent query of this query but added all possible replica of tables
  Opt_QueryGraph* getReplica();

  // Get replica table names
  list<ReplicaTable*> getReplicaTables();

  static string getNonJoinPredicateString(Opt_Comparison* ptrComp);

  // reset traversed for all joins
  void resetTraversed();

 protected:
  string getSQLQueryWithoutSelectWord();
  void checkWherePredicate(BExpression* ptrWherePredicate, int* nOr, int* nTables, string* tables);
  Opt_Comparison* createOnePredicateList(BExpression* ptrWherePredicate);
  void createPredicateList(BExpression* ptrWherePredicate);
  void createBExpression(list<Opt_Column*> ptrWherePredicate);
  void initializeCheckReplica();
  static string getQueryHelper(Opt_Comparison* ptrNoneJoinPredicates);
  // Do replica for this query
  int doReplica();
  // Check replica for a table
  // (1) DO NOT DO THIS YET A.x = B.y and A.x = C.z --> Add EITHER (C1 and B.y = C1.z) OR (B1 and C.z = B1.y)
  // (2) A.x = B.y AND B.y = C.z --> Add C1 (replica of C) and A.x = C1.z
  int checkReplica(Opt_Table* ptrTable);
  // add a replica C1 of  table C and join C1 to source table A
  void replicate(Opt_Table* ptrSource, Opt_Column* ptrColSource, Opt_Table* ptrTableReplica, Opt_Column* ptrColReplica);
  int  isChecked(string tableName);
  
};

#endif // _OPT_QUEYGRAPH_H_

