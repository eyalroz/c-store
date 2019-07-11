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
 *  UDeleteFromProjectionNode.cpp
 *  cstore
 *
 *  Created by Nga Tran on 6/23/05.
 *  
 *	Implementation of DELETE FROM projectionName WHERE ...
 */

#include "UDeleteProjectionNode.h"
#include "../../Operators/Union.h"

UDeleteProjectionNode::UDeleteProjectionNode() : UNode()
{
	m_bIsSinglePredicate = 1;
	m_lpDeleteOperator = NULL;
	m_bDeleteOperatorHasShown = 0;
}

UDeleteProjectionNode::UDeleteProjectionNode(Node* lpChild, string sProjection, bool bIsSinglePredicate) : UNode(lpChild)
{
	m_sProjection = sProjection;
	m_bIsSinglePredicate = bIsSinglePredicate;
	m_lpDeleteOperator = NULL;
	m_bDeleteOperatorHasShown = 0;
}


UDeleteProjectionNode::~UDeleteProjectionNode()
{
	 Log::writeToLog("UDeleteProjectionNode", 10, "~UDeleteProjectionNode");
	 
	if (m_lpDeleteOperator != NULL)
	{
		delete m_lpDeleteOperator;
		m_lpDeleteOperator = NULL;
	}
	
	if (m_lpReturnOp != NULL)
	{
		delete  m_lpReturnOp;
		m_lpReturnOp = NULL;
	}
}

// Run ROS only
Operator* UDeleteProjectionNode::runROS()
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
	

		// Run ROS of its child (where predicate)
		Operator* lpOpROS = NULL;
		lpOpROS = m_lpChild->runROS();
		
		if (lpOpROS == NULL) 
		{
			cout << "ERROR: ROS data sources is NULL. The results won't be correct" << endl;
			return m_lpROSReturnOp;
		}
		
		if (m_bIsSinglePredicate == 1)
		{
			m_lpDeleteOperator->deleteTuple(lpOpROS, true);
		}
		else
		{
			m_lpDeleteOperator->deleteByPositionList(lpOpROS, true);
		}
									
	m_lpROSReturnOp = m_lpDeleteOperator;
	return m_lpROSReturnOp;

}
		
// Run WOS only
Operator* UDeleteProjectionNode::runWOS()
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
	
		// Run WOS of its child (where predicate)
		Operator* lpOpWOS = NULL;
		lpOpWOS = m_lpChild->runWOS();
		
		if (lpOpWOS == NULL) 
		{
			cout << "ERROR: WOS data sources is NULL. The results won't be correct" << endl;
			return m_lpWOSReturnOp;
		}
		
		if (m_bIsSinglePredicate == 1)
		{
			m_lpDeleteOperator->deleteTuple(lpOpWOS, true);
		}
		else
		{
			m_lpDeleteOperator->deleteByPositionList(lpOpWOS, true);
		}
	
	m_lpROSReturnOp = m_lpDeleteOperator;
	
	return m_lpWOSReturnOp;
}


// Merge ROS and WOS operators
Operator* UDeleteProjectionNode::mergeROSandWOS()
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
void UDeleteProjectionNode::showROS(int* lpNodeID, ofstream* lpOfstream)
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


		m_lpChild->showROS(lpNodeID, lpOfstream);
	
		char sChildNode[15];	
		sprintf(sChildNode, "%d", m_lpChild->getNodeID());
		
		if (m_bIsSinglePredicate == 1)
		{
			sOps = sOps + "	lpDeleteOp->deleteTuple(lp_ROS" +  string(sChildNode) + ", true);\n";
		}
		else
		{
			sOps = sOps + "	lpDeleteOp->deleteByPositionList(lp_ROS" +  string(sChildNode) + ", true);\n";
		}
	
	lpOfstream->write(sOps.c_str(), sOps.length());

}
		
// show what WOS operator this node will run
void UDeleteProjectionNode::showWOS(int* lpNodeID, ofstream* lpOfstream)
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

		m_lpChild->showWOS(lpNodeID, lpOfstream);
	
		char sChildNode[15];	
		sprintf(sChildNode, "%d", m_lpChild->getNodeID());
		
		if (m_bIsSinglePredicate == 1)
		{
			sOps = sOps + "	lpDeleteOp->deleteTuple(lp_WOS" +  string(sChildNode) + ", false);\n";
		}
		else
		{
			sOps = sOps + "	lpDeleteOp->deleteByPositionList(lp_WOS" +  string(sChildNode) + ", false);\n";
		}

	lpOfstream->write(sOps.c_str(), sOps.length());
}


		
// show tree presentation
void UDeleteProjectionNode::showTree(string sTabs)
{
	cout << "showTree of UDeleteProjectionNode has not been implemented yet" << endl;
}


// show merge operators
void UDeleteProjectionNode::showMerge(ofstream* lpOfstream)
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
UDeleteProjectionNode* UDeleteProjectionNode::create()
{
	Log::writeToLog("UDeleteProjectionNode", 10, "Create without arguments");
		
	UDeleteProjectionNode* lpNode = new UDeleteProjectionNode();
	pushBack(lpNode);
	return lpNode;
}

UDeleteProjectionNode* UDeleteProjectionNode::create(Node* lpChild, string sProjection, bool bIsSinglePredicate)
{
	Log::writeToLog("UDeleteProjectionNode", 10, "Create with arguments: string sProjection, bool bIsSinglePredicate");
		
	UDeleteProjectionNode* lpNode = new UDeleteProjectionNode(lpChild, sProjection, bIsSinglePredicate);
	pushBack(lpNode);
	return lpNode;
}
