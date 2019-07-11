/*
 *  Opt_SnowFlakes.h
 *  C-Store - Optimimzation
 *
 *  Created by Nga Tran on 11/07/05.
 * 
 *  Questions, ask Nga Tran at nga@brandeis.edu or Tien Hoang at hoangt@brandeis.edu
 *	
 *  This class is responsible fr creating an equivalent snowflakes query from a given query
 */

#ifndef _OPT_SNOWFLAKE_H_
#define _OPT_SNOWFLAKE_H_

#include <fstream>
#include <iostream>
#include <list>
#include <string>

#include "Opt_QueryGraph.h"
#include "Opt_Table.h"
#include "Opt_SnowFlakeJoin.h"
#include "Opt_SnowflakeInterestingOrders.h"
#include "Opt_Column.h"
#include "Opt_Object.h"
#include "Opt_Plan.h"

using namespace std;

typedef struct SnowflakeBasedQuery
{
        int				m_iNumFactTables;

  	list<Opt_QueryGraph*>		mSnowFlakeQueryList;
	list<Opt_SnowFlakeJoin*>	mSnowFlakeJoinList;
	Opt_QueryGraph*			m_ptrJoinQuery;			// Join of the snowflakes (final query)
		
	// For select only
	list<Opt_Column*>		mSnowFlakeSelectList;		// Group by column
	list<string>			mSnowFlakeSelectQueryList;	// Snowflake query name of corresponding mSnowFlakeSelectList
				
	// For aggregate only
	list<Opt_Agg*>			mSnowFlakeAggregateList;	// Aggregate list
	list<string>			mSnowFlakeAggregateQueryList;	// Snowflake query name of corresponding mSnowFlakeAggregateList
		
	// For Group by only
	list<Opt_Column*>		mSnowFlakeGroupByColumnList;	// Group by column
	list<string>			mSnowFlakeGroupByQueryList;	// Snowflake query name of corresponding mSnowFlakeGroupByColumnList

	// For Order by only
	list<Opt_Object*>		mSnowFlakeOrderByObjectList;	// Order by object (column/ aggregate)
	list<string>			mSnowFlakeOrderByQueryList;	// Snowflake query name of corresponding mSnowFlakeGroupByColumnList
		
	// For interesting orders only
	list<Opt_SnowflakeInterestingOrders*>		mInterestingOrderList;
} SnowflakeBasedQuery;


class Opt_SnowFlakes: Opt_Object
{
	protected:

		// Member declaration
                Opt_QueryGraph*                 m_ptrOriginalQuery;          // Original query transformed from the SQL parser
		list<Opt_QueryGraph*>		mAllEquivalentQueries;	     // All equivalent queries of m_ptrOriginalQuery based on number of fact tables in the query
		list<SnowflakeBasedQuery*>      mAllEquivalentSnowflakeBasedQueries;
		
	public:
		// Default constructor
		Opt_SnowFlakes();
		
		// Constructor with an input query graph
		Opt_SnowFlakes(Opt_QueryGraph* ptrQuery);

		// snowflake query
		string toSnowflakeSQLs();

		// snowflake query
		string toSnowflakeSQLs(string sQueryName);

		// snowflake query
		string toSnowflakeSQLsOracle(string sQueryName);
		
		// Get/set
		Opt_QueryGraph*	getOriginalQuery();
		list<SnowflakeBasedQuery*> getSnowFlakeBasedQueryList();
		//		list<Opt_SnowFlakeJoin*> getSnowFlakeJoinList();

		// Get the best predicted plan of all decompositions
		Opt_Plan* getPlan();

		// Get all best predicted plans of all decompositions
		list<Opt_Plan*> getPlans();
		
		// Produce snowflake queries
		void produceSnowFlakeQueries();		 
		
	protected:
		
		// String of snowflake queries
		string toString(SnowflakeBasedQuery* ptrSnowflakeBasedQuery);

		// Return a string presentation for this aggregate
		virtual string toStringNoTableDot(SnowflakeBasedQuery* ptrSnowflakeBasedQuery);

		// snowflake query
		string toSnowflakeSQL(SnowflakeBasedQuery* ptrSnowflakeBasedQuery);

		// snowflake query
		string toSnowflakeSQLOracle(SnowflakeBasedQuery* ptrSnowflakeBasedQuery);

		// writing snowflake query into a file
		string toSnowflakeSQLOracle(string sQuery, string sFileName);

		// writing snowflake query into a file
		string toSnowflakeSQL(SnowflakeBasedQuery* ptrSnowflakeBasedQuery, string sFileName);

		// writing snowflake query into a file
		string toSnowflakeSQLOracle(SnowflakeBasedQuery* ptrSnowflakeBasedQuery, string sFileName);

		// produce all equivalent snowflake queries
		void produceAllEquivalentQueries(Opt_QueryGraph* ptrQuery);

		// Produce a snowflake-based query
		SnowflakeBasedQuery* produceSnowFlakeBasedQuery(Opt_QueryGraph* ptrSourceQuery);

		// Add a table and its possible related things to a querygraph
		void addTables(Opt_QueryGraph* ptrSourceQuery, Opt_QueryGraph** ptrQueryGraph, string sTableName, SnowflakeBasedQuery* ptrSnowflakeBasedQuery);

		// Add a table and its linked tables (descendents) to a snowflake in the snowflake-based query
		// Return 0 if it cannot be added to any snowflake or it cann be added to more than one snowflake
		// Return 1 if it is added. The snowflake name will be return along
		bool add(Opt_Table* ptrTable, Opt_QueryGraph* ptrSourceQuery, SnowflakeBasedQuery** ptrSnowflakeBasedQuery, string* ptrSnowFlakeName);

		// Add columns into selection list of snowflake queries
		// This columns will be needed for joining snowflake queries together
		void addSelectionColumnsForLaterJoins(Opt_QueryGraph* ptrSourceQuery, SnowflakeBasedQuery** ptrSnowflakeBasedQuery);

		// Add a joincolumn into its corresponding snowflake
		// Return 1 if the join is added query
		bool belongToASnowflake(Opt_Join* ptrJoin, SnowflakeBasedQuery** ptrSnowflakeBasedQuery);

		// Add a column into its corresponding snowflake
		// Return the name of the adding query
		string addSelectionColumn(Opt_Column* ptrColumn, SnowflakeBasedQuery** ptrSnowflakeBasedQuery);

		// Add a join predicate
		void addJoinPredicate(string sLeftQueryName, string	sRightQueryName, 
			Opt_Column* ptrLeftColumn, Opt_Column* ptrRightColumn, SnowflakeBasedQuery** ptrSnowflakeBasedQuery);

		// Add columns into selection list of snowflake queries
		// This columns will be needed for doing agrregates
		void addSelectionColumnsForLaterAggregates(Opt_QueryGraph* ptrSourceQuery, SnowflakeBasedQuery** ptrSnowflakeBasedQuery);

		// Look for the snowflake a column in select go with
		void getSnowFlakeSelect(Opt_QueryGraph* ptrSourceQuery, SnowflakeBasedQuery** ptrSnowflakeBasedQuery);

		// Get snowflake query name of a column
		string getQueryName(Opt_Column* ptrColumn, SnowflakeBasedQuery* ptrSnowflakeBasedQuery);
		
		// Get snowflake query name of a table
		string getQueryName(string sTableName, SnowflakeBasedQuery* ptrSnowflakeBasedQuery);

		// Look for the snowflake a column in group by go with
		void getSnowFlakeGroupBy(Opt_QueryGraph* ptrSourceQuery, SnowflakeBasedQuery** ptrSnowflakeBasedQuery);

		// Look for the snowflake an object in order by go with
		void getSnowFlakeOrder(Opt_QueryGraph* ptrSourceQuery, SnowflakeBasedQuery** ptrSnowflakeBasedQuery);

		
		// Add in interesting order in the interesting order list
		void addInterestingOrder(string sQueryName, list<Opt_Column*> columnList, SnowflakeBasedQuery** ptrSnowflakeBasedQuery);
		
		// Add in interesting order in the interesting order list
		void addInterestingOrder(string sQueryName, list<Opt_Object*> objectList, SnowflakeBasedQuery** ptrSnowflakeBasedQuery);
		
		// Add in interesting order in the interesting order list
		void addInterestingOrder(string sQueryName, Opt_Column* ptrColumn, SnowflakeBasedQuery** ptrSnowflakeBasedQuery);
		
		// Add in interesting order in the interesting order list
		void addInterestingOrder(string sQueryName, Opt_Object* ptrObject, SnowflakeBasedQuery** ptrSnowflakeBasedQuery);

		// Get interesting order for every snowflake
		void getInterestingOrder(SnowflakeBasedQuery** ptrSnowflakeBasedQuery);

		// Create join query for all snowflakes
		void getJoinQuery(SnowflakeBasedQuery** ptrSnowflakeBasedQuery);

		// Check if a table exists in mSnowFlakeQueryList
		bool exists(string sTableName, SnowflakeBasedQuery* ptrSnowflakeBasedQuery, string* ptrSnowFlakeName);

		/* Remove all all SnowflakeBasedQuery objects that are essentially the same */
		void removeDuplicateSnowflakeBasedQuery();
		
		// make plan for a snowflake based query
		Opt_Plan* makeSnowFlakeBasedPlan(SnowflakeBasedQuery* ptrSnowflakeBasedQuery);

};

#endif // _OPT_SNOWFLAKE_H_


