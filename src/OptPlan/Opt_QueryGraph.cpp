/*
 *  Opt_QueryGraph.h
 *  C-Store - Optimimzation
 *
 *  Created by Nga Tran on 10/28/05.
 * 
 *  Questions, ask Nga Tran at nga@brandeis.edu or Tien Hoang at hoangt@brandeis.edu
 *	
 *  This class implements a query_graph.
 *	Graph will be a list of tables. The links between tables are kept int the attributes of the tables
 */

#include "Opt_QueryGraph.h"

// Default constructor
Opt_QueryGraph::Opt_QueryGraph() {
  m_ptrHavingPredicate = NULL;
  m_sQueryName = "";
  m_nCountStar = 0;
}


// Constructor with query name
Opt_QueryGraph::Opt_QueryGraph(string sQueryname) {
  m_ptrHavingPredicate = NULL;
  m_sQueryName = sQueryname;
  m_nCountStar = 0;
}


// Debug constructor
Opt_QueryGraph::Opt_QueryGraph(list<Opt_Table*>  tableList, list<Opt_Column*> groupByList, 
			       list<Opt_Object*> orderByList, Opt_Comparison* ptrHavingPredicate) {
  m_ptrHavingPredicate = NULL;
  mTableList = tableList;
  mGroupByList = groupByList;
  mOrderByList = orderByList;
  m_ptrHavingPredicate = ptrHavingPredicate;
  m_nCountStar = 0;
}


Opt_QueryGraph::Opt_QueryGraph(Query* ptrQuery) {
  m_ptrHavingPredicate = NULL;
  m_sQueryName = "";
  m_nCountStar = 0;
  
  // ptrQuery is of type QSelect
  ListSelections* ptrListSelections = ((QSelect*) ptrQuery)->getSelections();
  ListProjections* ptrListProjections = ((QSelect*) ptrQuery)->getProjections();
  BExpression* ptrWherePredicate = ((QSelect*) ptrQuery)->getWherePredicate();
  ListOrdering* ptrListOrdering = ((QSelect*) ptrQuery)->getOrdering();
  ListCols* ptrGroupColumns = ((QSelect*) ptrQuery)->getGroupCols();
  BExpression* ptrHavingPredicate = ((QSelect*) ptrQuery)->getHavingPredicate();
  
  if (ptrListProjections != NULL) {
    list<Projection*> listProjections = ptrListProjections->getProjections();
    
    // For each projection in the list of projections, create a new object of type
    // Opt_Table and add this new object to the mTableList
    for (list<Projection*>::iterator i = listProjections.begin();
	 i != listProjections.end(); i++) {
      Opt_Table* ptrNewTable = NULL;

      // if there is no alias for table, alias is the same as table name
      string sAlias = (*i)->getAlias();
      if (sAlias.compare("") == 0) {
	sAlias = (*i)->get();
      }

      ptrNewTable = new Opt_Table((*i)->get(), sAlias);
      assert(ptrNewTable);
      mTableList.push_back(ptrNewTable);
    }
  }
  else {
    cout << "Opt_QueryGraph(): from list is empty" << endl;
  }

  // SELECT ...
  if (ptrListSelections != NULL) {
    list<EHolder*> listSelections = ptrListSelections->getSelections();
    list<EHolder*> listAggregates = ptrListSelections->getAggregates();
    
    // For each items in listSelections, get its name and the projection it belongs to
    // Create a new Opt_Column with these info and find the corresponding Opt_Table object
    // from mTableList with the above projection name and add this new Opt_Column to the
    // Opt_Table object
    for (list<EHolder*>::iterator i = listSelections.begin();
	 i != listSelections.end(); i++) {
      // assuming that Expression of type EColumn for now
      Expression* ptrExpression = (*i)->getExpression();
      string sProjectionName =((EColumn*) ptrExpression)->getProjection();
      string sColumnName = ((EColumn*) ptrExpression)->getName();
      string sColumnAlias = (*i)->getAlias();

      // If there is no alias for the column, alias is the same with column name
      if (sColumnAlias.compare("") == 0) {
	sColumnAlias = sColumnName;
      }
      
      Opt_Column* ptrNewColumn = NULL;
      ptrNewColumn = new Opt_Column(sColumnName, sProjectionName, sColumnAlias);
      assert(ptrNewColumn);

      for (list<Opt_Table*>::iterator j = mTableList.begin();
	   j != mTableList.end(); j++) {
	if ( ((*j)->getName()).compare(sProjectionName) == 0) {
	  (*j)->addSelection(ptrNewColumn);
	  break;
	}
      }
    }
    
    for (list<EHolder*>::iterator m = listAggregates.begin();
	 m != listAggregates.end(); m++) {
      // Expression* here is of type EAgg*
      Expression* ptrExpression = (*m)->getExpression();
      EColumn* ptrColumn = ((EAgg*) ptrExpression)->getRight();
      string sOp = ((EAgg*) ptrExpression)->getOp();
      string sAlias = (*m)->getAlias();
      
      string sColumnName = "", sProjectionName = "";
      // if ptrColumn is NULL, it's COUNT (*)
      if (ptrColumn != NULL) {
      	sColumnName = ptrColumn->getName();
	sProjectionName = ptrColumn->getProjection();
      }
      else {
	m_nCountStar = 1;
	continue;
      }

      Opt_Agg* ptrNewAgg = NULL;
      if (sOp.compare("avg") == 0) {
	ptrNewAgg = new Opt_Agg_Avg(sColumnName, sProjectionName);
      }
      else if (sOp.compare("count") == 0) {
	ptrNewAgg = new Opt_Agg_Count(sColumnName, sProjectionName);
      }
      else if (sOp.compare("max") == 0) {
	ptrNewAgg = new Opt_Agg_Max(sColumnName, sProjectionName);
      }
      else if (sOp.compare("min") == 0) {
	ptrNewAgg = new Opt_Agg_Min(sColumnName, sProjectionName);
      }
      else if (sOp.compare("sum") == 0) {
	ptrNewAgg = new Opt_Agg_Sum(sColumnName, sProjectionName);
      }
      else {
	cout << "Opt_QueryGraph(): unknown type of aggregate" << endl;
	exit (1);
      }
      assert(ptrNewAgg);
      ptrNewAgg->setAlias(sAlias);

      for (list<Opt_Table*>::iterator j = mTableList.begin();
	   j != mTableList.end(); j++) {
	if ( ((*j)->getName()).compare(sProjectionName) == 0) {
	  (*j)->addAggregate(ptrNewAgg);
	  break;
	}
      }
    }
  }
  else {
    cout << "Opt_QueryGraph(): select list is empty" << endl;
  }

  // WHERE...
  if (ptrWherePredicate != NULL) {
    int nOr = 0, nNumberOfTables = 0;
    string tables = "";
    
    checkWherePredicate(ptrWherePredicate, &nOr, &nNumberOfTables, &tables);
    
    // empty where predicate
    if (nNumberOfTables == 0) {
      cout << "[Log] --- where predicate is empty" << endl;
    }
    // if only one table in from clause
    else if (nNumberOfTables == 1) { 
      Opt_Table* ptrTable = (Opt_Table*)mTableList.front();
      // set the from clause of that table
      ptrTable->setNoneJoinPredicate(createOnePredicateList(ptrWherePredicate));
    }
    else if (nNumberOfTables >= 2 && nOr == 1) {
      cout << "unsupported query at this point" << endl;
      // unallocate memory
      for (list<Opt_Table*>::iterator i = mTableList.begin();
	   i != mTableList.end(); i++) {
	delete (*i);
      }
      exit(1);
    }    
    else { // if there are more than one tables and no OR
      // set the from clause of each table
      createPredicateList(ptrWherePredicate);
      // create a tree representation for all the where predicates of each tables
      for (list<Opt_Table*>::iterator j = mTableList.begin();
	   j != mTableList.end(); j++) {
	(*j)->createNoneJoinPredicates();
      }  
    }
  }
  else {
    cout << "Opt_QueryGraph(): where predicate is empty" << endl;
  }

  // Order by
  if (ptrListOrdering != NULL) {
    list<ColOrder*> listOrdering = ptrListOrdering->getListOrdering();
    for (list<ColOrder*>::iterator i = listOrdering.begin();
	 i != listOrdering.end(); i++) {
      EColumn* ptrEColumn = (*i)->getColumn();
      Opt_Column* ptrNewColumn = NULL;
      //ptrNewColumn = new Opt_Column(ptrEColumn->getName(), "");
      ptrNewColumn = new Opt_Column(ptrEColumn->getName(), ptrEColumn->getProjection());
      assert(ptrNewColumn);
      mOrderByList.push_back(ptrNewColumn);
    }
  }
  else {
    cout << "[Log] --- order by predicate is empty" << endl;
  }
  
  // Group by
  if (ptrGroupColumns != NULL) {
    list<EColumn*> listGroupBy = ptrGroupColumns->get();
    for (list<EColumn*>::iterator i = listGroupBy.begin();
	 i != listGroupBy.end(); i++) {
      Opt_Column* ptrNewColumn = NULL;
      //ptrNewColumn = new Opt_Column((*i)->getName(), "");
      ptrNewColumn = new Opt_Column((*i)->getName(), (*i)->getProjection());
      assert(ptrNewColumn);
      mGroupByList.push_back(ptrNewColumn);
    }
  }
  else {
    cout << "[Log] --- group by predicate is empty" << endl;
  }
  
  // Having - to be implemented
  if (ptrHavingPredicate != NULL) {
    m_ptrHavingPredicate = NULL;
  }
  else {
    cout << "[Log] --- having predicate is empty" << endl;
  }
  
  cout << "[Log] --- done constructing QueryGraph object from a Query object\n\n";
}


void Opt_QueryGraph::checkWherePredicate(BExpression* ptrWP, int* ptrNOr, int* ptrNTables, string* ptrSTables) {
  if (ptrWP->getOp() == "") {
    ptrNTables = 0;
    return;
  }
  else if (ptrWP->getOp() == "and" || ptrWP->getOp() == "or") {
    if (ptrWP->getOp() == "or")
      *ptrNOr = 1;
    checkWherePredicate(((BBinaryLogic*)ptrWP)->getLeft(), ptrNOr, ptrNTables, ptrSTables);
    checkWherePredicate(((BBinaryLogic*)ptrWP)->getRight(), ptrNOr, ptrNTables, ptrSTables);
  }
  else {
    Expression* ptrLeftParam = ((BComparison*) ptrWP)->getLeft();
    Expression* ptrRightParam = ((BComparison*)  ptrWP)->getRight();
    
    if (ptrLeftParam->isColumn()) {
      string sProjectionName = ((EColumn*) ptrLeftParam)->getProjection();
      if (ptrSTables->find(sProjectionName, 0) == string::npos) { 
	(*ptrSTables) += sProjectionName;
	(*ptrNTables)++;
      }
    }
    if (ptrRightParam->isColumn()) {
      string sProjectionName = ((EColumn*) ptrRightParam)->getProjection();
      if (ptrSTables->find(sProjectionName, 0) == string::npos) { 
	(*ptrSTables) += sProjectionName;
	(*ptrNTables)++;
      }
    }
  }
}


Opt_Comparison* Opt_QueryGraph::createOnePredicateList(BExpression* ptrWherePredicate) {
  string sPredicate = ptrWherePredicate->getOp();
  if (sPredicate.compare("and") == 0) {
    return new Opt_Comparison_And(createOnePredicateList(((BBinaryLogic*) ptrWherePredicate)->getLeft()),
				  createOnePredicateList(((BBinaryLogic*) ptrWherePredicate)->getRight()));
  }
  else if (sPredicate.compare("or") == 0) {
    return new Opt_Comparison_Or(createOnePredicateList(((BBinaryLogic*) ptrWherePredicate)->getLeft()),
				 createOnePredicateList(((BBinaryLogic*) ptrWherePredicate)->getRight()));
  }
  else {
    // there can only be none-join predicates since we have only one table in the from clause
    Expression* ptrLeftParam = ((BComparison*) ptrWherePredicate)->getLeft();
    Expression* ptrRightParam = ((BComparison*)  ptrWherePredicate)->getRight();
    
    string sColumnName = ((EColumn*) ptrLeftParam)->getName();
    string sProjectionName = ((EColumn*) ptrLeftParam)->getProjection();
    string sConstantValue = ptrRightParam->toString();
    
    Opt_Column* ptrColumn = NULL;
    Opt_String* ptrString = NULL;
    ptrColumn = new Opt_Column(sColumnName, sProjectionName);
    ptrString = new Opt_String(sConstantValue);
    assert(ptrColumn);
    assert(ptrString);
    
    Opt_Comparison* ptrComparison = NULL;
    ptrComparison = new Opt_Comparison(ptrColumn, ptrString, sPredicate);
    assert(ptrComparison);
    
    for (list<Opt_Table*>::iterator j = mTableList.begin();
	 j != mTableList.end(); j++) {
      if ( ((*j)->getName()).compare(sProjectionName) == 0) {
	(*j)->addNoneJoin(ptrComparison);
	break;
      }
    }
    return ptrComparison;
  }
}


// This function takes in a BExpression that is the WHERE clause of the original query
// then put the predicates in this BExpression to its proper tables
// Two type of predicates exist:
// 1. Join predicate between two columns of two different projections
// 2. Comparison predicate of any type between any column and a constant value
void Opt_QueryGraph::createPredicateList(BExpression* ptrWherePredicate) {
  string sPredicate = ptrWherePredicate->getOp();
  if (sPredicate.compare("and") == 0 || sPredicate.compare("or") == 0) {
    createPredicateList(((BBinaryLogic*) ptrWherePredicate)->getLeft());
    createPredicateList(((BBinaryLogic*) ptrWherePredicate)->getRight());
  }
  else {
    Expression* ptrLeftParam = ((BComparison*) ptrWherePredicate)->getLeft();
    Expression* ptrRightParam = ((BComparison*)  ptrWherePredicate)->getRight();

    if (ptrLeftParam->isColumn() && ptrRightParam->isColumn()) {
      string sLeftColumnName = ((EColumn*) ptrLeftParam)->getName();
      string sLeftProjectionName = ((EColumn*) ptrLeftParam)->getProjection();
      string sRightColumnName = ((EColumn*) ptrRightParam)->getName();
      string sRightProjectionName = ((EColumn*) ptrRightParam)->getProjection();
      
      Opt_Column* ptrFromColumn = NULL;
      Opt_Column* ptrToColumn = NULL;
      ptrFromColumn = new Opt_Column(sLeftColumnName, sLeftProjectionName);
      ptrToColumn = new Opt_Column(sRightColumnName, sRightProjectionName);
      assert(ptrFromColumn);
      assert(ptrToColumn);
      
      Opt_Join* ptrNewJoinPredicate = NULL;
      ptrNewJoinPredicate = new Opt_Join(ptrFromColumn, ptrToColumn);
      assert(ptrNewJoinPredicate);
            
      for (list<Opt_Table*>::iterator j = mTableList.begin();
           j != mTableList.end(); j++) {
        if ( ((*j)->getName()).compare(sLeftProjectionName) == 0) {
          (*j)->addJoin(ptrNewJoinPredicate);
	  break;
	}
      }
    }
    else {
      string sColumnName = ((EColumn*) ptrLeftParam)->getName();
      string sProjectionName = ((EColumn*) ptrLeftParam)->getProjection();
      string sConstantValue = ptrRightParam->toString(); 
      
      Opt_Column* ptrColumn = NULL;
      Opt_String* ptrString = NULL;
      ptrColumn = new Opt_Column(sColumnName, sProjectionName);
      ptrString = new Opt_String(sConstantValue);
      assert(ptrColumn);
      assert(ptrString);

      Opt_Comparison* ptrComparison = NULL;
      ptrComparison = new Opt_Comparison(ptrColumn, ptrString, sPredicate);
      assert(ptrComparison);
            
      for (list<Opt_Table*>::iterator j = mTableList.begin();
           j != mTableList.end(); j++) {
        if ( ((*j)->getName()).compare(sProjectionName) == 0) {
          (*j)->addNoneJoin(ptrComparison);
	  break;
	}
      }
    }
  }
}


// Default Destructor
Opt_QueryGraph::~Opt_QueryGraph() {
  for (list<Opt_Table*>::iterator i = mTableList.begin();
       i != mTableList.end(); i++) {
    delete (*i);
  }
  
  for (list<Opt_Column*>::iterator i = mGroupByList.begin();
       i != mGroupByList.end(); i++) {
    delete (*i);
  }
  
  for (list<Opt_Object*>::iterator i = mOrderByList.begin();
       i != mOrderByList.end(); i++) {
    delete (*i);
  }  
}

list<string> Opt_QueryGraph::getFactTableList() {
  return mFactTableList;
}

void Opt_QueryGraph::addFactTable(string sFactTable) {
  mFactTableList.push_back(sFactTable);
}

void Opt_QueryGraph::setFactTable() {
  for (list<string>::iterator i = mFactTableList.begin();
       i != mFactTableList.end(); i++) {
    for (list<Opt_Table*>::iterator j = mTableList.begin();
	 j != mTableList.end(); j++) {
      if ((*i).compare((*j)->getName()) == 0) {
	(*j)->setRole(TABLE_ROLE_FACT);
      }
    }
  }
}

void Opt_QueryGraph::resetFactTable() {
  mFactTableList.clear();

  for (list<Opt_Table*>::iterator iter = mTableList.begin(); iter != mTableList.end(); iter++) {
    (*iter)->setRole(TABLE_ROLE_DIMENSION);
  }
}

void Opt_QueryGraph::setCountStar(int nCountStar) {
  m_nCountStar = nCountStar;
}

int Opt_QueryGraph::isCountStar() {
  return m_nCountStar;
}

bool Opt_QueryGraph::isFactTable(string sTableName) {
  for (list<string>::iterator i = mFactTableList.begin();
       i != mFactTableList.end(); i++) {
    if ((*i).compare(sTableName) == 0)
      return 1;
  }
  return 0;
}

list<Opt_Table*> Opt_QueryGraph::getTableList() {
  return mTableList;
}

list<Opt_Column*> Opt_QueryGraph::getGroupByList() {
  return mGroupByList;
}

list<Opt_Object*> Opt_QueryGraph::getOrderByList() {
  return mOrderByList;
}

Opt_Comparison* Opt_QueryGraph::getHavingPredicate() {
  return m_ptrHavingPredicate;
}

string Opt_QueryGraph::getSQLQuery() {
   string sSelect =  "SELECT      \n";
   sSelect += getSQLQueryWithoutSelectWord();
   return sSelect;
}

string Opt_QueryGraph::getSQLQueryWithHint() {
   string sSelect =  "SELECT   /*+ NO_QUERY_TRANSFORMATION */   \n";
   sSelect += getSQLQueryWithoutSelectWord();
   return sSelect;
}

string Opt_QueryGraph::getSQLQueryWithoutSelectWord()
{
  string sSelect =  "            ";
  if (m_nCountStar == 1) {
    sSelect += "COUNT (*), ";
  }
  string sFrom =    "FROM        ";
  string sWhere =   "WHERE       ";
  string sGroupBy = "GROUP BY    ";
  string sOrderBy = "ORDER BY    ";

  for (list<Opt_Table*>::iterator i = mTableList.begin();
       i != mTableList.end(); i ++) {
    // From ...
    sFrom = sFrom + (*i)->getName() + ", ";
    
    list<Opt_Column*> selectionList = (*i)->getSelectionList();
    list<Opt_Agg*> aggregateList = (*i)->getAggregateList();
    list<Opt_Join*> mJoinList = (*i)->getJoinList();
    list<Opt_Comparison*> mNoneJoinList = (*i)->getNoneJoinList();
    Opt_Comparison* ptrNoneJoinPredicates = (*i)->getNoneJoin();

    // Select ...
    for (list<Opt_Column*>::iterator i = selectionList.begin();
	 i != selectionList.end(); i++) {
      sSelect = sSelect + (*i)->toString() + ", ";
    }
    for (list<Opt_Agg*>::iterator i = aggregateList.begin();
	 i != aggregateList.end(); i++) {
      sSelect = sSelect + (*i)->toString() + ", ";
    }
    
    // Where ...
    // currently not supporting OR
    for (list<Opt_Join*>::iterator i = mJoinList.begin();
	 i != mJoinList.end(); i++) {
      sWhere = sWhere + (*i)->toString() + "\n  AND       ";
    }
    
    if (ptrNoneJoinPredicates != NULL) {
      sWhere += getQueryHelper(ptrNoneJoinPredicates);
    }

    /*
    for (list<Opt_Comparison*>::iterator i = mNoneJoinList.begin();
	 i != mNoneJoinList.end(); i++) {
      sWhere = sWhere + (*i)->toString() + "\n  AND       ";
      }*/
  }


  // Group By ...
  for (list<Opt_Column*>::iterator i = mGroupByList.begin();
       i != mGroupByList.end(); i++) {
    sGroupBy = sGroupBy + (*i)->toString() + ", ";
  }
  
  // Order By ...
  for (list<Opt_Object*>::iterator i = mOrderByList.begin();
       i != mOrderByList.end(); i++) {
    sOrderBy = sOrderBy + (*i)->toString() + ", ";
  }
  
  // cleaning up the ends...
  sSelect.erase(sSelect.length() - 2, 1);
  sFrom.erase(sFrom.length() - 2, 1);
  sSelect = sSelect +"\n" + sFrom + "\n";
  if (sWhere.compare("WHERE       ") != 0) {
    sWhere.erase(sWhere.length() - 13, 13);
    sSelect = sSelect + sWhere + "\n";
  }
  
  if (sGroupBy.compare("GROUP BY    ") != 0) { 
    sGroupBy.erase(sGroupBy.length() - 2, 1);
    sSelect = sSelect + sGroupBy + "\n";
  }
  if (sOrderBy.compare("ORDER BY    ") != 0) {
    sOrderBy.erase(sOrderBy.length() - 2, 1);
    sSelect = sSelect + sOrderBy + "\n";
  }
  
  sSelect.erase(sSelect.length() - 1, 1);
  sSelect = sSelect + ";\n";
  return sSelect;
}

string Opt_QueryGraph::getQueryHelper(Opt_Comparison* ptrComp) {
  string sOp = ptrComp->getOp();
  if (sOp == "and") {
    return getQueryHelper((Opt_Comparison*) (ptrComp->getLeft())) + getQueryHelper((Opt_Comparison*) (ptrComp->getRight()));
  }
  else {
    return (ptrComp->getLeft())->toString() + " " + ptrComp->getOp() + " " + (ptrComp->getRight())->toString() + "\n  AND       ";;
  }
}


string Opt_QueryGraph::getNonJoinPredicateString(Opt_Comparison* ptrComp) {
  
  string s = getQueryHelper(ptrComp);  
  s.erase(s.length() - 13, 13);

  return s;
}

string Opt_QueryGraph::toStringNoTableDot() {
  string s = "";
  
  s += "\n+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
  s += "Printing out QUERYGRAPH:\n";
  s += "++++++++++++++++++++++++\n";
 
  for (list<Opt_Table*>::iterator i = mTableList.begin();
       i != mTableList.end(); i++) {
    s += (*i)->toStringNoTableDot();
  }

  s += "\nGroup By:\n";
  for (list<Opt_Column*>::iterator i = mGroupByList.begin();
       i != mGroupByList.end(); i++) {
    s += (*i)->toStringNoTableDot();
  }
  
  s += "\nOrder By:\n";
  for (list<Opt_Object*>::iterator i = mOrderByList.begin();
       i != mOrderByList.end(); i++) {
    s += (*i)->toStringNoTableDot();
  }    

  s += "\n\nFact Tables:\n";
  for (list<string>::iterator i = mFactTableList.begin();
       i != mFactTableList.end(); i++) {
    s = s + (*i) + " ";
  }
  s += "\n+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
  return s;
}


string Opt_QueryGraph::toString() {
  string s = "";
  
  s += "\n+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
  s += "Printing out QUERYGRAPH:\n";
  s += "++++++++++++++++++++++++\n";
 
  for (list<Opt_Table*>::iterator i = mTableList.begin();
       i != mTableList.end(); i++) {
    s += (*i)->toString();
  }

  s += "\nGroup By:\n";
  for (list<Opt_Column*>::iterator i = mGroupByList.begin();
       i != mGroupByList.end(); i++) {
    s += (*i)->toString();
  }
  
  s += "\nOrder By:\n";
  for (list<Opt_Object*>::iterator i = mOrderByList.begin();
       i != mOrderByList.end(); i++) {
    s += (*i)->toString();
  }    

  s += "\n\nFact Tables:\n";
  for (list<string>::iterator i = mFactTableList.begin();
       i != mFactTableList.end(); i++) {
    s = s + (*i) + " ";
  }
  s += "\n+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
  return s;
}

// Add a table
void Opt_QueryGraph::addTable(Opt_Table* ptrTable)
{
	mTableList.push_back(ptrTable);
}


// Add an order by object
void Opt_QueryGraph::addOrderByObject(Opt_Object* ptrObject)
{
  mOrderByList.push_back(ptrObject);
}

// Add an group by column
void Opt_QueryGraph::addGroupByColumn(Opt_Column* ptrCol)
{
  mGroupByList.push_back(ptrCol);
}

// Add a column into the selection list
// return true (1) if the column is added
// Query name is also returned along
bool Opt_QueryGraph::addSelectionColumn(Opt_Column* ptrColumn, string* ptrQueryName)
{
	*ptrQueryName = NOT_QUERY_NAME;
	
	// First look for the table of this column
	list<Opt_Table*>::iterator tableIt;
	for (tableIt = mTableList.begin(); tableIt != mTableList.end(); tableIt++)
	{
		Opt_Table* ptrTable = *tableIt;
		if (ptrColumn->getTableName().compare(ptrTable->getName()) == 0)
		{
			// Table found. Add the column into the selection list of this table
			ptrTable->addSelection(ptrColumn);
			*ptrQueryName = m_sQueryName;
			return 1;
		}
	}
	
	// Table not found
	return 0;
}

// clone the join and add it to this graph if its two join tables belong to this graph
// return true (1) if the join is added
bool Opt_QueryGraph::addJoin(Opt_Join* ptrJoin)
{
	string sFromTable = ptrJoin->getFromColumn()->getTableName();
	string sToTable = ptrJoin->getToColumn()->getTableName();

	int numTablesFound = 0;
	Opt_Table* ptrFromTable = NULL;

	// Go trhough all table
	list<Opt_Table*>::iterator tableIt;
	for (tableIt = mTableList.begin(); tableIt != mTableList.end(); tableIt++)
	{
		Opt_Table* ptrTable = *tableIt;
		if (sFromTable.compare(ptrTable->getName()) == 0)
		{
		  numTablesFound++;
		  ptrFromTable = ptrTable;
		}
		if  (sToTable.compare(ptrTable->getName()) == 0)
		{
		  numTablesFound++;
		}

		if (numTablesFound == 2)
		{
		  break;
		}  

	}

	if ( (numTablesFound == 2) && (ptrFromTable != NULL) )
	{
	  // This join should belong to this query
	  // clone the join first
	  Opt_Join* ptrClonedJoin = ptrJoin->clone();
	  ptrFromTable->addJoin(ptrClonedJoin);
	  return 1;
	}  	

	return 0;
}

// Get the queryGraph name
string Opt_QueryGraph::getQueryName()
{
	return m_sQueryName;
}
  
// Set the queryGraph name
void Opt_QueryGraph::setQueryName(string sQueryName)
{
	m_sQueryName = sQueryName;
}

// Check if a table is a fact table in the graph
bool Opt_QueryGraph::isFactTableInTable(string sTableName)
{
	// Search sTableName
	list<Opt_Table*>::iterator tableIt;
	for (tableIt = mTableList.begin(); tableIt != mTableList.end(); tableIt++)
	{
		Opt_Table* ptrTable = *tableIt;
		if ( (ptrTable->getName()).compare(sTableName) == 0)
		{
			// Found
			return ptrTable->isFactTable();
		}
	}
	
	// Not found --> not FACT table
	return 0;
}

// Return table of a tablename
Opt_Table* Opt_QueryGraph::getTable(string sTableName)
{
	// Go through all tables in the graph
	list<Opt_Table*>::iterator it;
	for (it = mTableList.begin(); it != mTableList.end(); it++)
	{
		// Get the table name
		string sCurrentTableName = (*it)->getName();
		if(sTableName.compare(sCurrentTableName) == 0)
		{
			// Found
			return (*it);
		}
	}
	
	// Not found
	return NULL;
}

// Set number of rows and cardinality for a table
void  Opt_QueryGraph::setTableInfo(string sTableName, long lRowNum, long lCardinality)
{
  // Go through all tables in the graph
	list<Opt_Table*>::iterator it;
	for (it = mTableList.begin(); it != mTableList.end(); it++)
	{
		// Get the table name
		string sCurrentTableName = (*it)->getName();
		if(sTableName.compare(sCurrentTableName) == 0)
		{
			// Found
		  (*it)->setRowNum(lRowNum);
		  (*it)->setCardinality(lCardinality);
		  return;
		}
	}
}

  
// Check if a table is in the graph
bool Opt_QueryGraph::isInGraph(string sTableName)
{
	// Go through all tables in the graph
	list<Opt_Table*>::iterator it;
	for (it = mTableList.begin(); it != mTableList.end(); it++)
	{
		// Get the table name
		string sCurrentTableName = (*it)->getName();
		if(sTableName.compare(sCurrentTableName) == 0)
		{
			// Found
			return 1;
		}
	}
	
	// Not found
	return 0;
}

// Check if a table is in the graph. If yes, return the query name as well
bool Opt_QueryGraph::isInGraph(string sTableName, string* ptrQueryName)
{
	*ptrQueryName = NOT_QUERY_NAME;
	
	bool bIsInGraph = isInGraph(sTableName);
	if (bIsInGraph)
	{
		*ptrQueryName = m_sQueryName;
	}
	
	return bIsInGraph;
}

 // Get all joins that have not marked snowflake join poiting to ptrTable
list<Opt_Join*> Opt_QueryGraph::getJoinsTo(Opt_Table* ptrTargetTable)
{
   return getJoinsTo(ptrTargetTable->getName());
}

// Get all joins that have not marked snowflake join poiting to ptrTable
list<Opt_Join*> Opt_QueryGraph::getJoinsTo(string sTableName)
{
  list<Opt_Join*> joinToList;

   // Go through all tables of this graph
   list<Opt_Table*>::iterator tableIt;
   for (tableIt = mTableList.begin(); tableIt != mTableList.end(); tableIt++)
   {
	Opt_Table* ptrTable = *tableIt;
     // Get all joins going from this table
     list<Opt_Join*>  joinList = ptrTable->getJoinList();
     list<Opt_Join*>::iterator joinIt;
     
     for (joinIt = joinList.begin(); joinIt != joinList.end(); joinIt++)
     {
	Opt_Join* ptrJoin = *joinIt;
			
	// check if the table on the other side of the join is the target table
	if ( (ptrJoin->getToColumn())->getTableName().compare( sTableName) == 0)
	{
	  // check if this join is not a snowflake join
	  if (!ptrJoin->isSnowflakesJoin())
	  {
	      // Add it to the return list
	      joinToList.push_back(ptrJoin);
	      // enough, do not add duplicate (same source table and same target table)
	      break;
	  }
	}  
     }	  
   }

   return  joinToList;
}

// Get all tables that link to a given table
list<Opt_Table*> Opt_QueryGraph::getPointerTablesTo(string sTableName)
{
  list<Opt_Table*> tables;

   // Go through all tables of this graph
   list<Opt_Table*>::iterator tableIt;
   for (tableIt = mTableList.begin(); tableIt != mTableList.end(); tableIt++)
   {
	Opt_Table* ptrTable = *tableIt;
     // Get all joins going from this table
     list<Opt_Join*>  joinList = ptrTable->getJoinList();
     list<Opt_Join*>::iterator joinIt;
     
     for (joinIt = joinList.begin(); joinIt != joinList.end(); joinIt++)
     {
	Opt_Join* ptrJoin = *joinIt;
			
	// check if the table on the other side of the join is the target table
	if ( (ptrJoin->getToColumn())->getTableName().compare( sTableName) == 0)
	{
	  // check if this join is not a snowflake join
	  if (!ptrJoin->isSnowflakesJoin())
	  {
	      // Add it to the return list
	      tables.push_back(ptrTable);
	      // enough, do not add duplicate (same source table and same target table)
	      break;
	  }
	}  
     }	  
   }

   return  tables;
}


// Get all tables that link to a given table
list<string>  Opt_QueryGraph::getStringTablesTo(string sTableName)
{
  list<string> tables;

   // Go through all tables of this graph
   list<Opt_Table*>::iterator tableIt;
   for (tableIt = mTableList.begin(); tableIt != mTableList.end(); tableIt++)
   {
	Opt_Table* ptrTable = *tableIt;
     // Get all joins going from this table
     list<Opt_Join*>  joinList = ptrTable->getJoinList();
     list<Opt_Join*>::iterator joinIt;
     
     for (joinIt = joinList.begin(); joinIt != joinList.end(); joinIt++)
     {
	Opt_Join* ptrJoin = *joinIt;
			
	// check if the table on the other side of the join is the target table
	if ( (ptrJoin->getToColumn())->getTableName().compare( sTableName) == 0)
	{
	  // check if this join is not a snowflake join
	  if (!ptrJoin->isSnowflakesJoin())
	  {
	      // Add it to the return list
	      tables.push_back(ptrTable->getName());
	      // enough, do not add duplicate (same source table and same target table)
	      break;
	  }
	}  
     }	  
   }

   return  tables;
}

  

// Get all tables that are linked from a given table
list<string> Opt_QueryGraph::getTablesFrom(string sTableName)
{
  list<string> tables;
  
  // Get table of this table name
  Opt_Table* ptrTable = getTable(sTableName);
  
  if (ptrTable != NULL)
  {
    // Get all joins going from this table
    list<Opt_Join*>  joinList = ptrTable->getJoinList();
    list<Opt_Join*>::iterator joinIt;     
    for (joinIt = joinList.begin(); joinIt != joinList.end(); joinIt++)
      {
	Opt_Join* ptrJoin = *joinIt;
	tables.push_back((ptrJoin->getToColumn())->getTableName());
      }
  }

  return tables;
}

// CLone this graph
// Also in this clone grpah mark snowflake join for joins if they point to the same table as ptrNonSnowflakeJoin but go from different table
Opt_QueryGraph* Opt_QueryGraph::clone(Opt_Join* ptrJoin)
{
	// Clone name and countStar
	string sQueryName(m_sQueryName);
  
	Opt_QueryGraph* ptrGraph = new Opt_QueryGraph(sQueryName);
	ptrGraph->m_nCountStar = m_nCountStar;
	
	// Clone Fact tables
	list<string>::iterator stringIt;
	for (stringIt = mFactTableList.begin(); stringIt != mFactTableList.end(); stringIt++)
	{
		string sFactTable(*stringIt);
		ptrGraph->addFactTable(sFactTable);
	}

	// CLone many to many joins
	map<string, string>::iterator nnIt;
	for (nnIt = mManyToManyJoinList.begin(); nnIt != mManyToManyJoinList.end(); nnIt++)
	  {
	    ptrGraph->addManyToManyJoin((*nnIt).first, (*nnIt).second);
	  }
	
	// Clone Table list
	list<Opt_Table*>::iterator tableIt;
	for (tableIt = mTableList.begin(); tableIt != mTableList.end(); tableIt++)
	{
		Opt_Table* ptrTable = (*tableIt);
		Opt_Table* ptrClonedTable = NULL;
		if (ptrTable != NULL)
		{
		  ptrClonedTable = ptrTable->clone(ptrJoin);		  
		  ptrGraph->addTable(ptrClonedTable);
		}
	}

	// CLone group  by list
	list<Opt_Column*>::iterator columnIt;
	for (columnIt = mGroupByList.begin(); columnIt != mGroupByList.end(); columnIt++)
	{
		Opt_Column* ptrColumn = (*columnIt);
		Opt_Column* ptrClonedColumn = NULL;
		if (ptrColumn != NULL)
		{
			ptrClonedColumn = ptrColumn->clone();
			ptrGraph->addGroupByColumn(ptrClonedColumn);
		}
	}

	// CLone order by list
	list<Opt_Object*>::iterator objectIt;
	for (objectIt = mOrderByList.begin(); objectIt != mOrderByList.end(); objectIt++)
	{
		Opt_Object* ptrObject = (*objectIt);
		Opt_Object* ptrClonedObject = NULL;
		if (ptrObject != NULL)
		{
			ptrClonedObject = ptrObject->clone();
			ptrGraph->addOrderByObject(ptrClonedObject);
		}
	}

	// Clone having predicate
	if (m_ptrHavingPredicate != NULL)
	{
		ptrGraph->m_ptrHavingPredicate = m_ptrHavingPredicate->clone();       
	}  

	// Clone replica Tables
	// ...

	// CLone check replica table
	// ...

	return ptrGraph;  
}


 // Get name of the replica table
string  Opt_QueryGraph::addReplicaTableName(string tableName)
{
   string sReplicaTableName(tableName);
   int numReplica = 1;

   // Go through all available replica
   list<ReplicaTable*>::iterator replicaIt;
   for (replicaIt = mReplicaTableList.begin(); replicaIt != mReplicaTableList.end(); replicaIt++)
   {
     ReplicaTable* ptrReplica = *replicaIt;
     if (sReplicaTableName.compare(ptrReplica->m_sName) == 0)
     {
       // This table was replicated before, increase the number of replica
       ptrReplica->m_iNumReplica++;
       numReplica =  ptrReplica->m_iNumReplica;
       break;
     } 
   }

   // the replica is not in the list yet, add it
   if (numReplica == 1)
   {
     ReplicaTable* ptrReplica = new ReplicaTable();
     ptrReplica->m_sName = tableName;
     ptrReplica->m_iNumReplica = 1;

     mReplicaTableList.push_back(ptrReplica);
   }

   char numReplicaInChar[4];
   sprintf( numReplicaInChar, "%d", numReplica);
   sReplicaTableName = sReplicaTableName.append("_");
   sReplicaTableName = sReplicaTableName.append(numReplicaInChar);

   return sReplicaTableName;
}

// all tables are not checked replica yet
void  Opt_QueryGraph::initializeCheckReplica()
{
  list<Opt_Table*>::iterator tableIt;
  for (tableIt = mTableList.begin(); tableIt != mTableList.end(); tableIt++)
  {
    CheckedReplicaTable* ptrTable = new CheckedReplicaTable();
    ptrTable->m_sTableName = (*tableIt)->getName();
    ptrTable->m_bIsChecked = 0;
    mCheckReplicaTableList.push_back(ptrTable);
  }
}


// Return replica if possible
// A replica is a equivalent query of this query but added all possible replica of tables
Opt_QueryGraph*  Opt_QueryGraph::getReplica()
{
  // Get exact copy first
  Opt_QueryGraph* ptrReplica = clone(NULL);
  if (ptrReplica->doReplica())
  {
    // Having the replica
    return ptrReplica;
  }

  // No replica
  delete ptrReplica;
  return NULL;
}

// Do replica for this query
int Opt_QueryGraph::doReplica()
{
  initializeCheckReplica();
  int hasReplica = 0;

  // Go through all tables
  list<Opt_Table*>::iterator tableIt;
  for (tableIt = mTableList.begin(); tableIt != mTableList.end(); tableIt++)
  {
    Opt_Table* ptrTable = (*tableIt);
    //    if (!isChecked(ptrTable->getName()))
    //    {
      if (checkReplica(ptrTable))
      {  
	hasReplica = 1;
      }
      //    }
  }

  return hasReplica;
}

int  Opt_QueryGraph::isChecked(string tableName)
{
  int isChecked = 0;

  // Serach for the table
  list<CheckedReplicaTable*>::iterator tableIt;
  for (tableIt = mCheckReplicaTableList.begin(); tableIt != mCheckReplicaTableList.end(); tableIt++)
  {
    CheckedReplicaTable* ptrCheckedTable = (*tableIt);
    if (ptrCheckedTable->m_sTableName.compare(tableName) == 0)
      {
	isChecked = ptrCheckedTable->m_bIsChecked;

	// Check it
	ptrCheckedTable->m_bIsChecked = 1;
	return isChecked;
      }
  }

  return isChecked;
}

// Check replica for a table
// CASE (1): A.x = B.y and A.x = C.z --> Add EITHER (C1 and B.y = C1.z) OR (B1 and C.z = B1.y)
//           Assume .  A.x = B.y always before  A.x = C.z 
//                     Choose  Add (C1 and B.y = C1.z)
// CASE (2): A.x = B.y AND B.y = C.z --> Add C1 (replica of C) and A.x = C1.z
int Opt_QueryGraph::checkReplica(Opt_Table* ptrTable)
{
  int hasReplica = 0;

  Opt_Table* ptrTableA = ptrTable;

  // See if this table has been checked
  if (isChecked(ptrTableA->getName()))
  {
    return hasReplica;
  }

  // Get join list of table A
  list<Opt_Join*> A_JoinList = ptrTableA->getJoinList();

  // The array to store all joins from A for doing CASE(1)
  int numJoinsFromA = A_JoinList.size();
  Opt_Join* A_JoinArray[numJoinsFromA];
  int i = 0;

  // DO CASE (2) first

  // Go through all joins starting from  table A
  list<Opt_Join*>::iterator A_JoinIt;
  for (A_JoinIt = A_JoinList.begin(); A_JoinIt != A_JoinList.end(); A_JoinIt++)
  {
    // Store for doing CASE (!) later
    A_JoinArray[i] = (*A_JoinIt);
    i++;

    // Get column x
    Opt_Column* ptrColX  = (*A_JoinIt)->getFromColumn();

    // Get column y
    Opt_Column* ptrColY  = (*A_JoinIt)->getToColumn();

    // Get table on the other side of the join( table B of column y)
    Opt_Table* ptrTableB = getTable(ptrColY->getTableName());

    if (ptrTableB != NULL)
    {
       // Get join list of table B
      list<Opt_Join*> B_JoinList = ptrTableB->getJoinList();  

       // Go through all joins starting from  table B
      list<Opt_Join*>::iterator B_JoinIt;
      for (B_JoinIt = B_JoinList.begin(); B_JoinIt != B_JoinList.end(); B_JoinIt++)
	{
	  // Get column y1
	  Opt_Column* ptrColY1  = (*B_JoinIt)->getFromColumn();
	 
	  // Check if y and y1 are the same
	  if (ptrColY->getColumnName().compare(ptrColY1->getColumnName()) == 0)
	   { 
	      // Replicate

	      // Get column z
 	      Opt_Column* ptrColZ  = (*B_JoinIt)->getToColumn();

	      // Get table on the other side of the join( table C of column z)
	      Opt_Table* ptrTableC = getTable(ptrColZ->getTableName());

	      // Check replica of table C first
	      checkReplica(ptrTableC);

	      // add a replica C1 and A.x = C1.z
	      replicate(ptrTableA, ptrColX, ptrTableC, ptrColZ);

	      hasReplica = 1;
	   }	  
	}
    }     
 }

 // DO CASE (1)
 for (i = 0; i < numJoinsFromA ; i++)
 {
   // Get column x
   Opt_Column* ptrColX  = A_JoinArray[i]->getFromColumn();

   // Get column y
   Opt_Column* ptrColY  = A_JoinArray[i]->getToColumn();

   // Get table on the other side of the join( table B of column y)
   Opt_Table* ptrTableB = getTable(ptrColY->getTableName());

   for (int j = i+1; j < numJoinsFromA ; j++)
   {
     // Get column x1
     Opt_Column* ptrColX1  = A_JoinArray[j]->getFromColumn();

     // CHeck if x and x1 are the same
     if (ptrColX->getColumnName().compare(ptrColX1->getColumnName()) == 0)
     { 
	// Replicate

	// Get column z
 	Opt_Column* ptrColZ  = A_JoinArray[j]->getToColumn();

	// Get table on the other side of the join( table C of column z)
	Opt_Table* ptrTableC = getTable(ptrColZ->getTableName());

	// Check replica of table C first
	checkReplica(ptrTableC);

	// add a replica C1 and A.x = C1.z
	replicate(ptrTableB, ptrColY, ptrTableC, ptrColZ);

         hasReplica = 1;
     }
   }
 }  
 
  
  return hasReplica;
}

// add a replica C1 of  table C and join C1 to source table A
void Opt_QueryGraph::replicate(Opt_Table* ptrSource, Opt_Column* ptrColSource, Opt_Table* ptrTableReplica, Opt_Column* ptrColReplica)
{
  // Get name for this replica table
  string replicaTableName = addReplicaTableName(ptrTableReplica->getName());

  // Add a join from source to the replica
  Opt_Column* ptrNewSource =  ptrColSource->clone();
  Opt_Column* ptrReplicaCol =  ptrColReplica->clone();
  ptrReplicaCol->setTableName(replicaTableName);
  Opt_Join* ptrNewJoin = new Opt_Join(ptrNewSource, ptrReplicaCol);
  ptrSource->addJoin(ptrNewJoin);

  // Clone to create a replica
  Opt_Table* ptrClonedTable = ptrTableReplica->cloneForReplica( replicaTableName,  replicaTableName);

  // Add this table into the queryq
  addTable(ptrClonedTable);

  // Replicate all tables joining from ptrTableReplica (recursion)  
  // Get join list of Replicate table
  list<Opt_Join*> joinList = ptrTableReplica->getJoinList();
  
  // Go through all joins
  list<Opt_Join*>::iterator joinIt;
  for (joinIt = joinList.begin(); joinIt != joinList.end(); joinIt++)
  {
     // Get source column
    Opt_Column* ptrColFrom  = ((*joinIt)->getFromColumn())->clone();
    ptrColFrom->setTableName(replicaTableName);

    // Get destination column
    Opt_Column* ptrColTo  = (*joinIt)->getToColumn();

    // Get table on the other side of the join( table B of column y)
    Opt_Table* ptrTableTo = getTable(ptrColTo->getTableName());

    replicate(ptrClonedTable, ptrColFrom, ptrTableTo, ptrColTo);
  }
} 

// Get replica table names
list<ReplicaTable*> Opt_QueryGraph::getReplicaTables()
{
  return mReplicaTableList;
}

// set many to many join
void Opt_QueryGraph::addManyToManyJoin(string sFromTable, string sToTable)
{
  mManyToManyJoinList[sFromTable] = sToTable;
}

bool  Opt_QueryGraph::isManyToManyJoin(string sFromTable, string sToTable)
{
  map<string, string>::iterator it;
  for (it = mManyToManyJoinList.begin(); it != mManyToManyJoinList.end(); it++)
    {      
      if ( ( ( (*it).first.compare(sFromTable) == 0) && ((*it).second.compare(sToTable) == 0) ) ||
	   ( ( (*it).first.compare(sToTable) == 0) && ((*it).second.compare(sFromTable) == 0) ) )
	{
	  return 1; // found
	}
      
    }

  return 0; // not found
}

// Get a list of joins between 2 plans
list<Opt_Join*> Opt_QueryGraph::getJoinsForPlans(Opt_Plan* ptrLeftPlan, Opt_Plan* ptrRightPlan)
{
  list<Opt_Join*> joins;

   // Go through all tables of this graph
   list<Opt_Table*>::iterator tableIt;
   for (tableIt = mTableList.begin(); tableIt != mTableList.end(); tableIt++)
   {
	Opt_Table* ptrTable = *tableIt;
	// Get all joins going from this table
	list<Opt_Join*>  joinList = ptrTable->getJoinList();
	list<Opt_Join*>::iterator joinIt;
     
	for (joinIt = joinList.begin(); joinIt != joinList.end(); joinIt++)
	  {
	    Opt_Join* ptrJoin = *joinIt;

	    // Check if 2 tables of the join belong to left and right plans acordingly
	    if ( (ptrLeftPlan->isInPlan(ptrJoin->getFromColumn()->getTableName()) ) &&
		 (ptrRightPlan->isInPlan(ptrJoin->getToColumn()->getTableName()) ))
	      {
		joins.push_back(ptrJoin);
	      }			
	    else if ( (ptrRightPlan->isInPlan(ptrJoin->getFromColumn()->getTableName()) ) &&
		   (ptrLeftPlan->isInPlan(ptrJoin->getToColumn()->getTableName())  ) )
	      {
		Opt_Join* ptrReversedJoin = new Opt_Join(ptrJoin->getToColumn(), ptrJoin->getFromColumn());
		joins.push_back(ptrReversedJoin);
	      }
	  }	  
   }

   return  joins;
  
}


// Get information for final clauses
void Opt_QueryGraph::getSelectInfo(list<Opt_Column*> *selections, list<Opt_Agg*> *aggregates, 
				  list<Opt_Column*> *groupBys, list<Opt_Object*> *orderBys)
{
  for (list<Opt_Table*>::iterator i = mTableList.begin(); i != mTableList.end(); i ++) 
  {
    list<Opt_Column*> selectionList = (*i)->getSelectionList();
    list<Opt_Agg*> aggregateList = (*i)->getAggregateList();

    (*selections).merge(selectionList);
    (*aggregates).merge(aggregateList);
  }

  // Group By
  *groupBys = mGroupByList;

  // Order By
  *orderBys = mOrderByList;
}

void Opt_QueryGraph::resetTraversed()
{
   // Go through alll tables
     list<Opt_Table*>::iterator tableIt;
     for (tableIt = mTableList.begin(); tableIt != mTableList.end(); tableIt++)
     {
       Opt_Table* ptrTable = *tableIt;
       // get joins of this table
       list<Opt_Join*> joinList = ptrTable->getJoinList();
       list<Opt_Join*>::iterator joinIt;
       for (joinIt = joinList.begin(); joinIt != joinList.end(); joinIt++)
	 {
	   Opt_Join* ptrJoin = *joinIt;
	   ptrJoin->resetTraversed();
	 }

     }
}

