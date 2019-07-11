/*
 *  Opt_SnowflakeInterestingOrders.h
 *  OptDev
 *
 *  Created by Nga Tran on 11/10/05.
 *  Questions, ask Nga Tran at nga@brandeis.edu or Tien Hoang at hoangt@brandeis.edu
 *	
 *  This class keeps interesting orders of a snowflake query
 */

#ifndef _OPT_SNOWFLAKEINTERESTINGORDER_H_
#define _OPT_SNOWFLAKEINTERESTINGORDER_H_

#include <list>
#include <string>

#include "Opt_Object.h"
#include "Opt_Column.h"

using namespace std;

typedef  list<Opt_Column*>  InterestingOrder;

class Opt_SnowflakeInterestingOrders : Opt_Object
{
	protected:
		// Member declaration
		string					m_sQueryName;		// snowflake query
		list<InterestingOrder>	mOrderList;	
	
	public:
		// Default constructor
		Opt_SnowflakeInterestingOrders();
		
		// Provide query name
		Opt_SnowflakeInterestingOrders(string sQueryName);
		
		// Provide query names and 1 object
		Opt_SnowflakeInterestingOrders(string sQueryName, list<Opt_Column*> order);
					
		// Destructor
		~Opt_SnowflakeInterestingOrders();
		
		// String presentation of this class
		string toString(string sIndent);

		// Return a string presentation for this aggregate
		virtual string toStringNoTableDot(string sIndent);
		
		void setQueryName(string sQueryName);
		string getQueryName();
		
		list<InterestingOrder> getOrderList();
		
		// Add an order
		// return true (1) if the order added
		bool addOrder(string sQueryName, list<Opt_Column*> order);
};

#endif // _OPT_SNOWFLAKEINTERESTINGORDER_H_
