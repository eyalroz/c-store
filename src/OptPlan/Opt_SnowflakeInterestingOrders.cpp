/*
 *  Opt_SnowflakeInterestingOrders.h
 *  OptDev
 *
 *  Created by Nga Tran on 11/10/05.
 *  Questions, ask Nga Tran at nga@brandeis.edu or Tien Hoang at hoangt@brandeis.edu
 *	
 *  This class keeps interesting orders of a snowflake query
 */

#include "Opt_SnowflakeInterestingOrders.h"

// Default constructor
Opt_SnowflakeInterestingOrders::Opt_SnowflakeInterestingOrders()
{
	m_sQueryName = "";
}
		
// Provide query name
Opt_SnowflakeInterestingOrders::Opt_SnowflakeInterestingOrders(string sQueryName)
{
	m_sQueryName = sQueryName;
}
		
// Provide query names and 1 object
Opt_SnowflakeInterestingOrders::Opt_SnowflakeInterestingOrders(string sQueryName, list<Opt_Column*> order)
{
	m_sQueryName = sQueryName;
	mOrderList.push_back(order);
}
					
// Destructor
Opt_SnowflakeInterestingOrders::~Opt_SnowflakeInterestingOrders()
{
}


// String presentation of this class
string Opt_SnowflakeInterestingOrders::toStringNoTableDot(string sIndent)
{
	string sReturn = sIndent;
	sReturn.append(m_sQueryName);
	sReturn.append(": \n");
	
	sIndent += "\t";
	list<InterestingOrder>::iterator orderIt;
	for (orderIt = mOrderList.begin(); orderIt != mOrderList.end(); orderIt++)
	{		
		sReturn.append(sIndent);
	
		InterestingOrder order = *orderIt;
		list<Opt_Column*>::iterator colIt;
		for (colIt = order.begin(); colIt != order.end(); colIt++)
		{
			sReturn.append((*colIt)->toStringNoTableDot());
			sReturn.append("     ");
		}
		
		sReturn.append("\n");
	}

	return sReturn;
}
		
// String presentation of this class
string Opt_SnowflakeInterestingOrders::toString(string sIndent)
{
	string sReturn = sIndent;
	sReturn.append(m_sQueryName);
	sReturn.append(": \n");
	
	sIndent += "\t";
	list<InterestingOrder>::iterator orderIt;
	for (orderIt = mOrderList.begin(); orderIt != mOrderList.end(); orderIt++)
	{		
		sReturn.append(sIndent);
	
		InterestingOrder order = *orderIt;
		list<Opt_Column*>::iterator colIt;
		for (colIt = order.begin(); colIt != order.end(); colIt++)
		{
			sReturn.append((*colIt)->toString());
			sReturn.append("     ");
		}
		
		sReturn.append("\n");
	}

	return sReturn;
}
		
void Opt_SnowflakeInterestingOrders::setQueryName(string sQueryName)
{
	m_sQueryName = sQueryName;
}

string Opt_SnowflakeInterestingOrders::getQueryName()
{
	return m_sQueryName;
}
		
list<InterestingOrder> Opt_SnowflakeInterestingOrders::getOrderList()
{
	return mOrderList;
}
		
// Add an order
// return true (1) if the order added
bool Opt_SnowflakeInterestingOrders::addOrder(string sQueryName, list<Opt_Column*> order)
{
	if (m_sQueryName.compare(sQueryName) == 0)
	{
		// Check if the order already existed
		list<InterestingOrder>::iterator it;
		for (it = mOrderList.begin(); it != mOrderList.end(); it++)
		{
			list<Opt_Column*> intertestingOrder = *it;
			
			// Check if number of columns in the  orders are the same
			if (intertestingOrder.size() != order.size())
			{
				continue;
			}
			
			// Compare if  orders are exactly the same
			bool isTheSame = 1;
			list<Opt_Column*>::iterator existColIt = intertestingOrder.begin();
			list<Opt_Column*>::iterator colIt = order.begin();
			while( (existColIt != intertestingOrder.end()) && (colIt != order.end()) )
			{
				if (((*existColIt)->getColumnName()).compare((*colIt)->getColumnName()) != 0)
				{
					isTheSame = 0;
				}
				existColIt++;
				colIt++;
			}
			
			if (isTheSame)
			{
				// the order already existed
				return 1;
			}
		}
	
		mOrderList.push_back(order);
		return 1;
	}
	
	return 0;
}
