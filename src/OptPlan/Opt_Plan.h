/*
 *  Opt_Plan.h
 *  C-Store - Optimimzation
 *
 *  Created by Nga Tran on 09/19/06
 * 
 *  Questions, ask Nga Tran at nga@brandeis.edu
 *	
 *  This class implements a plan
 *	. Plan is a binary tree (bushy tree)
 *	. Simplest plan: has only 1 table
 *      . Normal plan: has 2 children: left plan and right plan
 */

#ifndef _OPT_PLAN_H_
#define _OPT_PLAN_H_

#include "Opt_Table.h"
#include "Opt_QueryGraph.h"
#include "Opt_Comparison.h"
#include <cstdlib>

class Opt_QueryGraph;

using namespace std;

class Opt_Plan : Opt_Object
{
 protected:
  // Member declaration
  Opt_QueryGraph* m_ptrSourceQuery;
  Opt_Table*   m_ptrTable;   // only used for simple plan
  Opt_Plan*    m_ptrLeft;    // left child
  Opt_Plan*    m_ptrRight;  // Right child
  bool         m_bIsReferentialIntegrity;  // If set (1), join between left (foreign key) and right (primary key) is n-1
  // If m_ptrTable != NULL: simple plan, m_ptrLeft and m_ptrRight must be NULL
  // If m_ptrTable == NULL: normal plan, m_ptrLeft and m_ptrRight must be NOT NULL
  list<Opt_Join*> m_joins;   // Joins between left plan and right plan
  list<Opt_Column*> m_selections;
  long double m_ldCost;

 public:
  // Default constructor
  Opt_Plan();
  
  Opt_Plan(Opt_Table* ptrTable);
  Opt_Plan(Opt_Plan* ptrLeft, Opt_Plan* ptrRight);
  Opt_Plan(Opt_Plan* ptrLeft, Opt_Plan* ptrRight, bool bRefInt);
  
  // Default Destructor
  virtual ~Opt_Plan();

  // set Referential Integrity
  void setReferentialIntegrity();
  void unsetReferentialIntegrity();

  // Get cost related values
  long getCardinality();
  long double getSelectivity();  
  long double getCost();

  // make plan for a given query graph
  void makePlan(Opt_QueryGraph* ptrQueryGraph, Opt_QueryGraph* ptrOriginalQueryGraph);

  // Combine subplans
  static Opt_Plan* combineSubPlans(Opt_QueryGraph* ptrQueryGraph, list<Opt_Plan*> subPlans);  

  // get plan tree
  string getTree(string tab);

  // Print the plan to standard output
  void print(string tab);

  // Check if a given table is in the plan
  bool isInPlan(Opt_Table* ptrTable);

  // Check if a given table is in the plan
  bool isInPlan(string sTable);

  // Write SQL to a file
  string toFile(string sFileName);

  // get an SQL for a plan + a lot of views
  string getSQL();

  // Get only one SQL (noe views created)
  string getOneSQL();

  // Sort plans to descending cost
  static list<Opt_Plan*> sortPlans(list<Opt_Plan*> plans);

 protected:
  
  // Prepare before getting SQL
  void preSQL(list<Opt_Column*> *selections, list<Opt_Agg*> *aggregates, list<Opt_Column*> *groupBys, list<Opt_Object*> *orderBys);

  // Get all tables in this plan
  list<Opt_Table*> getPointerTablesInPlan();

  // Get all tables in this plan
  list<string> getTablesInPlan();
  
  // CLone this plan
  Opt_Plan* clone();
  
  // make plan for a given table in a query graph
  void  makePlan(string sTableName, Opt_QueryGraph* ptrQueryGraph, Opt_QueryGraph* ptrOriginalQueryGraph);

  // make plan staring from a given plan
  static Opt_Plan*  makePlanFromStartPlan(Opt_Plan* ptrAnchorPlan, list<Opt_Plan*> plans, Opt_QueryGraph* ptrQueryGraph);

  // reset member values
  static void reset(Opt_Plan* ptrPlan);

  // Check if at least one of the tables in ptrNewPlan is already in ptrPlan
  static bool belongTo(Opt_Plan* ptrNewPlan, Opt_Plan* ptrPlan);
  
  // Copy plans
  static list<Opt_Plan*> copyPlans(list<Opt_Plan*> plans);
  
  // return copies of all plans except ptrPlan
  static list<Opt_Plan*> minorCopy(Opt_Plan* ptrPlan, list<Opt_Plan*> plans);

  // Get all plans which are linked from ptrAnchorPlan
  static list<Opt_Plan*> getPlansFrom(Opt_Plan* ptrAnchorPlan, list<Opt_Plan*> plans, Opt_QueryGraph* ptrQueryGraph);

  // Check if ptrPlan is linked from ptrAnchorPlan
  static bool isLinkedFrom(Opt_Plan* ptrPlan, Opt_Plan* ptrAnchorPlan, Opt_QueryGraph* ptrQueryGraph);

  // Make join between 2 subplans
  void makeJoins(Opt_QueryGraph* ptrQueryGraph);

  // Get top left table in plan
  string getTopLeftTable();

  // Make selection list
  list<Opt_Column*> makeSelections(list<Opt_Column*> extraCols, list<Opt_Column*> parentJoinCols);

  // get a string to create view for this plan 
  string getViewSQL(unsigned int *ptrViewNum);

  // Get SQL for this plan
  string getPlanOneSQL(unsigned int *ptrViewNum, string sTab);

  // Add a column to selection column list
  void addSelectionColumn(Opt_Column* ptrCol);

};

#endif // _OPT_PLAN_H_
