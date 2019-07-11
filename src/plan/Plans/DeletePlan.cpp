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
 *  DeletePlan.cpp
 *  cstore
 *
 *  Created by Nga Tran on 6/21/05.
 *  
 *  Plan for delete query
 */

#include "DeletePlan.h"


DeletePlan::DeletePlan() : Plan()
{
	m_lpDeleteProjection = NULL;
	m_bIsDeletedAll = 0;
}

DeletePlan::~DeletePlan()
{
	Log::writeToLog("DeletePlan", 10, "~DeletePlan");
}

// Make plan for DELETE statement
void DeletePlan::makePlan(Query* lpQuery)
{
	Projection* lpProjection = NULL;
	BExpression* lpWherePredicate = NULL;
	
	parseQuery(lpQuery, &lpProjection, &lpWherePredicate);

	bool bIsSinglePredicate = 1;

	// generate predicate node
	Node* lpWhereNode = NULL;
	if ( (lpWherePredicate != NULL) && (lpWherePredicate->toString().compare("TRUE") != 0 ) )
	{
		// StringNodeMap
		// This variable is only as a parameter for translate function.
		// It does not have any effect in DELETE statement
		StringNodeMap	mapColumns;
		
		lpWhereNode = lpWherePredicate->translate(&mapColumns);

		// check if it is a single predicate
		unsigned int pos = lpWherePredicate->toString().find(" and ", 0);
		if (pos != string::npos)
		{
			bIsSinglePredicate = 0;
		}
		else 
		{
			pos = lpWherePredicate->toString().find(" or ", 0);
			if (pos != string::npos)
			{
				bIsSinglePredicate = 0;
			}
			else 
			{
				pos = lpWherePredicate->toString().find("not ", 0);
				if (pos != string::npos)
				{
					bIsSinglePredicate = 0;
				}
			}
		}
	}
	
	// Create a new root node
	m_lpRootNode = MResultNode::create();
	
	((MResultNode*) m_lpRootNode)->setDoMerge(0);
	
	// Create delete node	
	if (lpWhereNode == NULL)
	{
		// No where predicate. Delete all tuples
		m_lpDeleteProjection = DeleteProjectionNode::create(lpProjection->toString(), bIsSinglePredicate);
		m_bIsDeletedAll = 1;
	}
	else
	{
		// Having where predicate
		m_lpDeleteProjection = UDeleteProjectionNode::create(lpWhereNode, lpProjection->toString(), bIsSinglePredicate);
		m_bIsDeletedAll = 0;
	
		/*
		Node* lpNode = new UNegationNode(lpDeleteProjection);
		
		// Get all columns of a projection
		CatalogInstance* lpCatalog = NULL;
		lpCatalog = CatalogInstance::getCatalog();
		if (lpCatalog == NULL)
		{
			cout << "ERROR: cannot create catalog" << endl;
		}

		
		vector<string> *lpColumnNameList = lpCatalog->getColumnNames( lpProjection->toString() );
		
		if (lpColumnNameList->size() > 1)
		{
			// copy operator needs
			lpNode = new UCopyNode(lpNode, 0, lpColumnNameList->size());
		}
		
		// Get all columns of a projection
		vector<string>::iterator lpColumName;
		int i = 0;
		for (lpColumName = lpColumnNameList->begin(); lpColumName != lpColumnNameList->end(); lpColumName++ )
		{
			// Create a select node which will get amToTargetColumn
			SelectNode* lpSelectNode = new SelectNode((*lpColumName), lpProjection->toString());
		
			// Create a delete node for this column
			Node* lpProjectNode = new BProjectNode(lpSelectNode, lpNode, i);
		
			// Add the column project node into to root note
			( (MNode*) m_lpRootNode)->add(lpProjectNode);
			
			i++;
		}
		*/
	}
	
	// Add DeleteProjection node
	if (m_lpDeleteProjection != NULL)
	{
		((MNode*) m_lpRootNode)->add(m_lpDeleteProjection);
	}
	
}

// Parse DELETE query into specific lists
void DeletePlan::parseQuery(
			Query* lpQuery, Projection** lpProjection, BExpression** lpWherePredicate)
{
	// Get the projection	
	(*lpProjection) = ( (QDelete*) lpQuery)->getProjection();
		
	// Get where predicate
	(*lpWherePredicate) = ( (QDelete*) lpQuery)->getWherePredicate();
}

//---------------------------------------------------------------
// Memory management functions
		
	
// New a node and put it in the m_lpList
DeletePlan* DeletePlan::create()
{
	Log::writeToLog("DeletePlan", 10, "Create without arguments");
		
	DeletePlan* lpNode = new DeletePlan();
	pushBack(lpNode);
	return lpNode;
}
