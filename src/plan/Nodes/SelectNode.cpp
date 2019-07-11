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
 *  SelectNode.cpp
 *  cstore
 *
 *  Created by Nga Tran on 3/17/05.
 *  Copyright 2005 __MyCompanyName__. All rights reserved.
 *
 */

#include "SelectNode.h"
#include "../../common/DataSource.h"
#include "../../Wrappers/RLEDataSource.h"
#include "../../Wrappers/IntDataSource.h"
#include "../../AM/WOSManager.h"
#include "../../AM/CatalogInstance.h"
#include "../../Operators/Union.h"
#include <iostream>


// Default
SelectNode::SelectNode() : Node()
{
	m_bHasPredicate = 0;
	m_iPredicateType = -1;
	m_sRHS = "";
	m_nRHS = 0;
	m_sColumn = "";
	m_iRHSType = STR_TYPE;
	m_iRHS = 0;
	m_sProjection = "";
	m_iJoinType = SELECT_NOT_FOR_JOIN;
	setPredicateType(PREDICATE_SELECT); // itself is a select node
	
	m_lpROSPredicate = NULL;
	m_lpWOSPredicate = NULL;
	
	getCatalogInfo();
}


// No predicate
SelectNode::SelectNode(string sColumn, string sProjection) : Node()
{
	m_bHasPredicate = 0;
	m_iPredicateType = -1;
	m_sRHS = "";
	m_nRHS = 0;
	m_sColumn = sColumn;
	m_iRHSType = STR_TYPE;
	m_iRHS = 0;
	m_sProjection = sProjection;
	m_iJoinType = SELECT_NOT_FOR_JOIN;
	setPredicateType(PREDICATE_SELECT); // itself is a select node
	
	m_lpROSPredicate = NULL;
	m_lpWOSPredicate = NULL;
	
	getCatalogInfo();
}	

// No predicate, is for join
SelectNode::SelectNode(string sColumn, string sProjection, int iJoinType) : Node()
{
	m_bHasPredicate = 0;
	m_iPredicateType = -1;
	m_sRHS = "";
	m_nRHS = 0;
	m_sColumn = sColumn;
	m_iRHSType = STR_TYPE;
	m_iRHS = 0;
	m_sProjection = sProjection;
	m_iJoinType = iJoinType;
	setPredicateType(PREDICATE_SELECT); // itself is a select node
	
	m_lpROSPredicate = NULL;
	m_lpWOSPredicate = NULL;
	
	getCatalogInfo();
}
	

// Right hand side is a float
SelectNode::SelectNode(string sColumn, string sProjection, int iPredicateType, double nRHS)	 : Node()
{
	m_bHasPredicate = 1;
	m_iPredicateType = iPredicateType;
	m_sRHS = "";
	m_nRHS = nRHS;
	m_sColumn = sColumn;
	m_iRHSType = DBL_TYPE;
	m_iRHS = 0;
	m_sProjection = sProjection;
	m_iJoinType = SELECT_NOT_FOR_JOIN;
	setPredicateType(PREDICATE_SELECT); // itself is a select node
	
	m_lpROSPredicate = NULL;
	m_lpWOSPredicate = NULL;
	
	getCatalogInfo();
}


// Right hand side is a string
SelectNode::SelectNode(string sColumn, string sProjection, int iPredicateType, string sRHS) : Node()
{
	m_bHasPredicate = 1;
	m_iPredicateType = iPredicateType;
	m_sRHS = sRHS;
	m_nRHS = 0;
	m_sColumn = sColumn;
	m_iRHSType = STR_TYPE;
	m_iRHS = 0;
	m_sProjection = sProjection;
	m_iJoinType = SELECT_NOT_FOR_JOIN;
	setPredicateType(PREDICATE_SELECT); // itself is a select node
	
	m_lpROSPredicate = NULL;
	m_lpWOSPredicate = NULL;
	
	getCatalogInfo();
}

// Right hand side is an integer
SelectNode::SelectNode(string sColumn, string sProjection, int iPredicateType, int nRHS)	 : Node()
{
	m_bHasPredicate = 1;
	m_iPredicateType = iPredicateType;
	m_sRHS = "";
	m_nRHS = nRHS;
	m_sColumn = sColumn;
	m_iRHSType = INT_TYPE;
	m_iRHS = nRHS;
	m_sProjection = sProjection;
	m_iJoinType = SELECT_NOT_FOR_JOIN;
	setPredicateType(PREDICATE_SELECT); // itself is a select node
	
	m_lpROSPredicate = NULL;
	m_lpWOSPredicate = NULL;
	
	getCatalogInfo();
}

// Deallocate memory for its members
SelectNode::~SelectNode()
{
	  Log::writeToLog("SelectNode", 10, "~SelectNode"); 
	  
	if (m_lpReturnOp != NULL)
	{
		delete m_lpReturnOp;
		m_lpReturnOp = NULL;
	}
	
	if (m_lpROSReturnOp != NULL)
	{
		delete m_lpROSReturnOp;
		m_lpROSReturnOp = NULL;
	}
	
	if (m_lpWOSReturnOp != NULL)
	{
		delete m_lpWOSReturnOp;
		m_lpWOSReturnOp = NULL;
	}
	
	if (m_lpROSPredicate != NULL)
	{
		delete m_lpROSPredicate;
		m_lpROSPredicate = NULL;
	}
	
	if (m_lpWOSPredicate != NULL)
	{
		delete m_lpWOSPredicate;
		m_lpWOSPredicate = NULL;
	}
}

// Merge ROS and WOS operators
Operator* SelectNode::mergeROSandWOS()
{
	// Union
	// Union
	if (m_lpReturnOp != NULL)
	{
		// Has been merged
		return m_lpReturnOp;
	}
	
	if ( (m_lpROSReturnOp == NULL) || (m_lpWOSReturnOp == NULL) )
	{
		cout << "ERROR: Ros or Wos is NULL. Cannot union them." << endl;
		return m_lpReturnOp;
	}
	
	// Merge them
	m_lpReturnOp = new Union(m_lpROSReturnOp, m_lpWOSReturnOp, 1);	
	return m_lpReturnOp;
}

// Run ROS only
Operator* SelectNode::runROS()
{
	if (m_lpROSReturnOp != NULL)
	{
		return m_lpROSReturnOp;
	}
	
	// Get data source. 
	getROSDataSource();
	  
		
	// Predicates
	if ( (m_bHasPredicate) && (m_lpROSReturnOp!= NULL) )
	{
		m_lpROSPredicate = new Predicate((Predicate::PredicateType) m_iPredicateType);
		((DataSource*) m_lpROSReturnOp)->setPredicate(m_lpROSPredicate);
		
		if (m_iRHSType == INT_TYPE)
		{
			// RHS is an integer
			((DataSource*) m_lpROSReturnOp)->changeRHSBinding(m_iRHS);
		} else if (m_iRHSType == DBL_TYPE)
		{
			// RHS is a double
			
			// Later
			//((DataSource*) m_lpROSReturnOp)->changeRHSBinding(m_nRHS);
			((DataSource*) m_lpROSReturnOp)->changeRHSBinding((int) m_nRHS);
		}
		else
		{
			// RHS is a string
			
			// Later
			//((DataSource*) m_lpROSReturnOp)->changeRHSBinding(m_sRHS);
			((DataSource*) m_lpROSReturnOp)->changeRHSBinding(atoi(m_sRHS.c_str()));
		}
	}

	if (m_lpROSReturnOp == NULL)
	{
		cout << "		NO DATA SOURCE created - Enter different query" << endl;
	}

	return m_lpROSReturnOp;	

}

// Run WOS only
Operator* SelectNode::runWOS()
{
	if (m_lpWOSReturnOp != NULL)
	{
		return m_lpWOSReturnOp;
	}
	
	// Get data source. Still half hard code. Need to be changed when they are available
	getWOSDataSource(); 
	
	// Predicates
	if ( (m_bHasPredicate) && (m_lpWOSReturnOp!= NULL) )
	{
		m_lpWOSPredicate = new Predicate((Predicate::PredicateType) m_iPredicateType);
		((DataSource*) m_lpWOSReturnOp)->setPredicate(m_lpWOSPredicate);
		
		if (m_iRHSType == INT_TYPE)
		{
			// RHS is an integer
			((DataSource*) m_lpWOSReturnOp)->changeRHSBinding(m_iRHS);
		} else if (m_iRHSType == DBL_TYPE)
		{
			// RHS is a double
			
			// Later
			//((DataSource*) m_lpWROSReturnOp)->changeRHSBinding(m_nRHS);
			((DataSource*) m_lpWOSReturnOp)->changeRHSBinding((int) m_nRHS);
		}
		else
		{
			// RHS is a string
			
			// Later
			//((DataSource*) m_lpWOSReturnOp)->changeRHSBinding(m_sRHS);
			((DataSource*) m_lpWOSReturnOp)->changeRHSBinding(atoi(m_sRHS.c_str()));
		}
	}

	if (m_lpWOSReturnOp == NULL)
	{
		cout << "		NO DATA SOURCE created - Enter different query" << endl;
	}

	return m_lpWOSReturnOp;	
}

// show what operator this node will run
void SelectNode::showROS(int* lpNodeID, ofstream* lpOfstream)
{
	// The operator exists
	if (m_bROSHasShown)
	{
		return;
	}
	
	m_bROSHasShown = 1;
	
	// Set nodeID to have different variable name for this node
	setNodeID(lpNodeID);

	string sOps = "//SELECT:\n";
	
	char sNodeID[3];
	sprintf(sNodeID, "%d", m_iNodeID);

	sOps = sOps + "	// This should probably be taking the projection, but I don't\n";
	sOps = sOps + "	//seem to need it now that column names are unique\n";
	//sOps = sOps + "	ROSAM* lp_AM" + string(sNodeID) + " = CatalogInstance::getCatalog()->getWOSAMgetROSAM(\"" + m_sProjection + "\", \"" + m_sColumn + "\");\n";
	sOps = sOps + "	ROSAM* lp_AM" + string(sNodeID) + " = CatalogInstance::getCatalog()->getROSAM(\"" + m_sColumn + "\");\n";
	
	// Get catalog
	CatalogInstance* lp_CI = CatalogInstance::getCatalog();
	if ( lp_CI->isSorted(m_sProjection, m_sColumn ) )
	{
		sOps = sOps + "	Operator* lp_ROS" + string(sNodeID) + " = new RLEDataSource(lp_AM" + string(sNodeID) + ", true);\n";
	}
	else
	{
		sOps = sOps + "	Operator* lp_ROS" + string(sNodeID) + " = new IntDataSource(lp_AM" + string(sNodeID) + ", false, true);\n";
		
		/*
		if ( (m_iJoinType == SELECT_FOR_JOIN) || (m_iJoinType == SELECT_FOR_AFTER_JOIN) )
		{
			sOps = sOps + "	((IntDataSource*) lp_ROS" + string(sNodeID) + ")->setSingles(true);\n";
		}	
		*/
	}
		
	// Show: "ds->setPredicate(new Predicate(m_iPredicateType), RHS)"
	if (m_bHasPredicate) 
	{
		char p[3];
		sprintf(p, "%d", m_iPredicateType);
				
		sOps = sOps + "	( (DataSource*) lp_ROS" +  string(sNodeID) + ")->setPredicate(new Predicate(";
		sOps = sOps + "(Predicate::PredicateType) " + string(p) + "));\n";
		
		
		
		sOps = sOps + "	( (DataSource*) lp_ROS" +  string(sNodeID) + ")->changeRHSBinding(";
	
		if (m_iRHSType == INT_TYPE)
		{
			// RHS is an integer
			char rhs[15];
			sprintf(rhs, "%d", m_iRHS);
			sOps = sOps + string(rhs);
		} else if (m_iRHSType == DBL_TYPE)
		{
			// RHS is a double
			char rhs[15];
			sprintf(rhs, "%f", m_nRHS);
			sOps = sOps + string(rhs);
		}
		else
		{
			// RHS is a string
			sOps = sOps + m_sRHS;
		}
		
		sOps = sOps + ");\n";
	}
	
	
	lpOfstream->write(sOps.c_str(), sOps.length());
}


// show what operator this node will run
void SelectNode::showWOS(int* lpNodeID, ofstream* lpOfstream)
{
	// The operator exists
	if (m_bWOSHasShown)
	{
		return;
	}
	
	m_bWOSHasShown = 1;
	
	// Set nodeID to have different variable name for this node
	setNodeID(lpNodeID);

	string sOps = "//SELECT:\n";
	
	char sNodeID[3];
	sprintf(sNodeID, "%d", m_iNodeID);
	
	////
	sOps = sOps + "	// in ideal world, the first paramter would be projection name.\n";
	sOps = sOps + "	// for now catalog will recognize the column name instead.\n";
	
	sOps = sOps + "	WOSAM* lp_WOSAM" + string(sNodeID) + " = CatalogInstance::getCatalog()->getWOSAM(\"" + m_sColumn + "\", \"" + m_sColumn + "\");\n";
	
	sOps = sOps + "	Operator* lp_WOS" + string(sNodeID) + " = new IntDataSource(lp_WOSAM" + string(sNodeID) + ", true, false);\n";
	/*
	if ( (m_iJoinType == SELECT_FOR_JOIN) || (m_iJoinType == SELECT_FOR_AFTER_JOIN) )
	{
		sOps = sOps + "	((IntDataSource*) lp_WOS" + string(sNodeID) + ")->setSingles(true);\n";
	}
	*/
		
	// Show: "ds->setPredicate(new Predicate(m_iPredicateType), RHS)"
	if (m_bHasPredicate) 
	{
		char p[3];
		sprintf(p, "%d", m_iPredicateType);
				
		sOps = sOps + "	( (DataSource*) lp_WOS" +  string(sNodeID) + ")->setPredicate(new Predicate(";
		sOps = sOps + "(Predicate::PredicateType) " + string(p) + "));\n";
		
		
		
		sOps = sOps + "	( (DataSource*) lp_WOS" +  string(sNodeID) + ")->changeRHSBinding(";
	
		if (m_iRHSType == INT_TYPE)
		{
			// RHS is an integer
			char rhs[15];
			sprintf(rhs, "%d", m_iRHS);
			sOps = sOps + string(rhs);
		} else if (m_iRHSType == DBL_TYPE)
		{
			// RHS is a double
			char rhs[15];
			sprintf(rhs, "%f", m_nRHS);
			sOps = sOps + string(rhs);
		}
		else
		{
			// RHS is a string
			sOps = sOps + m_sRHS;
		}
		
		sOps = sOps + ");\n";
	}
	
	
	lpOfstream->write(sOps.c_str(), sOps.length());
}


// show tree presentation
void SelectNode::showTree(string sTabs)
{
	// The operator exists
	if (m_bTreeHasShown)
	{
		cout << endl;
		cout << sTabs << "SELECT - The operator was created before" << endl;
		
		return;
	}
	
	m_bTreeHasShown = 1;
		
	// Show: "DataSource ds = new DataSource(m_sColumn, bIsSorted);"	
	string sIsSorted;
	if (m_bIsSorted)
	{
		sIsSorted = "1";
	}
	else
	{
		sIsSorted = "0";
	}	
	
	
	cout << endl;
	cout << sTabs << "SELECT" << endl;
	
	cout << sTabs << "Operator: DataSource" << endl;
	cout << sTabs << "	" << "Argument1: Column name: " << m_sColumn << endl;
	cout << sTabs << "	" << "Argument2: Is the column sorted: " << sIsSorted << endl;
	
	// Has predicate
	// Show: "ds->setPredicate(new Predicate(m_iPredicateType), RHS)"
	if (m_bHasPredicate) 
	{
		char p[3];
		sprintf(p, "%d", m_iPredicateType);
		
		cout << endl;
		cout << sTabs << "	" << "PREDICATE" << endl;
		cout << sTabs << "	setPredicate" << endl;
		cout << sTabs << "		" << "Type: " << string(p) << endl;
		cout << sTabs << "		" << "Right hand side: ";

		if (m_iRHSType == INT_TYPE)
		{
			// RHS is an integer
			char rhs[15];
			sprintf(rhs, "%d", m_iRHS);
			cout << string(rhs);
		} else if (m_iRHSType == DBL_TYPE)
		{
			// RHS is a double
			char rhs[15];
			sprintf(rhs, "%f", m_nRHS);
			cout << string(rhs);
		}
		else
		{
			// RHS is a string
			cout << m_sRHS;
		}

		cout << endl;
	}
}

// show merge operators
void SelectNode::showMerge(ofstream* lpOfstream)
{
	//cout << "//UNION:" << endl;
	//cout << "	Operator* lp_Merge = new Union(ROS Operator, WOS Operator, 1);" << endl;
	
	char sTmp[15];	
	sprintf(sTmp, "%d", m_iNodeID);
	
	string sOps = "//UNION:\n";
	sOps = sOps + "	Operator* lp_Merge" + string(sTmp) + " = new Union(lp_ROS" + string(sTmp) + ", lp_WOS" + string(sTmp) + ", 1); \n";
	
	lpOfstream->write(sOps.c_str(), sOps.length());
}


// Check and set predicate
void SelectNode::checkAndSetPredicate(SelectNode* lpNode)
{
	cout << "checkAndSetPredicate -----" << endl;
	cout << "Join col: " << m_sColumn << endl;
	cout << "Predicate col: " << lpNode->m_sColumn << endl;

	if (m_sColumn.compare(lpNode->m_sColumn) == 0)
	{
		cout << "ASSIGN" << endl;
		
		m_bHasPredicate = 1;
		m_iPredicateType = lpNode->m_iPredicateType;
		m_iRHSType = lpNode->m_iRHSType;
		m_iRHS = lpNode->m_iRHS;
		m_sRHS = lpNode->m_sRHS;
		m_nRHS = lpNode->m_nRHS;
	}
}
		

// Get info from tha catalog
void SelectNode::getCatalogInfo()
{
	m_bIsSorted = 0; //not sorted
	m_iEncodingType = -1; //unknown encoding type
}

// Get dataSource for this column
void SelectNode::getROSDataSource()
{
	if (m_lpROSReturnOp != NULL)
	{	
		delete m_lpROSReturnOp;
		m_lpROSReturnOp = NULL;
	}
	
	// Alexr -- replacing the calls with catalog calls

	CatalogInstance* ci = CatalogInstance::getCatalog();

	ROSAM* am = NULL;

	// This should probably be taking the projection, but I don't
	// seem to need it now that column names are unique
	am = ci->getROSAM( m_sColumn );
	//am = ci->getROSAM(m_sProjection, m_sColumn );

	if ( ci->isSorted(m_sProjection, m_sColumn ) )
	{
		m_lpROSReturnOp  = new RLEDataSource(am, true);
	}
	else
	{
		m_lpROSReturnOp  = new IntDataSource(am, false, true);
		/*
		if ( (m_iJoinType == SELECT_FOR_JOIN) || (m_iJoinType == SELECT_FOR_AFTER_JOIN) )
		{
			((IntDataSource*) m_lpROSReturnOp)->setSingles(true);
		}
		*/
	 }
	 
	//delete ci;
}


// Get dataSource for this column
void SelectNode::getWOSDataSource()
{
	if (m_lpWOSReturnOp != NULL)
	{	
		delete m_lpWOSReturnOp;
		m_lpWOSReturnOp = NULL;
	}
	
	WOSAM* am = NULL;
	
	// in ideal world, the first paramter would be projection name.
	// for now catalog will recognize the column name instead.
	
	// am = CatalogInstance::getCatalog->getWOSAM(m_sProjection, m_sColumn);
	am = CatalogInstance::getCatalog()->getWOSAM( m_sColumn, m_sColumn );
	m_lpWOSReturnOp = new IntDataSource( am,
                                         true,   // value sorted
                                         false); // isROS
					
	/*
	if ( (m_iJoinType == SELECT_FOR_JOIN) || (m_iJoinType == SELECT_FOR_AFTER_JOIN) )
	{
	    ((IntDataSource*) m_lpWOSReturnOp)->setSingles(true);
	}
	*/
}

//---------------------------------------------------------------
// Memory management functions

// New a node and put it in the m_lpList
SelectNode* SelectNode::create()
{
	Log::writeToLog("SelectNode", 10, "Create without arguments");
		
	SelectNode* lpNode = new SelectNode();
	pushBack(lpNode);
	return lpNode;
}

// New a node and put it in the m_lpList
SelectNode* SelectNode::create(string sColumn, string sProjection)
{
	Log::writeToLog("SelectNode", 10, "Create with arguments: string sColumn, string sProjection");
		
	SelectNode* lpNode = new SelectNode(sColumn, sProjection);
	pushBack(lpNode);
	return lpNode;
}

// New a node and put it in the m_lpList
SelectNode* SelectNode::create(string sColumn, string sProjection, int iJoinType)
{
	Log::writeToLog("SelectNode", 10, "Create with arguments: string sColumn, string sProjection, int iJoinType");
		
	SelectNode* lpNode = new SelectNode(sColumn, sProjection, iJoinType);
	pushBack(lpNode);
	return lpNode;
}

// New a node and put it in the m_lpList
SelectNode* SelectNode::create(string sColumn, string sProjection, int iPredicateType, double nRHS)
{
	Log::writeToLog("SelectNode", 10, "Create with arguments: string sColumn, string sProjection, int iPredicateType, double nRHS");
		
	SelectNode* lpNode = new SelectNode(sColumn, sProjection, iPredicateType, nRHS);
	pushBack(lpNode);
	return lpNode;
}

// New a node and put it in the m_lpList
SelectNode* SelectNode::create(string sColumn, string sProjection, int iPredicateType, string sRHS)
{
	Log::writeToLog("SelectNode", 10, "Create with arguments: string sColumn, string sProjection, int iPredicateType, string sRHS");
		
	SelectNode* lpNode = new SelectNode(sColumn, sProjection, iPredicateType, sRHS);
	pushBack(lpNode);
	return lpNode;
}

// New a node and put it in the m_lpList
SelectNode* SelectNode::create(string sColumn, string sProjection, int iPredicateType, int sRHS)
{
	Log::writeToLog("SelectNode", 10, "Create with arguments: string sColumn, string sProjection, int iPredicateType, int sRHS");
		
	SelectNode* lpNode = new SelectNode(sColumn, sProjection, iPredicateType, sRHS);
	pushBack(lpNode);
	return lpNode;
}
