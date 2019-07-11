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
 *  UInsertColumnNode.cpp
 *  cstore
 *
 *  Created by Nga Tran on 6/9/05.
 * 
 *	Node to do insert a available value in to a given column
 */

#include "UInsertColumnNode.h"

UInsertColumnNode::UInsertColumnNode() : UNode()
{
	m_iValueType = UNKNOWN_TYPE;
	m_iValue = 0;
	m_nValue = 0;
	m_sValue = "";
}

UInsertColumnNode::UInsertColumnNode(Node* lpChild, int iValue) : UNode(lpChild)
{
	m_iValueType = INT_TYPE;
	m_iValue = iValue;
	m_nValue = 0;
	m_sValue = "";
}


UInsertColumnNode::UInsertColumnNode(Node* lpChild, double nValue) : UNode(lpChild)
{
	m_iValueType = DBL_TYPE;
	m_iValue = 0;
	m_nValue = nValue;
	m_sValue = "";
}


UInsertColumnNode::UInsertColumnNode(Node* lpChild, string sValue) : UNode(lpChild)
{
	m_iValueType = STR_TYPE;
	m_iValue = 0;
	m_nValue = 0;
	m_sValue = sValue;
}


UInsertColumnNode::~UInsertColumnNode()
{
	 Log::writeToLog("UInsertColumnNode", 10, "~UInsertColumnNode");
}


		
// Run ROS only
Operator* UInsertColumnNode::runROS()
{
	// The operator exists, only return it
	if (m_lpROSReturnOp != NULL)
	{
		return m_lpROSReturnOp;
	}	
	
	/////
	Operator* lpOp = NULL;
	if (m_lpChild != NULL)
	{
		lpOp = m_lpChild->runROS();
	}
	
	if (lpOp != NULL)
	{
		if (m_iValueType == INT_TYPE)
		{
			//m_lpROSReturnOp = new InsertOp(lpOp, m_iValue);
		}
		else if (m_iValueType == DBL_TYPE)
		{
			//m_lpROSReturnOp = new InsertOp(lpOp, m_nValue);
		}
		else if (m_iValueType == STR_TYPE)
		{
			//m_lpROSReturnOp = new InsertOp(lpOp, m_sValue);
		}
		
		cout << "//////////InsertOp operator has not been implemented yet. The results won't be correct///////////////" << endl;
	}
	 
	return m_lpROSReturnOp;

}

		
// Run WOS only
Operator* UInsertColumnNode::runWOS()
{
	// The operator exists, only return it
	if (m_lpWOSReturnOp != NULL)
	{
		return m_lpWOSReturnOp;
	}	
	
	/////
	Operator* lpOp = NULL;
	if (m_lpChild != NULL)
	{
		lpOp = m_lpChild->runWOS();
	}
	
	if (lpOp != NULL)
	{
		if (m_iValueType == INT_TYPE)
		{
			//m_lpWOSReturnOp = new InsertOp(lpOp, m_iValue);
		}
		else if (m_iValueType == DBL_TYPE)
		{
			//m_lpWOSReturnOp = new InsertOp(lpOp, m_nValue);
		}
		else if (m_iValueType == STR_TYPE)
		{
			//m_lpWOSReturnOp = new InsertOp(lpOp, m_sValue);
		}		
		
		cout << "//////////InsertOp operator has not been implemented yet. The results won't be correct///////////////" << endl;
	}
	 
	return m_lpWOSReturnOp;
}



// Shows what operator this node will run
void UInsertColumnNode::showROS(int* lpNodeID, ofstream* lpOfstream)
{
	// The operator exists
	if (m_bROSHasShown)
	{
		return;
	}
	
	commonShowROS(lpNodeID, "InsertOp", lpOfstream);
	
	// Show: "InsertOp* m_lpWOSReturnOp = new InsertOp(lpOp, sValue);"
	if (m_lpChild != NULL)
	{
		char sNodeID[15];	
		sprintf(sNodeID, "%d", m_iNodeID);
		
		char sChildNodeID[15];	
		sprintf(sChildNodeID, "%d", m_lpChild->getNodeID());
		
		char sValue[15];
		
		string sOps = "//INSERT COLUMN:\n";
		sOps = sOps + "	Operator* lp_ROS" + sNodeID + " = new InsertOp(lp_ROS" + sChildNodeID + ", ";
		
		if (m_iValueType == INT_TYPE)
		{			
			sprintf(sValue, "%d", m_iValue);
			sOps = sOps + sValue;
		}
		else if (m_iValueType == DBL_TYPE)
		{
			sprintf(sValue, "%f",  m_nValue);
			sOps = sOps + sValue;
		}
		else if (m_iValueType == STR_TYPE)
		{
			sOps = sOps + m_sValue;
		}
		
		sOps = sOps + ");\n";
		
		lpOfstream->write(sOps.c_str(), sOps.length());
	}
}


		
// show what WOS operator this node will run
void UInsertColumnNode::showWOS(int* lpNodeID, ofstream* lpOfstream)
{
	// The operator exists
	if (m_bWOSHasShown)
	{
		return;
	}
	
	commonShowWOS(lpNodeID, "InsertOp", lpOfstream);
	
	// Show: "InsertOp* m_lpWOSReturnOp = new InsertOp(lpOp, m_sValue);"
	if (m_lpChild != NULL)
	{
		char sNodeID[15];	
		sprintf(sNodeID, "%d", m_iNodeID);
		
		char sChildNodeID[15];	
		sprintf(sChildNodeID, "%d", m_lpChild->getNodeID());
		
		char sValue[15];
		
		string sOps = "//INSERT COLUMN:\n";
		sOps = sOps + "	Operator* lp_WOS" + sNodeID + " = new InsertOp(lp_WOS" + sChildNodeID + ", ";
		
		if (m_iValueType == INT_TYPE)
		{			
			sprintf(sValue, "%d", m_iValue);
			sOps = sOps + sValue;
		}
		else if (m_iValueType == DBL_TYPE)
		{
			sprintf(sValue, "%f",  m_nValue);
			sOps = sOps + sValue;
		}
		else if (m_iValueType == STR_TYPE)
		{
			sOps = sOps + m_sValue;
		}
		
		sOps = sOps + ");\n";
		
		lpOfstream->write(sOps.c_str(), sOps.length());
	}
}


		
// show tree presentation
void UInsertColumnNode::showTree(string sTabs)
{
	// The operator exists
	if (m_bTreeHasShown)
	{
		cout << endl;
		cout << sTabs << "INSERT COLUMN - The operator was created before" << endl;
		
		return;
	}
	
	m_bTreeHasShown = 1;
	
	// Show its information first
	cout << endl;
	cout << sTabs << "INSERT COLUMN" << endl;
	
	cout << sTabs << "Operator: InsertOp" << endl;
	cout << sTabs << "	" << "Argument1: Operator (below)" << endl;
	cout << sTabs << "	" << "Argument2: value: ";
	
	if (m_iValueType == INT_TYPE)
	{			
		cout << m_iValue << endl;
	}
	else if (m_iValueType == DBL_TYPE)
	{
		cout << m_nValue << endl;
	}
	else if (m_iValueType == STR_TYPE)
	{
		cout << m_sValue << endl;
	} 
		
	// Show child's information
	if (m_lpChild != NULL)
	{
		sTabs = sTabs + "	";
		m_lpChild->showTree(sTabs);
	}
}

//---------------------------------------------------------------
// Memory management functions
		
	
// New a node and put it in the m_lpList
UInsertColumnNode* UInsertColumnNode::create()
{
	Log::writeToLog("UInsertColumnNode", 10, "Create without arguments");
		
	UInsertColumnNode* lpNode = new UInsertColumnNode();
	pushBack(lpNode);
	return lpNode;
}

UInsertColumnNode* UInsertColumnNode::create(Node* lpChild, int iValue)
{
	Log::writeToLog("UInsertColumnNode", 10, "Create with arguments: Node* lpChild, int iValue");
		
	UInsertColumnNode* lpNode = new UInsertColumnNode(lpChild, iValue);
	pushBack(lpNode);
	return lpNode;
}

UInsertColumnNode* UInsertColumnNode::create(Node* lpChild, double nValue)
{
	Log::writeToLog("UInsertColumnNode", 10, "Create with arguments: Node* lpChild, double nValue");
		
	UInsertColumnNode* lpNode = new UInsertColumnNode(lpChild, nValue);
	pushBack(lpNode);
	return lpNode;
}

UInsertColumnNode* UInsertColumnNode::create(Node* lpChild, string sValue)
{
	Log::writeToLog("UInsertColumnNode", 10, "Create with arguments: Node* lpChild, string sValue");
		
	UInsertColumnNode* lpNode = new UInsertColumnNode(lpChild, sValue);
	pushBack(lpNode);
	return lpNode;
}

