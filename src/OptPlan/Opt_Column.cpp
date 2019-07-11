/*
 *  Opt_Column.h
 *  C-Store - Optimimzation
 *
 *  Created by Nga Tran on 10/28/05.
 * 
 *  Questions, ask Nga Tran at nga@brandeis.edu or Tien Hoang at hoangt@brandeis.edu
 *	
 *  This class implements an attribute of a table
 */

#include "Opt_Column.h"
#include <iostream>

using namespace std;

// Default constructor
Opt_Column::Opt_Column() {
  m_sColumnName = "";	
  m_sTableName = "";
  m_sColumnAlias = "";
  m_sTableAlias = "";
}
		
// Provide column and table names
Opt_Column::Opt_Column(string sColumnName, string sTableName) {
  m_sColumnName = sColumnName;	
  m_sTableName = sTableName;
  m_sColumnAlias = sColumnName;	
  m_sTableAlias = sTableName;
}

// Provide column and table names and column alias
Opt_Column::Opt_Column(string sColumnName, string sTableName, string sColumnAlias) {
  m_sColumnName = sColumnName;	
  m_sTableName = sTableName;
  m_sColumnAlias = sColumnAlias;	
  m_sTableAlias = "";
}
	
// Provide column and table names and their alias
Opt_Column::Opt_Column(string sColumnName, string sTableName, string sColumnAlias, string sTableAlias) {
  m_sColumnName = sColumnName;	
  m_sTableName = sTableName;
  m_sColumnAlias = sColumnAlias;	
  m_sTableAlias = sTableAlias;
}

// Default Destructor
Opt_Column::~Opt_Column()
{
}

// Clone
Opt_Column* Opt_Column::clone()
{
	return new Opt_Column(m_sColumnName, m_sTableName, m_sColumnAlias, m_sTableAlias);
}

// Clone with new table names, new table alias
Opt_Column* Opt_Column::clone(string sTableName, string sTableAlias)
{
	return new Opt_Column(m_sColumnName, sTableName, m_sColumnAlias, sTableAlias);
}
		
// Get and set functions
void Opt_Column::setColumnName(string sColumnName)
{
	m_sColumnName = sColumnName;
}


string Opt_Column::getColumnName()
{	
	return m_sColumnName;
}
		
void Opt_Column::setTableName(string sTableName)
{
	m_sTableName = sTableName;
}

string Opt_Column::getTableName()
{
	return m_sTableName;
}
		
void Opt_Column::setColumnAlias(string sColumnAlias)
{
	m_sColumnAlias = sColumnAlias;
}

string Opt_Column::getColumnAlias()
{
	return m_sColumnAlias;
}
		
void Opt_Column::setTableAlias(string sTableAlias)
{
	m_sTableAlias = sTableAlias;
}

string Opt_Column::getTableAlias()
{
	return m_sTableAlias;
}

string Opt_Column::toStringNoTableDot() {
  return m_sColumnName;
}
		
string Opt_Column::toString() {
  string s = "";

  if (m_sTableName.compare("") != 0) {
    s = m_sTableName + ".";
  }
  s += m_sColumnName;

  return s;
}

// Check if this object is a column
bool Opt_Column::isColumn()
{
	return 1; // true
}
