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
 *  DeleteProjectionNode.cpp
 *  cstore
 *
 *  Created by Nga Tran on 6/28/05.
 * 
 *	Implementation of DELETE FROM projectionName;
 *	Delete all tuples in the projection
 */

#include "DeleteProjectionNode.h"
#include "../../Operators/Union.h"

DeleteProjectionNode::DeleteProjectionNode() : Node()
{
	m_bIsSinglePredicate = 1;
	m_lpDeleteOperator = NULL;
	m_bDeleteOperatorHasShown = 0;
}

DeleteProjectionNode::DeleteProjectionNode(string sProjection, bool bIsSinglePredicate) : Node()
{
	m_sProjection = sProjection;
	m_bIsSinglePredicate = bIsSinglePredicate;
	m_lpDeleteOperator = NULL;
	m_bDeleteOperatorHasShown = 0;
}


DeleteProjectionNode::~DeleteProjectionNode()
{
	 Log::writeToLog("DeleteProjectionNode", 10, "~DeleteProjectionNode");
	 
	if (m_lpDeleteOperator != NULL)
	{
		delete m_lpDeleteOperator;
		m_lpDeleteOperator = NULL;
	}
	
	if (m_lpReturnOp != NULL)
	{
		delete m_lpReturnOp;
		m_lpReturnOp = NULL;
	}
	
}

// Run ROS only
Operator* DeleteProjectionNode::runROS()
{
	// The operator exists, only return it
	if (m_lpROSReturnOp != NULL)
	{
		return m_lpROSReturnOp;
	}	

	bool bIsNewDeleteVector = 0;
	if (m_lpDeleteOperator == NULL)
	{		
		// The following 2 lines of code will be replaced by
		// WOSManager* lpWOSManager = CatalogInstance::getCatalog()->getWOSManager(m_sProjection); 
		string sColumnAny = CatalogInstance::getCatalog()->getPrimaryColumnName(m_sProjection);
		WOSManager* lpWOSManager = CatalogInstance::getCatalog()->getWOSManager(sColumnAny);
		
		m_lpDeleteOperator = new DeleteOperator(lpWOSManager);
		
		bIsNewDeleteVector = 1;
	}
	
	if (m_lpDeleteOperator == NULL)
	{
		cout << "ERROR: Cannot create DeleteOperator. The results won't be correct" << endl;
		return m_lpROSReturnOp;
	}
	
	// delete all tuples
	if (bIsNewDeleteVector == 1)
	{
		m_lpDeleteOperator->deleteProjection();
	}
	m_lpROSReturnOp = m_lpDeleteOperator;
	
	return m_lpROSReturnOp;

}
		
// Run WOS only
Operator* DeleteProjectionNode::runWOS()
{
	// The operator exists, only return it
	if (m_lpWOSReturnOp != NULL)
	{
		return m_lpWOSReturnOp;
	}	
	
	bool bIsNewDeleteVector = 0;
	if (m_lpDeleteOperator == NULL)
	{
		// The following 2 lines of code will be replaced by
		// WOSManager* lpWOSManager = CatalogInstance::getCatalog()->getWOSManager(m_sProjection); 
		string sColumnAny = CatalogInstance::getCatalog()->getPrimaryColumnName(m_sProjection);
		WOSManager* lpWOSManager = CatalogInstance::getCatalog()->getWOSManager(sColumnAny);
		
		m_lpDeleteOperator = new DeleteOperator(lpWOSManager);
		bIsNewDeleteVector = 1;
	}
	
	if (m_lpDeleteOperator == NULL)
	{
		cout << "ERROR: Cannot create DeleteOperator. The results won't be correct" << endl;
		return m_lpWOSReturnOp;
	}
	
	// delete all tuples
	if (bIsNewDeleteVector == 1)
	{
		m_lpDeleteOperator->deleteProjection();
	}
	m_lpWOSReturnOp = m_lpDeleteOperator;

	return m_lpWOSReturnOp;
}


// Merge ROS and WOS operators
Operator* DeleteProjectionNode::mergeROSandWOS()
{
	// Union
	if (m_lpReturnOp != NULL)
	{
		// Has been merged
		return m_lpReturnOp;
	}
	
	if ( (m_lpROSReturnOp == NULL) || (m_lpWOSReturnOp == NULL) )
	{
		cout << "ERROR: Invalid ros or wos. The results won't be correct." << endl;
		return m_lpReturnOp;
	}
	
	// Merge them
	m_lpReturnOp = new Union(m_lpROSReturnOp, m_lpWOSReturnOp, 1);	
	return m_lpReturnOp;
}


// Shows what operator this node will run
void DeleteProjectionNode::showROS(int* lpNodeID, ofstream* lpOfstream)
{
	// The operator exists
	if (m_bROSHasShown)
	{
		return;
	}
	
	m_bROSHasShown = 1;
	
	// Set nodeID to have different variable name for this node
	setNodeID(lpNodeID);
	
	string sOps = "//DELETE ROS FROM PROJECTION: \n";

	bool bIsNewDeleteVector = 0;
	if (m_bDeleteOperatorHasShown == 0)
	{
		sOps = sOps + "	// The following 2 lines of code will be replaced by\n";
		sOps = sOps + "	// WOSManager* lpWOSManager = CatalogInstance::getCatalog()->getWOSManager(m_sProjection);\n";
		sOps = sOps + "	string sColumnAny = CatalogInstance::getCatalog()->getPrimaryColumnName(\"" + m_sProjection + "\");\n";
		sOps = sOps + "	WOSManager* lpROS_WOSManager = CatalogInstance::getCatalog()->getWOSManager(sColumnAny);\n";
		
		sOps = sOps + "	DeleteOperator* lpDeleteOp = new DeleteOperator(lpROS_WOSManager);\n";
		bIsNewDeleteVector = 1;
		m_bDeleteOperatorHasShown = 1;
	}
	
	// delete all tuples
	if (bIsNewDeleteVector)
	{
		sOps = sOps + "	lpDeleteOp->deleteProjection();\n";
	}
	lpOfstream->write(sOps.c_str(), sOps.length());

}
		
// show what WOS operator this node will run
void DeleteProjectionNode::showWOS(int* lpNodeID, ofstream* lpOfstream)
{
	// The operator exists
	if (m_bWOSHasShown)
	{
		return;
	}
	
	m_bWOSHasShown = 1;
	
	// Set nodeID to have different variable name for this node
	setNodeID(lpNodeID);
	
	string sOps = "//DELETE WOS FROM PROJECTION: \n";
	
	bool bIsNewDeleteVector = 0;
	if (m_bDeleteOperatorHasShown == 0)
	{
		sOps = sOps + "	// The following 2 lines of code will be replaced by\n";
		sOps = sOps + "	// WOSManager* lpWOSManager = CatalogInstance::getCatalog()->getWOSManager(m_sProjection);\n";
		sOps = sOps + "	string sColumnAny = CatalogInstance::getCatalog()->getPrimaryColumnName(\"" + m_sProjection + "\");\n";
		sOps = sOps + "	WOSManager* lpWOS_WOSManager = CatalogInstance::getCatalog()->getWOSManager(sColumnAny);\n";
		
		sOps = sOps + "	DeleteOperator* lpDeleteOp = new DeleteOperator(lpWOS_WOSManager);\n";
		bIsNewDeleteVector = 1;
		m_bDeleteOperatorHasShown = 1;
	}
	
	// delete all tuples
	if (bIsNewDeleteVector)
	{
		sOps = sOps + "	lpDeleteOp->deleteProjection();\n";
	}
	lpOfstream->write(sOps.c_str(), sOps.length());
}


		
// show tree presentation
void DeleteProjectionNode::showTree(string sTabs)
{
	cout << "showTree of DeleteProjectionNode has not been implemented yet" << endl;
}


// show merge operators
void DeleteProjectionNode::showMerge(ofstream* lpOfstream)
{
	char sNodeID[15];	
	sprintf(sNodeID, "%d", m_iNodeID);
	
	string sOps = "//UNION:\n";
	// Use child node ID to set name for this merge operator
	sOps = sOps + "	Operator* lp_Merge" + string(sNodeID) + " = new Union(lp_ROS" + string(sNodeID) + ", lp_WOS" + string(sNodeID) + ", 1); \n";
	
	lpOfstream->write(sOps.c_str(), sOps.length());
}

//---------------------------------------------------------------
// Memory management functions

// New a node and put it in the m_lpList
DeleteProjectionNode* DeleteProjectionNode::create()
{
	Log::writeToLog("DeleteProjectionNode", 10, "Create without arguments");
		
	DeleteProjectionNode* lpNode = new DeleteProjectionNode();
	pushBack(lpNode);
	return lpNode;
}

// New a node and put it in the m_lpList
DeleteProjectionNode* DeleteProjectionNode::create(string sProjection, bool bIsSinglePredicate)
{
	Log::writeToLog("DeleteProjectionNode", 10, "Create with 2 arguments: string sProjection, bool bIsSinglePredicate");
		
	DeleteProjectionNode* lpNode = new DeleteProjectionNode(sProjection, bIsSinglePredicate);
	pushBack(lpNode);
	return lpNode;
}


