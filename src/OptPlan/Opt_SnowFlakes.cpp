/*
 *  Opt_SnowFlakes.h
 *  C-Store - Optimimzation
 *
 *  Created by Nga Tran on 11/07/05.
 * 
 *  Questions, ask Nga Tran at nga@brandeis.edu or Tien Hoang at hoangt@brandeis.edu
 *	
 *  This class is responsible for creating an equivalent snowflakes query from a given query
 */

#include "Opt_SnowFlakes.h"

#define PREFIX "set timing on\nSET LINESIZE 200\nalter system flush buffer_cache;"
#define EXPLAIN "EXPLAIN PLAN FOR\n"
#define SUFFIX  "SELECT PLAN_TABLE_OUTPUT FROM TABLE(DBMS_XPLAN.DISPLAY());\n"


// Default constructor
Opt_SnowFlakes::Opt_SnowFlakes()
{
	m_ptrOriginalQuery = NULL;
}


// Constructor with a input query graph
Opt_SnowFlakes::Opt_SnowFlakes(Opt_QueryGraph* ptrQuery)
{
	m_ptrOriginalQuery = ptrQuery;
}

// Return a string presentation
string Opt_SnowFlakes::toStringNoTableDot(SnowflakeBasedQuery* ptrSnowflakeBasedQuery)
{
  	string sReturn = "";
	
	if (ptrSnowflakeBasedQuery->m_iNumFactTables <= 1)
	{
		// Only one query
		sReturn.append("ONLY ONE QUERY:\n\n");
		sReturn.append(m_ptrOriginalQuery->getSQLQuery());
		sReturn.append("\n");
		return sReturn;
	}
	
	
	// Snowflakes
	sReturn.append("SNOWFLAKES:\n");
	list<Opt_QueryGraph*>::iterator it;
	for (it =ptrSnowflakeBasedQuery-> mSnowFlakeQueryList.begin(); it != ptrSnowflakeBasedQuery->mSnowFlakeQueryList.end(); it++)
	{
		Opt_QueryGraph* ptr_QueryGraph = *it;
		sReturn.append("\nQuery: ");
		sReturn.append(ptr_QueryGraph->getQueryName());
		sReturn.append("\n\n");
		sReturn.append(ptr_QueryGraph->getSQLQuery());
		sReturn.append("\n");
	}
	sReturn.append("\n");
	
	// Join query
	sReturn.append("JOIN QUERY: \n\n");
	sReturn.append(ptrSnowflakeBasedQuery->m_ptrJoinQuery->getSQLQuery());
	sReturn.append("\n\n");
	
/*	
	// Joins
	sReturn.append("JOINS:\n");
	list<Opt_SnowFlakeJoin*>::iterator joinIt;
	for (joinIt = mSnowFlakeJoinList.begin(); joinIt != mSnowFlakeJoinList.end(); joinIt++)
	{
		sReturn.append((*joinIt)->toStringNoTableDot("\t"));
		sReturn.append("\n");
	}
	sReturn.append("\n");
	
	// Select
	sReturn.append("SELECT COLUMNS: {column, snowflake query}\n");
	list<Opt_Column*>::iterator colIt;
	list<string>::iterator snowflakeIt;
	colIt = mSnowFlakeSelectList.begin();
	snowflakeIt = mSnowFlakeSelectQueryList.begin();
	while ( (colIt != mSnowFlakeSelectList.end()) && (snowflakeIt != mSnowFlakeSelectQueryList.end()) )
	{
		sReturn.append("\t");
		sReturn.append((*colIt)->toStringNoTableDot());
		sReturn.append(", ");
		sReturn.append((*snowflakeIt));
		colIt++;
		snowflakeIt++;
		sReturn.append("\n");
	}
	sReturn.append("\n");
	
	// Aggregates
	sReturn.append("AGGREGATES: {aggregate, snowflake query} \n");
	list<Opt_Agg*>::iterator aggIt;
	aggIt = mSnowFlakeAggregateList.begin();
	snowflakeIt = mSnowFlakeAggregateQueryList.begin();
	while ( (aggIt != mSnowFlakeAggregateList.end()) && (snowflakeIt != mSnowFlakeAggregateQueryList.end()) )
	{
		sReturn.append("\t");
		sReturn.append((*aggIt)->toStringNoTableDot());
		sReturn.append(", ");
		sReturn.append((*snowflakeIt));
		aggIt++;
		snowflakeIt++;
		sReturn.append("\n");
	}
	sReturn.append("\n");
		
	// Group by
	sReturn.append("GROUP BY: {column, snowflake query} \n");
	colIt = mSnowFlakeGroupByColumnList.begin();
	snowflakeIt = mSnowFlakeGroupByQueryList.begin();
	while ( (colIt != mSnowFlakeGroupByColumnList.end()) && (snowflakeIt != mSnowFlakeGroupByQueryList.end()) )
	{
		sReturn.append("\t");
		sReturn.append((*colIt)->toStringNoTableDot());
		sReturn.append(", ");
		sReturn.append((*snowflakeIt));
		colIt++;
		snowflakeIt++;
		sReturn.append("\n");
	}
	sReturn.append("\n");
	
	// Order by
	sReturn.append("ORDER BY: {object, snowflake query} \n");
	list<Opt_Object*>::iterator objectIt;
	objectIt = mSnowFlakeOrderByObjectList.begin();
	snowflakeIt = mSnowFlakeOrderByQueryList.begin();
	while ( (objectIt != mSnowFlakeOrderByObjectList.end()) && (snowflakeIt != mSnowFlakeOrderByQueryList.end()) )
	{
		sReturn.append("\t");
		sReturn.append((*objectIt)->toStringNoTableDot());
		sReturn.append(", ");
		sReturn.append((*snowflakeIt));
		objectIt++;
		snowflakeIt++;
		sReturn.append("\n");
	}
	sReturn.append("\n");
*/	
	// Intertesting orders
	sReturn.append("INTERESTING ORDERS: \n");
	list<Opt_SnowflakeInterestingOrders*>::iterator ioIt;
	for (ioIt = ptrSnowflakeBasedQuery->mInterestingOrderList.begin(); ioIt != ptrSnowflakeBasedQuery->mInterestingOrderList.end(); ioIt++)
	{
		sReturn.append((*ioIt)->toStringNoTableDot("\t"));
		sReturn.append("\n");
	}
	
	return sReturn;
}

// String of snowflake queries
string Opt_SnowFlakes::toString(SnowflakeBasedQuery* ptrSnowflakeBasedQuery)
{
	string sReturn = "";
	
	if (ptrSnowflakeBasedQuery->m_iNumFactTables <= 1)
	{
		// Only one query
		sReturn.append("ONLY ONE QUERY:\n\n");
		sReturn.append(m_ptrOriginalQuery->getSQLQuery());
		sReturn.append("\n");
		return sReturn;
	}
	
	
	// Snowflakes
	sReturn.append("SNOWFLAKES:\n");
	list<Opt_QueryGraph*>::iterator it;
	for (it =ptrSnowflakeBasedQuery-> mSnowFlakeQueryList.begin(); it != ptrSnowflakeBasedQuery->mSnowFlakeQueryList.end(); it++)
	{
		Opt_QueryGraph* ptr_QueryGraph = *it;
		sReturn.append("\nQuery: ");
		sReturn.append(ptr_QueryGraph->getQueryName());
		sReturn.append("\n\n");
		sReturn.append(ptr_QueryGraph->getSQLQuery());
		sReturn.append("\n");
	}
	sReturn.append("\n");
	
	// Join query
	sReturn.append("JOIN QUERY: \n\n");
	sReturn.append(ptrSnowflakeBasedQuery->m_ptrJoinQuery->getSQLQuery());
	sReturn.append("\n\n");
	
/*	
	// Joins
	sReturn.append("JOINS:\n");
	list<Opt_SnowFlakeJoin*>::iterator joinIt;
	for (joinIt = mSnowFlakeJoinList.begin(); joinIt != mSnowFlakeJoinList.end(); joinIt++)
	{
		sReturn.append((*joinIt)->toString("\t"));
		sReturn.append("\n");
	}
	sReturn.append("\n");
	
	// Select
	sReturn.append("SELECT COLUMNS: {column, snowflake query}\n");
	list<Opt_Column*>::iterator colIt;
	list<string>::iterator snowflakeIt;
	colIt = mSnowFlakeSelectList.begin();
	snowflakeIt = mSnowFlakeSelectQueryList.begin();
	while ( (colIt != mSnowFlakeSelectList.end()) && (snowflakeIt != mSnowFlakeSelectQueryList.end()) )
	{
		sReturn.append("\t");
		sReturn.append((*colIt)->toString());
		sReturn.append(", ");
		sReturn.append((*snowflakeIt));
		colIt++;
		snowflakeIt++;
		sReturn.append("\n");
	}
	sReturn.append("\n");
	
	// Aggregates
	sReturn.append("AGGREGATES: {aggregate, snowflake query} \n");
	list<Opt_Agg*>::iterator aggIt;
	aggIt = mSnowFlakeAggregateList.begin();
	snowflakeIt = mSnowFlakeAggregateQueryList.begin();
	while ( (aggIt != mSnowFlakeAggregateList.end()) && (snowflakeIt != mSnowFlakeAggregateQueryList.end()) )
	{
		sReturn.append("\t");
		sReturn.append((*aggIt)->toString());
		sReturn.append(", ");
		sReturn.append((*snowflakeIt));
		aggIt++;
		snowflakeIt++;
		sReturn.append("\n");
	}
	sReturn.append("\n");
		
	// Group by
	sReturn.append("GROUP BY: {column, snowflake query} \n");
	colIt = mSnowFlakeGroupByColumnList.begin();
	snowflakeIt = mSnowFlakeGroupByQueryList.begin();
	while ( (colIt != mSnowFlakeGroupByColumnList.end()) && (snowflakeIt != mSnowFlakeGroupByQueryList.end()) )
	{
		sReturn.append("\t");
		sReturn.append((*colIt)->toString());
		sReturn.append(", ");
		sReturn.append((*snowflakeIt));
		colIt++;
		snowflakeIt++;
		sReturn.append("\n");
	}
	sReturn.append("\n");
	
	// Order by
	sReturn.append("ORDER BY: {object, snowflake query} \n");
	list<Opt_Object*>::iterator objectIt;
	objectIt = mSnowFlakeOrderByObjectList.begin();
	snowflakeIt = mSnowFlakeOrderByQueryList.begin();
	while ( (objectIt != mSnowFlakeOrderByObjectList.end()) && (snowflakeIt != mSnowFlakeOrderByQueryList.end()) )
	{
		sReturn.append("\t");
		sReturn.append((*objectIt)->toString());
		sReturn.append(", ");
		sReturn.append((*snowflakeIt));
		objectIt++;
		snowflakeIt++;
		sReturn.append("\n");
	}
	sReturn.append("\n");
*/	
	// Intertesting orders
	sReturn.append("INTERESTING ORDERS: \n");
	list<Opt_SnowflakeInterestingOrders*>::iterator ioIt;
	for (ioIt = ptrSnowflakeBasedQuery->mInterestingOrderList.begin(); ioIt != ptrSnowflakeBasedQuery->mInterestingOrderList.end(); ioIt++)
	{
		sReturn.append((*ioIt)->toString("\t"));
		sReturn.append("\n");
	}
	
	return sReturn;
}

// snowflake query
string Opt_SnowFlakes::toSnowflakeSQLsOracle(string sQueryName)
{
  int i = 1;
  string sReturn = "";

  if (m_ptrOriginalQuery->getFactTableList().size() <= 1)
	{
	  // Only one fact table
	  // cout << "Output query is exactly the same input one" << endl << endl;

	  string sFileName = sQueryName + ".sql";
	  sReturn.append(toSnowflakeSQLOracle(m_ptrOriginalQuery->getSQLQuery(), sFileName));
	}

	// get all snowflake-based queries
	list<SnowflakeBasedQuery*>::iterator queryIt;
	for (queryIt = mAllEquivalentSnowflakeBasedQueries.begin(); queryIt != mAllEquivalentSnowflakeBasedQueries.end(); queryIt++)
	{
	  
	  char sQueryNum[3];
	  sprintf(sQueryNum, "%d", i++);

	  string sFileName = sQueryName + "_" + string(sQueryNum) + ".sql";
	  //cout << "FILE NAME: " << sFileName << endl;

	  sReturn.append("------------------------\n\n");
	  sReturn.append(toSnowflakeSQLOracle(*queryIt, sFileName));
	}

	return sReturn;
  
}

// snowflake query
string Opt_SnowFlakes::toSnowflakeSQLs(string sQueryName)
{
  int i = 1;
  string sReturn = "";

  if (m_ptrOriginalQuery->getFactTableList().size() <= 1)
	{
	  // Only one fact table
	  cout << "Output query is exactly the same input one" << endl << endl;
	  return m_ptrOriginalQuery->getSQLQuery();	  
	}

	// get all snowflake-based queries
	list<SnowflakeBasedQuery*>::iterator queryIt;
	for (queryIt = mAllEquivalentSnowflakeBasedQueries.begin(); queryIt != mAllEquivalentSnowflakeBasedQueries.end(); queryIt++)
	{
	  
	  char sQueryNum[3];
	  sprintf(sQueryNum, "%d", i++);

	  string sFileName = sQueryName + "_" + string(sQueryNum) + ".sql";
	  sReturn.append("------------------------\n\n");
	  sReturn.append(toSnowflakeSQL(*queryIt, sFileName));
	}

	return sReturn;
  
}

// snowflake query
string Opt_SnowFlakes::toSnowflakeSQLs()
{
	string sReturn = "";

	if (m_ptrOriginalQuery->getFactTableList().size() <= 1)
	{
	  // Only one fact table
	  return m_ptrOriginalQuery->getSQLQuery();
	}

	// get all snowflake-based queries
	list<SnowflakeBasedQuery*>::iterator queryIt;
	for (queryIt = mAllEquivalentSnowflakeBasedQueries.begin(); queryIt != mAllEquivalentSnowflakeBasedQueries.end(); queryIt++)
	{
	  // get SQL for this 
	  sReturn.append("------------------------\n\n");
	  sReturn.append(toSnowflakeSQL(*queryIt));
	}

	return sReturn;
}


// writing snowflake query into a file
string Opt_SnowFlakes::toSnowflakeSQL(SnowflakeBasedQuery* ptrSnowflakeBasedQuery, string sFileName)
{

   string sQuery = toSnowflakeSQLOracle(ptrSnowflakeBasedQuery);
  try
    {
      ofstream fout(sFileName.c_str(), ios::out);     
      fout << sQuery << endl;
      fout.close();
    } 
  catch (std::exception&)
    {
      cout << "Cannot open: " << sFileName << " for storing coressponding query" << endl;
    }
 
  return sQuery;
}

// writing snowflake query into a file
string Opt_SnowFlakes::toSnowflakeSQLOracle(string sQuery, string sFileName)
{

  try
    {
      ofstream fout(sFileName.c_str(), ios::out);     
      fout << PREFIX << endl << endl;
      fout << sQuery << endl << endl;
      fout << EXPLAIN;
      fout << sQuery << endl << endl;
      fout << SUFFIX << endl;
      fout.close();
    } 
  catch (std::exception&)
    {
      cout << "Cannot open: " << sFileName << " for storing coressponding query" << endl;
    }
 
  return sQuery;
}


// writing snowflake query into a file
string Opt_SnowFlakes::toSnowflakeSQLOracle(SnowflakeBasedQuery* ptrSnowflakeBasedQuery, string sFileName)
{

   string sQuery = toSnowflakeSQLOracle(ptrSnowflakeBasedQuery);
  try
    {
      ofstream fout(sFileName.c_str(), ios::out);     
      fout << PREFIX << endl;
      fout << sQuery << endl;
      fout << SUFFIX << endl;
      fout.close();
    } 
  catch (std::exception&)
    {
      cout << "Cannot open: " << sFileName << " for storing coressponding query" << endl;
    }
 
  return sQuery;
}

// snowflake query
string Opt_SnowFlakes::toSnowflakeSQL(SnowflakeBasedQuery* ptrSnowflakeBasedQuery)
{
	string sReturn = "";
	
	if (ptrSnowflakeBasedQuery->m_iNumFactTables <= 1)
	{
		// Only one query
		sReturn.append(m_ptrOriginalQuery->getSQLQuery());
		sReturn.append("\n");
		return sReturn;
	}
	
	
	// Snowflakes
	list<Opt_QueryGraph*>::iterator it;
	for (it = ptrSnowflakeBasedQuery->mSnowFlakeQueryList.begin(); it != ptrSnowflakeBasedQuery->mSnowFlakeQueryList.end(); it++)
	{
		Opt_QueryGraph* ptr_QueryGraph = *it;
		sReturn.append("\nCREATE OR REPLACE VIEW ");
		sReturn.append(ptr_QueryGraph->getQueryName());
		sReturn.append(" AS \n");
		sReturn.append(ptr_QueryGraph->getSQLQuery());
		sReturn.append("\n");
	}
	sReturn.append("\n");
	
	// Join query
	sReturn.append(ptrSnowflakeBasedQuery->m_ptrJoinQuery->getSQLQueryWithHint());
	sReturn.append("\n\n");

	return sReturn;
}


// snowflake query
string Opt_SnowFlakes::toSnowflakeSQLOracle(SnowflakeBasedQuery* ptrSnowflakeBasedQuery)
{
	string sReturn = "";
	
	if (ptrSnowflakeBasedQuery->m_iNumFactTables <= 1)
	{
		// Only one query
		sReturn.append(m_ptrOriginalQuery->getSQLQuery());
		sReturn.append("\n");
		return sReturn;
	}
	
	
	// Snowflakes
	list<Opt_QueryGraph*>::iterator it;
	for (it = ptrSnowflakeBasedQuery->mSnowFlakeQueryList.begin(); it != ptrSnowflakeBasedQuery->mSnowFlakeQueryList.end(); it++)
	{
		Opt_QueryGraph* ptr_QueryGraph = *it;
		sReturn.append("\nCREATE OR REPLACE VIEW ");
		sReturn.append(ptr_QueryGraph->getQueryName());
		sReturn.append(" AS \n");
		sReturn.append(ptr_QueryGraph->getSQLQuery());
		sReturn.append("\n");
	}
	sReturn.append("\n");
	
	// Join query
	sReturn.append(ptrSnowflakeBasedQuery->m_ptrJoinQuery->getSQLQueryWithHint());
	sReturn.append("\n\n");

	// Join query
	sReturn.append(EXPLAIN);
	sReturn.append(ptrSnowflakeBasedQuery->m_ptrJoinQuery->getSQLQueryWithHint());
	sReturn.append("\n\n");

	return sReturn;

}

// Get/set
Opt_QueryGraph*	Opt_SnowFlakes::getOriginalQuery()
{
	return m_ptrOriginalQuery;
}

list<SnowflakeBasedQuery*> Opt_SnowFlakes::getSnowFlakeBasedQueryList()
{
	return mAllEquivalentSnowflakeBasedQueries;
}

// Get the best predicted plan of all decompositions
Opt_Plan* Opt_SnowFlakes::getPlan()
{
  // Produce snowfkale based queries first
  produceSnowFlakeQueries();

  //cout << "DONE  produceSnowFlakeQueries \n";
  
  // Get plan of the original query
  Opt_Plan* plan = new Opt_Plan();
  plan->makePlan(m_ptrOriginalQuery,m_ptrOriginalQuery);
  Opt_Plan* bestPlan = plan;

  //cout << "DONE makePlan for original query\n";
  //cout << "Number of snowflake baes queries: " << mAllEquivalentSnowflakeBasedQueries.size() << endl;

  // Get plans of all decomposition (all snowfflake based queries)
  Opt_Plan* plans[mAllEquivalentSnowflakeBasedQueries.size()];
  list<SnowflakeBasedQuery*>::iterator it = mAllEquivalentSnowflakeBasedQueries.begin();
  for (unsigned int i = 0; i < mAllEquivalentSnowflakeBasedQueries.size(); i++)
    {
      plans[i] = makeSnowFlakeBasedPlan(*it);
      //cout << "DONE makeSnowFlakeBasedPlan " << i << endl;

      //cout << endl << ".................." << endl;
      //plans[i]->print("");
      //cout << endl << "COST: " << plans[i]->getCost() << endl;
      long double bc = bestPlan->getCost();
      long double c = plans[i]->getCost();
      if (bc > c)
	bestPlan = plans[i];
      it++;
    }

  //cout << endl << "BEST " << endl; 
  //bestPlan->print("");

  return bestPlan;
}

// Get all best predicted plans of all decompositions
list<Opt_Plan*> Opt_SnowFlakes::getPlans()
{
  list<Opt_Plan*> plans;

    // Produce snowfkale based queries first
  produceSnowFlakeQueries();

  //cout << "DONE  produceSnowFlakeQueries \n";
  
  // Get plan of the original query
  Opt_Plan* ptrPlan = new Opt_Plan();
  ptrPlan->makePlan(m_ptrOriginalQuery,m_ptrOriginalQuery);
  plans.push_back(ptrPlan);

  //cout << "DONE makePlan for original query\n";
  //cout << "Number of snowflake based queries: " << mAllEquivalentSnowflakeBasedQueries.size() << endl;

  // Get plans of all decomposition (all snowfflake based queries)
  list<SnowflakeBasedQuery*>::iterator it = mAllEquivalentSnowflakeBasedQueries.begin();
  for (unsigned int i = 0; i < mAllEquivalentSnowflakeBasedQueries.size(); i++)
    {
      ptrPlan = makeSnowFlakeBasedPlan(*it);
      plans.push_back(ptrPlan);
      it++;
    }

  return plans;
}


// Produce all possible snowflake-based queries
void Opt_SnowFlakes::produceSnowFlakeQueries()
{
  if (m_ptrOriginalQuery->getFactTableList().size() <= 1)
  {
     // Only one fact table
    return;
  }

  //cout << "NUM FACT TABLES: " << m_ptrOriginalQuery->getFactTableList().size() << endl;

  // produce all equivalent  queries first
  produceAllEquivalentQueries(m_ptrOriginalQuery); 
  //cout << "NUM equivalent queries: " << mAllEquivalentQueries.size() << endl;

  // produce snowflake-based queries for the above queries
  list<Opt_QueryGraph*>::iterator queryIt;
  for (queryIt = mAllEquivalentQueries.begin(); queryIt != mAllEquivalentQueries.end(); queryIt++)
  {
    // Produce a snowflake-based query
    //cout << "Query Pointer: " << *queryIt << endl;
    SnowflakeBasedQuery* ptrSnowFlakeBasedQuery = produceSnowFlakeBasedQuery(*queryIt);
    if (ptrSnowFlakeBasedQuery != NULL)
    {
	mAllEquivalentSnowflakeBasedQueries.push_back(ptrSnowFlakeBasedQuery);
    }
  }
  //cout << "NUM equivalent snowflake based queries BEFORE duplicate removed: " << mAllEquivalentSnowflakeBasedQueries.size() << endl;
  

  /* Remove all the SnowflakeBasedQuery objects that are essentially the same*/
  
  removeDuplicateSnowflakeBasedQuery(); /* The function is at the end of this file */

  //cout << "NUM equivalent snowflake based queries AFTER duplicate removed: " << mAllEquivalentSnowflakeBasedQueries.size() << endl << endl;
}


// Produce a snowflake-based queriy
SnowflakeBasedQuery*  Opt_SnowFlakes::produceSnowFlakeBasedQuery(Opt_QueryGraph* ptrSourceQuery)
{
  //cout << "NGA Start produceSnowFlakeBasedQuery\n";

  // Reset traversed
  ptrSourceQuery->resetTraversed();

        SnowflakeBasedQuery* ptrSnowflakeBasedQuery = new SnowflakeBasedQuery;
	ptrSnowflakeBasedQuery->m_iNumFactTables = 0;

  	// Get tables in the query
	list<Opt_Table*> tableList = ptrSourceQuery->getTableList();
	
	// Go through all tables to get fact tables
	list<Opt_Table*>::iterator tableIt;
	for (tableIt = tableList.begin(); tableIt != tableList.end(); tableIt++)
	{
		Opt_Table* ptrTable = *tableIt;
		if (ptrTable->getRole() == TABLE_ROLE_FACT)
		{
			ptrSnowflakeBasedQuery->m_iNumFactTables++;
			// Create a query graph for each FACT table
			Opt_QueryGraph* ptrQueryGraph = NULL;
			ptrQueryGraph = new Opt_QueryGraph("SnowFlake_" + ptrTable->getName());

			// Set fact table for this query
			ptrQueryGraph->addFactTable(ptrTable->getName());

			// Adding fact and its dimesion tables recursively
			addTables(ptrSourceQuery, &ptrQueryGraph, ptrTable->getName(), ptrSnowflakeBasedQuery);
			
			ptrSnowflakeBasedQuery->mSnowFlakeQueryList.push_back(ptrQueryGraph);
		}
	}
	
	// Go trhough alll tables again. If a table is not in any snowflake yet, find one and add it in
	// If the table cannot be added in any snowflake (no link to anyone or may link to more than one snowflake),
	// this query cannot transform to snowflake-based query under those fact tables --> return NULL

	for (tableIt = tableList.begin(); tableIt != tableList.end(); tableIt++)
	{
	  Opt_Table* ptrTable = *tableIt;
	  string sSnowflakeName;
	  if (!exists(ptrTable->getName(), ptrSnowflakeBasedQuery, &sSnowflakeName))
	  { 
	       if (!add(ptrTable, ptrSourceQuery, &ptrSnowflakeBasedQuery, &sSnowflakeName))
	       {
	          delete ptrSnowflakeBasedQuery;
	          return NULL;
	       }	   
	  }
	}
	
	if (ptrSnowflakeBasedQuery->m_iNumFactTables <= 1)
	{
		// Only ony query
		ptrSnowflakeBasedQuery->mSnowFlakeQueryList.clear();
		return ptrSnowflakeBasedQuery;
	}

	// Add columns which will be needed for doing joins later
	addSelectionColumnsForLaterJoins(ptrSourceQuery, &ptrSnowflakeBasedQuery);

	// Add columns which will be needed for doing aggregation
	addSelectionColumnsForLaterAggregates(ptrSourceQuery, &ptrSnowflakeBasedQuery);

	// Restore the columns in the select clause
	getSnowFlakeSelect(ptrSourceQuery, &ptrSnowflakeBasedQuery);

	// Restore aggregates
	getSnowFlakeGroupBy(ptrSourceQuery, &ptrSnowflakeBasedQuery);

	// Restore order
	getSnowFlakeOrder(ptrSourceQuery, &ptrSnowflakeBasedQuery);

	// Find interesting oders
	getInterestingOrder(&ptrSnowflakeBasedQuery);
		
	// Create join query for all snowflakes
	getJoinQuery(&ptrSnowflakeBasedQuery);

	return ptrSnowflakeBasedQuery;
}


// produce all equivalent snowflake queries
void Opt_SnowFlakes::produceAllEquivalentQueries(Opt_QueryGraph* ptrQuery)
{
     bool bNoMorePosibility = 1;

     // Get all tables of the query
     list<Opt_Table*> tableList = ptrQuery->getTableList();   

     // Go through alll tables
     list<Opt_Table*>::iterator tableIt;
     for (tableIt = tableList.begin(); tableIt != tableList.end(); tableIt++)
     {
       // Get joins to this table
       list<Opt_Join*> joinList = ptrQuery->getJoinsTo(*tableIt);

       if (joinList.size() > 1)
       {

	 bNoMorePosibility = 0;
 	list<Opt_QueryGraph*> EquiQueryList;

	 // Produce all equivalent queries from this join
	 list<Opt_Join*>::iterator joinIt;
	 for (joinIt = joinList.begin(); joinIt != joinList.end(); joinIt++)
	 {
		Opt_Join* ptrJoin = *joinIt;
		Opt_QueryGraph* ptrQry = ptrQuery->clone(ptrJoin);
		//cout << "NGA Equivalent query: " << ptrQry << endl;
		EquiQueryList.push_back(ptrQry);
		//cout << "EQ query: ----\n";
		//cout << ptrQry->getSQLQuery() << endl;

	 }
	 
	 // Recursive find other equivalent queries
	 // Produce all equivalent queries from this join
	 list<Opt_QueryGraph*>::iterator queryIt;
	 for (queryIt = EquiQueryList.begin(); queryIt != EquiQueryList.end(); queryIt++)
	 {
		Opt_QueryGraph* ptrEquiQuery = *queryIt;
		produceAllEquivalentQueries(ptrEquiQuery);
	 }
	 
	 break;
       } 
     }

     if (bNoMorePosibility)
     {
       mAllEquivalentQueries.push_back(ptrQuery);
     }
}

// Add a table and its possible related things to a querygraph
void Opt_SnowFlakes::addTables(Opt_QueryGraph* ptrSourceQuery, Opt_QueryGraph** ptrQueryGraph, string sTableName, SnowflakeBasedQuery* ptrSnowflakeBasedQuery)
{
  
 	Opt_Table* ptrClonedTable = NULL;
	
	// Get tables in the source query
	list<Opt_Table*> tableList = ptrSourceQuery->getTableList();
	
	// Search sTableName
	list<Opt_Table*>::iterator tableIt;
	for (tableIt = tableList.begin(); tableIt != tableList.end(); tableIt++)
	{
		Opt_Table* ptrTable = *tableIt;
		if ( (ptrTable->getName()).compare(sTableName) == 0)
		{
			// Found, Half clone
			ptrClonedTable = ptrTable->cloneHalf();
			
			ptrTable->toString();
			
			// Go through all joins of this table
			list<Opt_Join*> joinList = ptrTable->getJoinList();
			list<Opt_Join*>::iterator joinIt;
			for (joinIt = joinList.begin(); joinIt != joinList.end(); joinIt++)
			{
			        Opt_Join* ptrJoin = *joinIt;

				//cout << "\nNGA JOIN: " << ptrJoin->getFromColumn()->toString() <<", " <<  ptrJoin->getToColumn()->toString() << endl;
				if (ptrJoin->isTraversed())
				  {
				  //cout << "NGA: TRAVERSED\n";
				    continue;
				  }

				// Only conitnue if the join is not a snowflake join
				if ( ptrJoin->isSnowflakesJoin())  
				  {
				    //cout << "NGA JOIN IS  A SF JOIN\n"; 
				    continue;
				  }

				bool isDuplicateToTable = 0;
				
				// Get the table on the other side of the join
				string sToTable =  (ptrJoin->getToColumn())->getTableName();
				
				// Check if the table on other side of the join is a fact table
				if (ptrSourceQuery->isFactTableInTable(sToTable))
				{
					// skip it: this table and its descendants should be in different snowflake
				  //cout << "NGA TO COLUMN OF JOIN IS A FACT TABLE\n"; 
					continue;
				}
				
				// Check if the table on other side of the join exists in THIS snowflake query 
				if ((*ptrQueryGraph)->isInGraph(sToTable))
				{
				  //cout << "NGA DUPLICATE JOIN\n";
					isDuplicateToTable = 1;
				}
				
				// Check if the table on other side of the join exists in other snowflake queries 
				string sSnowflakeName;
				if (exists(sToTable, ptrSnowflakeBasedQuery, &sSnowflakeName))
				{
					// skip it: this table and its descendants was already in different snowflake
				  //cout << "NGA JOIN TO COLUMN IS IN THE OTHER SF\n";
					continue;
				}
				
				// Not in any snowflake yet, add it to this snowflake:
				// First, keep the join
				Opt_Join* ptrClonedJoin = ptrJoin->clone();
				ptrClonedTable->addJoin(ptrClonedJoin);
				// Second, add the table  in the other side and its descendants to this snowflake as well
				if (!isDuplicateToTable)
				{
					addTables(ptrSourceQuery, ptrQueryGraph, sToTable, ptrSnowflakeBasedQuery);
				}
				// Third, mark this join has been put in an snowflake query
				ptrJoin->setTraversed();
			}
			
			// Add the table to the query graph
			(*ptrQueryGraph)->addTable(ptrClonedTable);
		}
	}
  
}


// Add a table and its linked tables (descendents) to a snowflake in the snowflake-based query
// Return 0 if it cannot be added to any snowflake or it cann be added to more than one snowflake
bool Opt_SnowFlakes::add(Opt_Table* ptrTable, Opt_QueryGraph* ptrSourceQuery, SnowflakeBasedQuery** ptrSnowflakeBasedQuery, string* ptrSnowFlakeName)
{
  string sSnowflake = "";
  int numCompare = -1;

  // Go through all joins of this table
  list<Opt_Join*> joinList = ptrTable->getJoinList();

  if (joinList.size() == 0)
  {  
    // this table is not linked to any table
    // It mmay be added into the snowflake of its parent
    *ptrSnowFlakeName = "";
    return 1;      
  }

  list<Opt_Join*>::iterator joinIt;
  for (joinIt = joinList.begin(); joinIt != joinList.end(); joinIt++)
  {
    numCompare++;
    Opt_Join* ptrJoin = *joinIt;
    // Get the table on the other side of the join
    string sToTable =  (ptrJoin->getToColumn())->getTableName();
    // Check if this table is in a snowflake
    string sCurrentSnowflake = "";
    if (exists(sToTable, *ptrSnowflakeBasedQuery, &sCurrentSnowflake))
    {
      if (numCompare == 0)
      {
	sSnowflake = sCurrentSnowflake;
      } 
      else
      {
	if (sSnowflake.compare(sCurrentSnowflake) !=0)
	{
	  // Not in same snowflake
	  return 0;
	}
      } 
    }
    else
    {
      // Does not exist, add this table

      // Get tables in the source query
      list<Opt_Table*> tableList = ptrSourceQuery->getTableList();
	
      // Search sToTable
      bool bFound = 0;
      list<Opt_Table*>::iterator tableIt;
      for (tableIt = tableList.begin(); tableIt != tableList.end(); tableIt++)
      {
	Opt_Table* ptrSearchTable = *tableIt;
	if ( (ptrSearchTable->getName()).compare(sToTable) == 0)
	{
	  bFound = 1;

	  if (!add(ptrSearchTable, ptrSourceQuery, ptrSnowflakeBasedQuery, &sCurrentSnowflake))
	  {
	    // Descendent cannot be added
	    return 0;
	  }
	  else
	  {
	    if ( ((*ptrSnowFlakeName).compare("") != 0) && (sSnowflake.compare("") == 0 ) )//(numCompare == 0)
	    {
	      sSnowflake = sCurrentSnowflake;
	    } 
	    else
	    {
	      if ( ((*ptrSnowFlakeName).compare("") != 0) && (sSnowflake.compare("") != 0 ) && (sSnowflake.compare(sCurrentSnowflake) != 0) )
	      {
		 // Not in same snowflake
		return 0;
	      }
	    }
	  }
	}
      }

      if (!bFound)
      {
	// Cannot be added to any snowflake
	return 0;
      }

    }
  }

  // Add this table into the sSnowflake 
  if (sSnowflake.compare("") != 0 )
  {

    list<Opt_QueryGraph*>::iterator it;
    for (it = (*ptrSnowflakeBasedQuery)->mSnowFlakeQueryList.begin(); it != (*ptrSnowflakeBasedQuery)->mSnowFlakeQueryList.end(); it++)
	{
		Opt_QueryGraph* ptrQueryGraph = *it;
		if ( ((ptrQueryGraph->getQueryName()).compare(sSnowflake) == 0) &&  (!ptrQueryGraph->isInGraph(ptrTable->getName())) )
		  {
		    addTables(ptrSourceQuery, &ptrQueryGraph, ptrTable->getName(), *ptrSnowflakeBasedQuery);
		    return 1;
		  }
	}

    //    Opt_Table* ptrClonedTable = ptrTable->clone();
    //    return addTable(ptrClonedTable, sSnowflake, ptrSnowflakeBasedQuery, ptrSourceQuery);

  }
 
  return 0;   // cannot be inserted in any snowflake
}


// Check if a table exists in mSnowFlakeQueryList
// The found snowflake name will be returned
bool Opt_SnowFlakes::exists(string sTableName,  SnowflakeBasedQuery* ptrSnowflakeBasedQuery, string* ptrSnowFlakeName)
{
	list<Opt_QueryGraph*>::iterator it;
	for (it = ptrSnowflakeBasedQuery->mSnowFlakeQueryList.begin(); it != ptrSnowflakeBasedQuery->mSnowFlakeQueryList.end(); it++)
	{
		Opt_QueryGraph* ptrQueryGraph = *it;
		if (ptrQueryGraph->isInGraph(sTableName)) 
		{
		  *ptrSnowFlakeName = ptrQueryGraph->getQueryName();
		  return 1;
		}
	}
	
	// Not found
	return 0;
}


// Get snowflake query name of a column
string Opt_SnowFlakes::getQueryName(Opt_Column* ptrColumn,  SnowflakeBasedQuery* ptrSnowflakeBasedQuery)
{
	return getQueryName(ptrColumn->getTableName(),  ptrSnowflakeBasedQuery);
}



// Get snowflake query name of a table
string Opt_SnowFlakes::getQueryName(string sTableName, SnowflakeBasedQuery* ptrSnowflakeBasedQuery)
{
	string sQuery = NOT_QUERY_NAME;
	
	// Go through all snowflakes
	list<Opt_QueryGraph*>::iterator graphIt;
	for (graphIt = ptrSnowflakeBasedQuery->mSnowFlakeQueryList.begin(); graphIt != ptrSnowflakeBasedQuery->mSnowFlakeQueryList.end(); graphIt++)
	{
		Opt_QueryGraph* ptrGraph = *graphIt;
		if (ptrGraph->isInGraph(sTableName, &sQuery))
		{
			// Found
			return sQuery;
		}
	}
	
	return sQuery;
}

// Add columns into selection list of snowflake queries
// This columns will be needed for joining snowflake queries together
void Opt_SnowFlakes::addSelectionColumnsForLaterJoins(Opt_QueryGraph* ptrSourceQuery, SnowflakeBasedQuery** ptrSnowflakeBasedQuery)
{
  //  cout << "START addSelectionColumnsForLaterJoins\n";
  	// Go through all relationships in the original query. If they are not marked "traversed",
	// they are not in any snowflake queries yet and they are needed to do join with each other later
	// --> Their join columns must be put in selection list of the snowflake queries
	// in order to have data for doing join later
	
	// Get tables in the query
	list<Opt_Table*> tableList = ptrSourceQuery->getTableList();
	
	// Go through all tables to get the relationships
	list<Opt_Table*>::iterator tableIt;
	for (tableIt = tableList.begin(); tableIt != tableList.end(); tableIt++)
	{
		Opt_Table* ptrTable = *tableIt;
		// Get all joins from this table
		list<Opt_Join*> joinList = ptrTable->getJoinList();
		list<Opt_Join*>::iterator joinIt;
		for (joinIt = joinList.begin(); joinIt != joinList.end(); joinIt++)
		{
			Opt_Join* ptrJoin = *joinIt;
			//cout << "CHECK TRAVRSED JOINS: " << ptrJoin->getFromColumn()->toString() << ", " << ptrJoin->getToColumn()->toString() << endl;
			if (!ptrJoin->isTraversed())
			{
			  //cout << "NOT TRAVRSED JOINS: " << ptrJoin->getFromColumn()->toString() << ", " << ptrJoin->getToColumn()->toString() << endl;

                                // Check if this join should be in one snowflake (special case)			  
  			        if ( belongToASnowflake(ptrJoin, ptrSnowflakeBasedQuery))
				{
				  //ptrJoin->setTraversed();
				  continue;
				}
			  
				// This join is not in any snowflake queries yet
				// Clone the join columns and put them into a coresponding snowflake queries
				Opt_Column* ptrFromColumn = (ptrJoin->getFromColumn())->clone();
				string sFromQueryName =	addSelectionColumn(ptrFromColumn, ptrSnowflakeBasedQuery);
				
				Opt_Column* ptrToColumn = (ptrJoin->getToColumn())->clone();
				string sToQueryName = addSelectionColumn(ptrToColumn, ptrSnowflakeBasedQuery);
				
				// Keep this join information
				ptrFromColumn = (ptrJoin->getFromColumn())->clone();
				ptrToColumn = (ptrJoin->getToColumn())->clone();
				addJoinPredicate(sFromQueryName, sToQueryName, ptrFromColumn, ptrToColumn, ptrSnowflakeBasedQuery);
			}
		}
	}
}

// Add columns into selection list of snowflake queries
// This columns will be needed for doing agrregates
void Opt_SnowFlakes::addSelectionColumnsForLaterAggregates(Opt_QueryGraph* ptrSourceQuery, SnowflakeBasedQuery** ptrSnowflakeBasedQuery)
{
	// Get tables in the query
	list<Opt_Table*> tableList = ptrSourceQuery->getTableList();
	
	// Go through all tables to get the aggregates
	list<Opt_Table*>::iterator tableIt;
	for (tableIt = tableList.begin(); tableIt != tableList.end(); tableIt++)
	{
		Opt_Table* ptrTable = *tableIt;
		
		// Get aggregates of the table
		list<Opt_Agg*>	aggregateList = ptrTable->getAggregateList();
		list<Opt_Agg*>::iterator AggregateIt;
		for (AggregateIt = aggregateList.begin(); AggregateIt != aggregateList.end(); AggregateIt++)
		{
			// FOR NOW, ONLY AGGREGATE ON COLUMN, NOT ON EXPRESSSION
			Opt_Agg* ptrAgg = *AggregateIt;
			Opt_Column* ptrAggColumn = (Opt_Column*) ptrAgg->getAggregate();
			
			// Clone it
			Opt_Column* ptrColumn = ptrAggColumn->clone();
			
			// Add it to the coressponding snowflake
			string sAddingQuery = addSelectionColumn(ptrColumn, ptrSnowflakeBasedQuery);
			
			// Keep this aggregate
			Opt_Agg* ptrClonedAgg = ptrAgg->clone();
			(*ptrSnowflakeBasedQuery)->mSnowFlakeAggregateList.push_back(ptrClonedAgg);
			(*ptrSnowflakeBasedQuery)->mSnowFlakeAggregateQueryList.push_back(sAddingQuery);
		}
		
	}  
}

// Add a joincolumn into its corresponding snowflake
// Return 1 if the join is added query
bool Opt_SnowFlakes::belongToASnowflake(Opt_Join* ptrJoin, SnowflakeBasedQuery** ptrSnowflakeBasedQuery)
{	
	// Go through all snowflakes
	list<Opt_QueryGraph*>::iterator graphIt;
	for (graphIt = (*ptrSnowflakeBasedQuery)->mSnowFlakeQueryList.begin(); graphIt != (*ptrSnowflakeBasedQuery)->mSnowFlakeQueryList.end(); graphIt++)
	{
		Opt_QueryGraph* ptrGraph = *graphIt;
		if (ptrGraph->addJoin(ptrJoin))
		{
			// Join added
			return 1;
		}
	}
	
	return 0;
}


// Add a column into its corresponding snowflake
// Return the name of the adding query
string Opt_SnowFlakes::addSelectionColumn(Opt_Column* ptrColumn, SnowflakeBasedQuery** ptrSnowflakeBasedQuery)
{
  string sAddingQuery;
	
	// Go through all snowflakes
	list<Opt_QueryGraph*>::iterator graphIt;
	for (graphIt = (*ptrSnowflakeBasedQuery)->mSnowFlakeQueryList.begin(); graphIt != (*ptrSnowflakeBasedQuery)->mSnowFlakeQueryList.end(); graphIt++)
	{
		Opt_QueryGraph* ptrGraph = *graphIt;
		if (ptrGraph->addSelectionColumn(ptrColumn, &sAddingQuery))
		{
			// Column added
			return sAddingQuery;
		}
	}
	
	return sAddingQuery;
}

// Add a join predicate
void Opt_SnowFlakes::addJoinPredicate(string sLeftQueryName, string	sRightQueryName, 
			Opt_Column* ptrLeftColumn, Opt_Column* ptrRightColumn, SnowflakeBasedQuery** ptrSnowflakeBasedQuery)
{
  // Go thorugh all snowflake joins
	list<Opt_SnowFlakeJoin*>::iterator joinIt;
	for (joinIt = (*ptrSnowflakeBasedQuery)->mSnowFlakeJoinList.begin(); joinIt != (*ptrSnowflakeBasedQuery)->mSnowFlakeJoinList.end(); joinIt++)
	{
		Opt_SnowFlakeJoin* ptrJoin = *joinIt;
		if (ptrJoin->addJoinPredicate(sLeftQueryName, sRightQueryName, ptrLeftColumn, ptrRightColumn))
		{
			// Added
			return;
		}
	}
	
	// Cannot be added to any available join
	// Create a new join
	Opt_SnowFlakeJoin* ptrJoin = new Opt_SnowFlakeJoin(sLeftQueryName, sRightQueryName, ptrLeftColumn, ptrRightColumn);
	(*ptrSnowflakeBasedQuery)->mSnowFlakeJoinList.push_back(ptrJoin);
}

// Look for the snowflake a column in group by go with
void Opt_SnowFlakes::getSnowFlakeGroupBy(Opt_QueryGraph* ptrSourceQuery, SnowflakeBasedQuery** ptrSnowflakeBasedQuery)
{
	// Get group by columns in the original query
	list<Opt_Column*> groupByList = ptrSourceQuery->getGroupByList();
	list<Opt_Column*>::iterator columnIt;
	for (columnIt = groupByList.begin(); columnIt != groupByList.end(); columnIt++)
	{
		// Clone it
		Opt_Column* ptrColumn = (*columnIt)->clone();
		
		// Get the snowflake of this column
		string sQueryName = getQueryName(ptrColumn, *ptrSnowflakeBasedQuery);
		
		// Transform it
		(*ptrSnowflakeBasedQuery)->mSnowFlakeGroupByColumnList.push_back(ptrColumn);
		(*ptrSnowflakeBasedQuery)->mSnowFlakeGroupByQueryList.push_back(sQueryName);
	}
}


// Look for the snowflake a column in select go with
void Opt_SnowFlakes::getSnowFlakeSelect(Opt_QueryGraph* ptrSourceQuery, SnowflakeBasedQuery** ptrSnowflakeBasedQuery)
{
	// Get tables in the query
	list<Opt_Table*> tableList = ptrSourceQuery->getTableList();
	
	// Go through all tables to get columns in the select clause
	list<Opt_Table*>::iterator tableIt;
	for (tableIt = tableList.begin(); tableIt != tableList.end(); tableIt++)
	{
		Opt_Table* ptrTable = *tableIt;
		
		// Get colums in select clause of the table
		list<Opt_Column*>	colList = ptrTable->getSelectionList();
		list<Opt_Column*>::iterator colIt;
		for (colIt = colList.begin(); colIt != colList.end(); colIt++)
		{
			// Clone it
			Opt_Column* ptrColumn = (*colIt)->clone();
		
			// Get the snowflake of this column
			string sQueryName = getQueryName(ptrColumn, *ptrSnowflakeBasedQuery);
		
			// Transform it
			(*ptrSnowflakeBasedQuery)->mSnowFlakeSelectList.push_back(ptrColumn);
			(*ptrSnowflakeBasedQuery)->mSnowFlakeSelectQueryList.push_back(sQueryName);
		}
	}
}


// Look for the snowflake an object in order by go with
void Opt_SnowFlakes::getSnowFlakeOrder(Opt_QueryGraph* ptrSourceQuery, SnowflakeBasedQuery** ptrSnowflakeBasedQuery)
{
	// Get order by objects in the original query
	list<Opt_Object*> orderByList = ptrSourceQuery->getOrderByList();
	list<Opt_Object*>::iterator objectIt;
	for (objectIt = orderByList.begin(); objectIt != orderByList.end(); objectIt++)
	{
		// Clone it
		Opt_Object* ptrObject = (*objectIt)->clone();
		
		// Get the snowflake of this column
		//cout << "TABLE of order: " << ptrObject->getTableName() << endl;
		string sQueryName = getQueryName(ptrObject->getTableName(), *ptrSnowflakeBasedQuery);
		
		// Transform it
		(*ptrSnowflakeBasedQuery)->mSnowFlakeOrderByObjectList.push_back(ptrObject);
		(*ptrSnowflakeBasedQuery)->mSnowFlakeOrderByQueryList.push_back(sQueryName);
	}
}
	

// Get interesting order for every snowflake
void Opt_SnowFlakes::getInterestingOrder(SnowflakeBasedQuery** ptrSnowflakeBasedQuery)
{
	// GET FROM JOINS
	// Go through all joins
	list<Opt_SnowFlakeJoin*>::iterator joinIt;
	for (joinIt = (*ptrSnowflakeBasedQuery)->mSnowFlakeJoinList.begin(); joinIt != (*ptrSnowflakeBasedQuery)->mSnowFlakeJoinList.end(); joinIt++)
	{
		Opt_SnowFlakeJoin* ptrJoin = *joinIt;
		addInterestingOrder(ptrJoin->getLeftQueryName(), ptrJoin->getLeftPredicateList(), ptrSnowflakeBasedQuery);
		addInterestingOrder(ptrJoin->getRightQueryName(), ptrJoin->getRightPredicateList(), ptrSnowflakeBasedQuery);
	}
	
	// GET FROM GROUP BY
	// Check if all group by column is in a snowflake
	list<string>::iterator stringIt = (*ptrSnowflakeBasedQuery)->mSnowFlakeGroupByQueryList.begin();
	list<Opt_Column*>::iterator colIt = (*ptrSnowflakeBasedQuery)->mSnowFlakeGroupByColumnList.begin();
	if ( (stringIt != (*ptrSnowflakeBasedQuery)->mSnowFlakeGroupByQueryList.end()) && (colIt != (*ptrSnowflakeBasedQuery)->mSnowFlakeGroupByColumnList.end()) )
	{
		list<Opt_Column*> sameSnowFlakeColumnList;
		
		// Add this column in a list
		sameSnowFlakeColumnList.push_back(*colIt);
		
		string sQueryName = *stringIt;
		stringIt++;
		colIt++;
		bool isOneSnowFlake = 1;
		
		while ( (stringIt != (*ptrSnowflakeBasedQuery)->mSnowFlakeGroupByQueryList.end()) && (colIt != (*ptrSnowflakeBasedQuery)->mSnowFlakeGroupByColumnList.end()) )
		{
			if (sQueryName.compare(*stringIt) != 0)
			{
				// At least 2 different snowflakes
				isOneSnowFlake = 0;
				break;
			}
			
			// Add this column in a list
			sameSnowFlakeColumnList.push_back(*colIt);
			
			stringIt++;
			colIt++;
		}
		
		if (isOneSnowFlake)
		{
			// All in 1 snowflake
			addInterestingOrder(sQueryName, (*ptrSnowflakeBasedQuery)->mSnowFlakeGroupByColumnList, ptrSnowflakeBasedQuery);
		}
		else
		{
			// Interesting order is the first columns which are in the same snowflake
			addInterestingOrder(sQueryName, sameSnowFlakeColumnList, ptrSnowflakeBasedQuery);
		}
	}
	
	// GET FROM ORDER BY
	// Check if all order by column is in a snowflake
	stringIt = (*ptrSnowflakeBasedQuery)->mSnowFlakeOrderByQueryList.begin();
	list<Opt_Object*>::iterator objectIt = (*ptrSnowflakeBasedQuery)->mSnowFlakeOrderByObjectList.begin();
	if (stringIt != (*ptrSnowflakeBasedQuery)->mSnowFlakeOrderByQueryList.end() && (objectIt != (*ptrSnowflakeBasedQuery)->mSnowFlakeOrderByObjectList.end()))
	{
		list<Opt_Object*> sameSnowFlakeObjectList;
		
		// Add this column in a list
		sameSnowFlakeObjectList.push_back(*objectIt);
		
		//cout << "SNOWFLAKE: " << *stringIt << endl;
		
		string sQueryName = *stringIt;
		stringIt++;
		objectIt++;
		bool isOneSnowFlake = 1;
	
		while ( (stringIt != (*ptrSnowflakeBasedQuery)->mSnowFlakeOrderByQueryList.end()) && (objectIt != (*ptrSnowflakeBasedQuery)->mSnowFlakeOrderByObjectList.end()) )
		{
			//cout << "SNOWFLAKE: " << *stringIt << endl;
			
			if (sQueryName.compare(*stringIt) != 0)
			{
				// At least 2 different snowflakes
				isOneSnowFlake = 0;
				break;
			}
			
			// Add this column in a list
			sameSnowFlakeObjectList.push_back(*objectIt);
			
			stringIt++;
			objectIt++;
		}
		
		if (isOneSnowFlake)
		{
			// All in 1 snowflake
			addInterestingOrder(sQueryName, (*ptrSnowflakeBasedQuery)->mSnowFlakeOrderByObjectList, ptrSnowflakeBasedQuery);
		}
		else
		{
			// Interesting order is the first objects which are in the same snowflake
			addInterestingOrder(sQueryName, sameSnowFlakeObjectList, ptrSnowflakeBasedQuery);
		}
	}
}	


// Add in interesting order in the interesting order list
void Opt_SnowFlakes::addInterestingOrder(string sQueryName, list<Opt_Column*> columnList, SnowflakeBasedQuery** ptrSnowflakeBasedQuery)
{
	// Look for query of this interesting order
	list<Opt_SnowflakeInterestingOrders*>::iterator ioIt;
	for (ioIt = (*ptrSnowflakeBasedQuery)->mInterestingOrderList.begin(); ioIt != (*ptrSnowflakeBasedQuery)->mInterestingOrderList.end(); ioIt++)
	{
		Opt_SnowflakeInterestingOrders* ptrIO = *ioIt;
		if (ptrIO->addOrder(sQueryName, columnList)) 
		{
			// Added
			return;
		}
	}
		
	// The snowflake not in yet
	Opt_SnowflakeInterestingOrders* ptrIO = new Opt_SnowflakeInterestingOrders(sQueryName, columnList);
	(*ptrSnowflakeBasedQuery)->mInterestingOrderList.push_back(ptrIO);
}


// Add in interesting order in the interesting order list
void Opt_SnowFlakes::addInterestingOrder(string sQueryName, list<Opt_Object*> objectList, SnowflakeBasedQuery** ptrSnowflakeBasedQuery)
{
	// check if all columns
	list<Opt_Column*> columnList;
	list<Opt_Object*>::iterator objectIt;
	for (objectIt = objectList.begin(); objectIt != objectList.end(); objectIt++)
	{
		
		if (!(*objectIt)->isColumn())
		{
			return;
		}
		
		// Casting to column
		Opt_Column* ptrCol = (Opt_Column*) (*objectIt);
		columnList.push_back(ptrCol);
	}
	
	addInterestingOrder(sQueryName, columnList, ptrSnowflakeBasedQuery);
}


// Add in interesting order in the interesting order list
void Opt_SnowFlakes::addInterestingOrder(string sQueryName, Opt_Column* ptrColumn, SnowflakeBasedQuery** ptrSnowflakeBasedQuery)
{
	// This order has only 1 column
	list<Opt_Column*> order;
	order.push_back(ptrColumn);
	addInterestingOrder(sQueryName, order, ptrSnowflakeBasedQuery);
}


// Add in interesting order in the interesting order list
void Opt_SnowFlakes::addInterestingOrder(string sQueryName, Opt_Object* ptrObject, SnowflakeBasedQuery** ptrSnowflakeBasedQuery)
{
	// Check if it is a column
	if (ptrObject->isColumn())
	{
		addInterestingOrder(sQueryName, ((Opt_Column*) ptrObject), ptrSnowflakeBasedQuery);
	}
}

// Create join query for all snowflakes
void Opt_SnowFlakes::getJoinQuery(SnowflakeBasedQuery** ptrSnowflakeBasedQuery)
{	
	// Get all snowflakes
	list<Opt_Table*> tableList;                                                                                                                                         
	list<Opt_QueryGraph*>::iterator sfIt;
	for (sfIt = (*ptrSnowflakeBasedQuery)->mSnowFlakeQueryList.begin(); sfIt != (*ptrSnowflakeBasedQuery)->mSnowFlakeQueryList.end(); sfIt++)
	{
		Opt_QueryGraph* ptrQueryGraph = *sfIt;
		
		// Create tablefor this snowflake
		Opt_Table* ptrTable = new Opt_Table(ptrQueryGraph->getQueryName(), ptrQueryGraph->getQueryName());
		
		// Insert all selection columns of this snowflake
		list<Opt_Column*>::iterator colIt = (*ptrSnowflakeBasedQuery)->mSnowFlakeSelectList.begin();
		list<string>::iterator strIt = (*ptrSnowflakeBasedQuery)->mSnowFlakeSelectQueryList.begin();
		while ((colIt != (*ptrSnowflakeBasedQuery)->mSnowFlakeSelectList.end()) && (strIt != (*ptrSnowflakeBasedQuery)->mSnowFlakeSelectQueryList.end()))
		{
			// Check if it is in the same snowflake
			if ( (*strIt).compare(ptrQueryGraph->getQueryName()) == 0)
			{
				// Add this column
				(*colIt)->setTableName(ptrQueryGraph->getQueryName());
				ptrTable->addSelection(*colIt);
			}
			colIt++;
			strIt++;
		}
		
		// Insert all aggregates of this snowflake
		list<Opt_Agg*>::iterator aggIt = (*ptrSnowflakeBasedQuery)->mSnowFlakeAggregateList.begin();
		strIt = (*ptrSnowflakeBasedQuery)->mSnowFlakeAggregateQueryList.begin();
		while ((aggIt != (*ptrSnowflakeBasedQuery)->mSnowFlakeAggregateList.end()) && (strIt != (*ptrSnowflakeBasedQuery)->mSnowFlakeAggregateQueryList.end()))
		{
			// Check if it is in the same snowflake
			if ( (*strIt).compare(ptrQueryGraph->getQueryName()) == 0)
			{
				// Add this aggregate
				(*aggIt)->setTableName(ptrQueryGraph->getQueryName());
				ptrTable->addAggregate(*aggIt);
			}
			aggIt++;
			strIt++;
		}
		
		// Add join			
		list<Opt_SnowFlakeJoin*>::iterator joinIt;
		for ( joinIt = (*ptrSnowflakeBasedQuery)->mSnowFlakeJoinList.begin();	joinIt != (*ptrSnowflakeBasedQuery)->mSnowFlakeJoinList.end();	joinIt++)
		{
			(*joinIt)->addJoins(&ptrTable); 
		}
		
		tableList.push_back(ptrTable); 
	} 
	
	// Get all group by columns
	list<Opt_Column*>::iterator colIt = (*ptrSnowflakeBasedQuery)->mSnowFlakeGroupByColumnList.begin();
	list<string>::iterator strIt = (*ptrSnowflakeBasedQuery)->mSnowFlakeGroupByQueryList.begin();

	while ((colIt != (*ptrSnowflakeBasedQuery)->mSnowFlakeGroupByColumnList.end()) && (strIt != (*ptrSnowflakeBasedQuery)->mSnowFlakeGroupByQueryList.end()))
	{
		// Set snowflake table name for this column
		(*colIt)->setTableName(*strIt);
		colIt++;
		strIt++;
	}
	
	// Get all order by columns
	list<Opt_Object*>::iterator objectIt = (*ptrSnowflakeBasedQuery)->mSnowFlakeOrderByObjectList.begin();
	strIt = (*ptrSnowflakeBasedQuery)->mSnowFlakeOrderByQueryList.begin();
	while ((objectIt != (*ptrSnowflakeBasedQuery)->mSnowFlakeOrderByObjectList.end()) && (strIt != (*ptrSnowflakeBasedQuery)->mSnowFlakeOrderByQueryList.end()))
	{
		// Set snowflake table name for this column
		(*objectIt)->setTableName(*strIt);
		objectIt++;
		strIt++;
	} 
	
	// Create a query graph
	(*ptrSnowflakeBasedQuery)->m_ptrJoinQuery = new Opt_QueryGraph(tableList, (*ptrSnowflakeBasedQuery)->mSnowFlakeGroupByColumnList, 
								       (*ptrSnowflakeBasedQuery)->mSnowFlakeOrderByObjectList, NULL);
}

/* This function traverses through the list all the SnowflakeBased Queries and remove
   those that are essentially the same */
void Opt_SnowFlakes::removeDuplicateSnowflakeBasedQuery() {
  list<SnowflakeBasedQuery*>::iterator iter1, iter2;
  
  /* Do a for-loop that check for all the duplicate SnowflakeBasedQuery:
     1.  First iterator that points to a SnowflakeBasedQuery
     2a. Second iterator starts from the next of the first operator and goes to the end
     2b. Second iterator also points to another (the next one) SnowflakeBasedQuery
     3.  Check if first SnowflakeBasedQuery is the same with the second SnowflakeBasedQuery 
     If yes -> remove the second */
  
  for (iter1 = mAllEquivalentSnowflakeBasedQueries.begin();
       iter1 != mAllEquivalentSnowflakeBasedQueries.end(); ) {
  
    list<Opt_QueryGraph*> firstList = (*iter1)->mSnowFlakeQueryList; /* Get the list of QueryGraph from the first iterator */
    iter2 = iter1; 
    iter2++; /* The second iterator now is the next of the first operator */

    /* Until the second iterator reaches the end of the list ... */

    while (iter2 != mAllEquivalentSnowflakeBasedQueries.end()) {

      /* Get the list of QueryGraph from the second iterator */

      list<Opt_QueryGraph*> secondList = (*iter2)->mSnowFlakeQueryList;
      
      /* The sizes of the two lists should be equal since they were generated
	 from the same QueryGraph */
      
      if (firstList.size() != secondList.size()) {
	cout << "[ERR] - Unknown error: sizes of lists should be equal" << endl;
      }
      
      /* Traverse through the two lists of QueryGraph and see if they match each other */

      list<Opt_QueryGraph*>::iterator sub_iter1 = firstList.begin();

      /* This counts the number of times a QueryGraph from the first list matches some QueryGraph from the second list 
	 If this number is equal to the length of either the first or the second list, it means the two lists are the same */

      unsigned int count = 0; 
      
      /* Go to the end of the first list of QueryGraphs */

      while (sub_iter1 != firstList.end()) {

	/* For each QueryGraph in the first list, compare it with each of the QueryGraph from the second list */

	list<Opt_QueryGraph*>::iterator sub_iter2 = secondList.begin();

	while (sub_iter2 != secondList.end()) { /* Go to the end of the second list of QueryGraphs */
	  
	  /* If the names of the queries are the same, check for more details 
	     If the names are different, move on to the next one */

	  if (((*sub_iter1)->getQueryName()).compare((*sub_iter2)->getQueryName()) == 0) {
	    
	    list<Opt_Table*> tableList1 = (*sub_iter1)->getTableList();
	    list<Opt_Table*> tableList2 = (*sub_iter2)->getTableList();
	    
	    /* If the tables in from clause of the first SnowflakeBasedQuery is not equal to those of the seconds
	       Go to the next QueryGraph of the second list */

	    if (tableList1.size() != tableList2.size()) {
	      sub_iter2++;
	      continue;
	    }

	    /* If the first list of all the tables are the the same with the second list, the two QueryGraphs are the same
	       Increment the total count by 1 */
	    	    	    
	    int threshold = tableList1.size();
	    list<Opt_Table*>::iterator tbl_iter1 = tableList1.begin();
	    int sub_count = 0; /* A sub_count... that keep tracks of how many times there are a pair of matching table's name */
	    
	    /* For each table in the first QueryGraph, traverse through the list of tables of the second QueryGraph
	       Record the number of times there is a matching pair (in sub_count)
	       In the end, if the number of matching pair (sub_count) is greater than or equal to the size of either table list -> BINGO */

	    while (tbl_iter1 != tableList1.end()) {
	      for (list<Opt_Table*>::iterator tbl_iter2 = tableList2.begin();
		   tbl_iter2 != tableList2.end(); tbl_iter2++) {
		if (((*tbl_iter1)->getName()).compare((*tbl_iter2)->getName()) == 0) {
		  sub_count++;
		}
	      }
	      tbl_iter1++;
	    }
	    if (sub_count >= threshold) {
	      count++;
	      break;
	    }
	  }
	  /* else, go to the next element in the second list */
	  sub_iter2++;
	}
	sub_iter1++;
      }

      /* If the two SnowflakeBasedQuery objects are indeed duplication of each other */

      if (count == firstList.size()) {
	
	/* Remove the second one */
	
	list<SnowflakeBasedQuery*>::iterator temp = iter2;
	iter2++;
	mAllEquivalentSnowflakeBasedQueries.remove(*temp);
	continue;
      }
      /* else do nothing and go on the the next loop iteration */
      
      iter2++; /* Increment second iterator */
    }
    iter1++;
  }
}


// make plan for a snowflake based query
Opt_Plan* Opt_SnowFlakes::makeSnowFlakeBasedPlan(SnowflakeBasedQuery* ptrSnowflakeBasedQuery)
{
  // Make plans for all snowflakes
  list<Opt_QueryGraph*> subQueries = ptrSnowflakeBasedQuery->mSnowFlakeQueryList;
  list<Opt_Plan*> subPlans;
  list<Opt_QueryGraph*>::iterator it;
  unsigned int i = 0;
  for (it = subQueries.begin(); it != subQueries.end(); it++)
  {
    //cout << endl << "SUBPLAN "  << "---------------" << endl << endl;
    Opt_Plan* plan = new Opt_Plan();
    plan->makePlan(*it,m_ptrOriginalQuery);
    subPlans.push_back(plan);
    // plan->print("   ");
    i++;
  }

  // combine the subplans to have a best plan
  //cout << "     BEFORE combineSubPlans\n";
  Opt_Plan* plan = Opt_Plan::combineSubPlans(m_ptrOriginalQuery, subPlans);
  //cout << "     DONE combineSubPlans\n";

  return plan;
}

