/*
 *  Opt_SnowFlakeJoin.h
 *  C-Store - Optimimzation
 *
 *  Created by Nga Tran on 11/9/05.
 * 
 *  Questions, ask Nga Tran at nga@brandeis.edu or Tien Hoang at hoangt@brandeis.edu
 *	
 *  This class keeps join predicates of 2 snowflake queries
 */

#ifndef _OPT_SNOWFLAKEJOIN_H_
#define _OPT_SNOWFLAKEJOIN_H_

#include <list>
#include <string>

#include "Opt_Object.h"
#include "Opt_Column.h"
#include "Opt_Table.h"
#include "Opt_Join.h"

using namespace std;

class Opt_SnowFlakeJoin : Opt_Object
{
	protected:
		// Member declaration
		string	m_sLeftQueryName;					// First snowflake query in to the join
		string	m_sRightQueryName;					// Second snowflake query in to the join
		list<Opt_Column*>	mLeftPredicateList;		// Columns in the join predicate beloing to the first snowflake
		list<Opt_Column*>	mRightPredicateList;	// Columns in the join predicate beloing to the second snowflake

	public:
		// Default constructor
		Opt_SnowFlakeJoin();
		
		// Provide 2 query names
		Opt_SnowFlakeJoin(string sLeftQueryName, string	sRightQueryName);
		
		// Provide 2 query names and 1 predicate
		Opt_SnowFlakeJoin(string sLeftQueryName, string	sRightQueryName, 
			Opt_Column* ptrLeftColumn, Opt_Column* ptrRightColumn);
			
		// Destructor
		~Opt_SnowFlakeJoin();
		
		// String presentation of this join
		string toString(string sIndent);

		// Return a string presentation for this aggregate
		virtual string toStringNoTableDot(string sIndent);
		
		void setLeftQueryName(string sLeftQueryName);
		string getLeftQueryName();
		
		void setRightQueryName(string sRightQueryName);
		string getRightQueryName();
		
		list<Opt_Column*> getLeftPredicateList();
		list<Opt_Column*> getRightPredicateList();
		
		// Add a join predicate
		// return true (1) if the predicate added
		bool addJoinPredicate(string sLeftQueryName, string	sRightQueryName, 
			Opt_Column* ptrLeftColumn, Opt_Column* ptrRightColumn);
			
		// Add all joins of this object in a table of a query
		void addJoins(Opt_Table** ptrTable);
};

#endif // _OPT_SNOWFLAKEJOIN_H_
