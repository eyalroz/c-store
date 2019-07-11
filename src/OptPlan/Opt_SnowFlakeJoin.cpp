/*
 *  Opt_SnowFlakeJoin.cpp
 *  C-Store - Optimimzation
 *
 *  Created by Nga Tran on 11/9/05.
 * 
 *  Questions, ask Nga Tran at nga@brandeis.edu or Tien Hoang at hoangt@brandeis.edu
 *	
 *  This class keeps join predicates of 2 snowflake queries
 */

#include "Opt_SnowFlakeJoin.h"

// Default constructor
Opt_SnowFlakeJoin::Opt_SnowFlakeJoin()
{
	m_sLeftQueryName = "";
	m_sRightQueryName = "";
}
		
// Provide 2 query names
Opt_SnowFlakeJoin::Opt_SnowFlakeJoin(string sLeftQueryName, string	sRightQueryName)
{
	m_sLeftQueryName = sLeftQueryName;
	m_sRightQueryName = sRightQueryName;
}
		
// Provide 2 query names and 1 predicate
Opt_SnowFlakeJoin::Opt_SnowFlakeJoin(string sLeftQueryName, string	sRightQueryName, 
			Opt_Column* ptrLeftColumn, Opt_Column* ptrRightColumn)
{
	m_sLeftQueryName = sLeftQueryName;
	m_sRightQueryName = sRightQueryName;
	mLeftPredicateList.push_back(ptrLeftColumn);
	mRightPredicateList.push_back(ptrRightColumn);
}

			
// Destructor
Opt_SnowFlakeJoin::~Opt_SnowFlakeJoin()
{
}



string Opt_SnowFlakeJoin::toStringNoTableDot(string sIndent)
{
  	string sReturn = sIndent;
	sReturn.append("Queries:\n");
	sReturn.append(sIndent);
	sReturn.append("\t");
	sReturn.append(m_sLeftQueryName);
	sReturn.append(", ");
	sReturn.append(m_sRightQueryName);
	sReturn.append("\n");
	sReturn.append(sIndent);
	sReturn.append("Join Predicates:\n");
	
	sIndent += "\t";
	list<Opt_Column*>::iterator leftIt = mLeftPredicateList.begin();
	list<Opt_Column*>::iterator rightIt = mRightPredicateList.begin();
	while ((leftIt != mLeftPredicateList.end()) && (rightIt != mRightPredicateList.end()) )
	{
		
		sReturn.append(sIndent);
		sReturn.append("{");
		sReturn.append((*leftIt)->getColumnName());
		sReturn.append(", ");
		sReturn.append((*rightIt)->getColumnName());
		sReturn.append("}\n");
		leftIt++;
		rightIt++;
	}
	
	return sReturn;
}

// String presentation of this join
string Opt_SnowFlakeJoin::toString(string sIndent)
{
	string sReturn = sIndent;
	sReturn.append("Queries:\n");
	sReturn.append(sIndent);
	sReturn.append("\t");
	sReturn.append(m_sLeftQueryName);
	sReturn.append(", ");
	sReturn.append(m_sRightQueryName);
	sReturn.append("\n");
	sReturn.append(sIndent);
	sReturn.append("Join Predicates:\n");
	
	sIndent += "\t";
	list<Opt_Column*>::iterator leftIt = mLeftPredicateList.begin();
	list<Opt_Column*>::iterator rightIt = mRightPredicateList.begin();
	while ((leftIt != mLeftPredicateList.end()) && (rightIt != mRightPredicateList.end()) )
	{
		
		sReturn.append(sIndent);
		sReturn.append("{");
		sReturn.append((*leftIt)->getColumnName());
		sReturn.append(", ");
		sReturn.append((*rightIt)->getColumnName());
		sReturn.append("}\n");
		leftIt++;
		rightIt++;
	}
	
	return sReturn;
}

		
void Opt_SnowFlakeJoin::setLeftQueryName(string sLeftQueryName)
{
	m_sLeftQueryName = sLeftQueryName;
}

string Opt_SnowFlakeJoin::getLeftQueryName()
{
	return m_sLeftQueryName;
}

		
void Opt_SnowFlakeJoin::setRightQueryName(string sRightQueryName)
{
	m_sRightQueryName = sRightQueryName;
}

string Opt_SnowFlakeJoin::getRightQueryName()
{
	return m_sRightQueryName;
}
		
list<Opt_Column*> Opt_SnowFlakeJoin::getLeftPredicateList()
{
	return mLeftPredicateList;
}

list<Opt_Column*> Opt_SnowFlakeJoin::getRightPredicateList()
{
	return mRightPredicateList;
}

		
// Add a join predicate
// return true (1) if the predicate added
bool Opt_SnowFlakeJoin::addJoinPredicate(string sLeftQueryName, string	sRightQueryName, 
			Opt_Column* ptrLeftColumn, Opt_Column* ptrRightColumn)
{
	if ( (m_sLeftQueryName.compare(sLeftQueryName) == 0) && 
	     (m_sRightQueryName.compare(sRightQueryName) == 0))
	{
		mLeftPredicateList.push_back(ptrLeftColumn);
		mRightPredicateList.push_back(ptrRightColumn);
		return 1;
	}
	
	if ( (m_sLeftQueryName.compare(sRightQueryName) == 0) && 
	     (m_sRightQueryName.compare(sLeftQueryName) == 0))
	{
		mLeftPredicateList.push_back(ptrRightColumn);
		mRightPredicateList.push_back(ptrLeftColumn);
		return 1;
	}
	
	return 0;
}

// Add all joins of this object in a table of a query
void Opt_SnowFlakeJoin::addJoins(Opt_Table** ptrTable)
{
	// Check if the table is the same with the left query of this join
	if (m_sLeftQueryName.compare((*ptrTable)->getName()) == 0)
	{
		// Add joins
		list<Opt_Column*>::iterator leftIt = mLeftPredicateList.begin();
		list<Opt_Column*>::iterator rightIt = 	mRightPredicateList.begin();
		while ( (leftIt != mLeftPredicateList.end()) && (rightIt != mRightPredicateList.end()))
		{
			Opt_Column* prtLeftCol = *leftIt;
			Opt_Column* prtRightCol = *rightIt;
			
			prtLeftCol->setTableName(m_sLeftQueryName);
			prtRightCol->setTableName(m_sRightQueryName);
			
			Opt_Join* ptrJoin = new Opt_Join(prtLeftCol, prtRightCol);                                                                                              
			(*ptrTable)->addJoin(ptrJoin);
			
			leftIt++;
			rightIt++;
		}
	}
}
