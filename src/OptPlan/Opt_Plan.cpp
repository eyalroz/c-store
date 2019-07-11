/*
 *  Opt_Plan.h
 *  C-Store - Optimimzation
 *
 *  Created by Nga Tran on 09/19/06
 * 
 *  Questions, ask Nga Tran at nga@brandeis.edu
 *	
 *  This class implements a plan
 *	. Plan is a binary tree (bushy tree)
 *	. Simplest plan: has only 1 table
 *      . Normal plan: has 2 children: left plan and right plan
 */

#include "Opt_Plan.h"

// Default constructor
Opt_Plan::Opt_Plan()
{
  m_ptrSourceQuery = NULL;
  m_ptrTable = NULL;
  m_ptrLeft = NULL;
  m_ptrRight = NULL;
  m_bIsReferentialIntegrity = 1;
}


Opt_Plan::Opt_Plan(Opt_Table* ptrTable)
{
  m_ptrSourceQuery = NULL;
  m_ptrTable = ptrTable;
  m_ptrLeft = NULL;
  m_ptrRight = NULL;
  m_bIsReferentialIntegrity = 1;
  m_ldCost = -1;
}


Opt_Plan::Opt_Plan(Opt_Plan* ptrLeft, Opt_Plan* ptrRight)
{
  m_ptrSourceQuery = NULL;
  m_ptrTable = NULL;
  m_ptrLeft = ptrLeft;
  m_ptrRight = ptrRight;
  m_bIsReferentialIntegrity = 1;
  m_ldCost = -1;
}

Opt_Plan::Opt_Plan(Opt_Plan* ptrLeft, Opt_Plan* ptrRight, bool bRefInt)
{
  m_ptrSourceQuery = NULL;
  m_ptrTable = NULL;
  m_ptrLeft = ptrLeft;
  m_ptrRight = ptrRight;
  m_bIsReferentialIntegrity = bRefInt;
  m_ldCost = -1;
}

// Default Destructor
Opt_Plan::~Opt_Plan()
{
  if (m_ptrLeft != NULL) delete m_ptrLeft;
  if (m_ptrRight != NULL) delete m_ptrRight;
}

// set Referential Integrity
void Opt_Plan::setReferentialIntegrity()
{
  m_bIsReferentialIntegrity = 1;
}


void Opt_Plan::unsetReferentialIntegrity()
{
  m_bIsReferentialIntegrity = 0;
}

// Get cost related values
long Opt_Plan::getCardinality()
{
  if (m_ptrTable != NULL)
  {
    return m_ptrTable->getCardinality();
  } 
  else
  { 
    if ((m_ptrLeft != NULL) && (m_ptrRight != NULL) ) 
    {
      if (m_bIsReferentialIntegrity) 
      {
	long card = (long) (((long double) m_ptrLeft->getCardinality()) * m_ptrRight->getSelectivity());
	return card;
      }
      else return m_ptrLeft->getCardinality();
    }
    else 
    {
      cout << "ERROR: Invalid plan" << endl;
    }
  }

  return 0;
}


long double Opt_Plan::getSelectivity()
{
  if (m_ptrTable != NULL)
  {
    return m_ptrTable->getSelectivity();
  } 
  else
  { 
    if ((m_ptrLeft != NULL) &&(m_ptrRight != NULL) ) 
    {
      if (m_bIsReferentialIntegrity) 
      {
	long double sel =  (m_ptrLeft->getSelectivity()) * (m_ptrRight->getSelectivity());
	return sel;
      }
      else return m_ptrLeft->getSelectivity();
    }
    else 
    {
      cout << "ERROR: Invalid plan" << endl;
    }
  }

  return 0;
}


long double Opt_Plan::getCost()
{  
  if (m_ldCost >= 0)
    return m_ldCost; // Computed before

  if (m_ptrTable != NULL)
  {
    m_ldCost = 0;
  } 
  else
  { 
    if ((m_ptrLeft != NULL) && (m_ptrRight != NULL) ) 
    {
      m_ldCost = m_ptrLeft->getCost() + m_ptrRight->getCost() + (m_ptrLeft->getCardinality() * m_ptrRight->getCardinality());
    }
    else 
    {
      cout << "ERROR: Invalid plan. Cannot get cost" << endl;
    }
  }

  return m_ldCost;
}


// Combine subplans
Opt_Plan* Opt_Plan::combineSubPlans(Opt_QueryGraph* ptrQueryGraph, list<Opt_Plan*> subPlans)
{
  // Get the anchor table: biggest (first) fact table
  //cout << "Opt_Plan: BEFORE ptrQueryGraph->getFactTableList()\n";
  list<string> factTables = ptrQueryGraph->getFactTableList();
  if (factTables.size() == 0)
  {
    cout << "Cannot get anchor table of the original query (no fact table has been defined)" << endl;
    return NULL;
  } 

  string sAnchorTable = *(factTables.begin());

  // Copy all subplans
  //cout << "Opt_Plan: BEFORE  copyPlans\n";
  list<Opt_Plan*> plans = copyPlans(subPlans);
  //cout << "Opt_Plan: DONE  copyPlans\n";



  // Get the plan consisting of the anchor table
  Opt_Plan* ptrAnchorPlan = NULL;
  list<Opt_Plan*>::iterator planIt = plans.begin();
  for (unsigned int i = 0; i < subPlans.size(); i++)
    {
      if ((*planIt)->isInPlan(sAnchorTable))
	{
	  ptrAnchorPlan = (*planIt);
	  plans.remove(*planIt);
	  break;
	}
      planIt++;
    }

  //cout << "Opt_Plan: DONE  Get the plan consisting of the anchor table\n";

  if (ptrAnchorPlan == NULL)
    {
      cout << "Anchor plan is NULL" << endl;
      return NULL;
    }

  // Make a plan which starts from the anchor plan and go through all links
  // Plan is a binary tree of left plan and right plan. Right plan must have selectivity < 1 
  Opt_Plan* ptrCombinedPlan = makePlanFromStartPlan(ptrAnchorPlan, plans, ptrQueryGraph);
  //cout << "Opt_Plan: DONE  makePlanFromStartPlan\n";
  
  // Get not-in-combine-plan plans
  list<Opt_Plan*> remainingPlans;
  list<Opt_Plan*>::iterator it;
  for (it = plans.begin(); it != plans.end(); it++)
  {
     // the plan is not in the combine plan yet
    if (!belongTo(*it, ptrCombinedPlan))
       remainingPlans.push_back(*it);
  }
  //cout << "Opt_Plan: DONE  remainingPlans\n";

  // Add non-in-plan-yet plan to the combined plan
  //cout << "Opt_Plan: Number of remaining plans: " << remainingPlans.size() << endl;
  while(remainingPlans.size() > 0)
  {
    while(remainingPlans.size() > 0)
      {    
	unsigned int previousSize = remainingPlans.size();
	//cout << "PREVIOUS SIZE: " <<  previousSize << endl;
	for (unsigned int i = 0; i <  previousSize; i++)
	  {
	    //cout << "i: " << i << endl;
	    
	    // Take the front table out
	    Opt_Plan* ptrPlan = remainingPlans.front();
	    remainingPlans.pop_front();

	    // Check if this plan is linked to at least one of tables in the combined plan
	    if (!isLinkedFrom(ptrPlan,ptrCombinedPlan, ptrQueryGraph))
	      {
		// Not linked with the combined plan yet, wait for its other linked plans added in the combined plan first
		remainingPlans.push_back(ptrPlan);
	      }
	    else
	      {
		Opt_Plan* ptrNewPlan = new Opt_Plan(ptrCombinedPlan, ptrPlan, 0);
		ptrCombinedPlan = ptrNewPlan;
		//cout << "Opt_Plan: DONE with a remaining plan\n";
	      }
	  }
	
	if (remainingPlans.size() == previousSize)
	  {
	    // No link from combinedplan to the remaining plan
	    break;
	  }    
      }

    while (remainingPlans.size() > 0)
      {
	unsigned int previousSize = remainingPlans.size();
	//cout << "PREVIOUS SIZE: " <<  previousSize << endl;
	for (unsigned int i = 0; i <  previousSize; i++)
	  {
	    //cout << "i: " << i << endl;

	    // Take the front table out
	    Opt_Plan* ptrPlan = remainingPlans.front();
	    remainingPlans.pop_front();
	   
	    // Check if this plan links to at least one of tables in the combined plan
	    if (!isLinkedFrom(ptrCombinedPlan, ptrPlan,ptrQueryGraph))
	      {
		// Not linked with the combined plan yet, wait for its other linked plans added in the combined plan first
		remainingPlans.push_back(ptrPlan);
	      }
	    else
	      {
		Opt_Plan* ptrNewPlan = new Opt_Plan(ptrPlan, ptrCombinedPlan,  0);
		ptrCombinedPlan = ptrNewPlan;
		//cout << "Opt_Plan: DONE with a remaining plan\n";
	      }
	  }

	if (remainingPlans.size() == previousSize)
	  {
	    // No link from combinedplan to the remaining plan
	    break;
	  } 
      }
  }
  ptrCombinedPlan->m_ptrSourceQuery = ptrQueryGraph;
     return ptrCombinedPlan;
}


// make plan staring from a given plan
Opt_Plan* Opt_Plan::makePlanFromStartPlan(Opt_Plan* ptrAnchorPlan, list<Opt_Plan*> plans, Opt_QueryGraph* ptrQueryGraph)
{
  Opt_Plan* ptrReturnPlan = NULL;

  // Get all plans which are linked from this plan
  list<Opt_Plan*> linkedPlans = getPlansFrom(ptrAnchorPlan, plans, ptrQueryGraph);

  if (linkedPlans.size() == 0)
  {
    // No link from this plan
    return ptrAnchorPlan;
  }  
  else
  {
    Opt_Plan* newLinkedPlans[linkedPlans.size()];

    //  repeat to make plans for all linked plans
    list<Opt_Plan*>::iterator it = linkedPlans.begin();
    for (unsigned int i = 0; i < linkedPlans.size(); i++)
    {
      Opt_Plan* ptrPlan = *it;
      list<Opt_Plan*> otherPlans = minorCopy(ptrPlan, plans);

      newLinkedPlans[i] =makePlanFromStartPlan(*it, otherPlans, ptrQueryGraph);
      it++;
    }

    // Sort plans in selectivity ascending order
    // If same selectivity, sort in ascending cardinality order
    for (unsigned int i = 0; i < linkedPlans.size()-1; i++)
    {
      for (unsigned int j = i + 1; j < linkedPlans.size(); j++)
      {
	if ( (newLinkedPlans[i]->getSelectivity() > newLinkedPlans[j]->getSelectivity()) ||
	     ( (newLinkedPlans[i]->getSelectivity() == newLinkedPlans[j]->getSelectivity()) && 
	       (newLinkedPlans[i]->getCardinality() > newLinkedPlans[j]->getCardinality()) ) )
	{
	  // Swap
	  Opt_Plan* plan = newLinkedPlans[i];
	  newLinkedPlans[i] = newLinkedPlans[j];
	  newLinkedPlans[j] = plan;
	}
      }
    }

    // Make plan in selectivity orders. Only plans which have selectivities < 1 are put together
    
    Opt_Plan* leftPlan = ptrAnchorPlan;
    for (unsigned int i = 0; i < linkedPlans.size(); i++)
    {
        long double s = newLinkedPlans[i]->getSelectivity();
        if (s == 1)
	  break; // no more good plans
      
        // ignore  newLinkedPlans[i] if at least one of the tables in it is already in leftPlan
        if (belongTo( newLinkedPlans[i], leftPlan))
	  continue;

	// Make a plan which is a parent of the leftPlan and plans[i]
	bool referentialIntegrity = 1;
	// Check if this to plans are joined by many to many join
	string sLeftTable = leftPlan->getTopLeftTable();
	string sRightTable = newLinkedPlans[i]->getTopLeftTable();
	
	//if( (sLeftTable.compare("customer")== 0) && (sRightTable.compare("supplier")==0))
	//  cout << endl << "CUSTOMER & SUPPLIER  1----------------" << endl;

	if (ptrQueryGraph->isManyToManyJoin(sLeftTable, sRightTable))
	  {
	  referentialIntegrity = 0;
	  //cout << endl << "FOUND 1----------------" << endl;
	  }

	Opt_Plan* plan = new Opt_Plan(leftPlan, newLinkedPlans[i], referentialIntegrity);
	leftPlan = plan;      
    }

    ptrReturnPlan = leftPlan;
  }  

  return ptrReturnPlan;
  
} 


// make plan for a given query graph
void Opt_Plan::makePlan(Opt_QueryGraph* ptrQueryGraph, Opt_QueryGraph* ptrOriginalQueryGraph)
{
  m_ptrSourceQuery = ptrOriginalQueryGraph;

  // Get the anchor table: biggest (first) fact table
  list<string> factTables = ptrQueryGraph->getFactTableList();
  if (factTables.size() == 0)
  {
    cout << "Cannot get anchor table (no fact table has been defined)" << endl;
  } 

  string sAnchorTable = *(factTables.begin());

  // Make a plan which starts from the anchor table and go through all links
  // Plan is a binary tree of left plan and right plan. Right plan must have selectivity < 1 
  makePlan(sAnchorTable, ptrQueryGraph, ptrOriginalQueryGraph);
  
  
  // Get not-in-plan tables
  list<Opt_Table*> remainingTables;
  list<Opt_Table*> allTables = ptrQueryGraph->getTableList();
  list<Opt_Table*>::iterator it;
  for (it = allTables.begin(); it != allTables.end(); it++)
  {
     Opt_Table* ptrTable = *it;
     // the table is not in the plan yet
     if (!isInPlan(ptrTable))
       remainingTables.push_back(ptrTable);
  }

  // Add non-in-plan-yet tables to the plan
  while(remainingTables.size() > 0)
  {    
    // Take the front table out
    Opt_Table* ptrTable = remainingTables.front();
    remainingTables.pop_front();

    // Check if this table is linked to at least one of tables in the plan

    // Get a list of tables linked to and from this table
    list<string> linkedTablesTo = ptrQueryGraph->getStringTablesTo(ptrTable->getName());
    list<string>::iterator sit;    

    // CHeck if at least one of the linked tables is in the plan
    bool added = 0;
    for (sit = linkedTablesTo.begin(); sit != linkedTablesTo.end(); sit++)
    {
      string sTable = *sit;
      if (isInPlan(sTable))
      {
	// its linked table in plan --> add it in the plan
	Opt_Plan* leftPlan = clone();
	Opt_Plan* rightPlan = new Opt_Plan(ptrTable);
	reset(this);
	m_ptrLeft = leftPlan;
	m_ptrRight = rightPlan;
	m_bIsReferentialIntegrity = 0;
	added = 1;
	break;
      }
    }


    list<string> linkedTablesFrom =  ptrQueryGraph->getTablesFrom(ptrTable->getName());
    // CHeck if at least one of the linked tables is in the plan
    for (sit = linkedTablesFrom.begin(); sit != linkedTablesFrom.end(); sit++)
    {
      string sTable = *sit;
      if (isInPlan(sTable))
      {
	// its linked table in plan --> add it in the plan
	Opt_Plan* rightPlan = clone();
	Opt_Plan* leftPlan = new Opt_Plan(ptrTable);
	reset(this);
	m_ptrLeft = leftPlan;
	m_ptrRight = rightPlan;
	m_bIsReferentialIntegrity = 0;
	added = 1;
	break;
      }
    }
   
    if (!added)
    {
      // Not linked with any tables in plan, wait for its linked table added in plan first
      remainingTables.push_back(ptrTable);
    }

  }

}  

// make plan for a given table in a query graph
void Opt_Plan::makePlan(string sTableName, Opt_QueryGraph* ptrQueryGraph, Opt_QueryGraph* ptrOriginalQueryGraph)
{
  // Reset all values
  reset(this);

  // Get table pointer
  Opt_Table* ptrTable = ptrQueryGraph->getTable(sTableName);
  if (ptrTable == NULL)
  {
    cout << "No ptrTable of " << sTableName << " found" << endl;
    return;
  }

  // Get all tables which are linked from this tables
  list<string> linkedTables = ptrQueryGraph->getTablesFrom(sTableName);

  if (linkedTables.size() == 0)
  {
    // One-table plan
    m_ptrTable = ptrTable;
  }  
  else
  {
    //  make plans for all linked tables
    Opt_Plan* plans[linkedTables.size()];

    list<string>::iterator it = linkedTables.begin();
    for (unsigned int i = 0; i < linkedTables.size(); i++)
    {
      plans[i] = new Opt_Plan();
      plans[i]->makePlan(*it, ptrQueryGraph, ptrOriginalQueryGraph);
      it++;
    }

    // Sort plans in selectivity ascending order
    // If same selectivity, sort in ascending cardinality order
    for (unsigned int i = 0; i < linkedTables.size()-1; i++)
    {
      for (unsigned int j = i + 1; j < linkedTables.size(); j++)
      {
	if ( (plans[i]->getSelectivity() > plans[j]->getSelectivity()) ||
	     ( (plans[i]->getSelectivity() == plans[j]->getSelectivity()) && (plans[i]->getCardinality() > plans[j]->getCardinality()) ) )
	{
	  // Swap
	  Opt_Plan* plan = plans[i];
	  plans[i] = plans[j];
	  plans[j] = plan;
	}
      }
    }

    // Make plan in selectivity orders. Only plans which have selectivities < 1 are put together
    // make plan for the given table first    
    Opt_Plan* leftPlan = new Opt_Plan(ptrTable);
    bool planMade = 0;
    for (unsigned int i = 0; i < linkedTables.size(); i++)
    {
      long double s = plans[i]->getSelectivity();
      if (s == 1)
	break; // no more good plans
      else
      {
	// ignore plans[i] if at least one of the tables in it is already in leftPlan
	if (belongTo(plans[i], leftPlan))
	  continue;

	planMade = 1;

	bool referentialIntegrity = 1;
	// Check if this to plans are joined by many to many join
	string sLeftTable = leftPlan->getTopLeftTable();
	string sRightTable = plans[i]->getTopLeftTable();

        //if( (sLeftTable.compare("customer")== 0) && (sRightTable.compare("supplier")==0))
	//  cout << endl << "CUSTOMER & SUPPLIER ----------------" << endl;

	if (ptrOriginalQueryGraph->isManyToManyJoin(sLeftTable, sRightTable))
	  {
	    referentialIntegrity = 0;
	    //cout << endl << "FOUND ----------------" << endl;
	  }

	if ( (i == linkedTables.size() - 1) || (plans[i+1]->getSelectivity() == 1) )
	  {
	    // The last one need to be added
	    m_ptrLeft = leftPlan;
	    m_ptrRight = plans[i];
	    m_bIsReferentialIntegrity = referentialIntegrity;
	  }
	else
	  {
	    // Make a plan which is a parent of the leftPlan and plans[i]       
	    Opt_Plan* plan = new Opt_Plan(leftPlan, plans[i], referentialIntegrity);
	    leftPlan = plan;      
	  }
      }
    }

    if (!planMade)
      {
	reset(this);
	m_ptrTable = ptrTable;
	delete leftPlan;
      }
    else if (m_ptrLeft == NULL)
      {
	m_ptrLeft = leftPlan->m_ptrLeft;
	m_ptrRight = leftPlan->m_ptrRight;
	m_bIsReferentialIntegrity = leftPlan->m_bIsReferentialIntegrity;
      }

  }  
} 


// reset member values
void Opt_Plan::reset(Opt_Plan* ptrPlan)
{
   // Reset all values
    ptrPlan->m_ptrTable = NULL;
    ptrPlan->m_ptrLeft = NULL;
    ptrPlan->m_ptrRight = NULL;
} 

// Get all tables in this plan
list<Opt_Table*> Opt_Plan::getPointerTablesInPlan()
{
  list<Opt_Table*> tables;

  if (m_ptrTable != NULL)
  {
    tables.push_back(m_ptrTable);
  }
  else if ( (m_ptrLeft != NULL) && (m_ptrRight != NULL) )
  {
    list<Opt_Table*> leftTables = m_ptrLeft->getPointerTablesInPlan();
    list<Opt_Table*> rightTables = m_ptrRight->getPointerTablesInPlan();

    tables.merge(leftTables);
    tables.merge(rightTables);    
  }

  return tables;
}


// Get all tables in this plan
list<string> Opt_Plan::getTablesInPlan()
{
  list<string> tables;

  if (m_ptrTable != NULL)
  {
    tables.push_back(m_ptrTable->getName());
  }
  else if ( (m_ptrLeft != NULL) && (m_ptrRight != NULL) )
  {
    list<string> leftTables = m_ptrLeft->getTablesInPlan();
    list<string> rightTables = m_ptrRight->getTablesInPlan();

    tables.merge(leftTables);
    tables.merge(rightTables);    
  }

  return tables;
}

// Check if a given table is in the plan
bool Opt_Plan::isInPlan(Opt_Table* ptrTable)
{
  if (m_ptrTable != NULL)
  {
    if (m_ptrTable == ptrTable)
      return 1;
    else
      return 0;
  }
  else
  {
    if ( (m_ptrLeft != NULL) && (m_ptrRight != NULL) )
    {
      if (m_ptrLeft->isInPlan(ptrTable))
	return 1;
      else
	return m_ptrRight->isInPlan(ptrTable);
    }
  }
  return 0;
}


// Check if a given table is in the plan
bool Opt_Plan::isInPlan(string sTable)
{
  if (m_ptrTable != NULL)
  {
    if (m_ptrTable->getName().compare(sTable) == 0)
      return 1;
    else
      return 0;
  }
  else
  {
    if ( (m_ptrLeft != NULL) && (m_ptrRight != NULL) )
    {
      if (m_ptrLeft->isInPlan(sTable))
	return 1;
      else
	return m_ptrRight->isInPlan(sTable);
    }
  }
  return 0;
}

// CLone this plan
Opt_Plan* Opt_Plan::clone()
{
  Opt_Plan* plan = new Opt_Plan();
  plan->m_ptrTable = m_ptrTable;
  plan->m_ptrLeft = m_ptrLeft;
  plan->m_ptrRight = m_ptrRight;
  plan->m_bIsReferentialIntegrity = m_bIsReferentialIntegrity;
  return plan;
}

// get plan tree
string Opt_Plan::getTree(string tab)
{
  string s;

  if (m_ptrTable != NULL)
  {
    s.append(tab);
    s.append(m_ptrTable->getName());
    s.append("\n");
  }
  else
  {
    if ( (m_ptrLeft != NULL) && (m_ptrRight != NULL) )
    {
      tab = tab + "  ";
      s.append(m_ptrLeft->getTree(tab));
      s.append(m_ptrRight->getTree(tab));
    }
  }

  return s;
}

// Print the plan to standard output
void Opt_Plan::print(string tab)
{
  if (m_ptrTable != NULL)
  {
    //cout << tab << "TABLE: " << endl;
    cout << tab << m_ptrTable->getName() << endl;
    //cout << tab << "-- Sel: " << m_ptrTable->getSelectivity() << endl;
    //cout << tab << "-- Card: " << m_ptrTable->getCardinality() << endl;
    //cout << tab << "-- RowNum: " << m_ptrTable->getRowNum() << endl;
  }
  else
  {
    if ( (m_ptrLeft != NULL) && (m_ptrRight != NULL) )
    {
      tab = tab + "  ";
      //cout << tab << "LEFT: " << endl;
      // cout << tab << "-- Sel: " << m_ptrLeft->getSelectivity() << endl;
      //cout << tab << "-- Card: " << m_ptrLeft->getCardinality() << endl;
      //cout << tab << "-- Cost: " << m_ptrLeft->getCost() << endl;
      //cout << tab << "-- RI: " << m_ptrLeft->m_bIsReferentialIntegrity << endl;
      m_ptrLeft->print(tab);

      //cout << tab << "RIGHT: " << endl;
      //cout << tab << "-- Sel: " << m_ptrRight->getSelectivity() << endl;
      //cout << tab << "-- Card: " << m_ptrRight->getCardinality() << endl;
      //cout << tab << "-- Cost: " << m_ptrRight->getCost() << endl;
      //cout << tab << "-- RI: " << m_ptrRight->m_bIsReferentialIntegrity << endl;
      m_ptrRight->print(tab);
    }
  }

}

/*
// Print the plan to standard output
void Opt_Plan::print(string tab)
{
  if (m_ptrTable != NULL)
  {
    cout << tab << "TABLE: " << endl;
    cout << tab << m_ptrTable->getName() << endl;
    cout << tab << "-- Sel: " << m_ptrTable->getSelectivity() << endl;
    cout << tab << "-- Card: " << m_ptrTable->getCardinality() << endl;
    cout << tab << "-- RowNum: " << m_ptrTable->getRowNum() << endl;
  }
  else
  {
    if ( (m_ptrLeft != NULL) && (m_ptrRight != NULL) )
    {
      tab = tab + "  ";
      cout << tab << "LEFT: " << endl;
      cout << tab << "-- Sel: " << m_ptrLeft->getSelectivity() << endl;
      cout << tab << "-- Card: " << m_ptrLeft->getCardinality() << endl;
      cout << tab << "-- Cost: " << m_ptrLeft->getCost() << endl;
      cout << tab << "-- RI: " << m_ptrLeft->m_bIsReferentialIntegrity << endl;
      m_ptrLeft->print(tab);

      cout << tab << "RIGHT: " << endl;
      cout << tab << "-- Sel: " << m_ptrRight->getSelectivity() << endl;
      cout << tab << "-- Card: " << m_ptrRight->getCardinality() << endl;
      cout << tab << "-- Cost: " << m_ptrRight->getCost() << endl;
      cout << tab << "-- RI: " << m_ptrRight->m_bIsReferentialIntegrity << endl;
      m_ptrRight->print(tab);
    }
  }

}
*/

// Check if at least one of the tables in ptrNewPlan is already in ptrPlan
bool Opt_Plan::belongTo(Opt_Plan* ptrNewPlan, Opt_Plan* ptrPlan)
{
  list<Opt_Table*> tables = ptrNewPlan->getPointerTablesInPlan();
  list<Opt_Table*>::iterator it;
  for (it = tables.begin(); it != tables.end(); it++)
    {
      if (ptrPlan->isInPlan(*it))
	return 1; // found		       
    }

  // not found
  return 0;
}

// Copy plan
list<Opt_Plan*> Opt_Plan::copyPlans(list<Opt_Plan*> plans)
{
  list<Opt_Plan*> newPlans;
  list<Opt_Plan*>::iterator it;
  for (it = plans.begin(); it != plans.end(); it++)
    {
      newPlans.push_back(*it);
    }

  return newPlans;
}

// return copies of all plans except ptrPlan
list<Opt_Plan*> Opt_Plan::minorCopy(Opt_Plan* ptrPlan, list<Opt_Plan*> plans)
{
  list<Opt_Plan*> newPlans;
  list<Opt_Plan*>::iterator it;
  for (it = plans.begin(); it != plans.end(); it++)
    {
      Opt_Plan* ptrItPlan = *it;
      if (ptrItPlan != ptrPlan)
	newPlans.push_back(ptrItPlan);
    }

  return newPlans;
}


// Get all plans which are linked from ptrAnchorPlan
list<Opt_Plan*> Opt_Plan::getPlansFrom(Opt_Plan* ptrAnchorPlan, list<Opt_Plan*> plans, Opt_QueryGraph* ptrQueryGraph)
{
  // Get all tables in the anchor plan
  list<string> anchorPlanTables = ptrAnchorPlan->getTablesInPlan();

  // Get all tables linked from anchorPlanTables
  list<string> linkedTables;
  list<string>::iterator it;
  for (it = anchorPlanTables.begin(); it != anchorPlanTables.end(); it++)
    {
      list<string> tables = ptrQueryGraph->getTablesFrom(*it);
      linkedTables.merge(tables);
    }

  // Get plans containing at least one of the linked table
  list<Opt_Plan*> linkedPlans;
  list<Opt_Plan*>::iterator planIt;
  for (planIt = plans.begin(); planIt != plans.end(); planIt++)
    {
      for(it = linkedTables.begin(); it != linkedTables.end(); it++)
	{
	  Opt_Plan* ptrPlan = *planIt;
	  string sTable = *it;
	  if (ptrPlan->isInPlan(sTable))
	    {
	      linkedPlans.push_back(ptrPlan);
	      break;
	    }
	}
    }

  return linkedPlans;
  
}


// Check if ptrPlan is linked from ptrAnchorPlan
bool Opt_Plan::isLinkedFrom(Opt_Plan* ptrPlan, Opt_Plan* ptrAnchorPlan, Opt_QueryGraph* ptrQueryGraph)
{
  // FROM
  // Get all tables in the anchor plan
  list<string> anchorPlanTables = ptrAnchorPlan->getTablesInPlan();

  // Get all tables linked from anchorPlanTables
  list<string> linkedTables;
  list<string>::iterator it;
  for (it = anchorPlanTables.begin(); it != anchorPlanTables.end(); it++)
    {
      list<string> tables = ptrQueryGraph->getTablesFrom(*it);
      linkedTables.merge(tables);
    }

  //  Get plans containing at least one of the linked table
  for(it = linkedTables.begin(); it != linkedTables.end(); it++)
    {
      if (ptrPlan->isInPlan(*it))
	{
	  return 1;// is in
	}
    }
  /*
  // TO
  // Get all tables in the  plan
  list<string> planTables = ptrPlan->getTablesInPlan();

  // Get all tables linked from anchorPlanTables
  list<string> linkedPlanTables;
  for (it = planTables.begin(); it != planTables.end(); it++)
    {
      list<string> tables = ptrQueryGraph->getTablesFrom(*it);
      linkedPlanTables.merge(tables);
    }

  //  Get plans containing at least one of the linked table
  for(it = linkedPlanTables.begin(); it != linkedPlanTables.end(); it++)
    {
      if (ptrAnchorPlan->isInPlan(*it))
	{
	  return 1;// is in
	}
    }
  */
  return 0; // not in
  
}


// Get top left table
string Opt_Plan::getTopLeftTable()
{
  if (m_ptrTable != NULL)
    return m_ptrTable->getName();
  else if (m_ptrLeft != NULL)
    return m_ptrLeft->getTopLeftTable();

  cout << "Invalid plan. Cannot get top left table" << endl;
  return "";

}


// Make join between 2 subplans
void Opt_Plan::makeJoins(Opt_QueryGraph* ptrQueryGraph)
{
  if (m_ptrTable != NULL)
    return; // single-table plan

  if ( (m_ptrLeft != NULL) && (m_ptrRight != NULL) )
    {
      m_ptrLeft->makeJoins(ptrQueryGraph);
      m_ptrRight->makeJoins(ptrQueryGraph);

      m_joins = ptrQueryGraph->getJoinsForPlans(m_ptrLeft, m_ptrRight);
    }
  else
    {
      cout << "Invalid plan. Cannot make joins" << endl;
    }
}

// Write SQL to a file + a lot of views
string Opt_Plan::toFile(string sFileName)
{
  string sTree = getTree("--   ");
  string s = getOneSQL();
  //string s = getSQL();
 
  try
    {
      ofstream fout(sFileName.c_str(), ios::out);     
      fout << sTree << endl << endl;
      fout << s  << endl;
      fout.close();
    } 
  catch (std::exception&)
    {
      cout << "Cannot open: " << sFileName << " for storing coressponding query" << endl;
    }
 
  return s;
}

// Get only one SQL (noe views created)
string Opt_Plan::getOneSQL()
{
  string sPre = "set timing on\n";
  sPre += "SET LINESIZE 200\n";
  sPre += "alter system flush buffer_cache;\n\n";

  // Get final info for select query
  list<Opt_Column*> selections;
  list<Opt_Agg*> aggregates; 
  list<Opt_Column*> groupBys;
  list<Opt_Object*> orderBys;

  preSQL(&selections, &aggregates, &groupBys, &orderBys);

  string s = "SELECT /*+ NO_QUERY_TRANSFORMATION */ \n";

  // Columns
  string sSelCols = "";
   for (list<Opt_Column*>::iterator i = selections.begin(); i != selections.end(); i++) 
    {      
       if (sSelCols.compare("") != 0)
	{
	  sSelCols += ", ";
	}
       sSelCols += (*i)->toStringNoTableDot();
    }
   // Aggregates
    for (list<Opt_Agg*>::iterator i = aggregates.begin(); i != aggregates.end(); i++) 
      {
	if (sSelCols.compare("") != 0)
	{
	  sSelCols += ", ";
	}
	sSelCols += (*i)->toStringNoTableDot();
      }

    s = s + sSelCols  + "\nFROM\n";
    string sTab = "     ";    
    //s += sTab + "(\n";

    unsigned int viewNum = 1;
    if (m_ptrTable != NULL)
      {
	s += getPlanOneSQL(&viewNum, sTab + "     ");
      }
    else if ( (m_ptrLeft != NULL) && (m_ptrRight != NULL))
      {
	s += m_ptrLeft->getPlanOneSQL(&viewNum, sTab + "     ");
	s += ",\n";
	viewNum++;
	s += m_ptrRight->getPlanOneSQL(&viewNum, sTab + "     ");
      }
    else 
      {
	cout << "Invalid query. Cannot get SQL\n";
      }  

    s += "\n";// + sTab + ")\n";

    if (m_joins.size() > 0)
      {
	s += "WHERE ";
	list<Opt_Join*>::iterator joinIt;
	for(joinIt = m_joins.begin(); joinIt != m_joins.end(); joinIt++)
	{
	  s += "\n";
	  if (joinIt != m_joins.begin())
	    {
	      s += "AND   ";
	    }
	  s += (*joinIt)->getFromColumn()->getColumnName();
	  s += " = ";
	  s += (*joinIt)->getToColumn()->getColumnName();
	}
      }   

    // Group By
    if (groupBys.size() > 0)
      {
	s += "\nGROUP BY ";
	for (list<Opt_Column*>::iterator i = groupBys.begin();i != groupBys.end(); i++)
	  {
	    if (i != groupBys.begin())
	      {
		s += ", ";
	      }
	    s += (*i)->toStringNoTableDot();
	  }
      }
  
    //  Order By
    if (orderBys.size() > 0)
      {
	s += "\nORDER BY ";
	for (list<Opt_Object*>::iterator i = orderBys.begin();i != orderBys.end(); i++)
	  {
	    if (i != orderBys.begin())
	      {
		s += ", ";
	      }
	    s += (*i)->toStringNoTableDot();
	  }
      }

    s += ";\n\n";
  
    // Get explain
    string sExplain = "EXPLAIN PLAN FOR\n";
    sExplain.append(s);
    sExplain +="\nSELECT PLAN_TABLE_OUTPUT FROM TABLE(DBMS_XPLAN.DISPLAY());\n";

    string sFull = sPre + s + sExplain;
  return sFull;
}

// Prepare before getting SQL
void Opt_Plan::preSQL(list<Opt_Column*> *selections, list<Opt_Agg*> *aggregates, list<Opt_Column*> *groupBys, list<Opt_Object*> *orderBys)
{
  // -------------------------------
  // make joins for this plan
  makeJoins(m_ptrSourceQuery);

  // -------------------------------
  // Get final info for select query
  m_ptrSourceQuery->getSelectInfo(selections, aggregates, groupBys, orderBys);

  // -------------------------------
  // make selection list for the plan and its all sub plans
  // Seletion list of subplan must consist of columns of final selection, aggregates and clolumns in its parents' joins
  list<Opt_Column*> extraCols;
  list<Opt_Column*> sel = *selections;
  extraCols.merge(sel);
  
  // get columns of the aggregate
  list<Opt_Column*> aggCols;
  list<Opt_Agg*>::iterator aggIt;      
  for(aggIt = aggregates->begin(); aggIt != aggregates->end(); aggIt++)
    {
      Opt_Column* ptrCol = (*aggIt)->getColumn();
      aggCols.push_back(ptrCol);
    }
  extraCols.merge(aggCols);

  // Make selection list
  list<Opt_Column*> parentJoins;
  makeSelections(extraCols, parentJoins);
}


// get an SQL for a plan
string Opt_Plan::getSQL()
{
  

  string s = "set timing on\n";
  s += "SET LINESIZE 200\n";
  s += "alter system flush buffer_cache;\n\n";

  unsigned int viewNum = 1;

  // Get final info for select query
  list<Opt_Column*> selections;
  list<Opt_Agg*> aggregates; 
  list<Opt_Column*> groupBys;
  list<Opt_Object*> orderBys;

  preSQL(&selections, &aggregates, &groupBys, &orderBys);

  /*
  // -------------------------------
  // make joins for this plan
  makeJoins(m_ptrSourceQuery);

  // -------------------------------
  // Get final info for select query
  list<Opt_Column*> selections;
  list<Opt_Agg*> aggregates; 
  list<Opt_Column*> groupBys;
  list<Opt_Object*> orderBys;
  m_ptrSourceQuery->getSelectInfo(&selections, &aggregates, &groupBys, &orderBys);

  // -------------------------------
  // make selection list for the plan and its all sub plans
  // Seletion list of subplan must consist of columns of final selection, aggregates and clolumns in its parents' joins
  list<Opt_Column*> extraCols;
  list<Opt_Column*> sel = selections;
  extraCols.merge(sel);
  
  // get columns of the aggregate
  list<Opt_Column*> aggCols;
  list<Opt_Agg*>::iterator aggIt;      
  for(aggIt = aggregates.begin(); aggIt != aggregates.end(); aggIt++)
    {
      Opt_Column* ptrCol = (*aggIt)->getColumn();
      aggCols.push_back(ptrCol);
    }
  extraCols.merge(aggCols);

  // Make selection list
  list<Opt_Column*> parentJoins;
  makeSelections(extraCols, parentJoins);
  */

  // --------------------------------
  // get string of create view
  unsigned int iFinalView = viewNum;
  s += getViewSQL(&viewNum);

  // Get final select query
  // Selection list
  string sFinal = "SELECT   /*+ NO_QUERY_TRANSFORMATION */   \n";
  // Columns
  string sSelCols = "";
   for (list<Opt_Column*>::iterator i = selections.begin(); i != selections.end(); i++) 
    {      
       if (sSelCols.compare("") != 0)
	{
	  sSelCols += ", ";
	}
       sSelCols += (*i)->toStringNoTableDot();
    }
   // Aggregates
    for (list<Opt_Agg*>::iterator i = aggregates.begin(); i != aggregates.end(); i++) 
      {
	if (sSelCols.compare("") != 0)
	{
	  sSelCols += ", ";
	}
	sSelCols += (*i)->toStringNoTableDot();
      }

    char buff[30];
    sprintf(buff, "%d", iFinalView);
    string sFinalViewNum(buff);
    sFinal += sSelCols  + "\nFROM v" + sFinalViewNum;

  // Group By
    if (groupBys.size() > 0)
      {
	sFinal += "\nGROUP BY ";
	for (list<Opt_Column*>::iterator i = groupBys.begin();i != groupBys.end(); i++)
	  {
	    if (i != groupBys.begin())
	      {
		sFinal += ", ";
	      }
	    sFinal += (*i)->toStringNoTableDot();
	  }
      }
  
    //  Order By
    if (orderBys.size() > 0)
      {
	sFinal += "\nORDER BY ";
	for (list<Opt_Object*>::iterator i = orderBys.begin();i != orderBys.end(); i++)
	  {
	    if (i != orderBys.begin())
	      {
		sFinal += ", ";
	      }
	    sFinal += (*i)->toStringNoTableDot();
	  }
      }

    sFinal += ";\n\n";

    s += sFinal;

    string sFinalExplain = "EXPLAIN PLAN FOR\n";
    sFinalExplain += sFinal;
    sFinalExplain += "SELECT PLAN_TABLE_OUTPUT FROM TABLE(DBMS_XPLAN.DISPLAY());";

    s += sFinalExplain;

  return s;
}

// Make selection list
list<Opt_Column*> Opt_Plan::makeSelections(list<Opt_Column*> extraCols, list<Opt_Column*> parentJoinCols)
{
  list<Opt_Column*> forParentSelectionCols;

  list<Opt_Column*>::iterator colIt;
   if (m_ptrTable != NULL)
   {     
     for(colIt = extraCols.begin(); colIt != extraCols.end(); colIt++)
       {
	 if ( (*colIt)->getTableName().compare(m_ptrTable->getName()) == 0 )
	   addSelectionColumn(*colIt);
       }

     forParentSelectionCols = m_selections;

     for(colIt = parentJoinCols.begin(); colIt != parentJoinCols.end(); colIt++)
       {
	 if ( (*colIt)->getTableName().compare(m_ptrTable->getName()) == 0 )
	   addSelectionColumn(*colIt);
       } 

     return forParentSelectionCols;
     
   }

   if ( (m_ptrLeft != NULL) && (m_ptrRight != NULL) )
    {
      // Add left and right join cols
      list<Opt_Column*> leftJoinCols; 
      list<Opt_Column*> rightJoinCols;
      list<Opt_Join*>::iterator joinIt;
      for (joinIt = m_joins.begin(); joinIt != m_joins.end(); joinIt++)
	{
	  leftJoinCols.push_back((*joinIt)->getFromColumn());
	  rightJoinCols.push_back((*joinIt)->getToColumn());
	}


      list<Opt_Column*> newExtraCols = extraCols;    
      newExtraCols.merge(parentJoinCols);
            
      list<Opt_Column*> leftSelectCols = m_ptrLeft->makeSelections(newExtraCols, leftJoinCols);
      list<Opt_Column*> rightSelectCols = m_ptrRight->makeSelections(newExtraCols, rightJoinCols);

      // Add those return columns from left and right children to its selection list
      for (colIt = leftSelectCols.begin(); colIt != leftSelectCols.end(); colIt++)
	addSelectionColumn(*colIt);
      for (colIt = rightSelectCols.begin(); colIt != rightSelectCols.end(); colIt++)
	addSelectionColumn(*colIt);

      // Get return columns
      for (colIt = extraCols.begin(); colIt != extraCols.end(); colIt++)
	{
	  if (isInPlan((*colIt)->getTableName()))
	    forParentSelectionCols.push_back(*colIt);
	}
    }
  else
    {
      cout << "Invalid plan. Cannot make joins" << endl;
    }

   return forParentSelectionCols;
}

// Get SQL for this plan
string Opt_Plan::getPlanOneSQL(unsigned int *ptrViewNum, string sTab)
{
  string sTab1 = sTab + "   ";
  char buff[100];
  sprintf(buff, "%d", *ptrViewNum);
  string sNum(buff);

  string s = sTab + "(\n";
  string sTab2 = sTab + " ";
  s += sTab2 + "SELECT\n";
  list<Opt_Column*>::iterator colIt;
  if (m_selections.size() == 0)
    {
      s += sTab1 + "*";
    }
  else
    {
      s += sTab1 ;
      for (colIt =  m_selections.begin(); colIt != m_selections.end(); colIt++)
	{
	  if (colIt != m_selections.begin())
	    {
	      s += ", ";
	    }

	  s += (*colIt)->getColumnName();
	}
    }

  s += "\n" +  sTab2 + "FROM ";

  if (m_ptrTable != NULL)
    {
      s += "\n" +  sTab1 + m_ptrTable->getName();
      
      // Get non join predicates
      Opt_Comparison* ptrNonJoinPredicates = m_ptrTable->getNoneJoin();
      if (ptrNonJoinPredicates != NULL)
	{
	  s +=   "\n" + sTab2 + "WHERE \n";
	  s +=  sTab1 + Opt_QueryGraph::getNonJoinPredicateString(ptrNonJoinPredicates);
	}
      s += "\n" + sTab + ") v" + sNum;

    }
  else if ( (m_ptrLeft != NULL) && (m_ptrRight != NULL) )
    {      
      s += "\n";// + sTab1 + "(\n";
      string sNewTab = sTab + "     ";
      string sNewTab1 = sNewTab + "   ";

      *ptrViewNum = *ptrViewNum + 1;
      string sLeftView = m_ptrLeft->getPlanOneSQL(ptrViewNum, sNewTab);
      s += sLeftView;
      s += ", \n";

      *ptrViewNum = *ptrViewNum + 1;
      string sRightView = m_ptrRight->getPlanOneSQL(ptrViewNum, sNewTab);
      s += sRightView;

      if (m_joins.size() > 0)
      {
	s += "\n";// + sTab1 + ")\n";
	s += sTab + "WHERE \n";
	list<Opt_Join*>::iterator joinIt;
	for(joinIt = m_joins.begin(); joinIt != m_joins.end(); joinIt++)
	{
	  s += sTab1;
	  if (joinIt != m_joins.begin())
	    {
	      s +=  "AND   ";
	    }
	  s += (*joinIt)->getFromColumn()->getColumnName();
	  s += " = ";
	  s += (*joinIt)->getToColumn()->getColumnName();
	  s += "\n";
	}
      } 

      s +=  sTab + ") v" + sNum;

    }
  else
    {
       cout << "Invalid plan. Cannot get SQL for query" << endl;       
    }

  return s;
}


// get a string to create view for this plan 
string  Opt_Plan::getViewSQL(unsigned int *ptrViewNum)
{
  unsigned int thisViewNum = *ptrViewNum;
  string sViews = "";

  char buff[100];
  sprintf(buff, "%d", thisViewNum);
  string sNum(buff);


  string sThisView = "CREATE OR REPLACE VIEW v" +  sNum + " AS \n";
  sThisView += "SELECT   /*+ NO_QUERY_TRANSFORMATION */   \n";
  list<Opt_Column*>::iterator colIt;
  if (m_selections.size() == 0)
    {
      sThisView += "*";
    }
  else
    {
      for (colIt =  m_selections.begin(); colIt != m_selections.end(); colIt++)
	{
	  if (colIt != m_selections.begin())
	    {
	      sThisView += ", ";
	    }

	  sThisView += (*colIt)->getColumnName();
	}
    }

  sThisView += "\nFROM ";

  if (m_ptrTable != NULL)
    {
      sThisView = sThisView + m_ptrTable->getName() + "\n";
      
      // Get non join predicates
      Opt_Comparison* ptrNonJoinPredicates = m_ptrTable->getNoneJoin();
      if (ptrNonJoinPredicates != NULL)
	{
	  sThisView +=  "WHERE \n";
	  sThisView += Opt_QueryGraph::getNonJoinPredicateString(ptrNonJoinPredicates);
	}


    }
  else if ( (m_ptrLeft != NULL) && (m_ptrRight != NULL) )
    {
      *ptrViewNum = *ptrViewNum + 1;
      unsigned int iLeftView =  *ptrViewNum;
      string sLeftView = m_ptrLeft->getViewSQL(ptrViewNum);
      sViews += sLeftView;

      *ptrViewNum = *ptrViewNum + 1;
      unsigned int iRightView = *ptrViewNum;
      string sRightView = m_ptrRight->getViewSQL(ptrViewNum);
      sViews += sRightView;
      
      char buff[100];
      sprintf(buff, "%d", iLeftView);
      string sLeftViewNum(buff);
      sprintf(buff, "%d", iRightView);
      string sRightViewNum(buff);

      sThisView = sThisView + "v" + sLeftViewNum + ", v" + sRightViewNum + "\n";
      
      if (m_joins.size() > 0)
	{
	  sThisView += "WHERE ";
	  list<Opt_Join*>::iterator joinIt;
	  for(joinIt = m_joins.begin(); joinIt != m_joins.end(); joinIt++)
	    {
	      if (joinIt != m_joins.begin())
		{
		  sThisView += "AND   ";
		}
	      sThisView += (*joinIt)->getFromColumn()->getColumnName();
	      sThisView += " = ";
	      sThisView += (*joinIt)->getToColumn()->getColumnName();
	      sThisView += "\n";
	    }
	}      
    }
  else
    {
       cout << "Invalid plan. Cannot make joins" << endl;
       return "";
    }

  sThisView += ";\n\n";
  sViews += sThisView;

  return sViews;
}
 
// Add a column to selection column list
void Opt_Plan::addSelectionColumn(Opt_Column* ptrCol)
{
  // Only add if the column if not in the list yet
  bool notFound = 1;

  list<Opt_Column*>::iterator colIt;
  for(colIt = m_selections.begin(); colIt != m_selections.end(); colIt++)
    {
      if ((*colIt)->getColumnName().compare(ptrCol->getColumnName()) == 0)
	{
	  notFound = 0;
	}
    }

  if (notFound)
    {
      m_selections.push_back(ptrCol);
    }
}

// Sort plans to descending cost
list<Opt_Plan*> Opt_Plan::sortPlans(list<Opt_Plan*> plans)
{
  // transform list to array
  Opt_Plan* planArray[plans.size()];
  list<Opt_Plan*>::iterator planIt = plans.begin();
  for (unsigned int i = 0; i < plans.size(); i++)
    {
      planArray[i] = *planIt;
      planIt++;
    }

  // Sort on array
  for (unsigned int i = 0; i < plans.size() - 1; i++)
    {
      for (unsigned int j = i + 1;  j < plans.size(); j++)
	{
	  if (planArray[i]->getCost() > planArray[j]->getCost())
	    {
	      // swap
	      Opt_Plan* ptrTmpPlan = planArray[i];
	      planArray[i] =  planArray[j];
	      planArray[j] = ptrTmpPlan;
	    }
	}
    }

  // trabsform back to list
  list<Opt_Plan*> sortedPlans;
  for (unsigned int i = 0; i < plans.size(); i++)
    {
      sortedPlans.push_back(planArray[i]);
    }

  return sortedPlans;
}
