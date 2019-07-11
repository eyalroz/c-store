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

#include "Opt_Table.h"
#include <cassert>
#include <iostream>

using namespace std;
using std::cout;

// Default constructor
Opt_Table::Opt_Table() {
  m_sName = "";
  m_sAlias = "";
  m_iRole = TABLE_ROLE_DIMENSION;
  m_ptrNoneJoinPredicates = NULL;
  m_lRowNum = 0;
  m_lCardinality = 0;
}

Opt_Table::Opt_Table(string sName) {
  m_sName = sName;
  m_sName = "";
  m_iRole = TABLE_ROLE_DIMENSION;
  m_ptrNoneJoinPredicates = NULL;
  m_lRowNum = 0;
  m_lCardinality = 0;
}

Opt_Table::Opt_Table(string sName, string sAlias) {
  m_sName = sName;
  m_sAlias = sAlias;
  m_iRole = TABLE_ROLE_DIMENSION;
  m_ptrNoneJoinPredicates = NULL;
  m_lRowNum = 0;
  m_lCardinality = 0;
}

// Default Destructor
Opt_Table::~Opt_Table() {
  for (list<Opt_Column*>::iterator i = mSelectionList.begin();
       i != mSelectionList.end(); i++) {
    delete (*i);
  }

  for (list<Opt_Agg*>::iterator i = mAggregateList.begin();
       i != mAggregateList.end(); i++) {
    delete (*i);
  }
  
  for (list<Opt_Join*>::iterator i = mJoinList.begin();
       i != mJoinList.end(); i++) {
    delete (*i);
  }
  
  for (list<Opt_Comparison*>::iterator i = mNoneJoinList.begin();
       i != mNoneJoinList.end(); i++) {
    delete (*i);
  }
  
  if (m_ptrNoneJoinPredicates != NULL) {
    Opt_Comparison* ptrTemp;
    while (m_ptrNoneJoinPredicates->getOp() == "and" || m_ptrNoneJoinPredicates->getOp() == "or") {
      ptrTemp = m_ptrNoneJoinPredicates;
      m_ptrNoneJoinPredicates = (Opt_Comparison*) m_ptrNoneJoinPredicates->getRight();
      delete ptrTemp;
    }
  }
}

// same as setName
void Opt_Table::setTableName(string sTableName) {
  m_sName = sTableName;
}

void Opt_Table::setName(string sName) {
  m_sName = sName;
}

string Opt_Table::getName() {
  return m_sName;
}

void Opt_Table::setAlias(string sAlias) {
  m_sAlias = sAlias;
}

string Opt_Table::getAlias() {
  return m_sAlias;
}

void Opt_Table::setRole(int iRole) {
  m_iRole = iRole;
}

int Opt_Table::getRole() {
  return m_iRole;
}

void Opt_Table::setRowNum(long lRowNum)
{
  m_lRowNum = lRowNum;
}

long  Opt_Table::getRowNum()
{
  return m_lRowNum;
}

void Opt_Table::setCardinality(long lCardinality)
{
  m_lCardinality = lCardinality;
}

long  Opt_Table::getCardinality()
{
  return m_lCardinality;
}
  

long double  Opt_Table::getSelectivity()
{
  if (m_lRowNum == 0)
    return 1;

  // selectivity is from 0 to 1
  long double sel = (long double) m_lCardinality;
  sel = sel / m_lRowNum;

  return sel;
}

list<Opt_Column*> Opt_Table::getSelectionList() {
  return mSelectionList;
}

list<Opt_Agg*> Opt_Table::getAggregateList() {
  return mAggregateList;
}

list<Opt_Join*> Opt_Table::getJoinList() {
  return mJoinList;
}

list<Opt_Comparison*> Opt_Table::getNoneJoinList() {
  return mNoneJoinList;
}

Opt_Comparison* Opt_Table::getNoneJoin() {
  return m_ptrNoneJoinPredicates;
}

void Opt_Table::addSelection(Opt_Column* ptrColumn) {
  if (ptrColumn == NULL) {
    cout << "cannot added a NULL Opt_Column to Opt_Table" << endl;
    return;
  }

  // Check if this column exists
  list<Opt_Column*>::iterator it;
  for (it = mSelectionList.begin(); it != mSelectionList.end(); it++)
    {
      Opt_Column* ptrCol = *it;
      if (ptrCol->getColumnName().compare(ptrColumn->getColumnName()) == 0)
	{
	  // exist
	  return;
	}
    }

  // Not added yet
  mSelectionList.push_back(ptrColumn);
}

void Opt_Table::addAggregate(Opt_Agg* ptrAgg) {
  if (ptrAgg == NULL) {
    cout << "cannot added a NULL Opt_Column to Opt_Table" << endl;
    return;
  }
  mAggregateList.push_back(ptrAgg);
}

void Opt_Table::addJoin(Opt_Join* ptrJoin) {
  if (ptrJoin == NULL) {
    cout << "cannot added a NULL Opt_Column to Opt_Table" << endl;
    return;
  }
  mJoinList.push_back(ptrJoin);
}

void Opt_Table::addNoneJoin(Opt_Comparison* ptrNoneJoin) {
  mNoneJoinList.push_back(ptrNoneJoin);
}

void Opt_Table::setNoneJoinPredicate(Opt_Comparison* ptrComparison) {
  m_ptrNoneJoinPredicates = ptrComparison;
}

// Return a string presentation
string Opt_Table::toStringNoTableDot()
{
  string s = "";
  
  s += "\n=========================================================\n";
  s += m_sName;
  if (m_iRole == TABLE_ROLE_FACT) {
    s += " (F)";
  }
  s += "\n-------------\n";

  s += "Selection: \n";
  for (list<Opt_Column*>::iterator i = mSelectionList.begin();
       i != mSelectionList.end(); i++) {
    s = s + (*i)->toStringNoTableDot() + ", ";
  }
  
  s += "\n\nAggregate: \n";
  for (list<Opt_Agg*>::iterator i = mAggregateList.begin();
       i != mAggregateList.end(); i++) {
    s = s + (*i)->toStringNoTableDot() + ", ";
  }

  s += "\n\nJoin Predicate: \n";
  for (list<Opt_Join*>::iterator i = mJoinList.begin();
       i != mJoinList.end(); i++) {

    s = s + (*i)->toStringNoTableDot() + ", ";
  }

  s += "\n\nNone-Join Predicate: \n";
  for (list<Opt_Comparison*>::iterator i = mNoneJoinList.begin();
       i != mNoneJoinList.end(); i++) {
    s = s + (*i)->toStringNoTableDot() + ", ";
  }
  
  s += "\n=========================================================\n\n";
  return s;
}


string Opt_Table::toString() {
  string s = "";
  
  s += "\n=========================================================\n";
  s += m_sName;
  if (m_iRole == TABLE_ROLE_FACT) {
    s += " (F)";
  }
  s += "\n-------------\n";

  s += "Selection: \n";
  for (list<Opt_Column*>::iterator i = mSelectionList.begin();
       i != mSelectionList.end(); i++) {
    s = s + (*i)->toString() + ", ";
  }
  
  s += "\n\nAggregate: \n";
  for (list<Opt_Agg*>::iterator i = mAggregateList.begin();
       i != mAggregateList.end(); i++) {
    s = s + (*i)->toString() + ", ";
  }

  s += "\n\nJoin Predicate: \n";
  for (list<Opt_Join*>::iterator i = mJoinList.begin();
       i != mJoinList.end(); i++) {

    s = s + (*i)->toString() + ", ";
  }

  s += "\n\nNone-Join Predicate: \n";
  for (list<Opt_Comparison*>::iterator i = mNoneJoinList.begin();
       i != mNoneJoinList.end(); i++) {
    s = s + (*i)->toString() + ", ";
  }
  
  s += "\n=========================================================\n\n";
  return s;
}

void Opt_Table::createNoneJoinPredicates() {
  if (mNoneJoinList.size() == 0) {
    return;
  }
  else if (mNoneJoinList.size() == 1) {
    m_ptrNoneJoinPredicates = (Opt_Comparison*) (mNoneJoinList.front());
  }
  else {
    m_ptrNoneJoinPredicates = new Opt_Comparison_And(NULL, NULL);
    assert(m_ptrNoneJoinPredicates);
    Opt_Comparison_And* ptrTemp = (Opt_Comparison_And*) m_ptrNoneJoinPredicates;
    
    list<Opt_Comparison*>::iterator i = mNoneJoinList.begin();
    m_ptrNoneJoinPredicates->setLeft(*i);
    while (1) {
      i++;i++;
      if (i == mNoneJoinList.end()) {
	i--;
	ptrTemp->setRight(*i);
	break;
      }
      else {
	i--;
	Opt_Comparison_And* ptrNextAnd = new Opt_Comparison_And(*i, NULL);
	assert(ptrNextAnd);
	ptrTemp->setRight(ptrNextAnd);
	ptrTemp=ptrNextAnd;
      }
    }
  }
}

string Opt_Table::printNoneJoin() {
  return m_ptrNoneJoinPredicates->toString();
}

// Half Clone: Cloned table only has the same role, name and m_ptrNoneJoinPredicates
Opt_Table* Opt_Table::cloneForReplica(string sTableName, string sTableAlias)
{
	string sName(sTableName);
	string sAlias(sTableAlias);

	// Clone name and role
	Opt_Table* ptrTable = new Opt_Table(sName, sAlias);
	ptrTable->m_iRole = m_iRole;
	ptrTable->m_lRowNum = m_lRowNum;
	ptrTable->m_lCardinality = m_lCardinality;
 
	// Clone non join predicate
	Opt_Comparison*	ptrNoneJoinPredicates =  NULL;
	if (m_ptrNoneJoinPredicates != NULL) ptrNoneJoinPredicates = m_ptrNoneJoinPredicates->clone(sName, sAlias);
	ptrTable->setNoneJoinPredicate(ptrNoneJoinPredicates);

	return ptrTable;
}


// Half Clone: Cloned table only has the same role, name, mSelectionList and m_ptrNoneJoinPredicates
Opt_Table* Opt_Table::cloneHalf()
{
	string sName(m_sName);
	string sAlias(m_sAlias);

	// Clone name and role
	Opt_Table* ptrTable = new Opt_Table(sName, sAlias);
	ptrTable->m_iRole = m_iRole;
	ptrTable->m_lRowNum = m_lRowNum;
	ptrTable->m_lCardinality = m_lCardinality;
	
	// Clone selection list
	list<Opt_Column*>::iterator it;
	for (it = mSelectionList.begin(); it != mSelectionList.end(); it++)
	{
		Opt_Column* ptrCol = (*it);
		Opt_Column* ptrClonedCol = NULL;
		if (ptrCol != NULL)
		{
			ptrClonedCol = ptrCol->clone();
			ptrTable->addSelection(ptrClonedCol);
		}
	}
  
	// Clone non join predicate
	Opt_Comparison*	ptrNoneJoinPredicates =  NULL;
	if (m_ptrNoneJoinPredicates != NULL) ptrNoneJoinPredicates = m_ptrNoneJoinPredicates->clone();
	ptrTable->setNoneJoinPredicate(ptrNoneJoinPredicates);

	return ptrTable;
}

// CLone this table
Opt_Table* Opt_Table::clone()
{
  return clone(NULL);
}

// CLone this table
// Also in this clone table mark snowflake join for joins if they point to the same table as ptrNonSnowflakeJoin but go from different table
Opt_Table* Opt_Table::clone(Opt_Join* ptrNonSnowflakeJoin)
{
	// Do half clone first
	Opt_Table* ptrTable = cloneHalf();

	// Clone aggregate list
	list<Opt_Agg*>::iterator aggIt;
	for (aggIt = mAggregateList.begin(); aggIt != mAggregateList.end(); aggIt++)
	{
		Opt_Agg* ptrAgg = (*aggIt);
		Opt_Agg* ptrClonedAgg = NULL;
		if (ptrAgg != NULL)
		{
			ptrClonedAgg = ptrAgg->clone();
			ptrTable->addAggregate(ptrClonedAgg);
		}
	}

	// Clone non join predicate list
	list<Opt_Comparison*>::iterator nonJoinIt;
	for (nonJoinIt = mNoneJoinList.begin(); nonJoinIt != mNoneJoinList.end(); nonJoinIt++)
	{
		Opt_Comparison* ptrPredicate = (*nonJoinIt);
		Opt_Comparison* ptrClonePredicate = NULL;
		if (ptrPredicate != NULL)
		{
			ptrClonePredicate = ptrPredicate->clone();
			ptrTable->addNoneJoin(ptrClonePredicate);
		}
	  }

	// Clone join list
	list<Opt_Join*>::iterator joinIt;
	for (joinIt = mJoinList.begin(); joinIt != mJoinList.end(); joinIt++)
	{
		Opt_Join* ptrJoin = (*joinIt);
		Opt_Join* ptrClonedJoin = NULL;
		if (ptrJoin != NULL)
		{
		   if (ptrNonSnowflakeJoin != NULL)
			ptrClonedJoin = ptrJoin->clone(ptrNonSnowflakeJoin);
		   else
		     ptrClonedJoin = ptrJoin->clone();

		  ptrTable->addJoin(ptrClonedJoin);
		}
	}

	return ptrTable;
}

// Check if this is a fact table
bool Opt_Table::isFactTable()
{
	if (m_iRole == TABLE_ROLE_FACT) return 1;
	else return 0;
}
