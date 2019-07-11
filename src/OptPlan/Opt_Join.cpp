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

#include "Opt_Join.h"

// Default constructor
Opt_Join::Opt_Join()
{
	m_ptrFromColumn = NULL;
	m_ptrToColumn = NULL;
	m_bIsTraversed = 0;
	m_bIsSnowflakeJoinPredicate = 0;
}
		
// Provide column and table names
Opt_Join::Opt_Join(Opt_Column* ptrFromColumn, Opt_Column* ptrToColumn)
{
	m_ptrFromColumn = ptrFromColumn;
	m_ptrToColumn = ptrToColumn;
	m_bIsTraversed = 0;
	m_bIsSnowflakeJoinPredicate = 0;
}

  
// Default Destructor
Opt_Join::~Opt_Join()
{
}

// Clone
Opt_Join* Opt_Join::clone()
{
	Opt_Column* ptrFrom = NULL;
	Opt_Column* ptrTo = NULL;
	
	if (m_ptrFromColumn != NULL) ptrFrom = m_ptrFromColumn->clone();
	if (m_ptrToColumn != NULL) ptrTo = m_ptrToColumn->clone();
	
	Opt_Join* ptrJoin = new Opt_Join(ptrFrom, ptrTo);
	ptrJoin->m_bIsTraversed = m_bIsTraversed;
	ptrJoin->m_bIsSnowflakeJoinPredicate = m_bIsSnowflakeJoinPredicate;

	return ptrJoin;
}

// Clone
// Also  mark snowflake join for this join if it points to the same table as ptrNonSnowflakeJoin but goes from different table
Opt_Join*  Opt_Join::clone(Opt_Join* ptrNonSnowflakeJoin)
{
	// Normal clone first
  	Opt_Join* ptrJoin = clone();

		
	// Check if this join points to the same table as ptrNonSnowflakeJoin
	if (ptrJoin->getToColumn()->getTableName().compare(ptrNonSnowflakeJoin->getToColumn()->getTableName()) == 0)
	{
	     // Check if From tables are different
	     if (ptrJoin->getFromColumn()->getTableName().compare(ptrNonSnowflakeJoin->getFromColumn()->getTableName()) != 0)
	     {
	       ptrJoin->setSnowflakesJoin();

    	     }
	}

	return ptrJoin;
}

		
void Opt_Join::setFromColumn(Opt_Column* ptrFromColumn)
{
	m_ptrFromColumn = ptrFromColumn;
}

Opt_Column* Opt_Join::getFromColumn()
{
	return m_ptrFromColumn;
}

		
void Opt_Join::setToColumn(Opt_Column* ptrToColumn)
{
	m_ptrToColumn = ptrToColumn;
}

Opt_Column* Opt_Join::getToColumn()
{
	return m_ptrToColumn;
}

string Opt_Join::toStringNoTableDot() {
  string s = "";
  
  s = m_ptrFromColumn->toStringNoTableDot() + " = " + m_ptrToColumn->toStringNoTableDot();

  return s;  
}

string Opt_Join::toString() {
  string s = "";
  
  s = m_ptrFromColumn->toString() + " = " + m_ptrToColumn->toString();

  return s;  
}

// Mark this column traversed
void Opt_Join::setTraversed()
{
	m_bIsTraversed = 1;
}

// UNMark this column traversed
void Opt_Join::resetTraversed()
{
	m_bIsTraversed = 0;
}
  
// check if this column traversed
bool Opt_Join::isTraversed()
{
	return m_bIsTraversed;
}


// Mark this join is a join between 2 snowfalkes
void Opt_Join::setSnowflakesJoin()
{
  m_bIsSnowflakeJoinPredicate = 1;
}

// check if this is a snowflake join
bool Opt_Join::isSnowflakesJoin()
{
  return  m_bIsSnowflakeJoinPredicate;
}
