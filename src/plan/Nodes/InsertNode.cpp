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
 *  InsertNode.cpp
 *  cstore
 *
 *  Created by Nga Tran on 6/6/05.
 *  
 *	Implementation of INSERT INTO projectionName VALUES(...);
 *	The name of this class should be changed to InsertProjectionNode
 */

#include "InsertNode.h"

InsertNode::InsertNode() : Node()
{
	m_lpProjection = NULL;
}

InsertNode::InsertNode(Projection* lpProjection, list<EColumn*>	 listColumns, list<Expression*> listValues) : Node()
{
	m_lpProjection = lpProjection;
	m_listColumns = listColumns;
	m_listValues = listValues;
}

InsertNode::~InsertNode()
{
	 Log::writeToLog("InsertNode", 10, "~InsertNode"); 
	 
	if (m_lpReturnOp != NULL)
	{
		delete m_lpReturnOp;
		m_lpReturnOp = NULL;
	}
}
		
// Run 
Operator* InsertNode::run(bool rosOnly)
{
  if (rosOnly)
    {
      // NO WOS --> insert nothing
      return m_lpReturnOp;
    }
	if (m_lpReturnOp != NULL)
	{
		return m_lpReturnOp;
	}
	
	// Get WOSManager
	// The following 2 lines of code will be replaced by
	// WOSManager* lpWOSManager = CatalogInstance::getCatalog()->getWOSManager(m_lpProjection->toString()); 
	string sColumnAny = CatalogInstance::getCatalog()->getPrimaryColumnName(m_lpProjection->toString());
	WOSManager* lpWOSManager = CatalogInstance::getCatalog()->getWOSManager(sColumnAny);
  
	m_lpReturnOp = new InsertOperator(lpWOSManager);
	
	// Get storage key of the projection
	int iStorageKey = ( (InsertOperator*) m_lpReturnOp)->initTuple();
	
	//Get all column to be inserted
	list<EColumn*>::iterator lpColumn = m_listColumns.begin();
	list<Expression*>::iterator lpValue = m_listValues.begin();
	while ( (lpColumn != m_listColumns.end()) && (lpValue != m_listValues.end()) )
	{
		// get column index in the projection
		int iColIndex = CatalogInstance::getCatalog()->getColumnIndex(m_lpProjection->toString(), (*lpColumn)->toString());

		// Get type (integer , float or string)
		if ((*lpValue)->getType(NULL) == TYPE_STRING)
		{
			// String
			// lpInsertOp->populateField(iColIndex, iStorageKey, ((EString*) (*lpValue))->getString());
			cout << "String-type column is not supported yet. Insert fails" << endl;
			return m_lpReturnOp;
		}
		else if ((*lpValue)->getType(NULL) == TYPE_INTEGER)
		{
			// Integer
			( (InsertOperator*) m_lpReturnOp)->populateField(iColIndex, iStorageKey, ((EInteger*) (*lpValue))->get());
		}
		else if ((*lpValue)->getType(NULL) == TYPE_FLOAT)
		{
			// Double
			// lpInsertOp->populateField(iColIndex, iStorageKey, ((EFloat*) (*lpValue))->get());
			cout << "Double-type column is not supported yet. Insert fails" << endl;
			return m_lpReturnOp;
		}
		else
		{
			cout << "ERROR: Invalid value: " << (*lpValue)->toString() << " for column: " << (*lpColumn)->toString() << endl;
			cout << "Insert fails" << endl;
			return m_lpReturnOp;
		}
		
		lpColumn++;
		lpValue++;
	}
	
	( (InsertOperator*) m_lpReturnOp)->appendTuple(iStorageKey);
	
	return m_lpReturnOp;
}

		
// show what operator this node will run
void InsertNode::show(int* lpNodeID, ofstream* lpOfstream, bool rosOnly)
{
  string sOps = "//INSERT:\n";

  if (rosOnly)
    {
      sOps += "// ROS ONLY. No WOS to do insert\n";
    }
  else
    {
	// The operator exists
	if (m_bHasShown)
	{
		return;
	}
	
	m_bHasShown = 1;
	
	// Set nodeID to have different variable name for this node
	setNodeID(lpNodeID);
	
	bool bSuccess = 1;
	
	
	
	sOps = sOps + "	// The following 2 lines of code will be replaced by\n";
	sOps = sOps + "	// WOSManager* lpWOSManager = CatalogInstance::getCatalog()->getWOSManager(m_lpProjection->toString());\n";
	sOps = sOps + "	string sColumnAny = CatalogInstance::getCatalog()->getPrimaryColumnName(\"" + m_lpProjection->toString() + "\");\n";
	sOps = sOps + "	WOSManager* lpWOSManager = CatalogInstance::getCatalog()->getWOSManager(sColumnAny);\n";
		
	sOps = sOps + "	InsertOperator* lpInsertOp = new InsertOperator(lpWOSManager);\n";
	
	// Get storage key of the projection
	sOps = sOps + "	int iStorageKey = lpInsertOp->initTuple();\n";
	
	sOps = sOps + "	int iColIndex = 0;\n";
	
	//Get all column to be inserted
	list<EColumn*>::iterator lpColumn = m_listColumns.begin();
	list<Expression*>::iterator lpValue = m_listValues.begin();
	while ( (lpColumn != m_listColumns.end()) && (lpValue != m_listValues.end()) )
	{
		// get column index in the projection
		sOps = sOps + "	iColIndex = CatalogInstance::getCatalog()->getColumnIndex(\"" + m_lpProjection->toString() + "\", \"" + (*lpColumn)->toString() + "\");\n";

		// Get type (integer , float or string)
		if ((*lpValue)->getType(NULL) == TYPE_STRING)
		{
			// String
			// lpInsertOp->populateField(iColIndex, iStorageKey, ((EString*) (*lpValue))->getString());
			sOps = sOps + "	//String-type column is not supported yet. Insert fails\n";
			 bSuccess = 0;
			break;
		}
		else if ((*lpValue)->getType(NULL) == TYPE_INTEGER)
		{
			// Integer
			char sValue[15];
			sprintf(sValue, "%d", ((EInteger*) (*lpValue))->get());
			sOps = sOps + "	lpInsertOp->populateField(iColIndex, iStorageKey, " +  string(sValue) + ");\n";
		}
		else if ((*lpValue)->getType(NULL) == TYPE_FLOAT)
		{
			// Double
			// lpInsertOp->populateField(iColIndex, iStorageKey, ((EFloat*) (*lpValue))->get());
			sOps = sOps + "	//Double-type column is not supported yet. Insert fails\n";
			 bSuccess = 0;
			break;
		}
		else
		{
			sOps = sOps + "	//ERROR: Invalid value: " + (*lpValue)->toString() + " for column: " + (*lpColumn)->toString() + "\n";
			sOps = sOps + "	//Insert fails\n";
			 bSuccess = 0;
			break;
		}
		
		lpColumn++;
		lpValue++;
	}
	
	if ( bSuccess == 1)
	{
		sOps = sOps + "	lpInsertOp->appendTuple(iStorageKey);\n";
	}
    }	
	lpOfstream->write(sOps.c_str(), sOps.length());
}


// show tree presentation
void InsertNode::showTree(string sTabs)
{
	// The operator exists
	if (m_bTreeHasShown)
	{
		cout << endl;
		cout << sTabs << "INSERT - The operator was created before" << endl;
		
		return;
	}
	
	m_bTreeHasShown = 1;
	
	cout << sTabs << "INSERT has not implemented yet" << endl;	
}

//---------------------------------------------------------------
// Memory management functions

// New a node and put it in the m_lpList
InsertNode* InsertNode::create()
{
	Log::writeToLog("InsertNode", 10, "Create without arguments");
		
	InsertNode* lpNode = new InsertNode();
	pushBack(lpNode);
	return lpNode;
}

// New a node and put it in the m_lpList
InsertNode* InsertNode::create(Projection* lpProjection, list<EColumn*>	 listColumns, list<Expression*> listValues)
{
	Log::writeToLog("InsertNode", 10, "Create with arguments: Projection* lpProjection, list<EColumn*>	 listColumns, list<Expression*> listValues");
		
	InsertNode* lpNode = new InsertNode(lpProjection, listColumns, listValues);
	pushBack(lpNode);
	return lpNode;
}


