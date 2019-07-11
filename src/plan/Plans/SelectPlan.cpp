/* Copyright (c) 2005, Regents of Massachusetts Institute of Technology, 
 * Brandeis University, Brown University, and University of Massachusetts 
 * Boston. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, 
 *     this list of conditions and the following disclaimer.
 *   - Redistributions in binary form must reproduce the above copyright 
 *     notice, this list of conditions and the following disclaimer in the 
 *     documentation and/or other materials provided with the distribution.
 *   - Neither the name of Massachusetts Institute of Technology, 
 *     Brandeis University, Brown University, or University of 
 *     Massachusetts Boston nor the names of its contributors may be used 
 *     to endorse or promote products derived from this software without 
 *     specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED 
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 *  SelectPlan.cpp
 *  cstore
 *
 *  Created by Nga Tran on 6/20/05.
 * 
 *	Plan for select query
 */

#include "SelectPlan.h"

SelectPlan::SelectPlan() : Plan()
{
}

SelectPlan::~SelectPlan()
{
	Log::writeToLog("SelectPlan", 10, "~SelectPlan");  
}


// Generate a plan
void SelectPlan::makePlan(Query* lpQuery)
{
	list<EHolder*>	selections;
	list<EHolder*>	aggregates;
	ListProjections	*lpProjections;
	BExpression		*lpWherePredicate;
	
	// Get query inforamtion
	parseQuery(lpQuery, &selections, &aggregates, &lpProjections, &lpWherePredicate);
		
	// Selection list and aggregate list are empty, do nothing
	if ( (selections.size() == 0) && (aggregates.size() == 0) )
	{
		cout << endl << "Nothing to show. No plan generated" << endl;
		return;
	}
	
	// Multi-column groupby is not supported
	if ( (aggregates.size() > 0 ) && (selections.size() > 1) )
	{
		cout << endl << "NOTICE: Current version does not support multi-column group by. No plan generated" << endl;
		return;
	}
	
	// StringNodeMap
	StringNodeMap	mapColumns;
	
	// generate predicate nodes
	Node* lpNode = NULL;
	if (lpWherePredicate != NULL)
	{	
		lpNode =  lpWherePredicate->translate(&mapColumns);
	}
	
	// Check Predicate type
	if (lpNode != NULL)
	{
		int iPredicateType = lpNode->getPredicateType();
		
		if (iPredicateType == PREDICATE_JOIN_AND_OTHER)
		{
			// Join and/ or something
			cout << endl << "NOTICE: Current version does not support this kind of predicate" << endl;
			return;
		}
		
		if ( (iPredicateType == PREDICATE_JOIN) && (aggregates.size() > 0 ) )
		{
			// Join and aggregate
			cout << endl << "NOTICE: Current version does not support join and aggregate together" << endl;
			return;
		}
		
		if ( (iPredicateType == PREDICATE_JOIN_AND_SELECT) || (iPredicateType == PREDICATE_JOIN) )
		{
			// One join and no aggregate
			
			if (iPredicateType == PREDICATE_JOIN_AND_SELECT)
			{
				// Get only join node
				// The other condition (predicate) has already set in the BConjuntionNode
				lpNode = ((BConjunctionNode*) lpNode)->getLeft();
			}
			
			Node* lpSelections = createSelectionNodesForJoin(lpNode, selections);
			
			// Create a new root node
			m_lpRootNode = MResultNode::create();
			
			// Add selection nodes
			if (lpSelections != NULL)
			{
				((MNode*) m_lpRootNode)->add( ((MNode*) lpSelections)->getChildren());
			}
			return;
		}
		
	}
	 
	 
	// NO JOIN
	
	// One attribute predicate, keep the predicate node for possible later
	bool isSinglePredicate = 1;
	if (mapColumns.size() == 1)
	{
		// Check if there is AND, OR in the predicate	
		string sPredicate = lpWherePredicate->toString();
			
		unsigned int pos = sPredicate.find(" and ", 0);
		if (pos != string::npos)
		{
			isSinglePredicate = 0;
		}
		else 
		{
			pos = sPredicate.find(" or ", 0);
			if (pos != string::npos)
			{
				isSinglePredicate = 0;
			}
			else 
			{
				pos = sPredicate.find("not ", 0);
				if (pos != string::npos)
				{
					isSinglePredicate = 0;
				}
				
				else 
				{
					pos = sPredicate.find(" <> ", 0);
					if (pos != string::npos)
					{
						isSinglePredicate = 0;
					}
				}
			}
		}
		
		if (isSinglePredicate == 1)
		{
			m_lpPredicate = lpNode;
		}
	}


	// generate copyNode
	bool bNeedPositionFilter = 0;

	if (lpNode != NULL)
	{
		lpNode = createCopyNode(lpNode, &bNeedPositionFilter, &mapColumns, selections, aggregates);
	}	
	
	// Complex where predicate (having and, or, not)
	// --> need to filter positions
	if (isSinglePredicate == 0)
	{
 		bNeedPositionFilter = 1;
	}
	
	// generate selection nodes
	Node* lpSelections = NULL;
	Node* lpFirstProject = NULL;
	Node* lpGroup = NULL;
	int	iCopyIndex = 0;
	lpSelections = createSelectionNodes(
		lpNode, bNeedPositionFilter, selections, &lpFirstProject, &lpGroup, mapColumns, &iCopyIndex);

	// Get first projection name in the ProjectionList
	// in order to get the * column in COUNT(*)
	list<Projection*> projections = lpProjections->getProjections();
	string sFirstProjection = (projections.front())->get();
	
	// generate aggregate nodes
	Node* lpAggregates = NULL;
	lpAggregates = createAggregateNodes(
		lpNode, bNeedPositionFilter, selections, lpFirstProject, 
		lpGroup, mapColumns, &iCopyIndex, aggregates, sFirstProjection);
	
	// process having conditions
	// ...
	
	// sort
	// ...

	// generate root node
	createRoot(lpSelections, lpAggregates);
}

// Parse the query (SELECT) into specific lists
void SelectPlan::parseQuery(
			Query* lpQuery, list<EHolder*> *lpSelections, list<EHolder*> *lpAggregates,
			ListProjections** lpProjections, BExpression** lpWherePredicate)
{
	// Get selections and aggregates
	ListSelections*		lpSelectionAggregateList = ( (QSelect*) lpQuery)->getSelections();
	
	if (lpSelectionAggregateList != NULL)
	{
		// Get selections
		(*lpSelections) = lpSelectionAggregateList->getSelections();
		
		// Get aggregates
		(*lpAggregates) = lpSelectionAggregateList->getAggregates();
	}
	
	// Get projections
	(*lpProjections) = ( (QSelect*) lpQuery)->getProjections();

	// Get where predicate
	(*lpWherePredicate) = ( (QSelect*) lpQuery)->getWherePredicate();
}	


// Show query
void SelectPlan::showQuery(
			list<EHolder*> selections, list<EHolder*> aggregates,
			ListProjections* lpProjections, BExpression* lpWherePredicate)
{
	cout << endl << "SELECTIONS:" << endl;
	if(selections.size() > 0)
	{
		list<EHolder*>::iterator it;
		for (it = selections.begin(); it != selections.end(); it++)
		{
			EHolder* lpEHolder = (*it);
			Expression* lpColumn = lpEHolder->getExpression();
			if (lpColumn != NULL)
			{
				cout << "	" << lpColumn->toString() << endl;
			}
		}
	}
	
	cout << endl << "AGGREGATES:" << endl;
	if(aggregates.size() > 0)
	{
		list<EHolder*>::iterator it;
		for (it = aggregates.begin(); it != aggregates.end(); it++)
		{
			EHolder* lpEHolder = (*it);
			Expression* lpAggregate = lpEHolder->getExpression();
		
			if (lpAggregate != NULL) 
			{
				
				cout << "	" << lpAggregate->toString() << endl;
			}
		}
	}

	cout << endl << "PROJECTIONS:" << endl;
	if (lpProjections != NULL)
	{
		cout << "	" << lpProjections->toString() << endl;
	}

	cout << endl << "WHERE PREDICATES:" << endl;
	if (lpWherePredicate != NULL)
	{
		cout << "	" << lpWherePredicate->toString() << endl;
	}	
}


// Create copy for masking later
Node* SelectPlan::createCopyNode(
		Node* lpChild, bool* bNeedPositionFilter, StringNodeMap* lpMapColumns,
		list<EHolder*> selections, list<EHolder*> aggregates)
{
	(*bNeedPositionFilter) = 1;

	// No position filter needed
	if ( (lpChild == NULL) || (lpMapColumns->size() == 0) )
	{
		(*bNeedPositionFilter) = 0;
		return NULL;
	}
	
	// CopyNode can be ignored when selection list, aggregate list and 
	// where predicates have exactly one and same attribute
	
	// Check if there is only one attribute in both selection list and where predicate
	if ( (selections.size() == 1) &&  (lpMapColumns->size() == 1)  && (m_lpPredicate != NULL))
	{
		// Get the first (and only one) column in the selection list
		EHolder* lpEHolder =  selections.front();
		Expression* lpSelectionColumn = lpEHolder->getExpression();
		if (lpSelectionColumn != NULL)
		{	
			// Check if the attribute in the selection list is the same as the one in the where predicate
			StringNodeMap::iterator pos;
			pos = lpMapColumns->find(lpSelectionColumn->toString());
			if (pos != lpMapColumns->end())
			{
				// Check if this attribute is the same as the one in the aggregate list
				if (aggregates.size() == 0)
				{
					// No CopyNode is required in this case -> lpChild unchanged
					(*bNeedPositionFilter) = 0;				
					return lpChild;
				}
				else
				{
					(*bNeedPositionFilter) = 0;
					
					// get all aggregates
				
					list<EHolder*>::iterator it;
					for (it = aggregates.begin(); it != aggregates.end(); it++)
					{
						EHolder* lpEHolder = (*it);
						Expression* lpAggregate = lpEHolder->getExpression();
		
						if ( (lpAggregate != NULL) && (lpAggregate->isAggregate()) )
						{
							// Casting
							EAgg* lpAgg = (EAgg*) lpAggregate;
			
							// Get column name
							Expression* lpAggregateColumn = (lpAgg)->getRight();
							if (lpAggregateColumn == NULL )
							{
								// COUNT (*)
							}
							else
							{
								// COUNT, MIN, MAX, AVG, SUM (column)
			
								// Check if selection column = aggregate column
								if (lpAggregateColumn->toString() != lpSelectionColumn->toString())
								{
									// Copy needed
									(*bNeedPositionFilter) = 1;
									break; // go out off the for loop
								}
							}
						}
					}
					
					// No position filter needed -> lpChild unchanged
					if (*bNeedPositionFilter == 0)
					{				
						return lpChild;
					}
				}
			}
		}
	}
	
	// Position filter will be needed later but copy node might not be needed
	// Copy node is only needed when there are more than 2 operators need to do position filter

	int numOutputColumns = selections.size() + aggregates.size();
	if (numOutputColumns < 2)
	{
		// Only one ouput column --> No copy needed
		return lpChild;
	}
	
	int iNumPositionFilter = 0;
		
	// Get all selection columns
	list<EHolder*>::iterator it;
	for (it = selections.begin(); it != selections.end(); it++)
	{
		EHolder* lpEHolder = (*it);
		Expression* lpColumn = lpEHolder->getExpression();
		if (lpColumn != NULL)
		{
			iNumPositionFilter++;
		
			// Special condition: One-column predicate
			// Check if the attribute in the selection list is the same as the only one in the where predicate
			if ( (lpMapColumns->size() == 1)  && (m_lpPredicate != NULL) )
			{
				StringNodeMap::iterator pos;
				pos = lpMapColumns->find(lpColumn->toString());
				if ( (pos != lpMapColumns->end()))
				{
					// Same column in where predicate, no position filter needed
					//iNumPositionFilter--;
				}
			}
		}
			
		if (iNumPositionFilter == 2)
		{
			// Enough to create a copy node, stop checking
			break;
		}
	}
		
	if (iNumPositionFilter < 2) // Don't check aggregate if iNumPositionFilter is already >= 2
	{
		// Get all aggregate columns		
		for (it = aggregates.begin(); it != aggregates.end(); it++)
		{
			EHolder* lpEHolder = (*it);
			Expression* lpAggregate = lpEHolder->getExpression();
	
			if (lpAggregate == NULL)
			{
				continue;
			}
				
			// Casting
			EAgg* lpAgg = (EAgg*) lpAggregate;
			
			// Get column name
			EColumn* lpColumn = (lpAgg)->getRight();
			if (lpColumn == NULL )
			{
				// COUNT (*)
				if (selections.size() == 1) 
				{
					// One-column group. Get that column for the agregate column
					// -> no position filter needs
				}
				else if (selections.size() > 1)
				{
					// One-column group. Get the first column of the group for the agregate column
					// -> no position filter needs
				}
				else
				{
					// No group
					iNumPositionFilter++;
				}
			}
			else
			{		
				// COUNT, MIN, MAX, AVG, SUM (column)
				iNumPositionFilter++;
				
				if (selections.size() >= 1)
				{
					// Having group
					// Check if the column of the aggregate is the same the first column in the selection list
				
					// Get the first column in the selection list
					EHolder* lpEHolder =  selections.front();
					string sFirstSelectionColumn = (lpEHolder->getExpression())->toString();
					if (sFirstSelectionColumn.compare(lpColumn->toString()) == 0)
					{	
						// Use the first column in the group for the agregate column
						// -> no position filter needs
						iNumPositionFilter--;
					}
				}
			}
				
			if (iNumPositionFilter == 2)
			{
				// Enough to create a copy node, stop checking
				break;
			}
		}
	}
		
	if (iNumPositionFilter < 2)
	{
		// No copy needed -> lpChild unchanged
		return  lpChild;
	}

	
	// Copy needed
	UCopyNode* lpNode = NULL;
	lpNode = UCopyNode::create(lpChild, 0, selections.size() + aggregates.size());
		
	return lpNode;	
}


// Create selection nodes
Node* SelectPlan::createSelectionNodes(
	Node* lpChild, bool bNeedPositionFilter, list<EHolder*> selections,
	Node** lpFirstProject, Node** lpGoup, StringNodeMap mapColumns, int* lpCopyIndex)
{	
	MNode* lpSelections = NULL;
	
	// No selection to be added
	if (selections.size() ==  0)
	{
		return lpSelections;
	}
	
	lpSelections = MNode::create();
	
	// Qyery is too simple to create a new node 
	if (	(selections.size() == 1) && // Only one column selection list
			(lpChild != NULL) &&		// having where predicate clauses
			(!bNeedPositionFilter)		// no position filter needed
		)
	{
		lpSelections->add(lpChild);
		(*lpGoup) = lpChild;
	}
	else
	{
		// Get all selection columns
		list<EHolder*>::iterator it;
		for (it = selections.begin(); it != selections.end(); it++)
		{
			EHolder* lpEHolder = (*it);
			Expression* lpColumn = lpEHolder->getExpression();
			if (lpColumn != NULL)
			{
			
				// One-column predicate
				if ( (mapColumns.size() == 1) && (m_lpPredicate != NULL) && (!bNeedPositionFilter) )
				{
					// Check if the attribute in the selection list is the same as the only one in the where predicate
					StringNodeMap::iterator pos;
					pos = mapColumns.find(lpColumn->toString());
					if ( (pos != mapColumns.end()) && (m_lpPredicate != NULL))
					{
						// Same column in where predicate, reuse it
						//////lpSelections->add(m_lpPredicate);
						
						if ((*lpCopyIndex) == 0)
						{
							// For now, only support one-column groupby
							(*lpGoup) = m_lpPredicate;
						}
						//////continue;
					}
				}
				
				// Get projection name				
				string sProjection = ((EColumn*)lpColumn)->getProjection();

				SelectNode* lpSelect = NULL;
				lpSelect = SelectNode::create(lpColumn->toString(), sProjection);
				
				if (lpSelect != NULL)
				{
					if(!bNeedPositionFilter)
					{
						// No further set bistring
						UProjectNode* lpProject = UProjectNode::create(lpSelect);
						
						if (lpProject != NULL)
						{
							// Add to selection list
							lpSelections->add(lpProject);
						}
						
						if ((*lpCopyIndex) == 0)
						{
							// First column
							(*lpFirstProject) = lpProject;
						
							// For now, only support one-column groupby
							(*lpGoup) = lpProject;
						}
						
					}
					else
					{
						// Need set position bitstring
						BProjectNode* lpProject = BProjectNode::create(lpSelect, lpChild, (*lpCopyIndex));
											
						if (lpProject != NULL)
						{
							// Add to selection list
							lpSelections->add(lpProject);
						}
						
						if ((*lpCopyIndex) == 0)
						{
							// First column
							(*lpFirstProject) = lpProject;
						
							// For now, only support one-column groupby
							(*lpGoup) = lpProject;
						}
					}
					
					(*lpCopyIndex)++;
				}
			}
		}
	}
	
	return lpSelections;
}

// Create selection nodes for join predicate
Node* SelectPlan::createSelectionNodesForJoin(Node* lpChild, list<EHolder*> selections)
{
	MNode* lpSelections = NULL;
	
	// No selection to be added
	if (selections.size() ==  0)
	{
		return lpSelections;
	}
	
	lpSelections = MNode::create();
	
	// Get all selection columns
	list<EHolder*>::iterator it;
	for (it = selections.begin(); it != selections.end(); it++)
	{
		EHolder* lpEHolder = (*it);
		Expression* lpColumn = lpEHolder->getExpression();
		if (lpColumn != NULL)
		{
			//  Create selection node
			SProjectUnionNode* lpProject = NULL;
			lpProject = SProjectUnionNode::create(lpChild, (EColumn*) lpColumn); 			
			
			// Add to selection list
			lpSelections->add(lpProject);
		}
	}
	
	return lpSelections;
}


// Create aggregate nodes
Node*  SelectPlan::createAggregateNodes(
	Node* lpChild,  bool bNeedPositionFilter, list<EHolder*> selections,
	Node* lpFirstProject, Node* lpGroup, StringNodeMap mapColumns, int* lpCopyIndex, 
	list<EHolder*> aggregates, string sFirstProjection)
{
	MNode* lpAggregates = NULL;
	
	//No aggregate
	if (aggregates.size() == 0)
	{		
		return lpAggregates;
	}
	
	lpAggregates = MNode::create();
	
	bool newSelectNode = 1;
	
	list<EHolder*>::iterator it;
	for (it = aggregates.begin(); it != aggregates.end(); it++)
	{
		newSelectNode = 1;
		
		EHolder* lpEHolder = (*it);
		Expression* lpAggregate = lpEHolder->getExpression();
		
		if (lpAggregate == NULL)
		{
			continue;
		}
		
		// Casting
		EAgg* lpAgg = (EAgg*) lpAggregate;
			
		// Get column name
		EColumn* lpColumn = (lpAgg)->getRight();
		if (lpColumn == NULL )
		{
			// COUNT (*)
			
			// Get a column from the catalog for the * column   
			CatalogInstance *lpCatalog = CatalogInstance::getCatalog();
			string sColumnName = lpCatalog->getPrimaryColumnName( sFirstProjection );
			
			lpAgg->setRight(EColumn::create(sFirstProjection, sColumnName));
				
			if ( (lpChild != NULL) && (!bNeedPositionFilter) )
			{
				// having where predicate clauses but no position filter needed
				newSelectNode = 0; // No need to create a select node
			}
			else if ( (selections.size() >= 1) && (lpFirstProject != NULL) )
			{
				// Having group. Get the first column in the group for the agregate column
				newSelectNode = 0; // No need to create a select node
				lpChild = lpFirstProject; // Agregate column is also the group column
			}
		}
		else
		{
			// COUNT, MIN, MAX, AVG, SUM (column)
			
			if ( (lpChild != NULL) && (!bNeedPositionFilter) )
			{
				// having where predicate clauses but no position filter needed
				StringNodeMap::iterator pos;
				pos = mapColumns.find(lpColumn->toString());
				if (pos != mapColumns.end())
				{
					newSelectNode = 0; // No need to create a select node
				}
			}
			else if ( (selections.size() >= 1) && (lpFirstProject != NULL) )
			{
				// Having group
				// Check if the column of the aggregate is the same the first column in the selection list
				
				// Get the first column in the selection list
				EHolder* lpEHolder =  selections.front();
				string sFirstSelectionColumn = (lpEHolder->getExpression())->toString();
				if (sFirstSelectionColumn.compare(lpColumn->toString()) == 0)
				{	
					// Get the first column in the group for the agregate column
					newSelectNode = 0; // No need to create a select node
					lpChild = lpFirstProject; // Agregate column is also the group column
				}
			}
		}

		
		// Translate to a AggNode
		Node* lpAggNode = lpAgg->translate(newSelectNode, lpChild, lpGroup, lpCopyIndex);
		
		if (lpAggNode != NULL)
		{
			// Check if hash aggregate should ne used
			// Only use hash aggregate when there is column group by and the column is not sorted
			if (selections.size() > 0)
			{
				// Get the group by column
				EHolder* lpEHolder = (selections.front());
				Expression* lpColumn = lpEHolder->getExpression();
				string sColumn = ((EColumn*) lpColumn)->toString();
				string sProjection = ((EColumn*) lpColumn)->getProjection();
				
				// Check if the column is sorted
				CatalogInstance* lpCatalog = CatalogInstance::getCatalog();				
				if (!lpCatalog->isSorted(sProjection, sColumn))
				{
					// The column is not sorted
					
					// Get hash table size 
					int iHashTableSize = 150000;
					// Integrate with Catalog later
					//iHashTableSize = lpCatalog->getHashTableSize(sProjection, sColumn);
			
					((BAggNode*) lpAggNode)->setUnsortedGroupBy(iHashTableSize);
					
					//cout << " - UNSORTED";
				}
			}
		
			// Add to aggregate list
			lpAggregates->add(lpAggNode);
		}
	}
	
	return lpAggregates;
}

// Generate root node
void SelectPlan::createRoot(Node* lpSelections, Node* lpAggregates)
{	
	// Create a new root node
	m_lpRootNode = MResultNode::create();
	
	// Add aggregate nodes
	if (lpAggregates != NULL)
	{
		((MNode*) m_lpRootNode)->add( ((MNode*) lpAggregates)->getChildren());
		((MResultNode*) m_lpRootNode)->setAggregate(); // has aggregate
		if (lpSelections != NULL) 
		{
			// Has group by
			((MResultNode*) m_lpRootNode)->setGroupby();
		}
	}	
	else {
		// Add selection nodes
		if (lpSelections != NULL)
		{
			((MNode*) m_lpRootNode)->add( ((MNode*) lpSelections)->getChildren());
		}
	}

}


//---------------------------------------------------------------
// Memory management functions
		
	
// New a node and put it in the m_lpList
SelectPlan* SelectPlan::create()
{
	Log::writeToLog("SelectPlan", 10, "Create without arguments");
		
	SelectPlan* lpNode = new SelectPlan();
	pushBack(lpNode);
	return lpNode;
}
