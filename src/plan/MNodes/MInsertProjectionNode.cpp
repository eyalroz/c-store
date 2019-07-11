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
 *  MInsertProjectionNode.cpp
 *  cstore
 *
 *  Created by Nga Tran on 6/10/05.
 *  
 *	Implementation of INSERT INTO projectionName SELECT ....
 */

#include "MInsertProjectionNode.h"

MInsertProjectionNode::MInsertProjectionNode() : MNode()
{
	m_lpProjection = NULL;
}


MInsertProjectionNode::MInsertProjectionNode(list<Node*> lpChildren, Projection* lpProjection, list<EColumn*> listColumns) : MNode(lpChildren)
{
	m_lpProjection = lpProjection;
	m_listColumns = listColumns;
}		


MInsertProjectionNode::~MInsertProjectionNode()
{
	Log::writeToLog("MInsertProjectionNode", 10, "~MInsertProjectionNode");
	 
	 if (m_lpReturnOp != NULL)
	{
		delete m_lpReturnOp;
		m_lpReturnOp = NULL;
	}
}

// Run the operator of this node
Operator* MInsertProjectionNode::run()
{	
	if (m_lpReturnOp != NULL)
	{
		// already ran
		return m_lpReturnOp;
	}
	
	if (m_listColumns.size() != m_children.size())
	{
		cout << "ERROR: Number of columns in the insert projection is different from the number of columns in the select query" << endl;
		return m_lpReturnOp;
	}
	
	int numCols = m_children.size();
	Operator* srcs[numCols];
	int numColumns[numCols];
	int i = 0;
	
	// Run all child nodes first	
	list<EColumn*>::iterator lpColumn = m_listColumns.begin();
	list<Node*>::iterator lpDataSource = m_children.begin();
	while ( (lpColumn != m_listColumns.end()) && (lpDataSource != m_children.end()) )
	{
		// get column index in the projection
		int iColIndex = CatalogInstance::getCatalog()->getColumnIndex(m_lpProjection->toString(), (*lpColumn)->toString());
	
		// run data source
		if ((*lpDataSource) != NULL)
		{		
			// only insert into WOS
			Operator* lpOpWOS = NULL;
			lpOpWOS = (*lpDataSource)->runWOS();
			
			srcs[i] = lpOpWOS;
			numColumns[i] = iColIndex;
			
			i++;
		}
		lpColumn++;
		lpDataSource++;
	}
	
	// Get WOSManager
	// The following 2 lines of code will be replaced by
	// WOSManager* lpWOSManager = CatalogInstance::getCatalog()->getWOSManager(m_lpProjection->toString()); 
	string sColumnAny = CatalogInstance::getCatalog()->getPrimaryColumnName(m_lpProjection->toString());
	WOSManager* lpWOSManager = CatalogInstance::getCatalog()->getWOSManager(sColumnAny);
  
	m_lpReturnOp = new InsertOperator(lpWOSManager);
	( (InsertOperator*) m_lpReturnOp)->appendColumns(srcs, numCols, numColumns); 

	return m_lpReturnOp;

}

// show what operator this node will run
void MInsertProjectionNode::show(int* lpNodeID, ofstream* lpOfstream)
{
	// The operator exists
	if (m_bROSHasShown)
	{
		return;
	}
	
	m_bROSHasShown = 1;
	
	// Set nodeID to have different variable name for this node
	setNodeID(lpNodeID);
	
	string sOps = "";
	
	// Run all child nodes first
	int numCols = m_children.size();
	
	if (m_listColumns.size() != m_children.size())
	{
		sOps = sOps + "	//ERROR: Number of columns in the insert projection is different from the number of columns in the select query. Insert fails\n";
		lpOfstream->write(sOps.c_str(), sOps.length());
	}
	
	char sTmp[15];	
	sprintf(sTmp, "%d", numCols);
	
	string sSrcs = "	Operator* srcs[" + string(sTmp) + "] = {";
	string sNumCols = "	int numColumns[" + string(sTmp) + "] = {";
	
	int i = 0;
	
	// Run all child nodes first	
	list<EColumn*>::iterator lpColumn = m_listColumns.begin();
	list<Node*>::iterator lpDataSource = m_children.begin();
	while ( (lpColumn != m_listColumns.end()) && (lpDataSource != m_children.end()) )
	{
		// get column index in the projection
		int iColIndex = CatalogInstance::getCatalog()->getColumnIndex(m_lpProjection->toString(), (*lpColumn)->toString());
		 
		// run data source
		if ((*lpDataSource) != NULL)
		{		
			// only insert into WOS
			(*lpDataSource)->showWOS(lpNodeID, lpOfstream);
			
			sprintf(sTmp, "%d", (*lpDataSource)->getNodeID());
			sSrcs = sSrcs + "lp_WOS" + string(sTmp);
			
			sprintf(sTmp, "%d", iColIndex);
			sNumCols = sNumCols + string(sTmp);
				
			//srcs[i] = lpOpWOS;
			//numColumns[i] = iColIndex;
			
			i++;
			
			if (i < numCols) 
			{
				sSrcs = sSrcs + ", ";
				sNumCols = sNumCols + ", ";
			}
		}
		lpColumn++;
		lpDataSource++;
	}
	
	sSrcs = sSrcs + "};\n";
	sNumCols = sNumCols + "};\n";
	
	char sCols[15];	
	sprintf(sCols, "%d", numCols);

	sOps = sOps + sSrcs;
	sOps = sOps + sNumCols;
	
	sOps = sOps + "\n//INSERT\n";
	sOps = sOps + "	// The following 2 lines of code will be replaced by\n";
	sOps = sOps + "	// WOSManager* lpWOSManager = CatalogInstance::getCatalog()->getWOSManager(m_lpProjection->toString());\n";
	sOps = sOps + "	string sColumnAny = CatalogInstance::getCatalog()->getPrimaryColumnName(\"" + m_lpProjection->toString() + "\");\n";
	sOps = sOps + "	WOSManager* lpWOSManager = CatalogInstance::getCatalog()->getWOSManager(sColumnAny);\n\n";
 
	sOps = sOps + "	InsertOperator* lpInsertOp = new InsertOperator(lpWOSManager);\n";
	sOps = sOps + "	lpInsertOp->appendColumns(srcs, " + sCols + ", numColumns);\n"; 

	lpOfstream->write(sOps.c_str(), sOps.length());	
}

// show tree presentation
void MInsertProjectionNode::showTree(string sTabs)
{
	// Show its information first
	cout << endl;
	cout << sTabs << "Insert new values into all columns of the projection" << endl;
	
	MNode::showTree(sTabs);
}


//---------------------------------------------------------------
// Memory management functions

// New a node and put it in the m_lpList
MInsertProjectionNode* MInsertProjectionNode::create()
{
	Log::writeToLog("MInsertProjectionNode", 10, "Create without arguments");
		
	MInsertProjectionNode* lpNode = new MInsertProjectionNode();
	pushBack(lpNode);
	return lpNode;
}

// New a node and put it in the m_lpList
MInsertProjectionNode* MInsertProjectionNode::create(list<Node*> lpChildren, Projection* lpProjection, list<EColumn*> listColumns)
{
	Log::writeToLog("MInsertProjectionNode", 10, "Create with 1 argument: list of child nodes");
		
	MInsertProjectionNode* lpNode = new MInsertProjectionNode(lpChildren, lpProjection, listColumns);
	pushBack(lpNode);
	return lpNode;
}
