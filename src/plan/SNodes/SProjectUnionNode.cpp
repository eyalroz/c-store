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
 *  MProjectUnionNode.cpp
 *  cstore
 *
 *  Created by Nga Tran on 6/22/05.
 *  
 *	Projection Node which has to use Union
 *
 *	There will be 2 datasources for the projection which will be used to setPositionFilter
 *	for the joins and then union together
 */

#include "SProjectUnionNode.h"

// Default constructor
SProjectUnionNode::SProjectUnionNode()
{
	m_lpJoin = NULL;	
	m_lpSelect1 = NULL;
	m_lpSelect2 = NULL;
	m_sProjection = "";
}


// Constructor
SProjectUnionNode::SProjectUnionNode(Node* lpJoin, EColumn* lpSelectionColumn)
{
	m_lpJoin = lpJoin;	
	m_lpSelect1 = NULL;
	m_lpSelect2 = NULL;
	m_sProjection = "";
	
	if (lpSelectionColumn == NULL)
	{
		cout << "ERROR: invalid selection column. The results won't be correct" << endl;
		return;
	}
	
	// Keep the projection name of the input column
	m_sProjection = lpSelectionColumn->getProjection();
	
	// Create 2 SelectNode (4 datasources: 2 ros and 2 wos) for filter 4 join outputs
	m_lpSelect1 = SelectNode::create(lpSelectionColumn->toString(), lpSelectionColumn->getProjection(), SELECT_FOR_AFTER_JOIN);
	//m_lpSelect2 = SelectNode::create(lpSelectionColumn->toString(), lpSelectionColumn->getProjection(), SELECT_FOR_AFTER_JOIN);
}


// Deallocate memory
SProjectUnionNode::~SProjectUnionNode()
{
  /*
	 Log::writeToLog("SProjectUnionNode", 10, "~SProjectUnionNode");
	 
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
	
	if (m_lpReturnOp != NULL)
	{
		delete m_lpReturnOp;
		m_lpReturnOp = NULL;
	}
  */
}

		
// Run ROS
Operator* SProjectUnionNode::runROS()
{
 
	// The operator exists, only return it
	if (m_lpROSReturnOp != NULL)
	{
		return m_lpROSReturnOp;
	}
	
	// Run join node first
	if (m_lpJoin == NULL)
	{
		cout << "ERROR: Join Node is NULL. The results won't be correct" << endl;
		return m_lpROSReturnOp;
	}
	m_lpJoin->run();
	
	// Get join outputs
	JoinOutputs* lpJoinOutputs = ( (BJoinNode*) m_lpJoin)->getJoinOutputs();	
	if (lpJoinOutputs == NULL)
	{
		cout << "ERROR: Join operators are not created. The results won't be correct" << endl;
		return m_lpROSReturnOp;
	}
	
	// Run ROS of 2 select nodes
	//if ( (m_lpSelect1 == NULL) || (m_lpSelect2 == NULL) )
	if  (m_lpSelect1 == NULL) 
	{
		cout << "ERROR: Select node is NULL. The results won't be correct" << endl;
		return m_lpROSReturnOp;
	}
	Operator* lpROS1 = m_lpSelect1->runROS();
	//Operator* lpROS2 = m_lpSelect2->runROS();
	
	//if ( (lpROS1 == NULL) || (lpROS2 == NULL) )
	if (lpROS1 == NULL)
	{
		cout << "ERROR: ROS Datasources are not created. The results won't be correct" << endl;
		return m_lpROSReturnOp;
	}
	
	// Compare column's projection with the first projection of the join
	string sJoinProjection = lpJoinOutputs->getFirstProjection();
	if (m_sProjection.compare(sJoinProjection) == 0)
	{
		// Column of first projection of the join
		((DataSource*) lpROS1)->setPositionFilter(lpJoinOutputs->getRosRos(), 0);
		//cout << "NGA: lpROSROS B1: " << lpJoinOutputs->getRosRos() << endl;

		//((DataSource*) lpROS2)->setPositionFilter(lpJoinOutputs->getRosWos(), 0);
		
		// First union level
		//m_lpROSReturnOp = new Union(lpROS1, lpROS2, 1);
		m_lpROSReturnOp = lpROS1;

		//cout << "NGA: REACH HERE 1111\n";
		 
		//cout << "NGA: pointerA: " <<lpROS1 << endl;
		//cout << "NGA: pointerB: " << m_lpROSReturnOp << endl;
		
		return m_lpROSReturnOp;

	}
	
	// Compare column's projection with the second projection of the join 
	sJoinProjection = lpJoinOutputs->getSecondProjection();
	if (m_sProjection.compare(sJoinProjection) == 0)
	{
		// Column of second projection of the join
		((DataSource*) lpROS1)->setPositionFilter(lpJoinOutputs->getRosRos(), 1);
		//cout << "NGA: lpROSROS B2: " << lpJoinOutputs->getRosRos() << endl;

		//((DataSource*) lpROS2)->setPositionFilter(lpJoinOutputs->getWosRos(), 1);
		
		// First union level
		//m_lpROSReturnOp = new Union(lpROS1, lpROS2, 1);
		m_lpROSReturnOp = lpROS1;

		//cout << "NGA: REACH HERE 2222\n";
		//cout << "NGA: pointerA: " <<lpROS1 << endl;
		//cout << "NGA: pointerB: " << m_lpROSReturnOp << endl;

		return m_lpROSReturnOp;
	}
	
	// No match projection (should never happen)
	//cout << "NGA runROS(): should not happen\n";
	return m_lpROSReturnOp;
}

		
// Run WOS
Operator* SProjectUnionNode::runWOS()
{
	// The operator exists, only return it
	if (m_lpWOSReturnOp != NULL)
	{
		return m_lpWOSReturnOp;
	}
	
	// Run join node first
	if (m_lpJoin == NULL)
	{
		cout << "ERROR: Join Node is NULL. The results won't be correct" << endl;
		return m_lpWOSReturnOp;
	}
	m_lpJoin->run();
	
	// Get join outputs
	JoinOutputs* lpJoinOutputs = ( (BJoinNode*) m_lpJoin)->getJoinOutputs();	
	if (lpJoinOutputs == NULL)
	{
		cout << "ERROR: Join operators are not created. The results won't be correct" << endl;
		return m_lpWOSReturnOp;
	}
	
	// Run ROS of 2 select nodes
	if ( (m_lpSelect1 == NULL) || (m_lpSelect2 == NULL) )
	{
		cout << "ERROR: Select node is NULL. The results won't be correct" << endl;
		return m_lpWOSReturnOp;
	}
	Operator* lpWOS1 = m_lpSelect1->runWOS();
	Operator* lpWOS2 = m_lpSelect2->runWOS();
	
	if ( (lpWOS1 == NULL) || (lpWOS2 == NULL) )
	{
		cout << "ERROR: ROS Datasources are not created. The results won't be correct" << endl;
		return m_lpWOSReturnOp;
	}
	
	// Compare column's projection with the first projection of the join
	string sJoinProjection = lpJoinOutputs->getFirstProjection();
	if (m_sProjection.compare(sJoinProjection) == 0)
	{
		// Column of first projection of the join
		((DataSource*) lpWOS1)->setPositionFilter(lpJoinOutputs->getWosRos(), 0);
		((DataSource*) lpWOS2)->setPositionFilter(lpJoinOutputs->getWosWos(), 0);
		
		// First union level
		m_lpWOSReturnOp = new Union(lpWOS1, lpWOS2, 1);
		
		return m_lpWOSReturnOp;
	}
	
	// Compare column's projection with the second projection of the join 
	sJoinProjection = lpJoinOutputs->getSecondProjection();
	if (m_sProjection.compare(sJoinProjection) == 0)
	{
		// Column of second projection of the join
		((DataSource*) lpWOS1)->setPositionFilter(lpJoinOutputs->getRosWos(), 1);
		((DataSource*) lpWOS2)->setPositionFilter(lpJoinOutputs->getWosWos(), 1);
		
		// First union level
		m_lpWOSReturnOp = new Union(lpWOS1, lpWOS2, 1);
		
		return m_lpWOSReturnOp;
	}
	
	// No match projection (should never happen)
	return m_lpWOSReturnOp;
}

// Merge ROS and WOS operators
Operator* SProjectUnionNode::mergeROSandWOS()
{
	// Union
	if (m_lpReturnOp != NULL)
	{
		// Has been merged
		return m_lpReturnOp;
	}
	
	if ( (m_lpROSReturnOp == NULL) || (m_lpWOSReturnOp == NULL) )
	{
		cout << "ERROR: level-1 unions are not created. The results won't be correct" << endl;
		return m_lpReturnOp;
	}
		
	// Merge them
	m_lpReturnOp = new Union(m_lpROSReturnOp, m_lpWOSReturnOp, 1);	
	return m_lpReturnOp;
}


// show what ROS operators this node will run
void SProjectUnionNode::showROS(int* lpNodeID, ofstream* lpOfstream)
{
	// The operator exists
	if (m_bROSHasShown)
	{
		return;
	}
	
	m_bROSHasShown = 1;
	
	// Set nodeID to have different variable name for this node
	setNodeID(lpNodeID);

	
	// Get join outputs
	JoinOutputs* lpJoinOutputs = NULL;

	
	if (m_lpJoin != NULL) 
	{
		m_lpJoin->show(lpNodeID, lpOfstream);
		lpJoinOutputs = ( (BJoinNode*) m_lpJoin)->getJoinOutputs();
	}

	if (m_lpSelect1 != NULL) 
	{
		m_lpSelect1->showROS(lpNodeID, lpOfstream);
	}
	/*
	if (m_lpSelect2 != NULL) 
	{
		m_lpSelect2->showROS(lpNodeID, lpOfstream);
	}
	*/
	
	char sSelect1[15];	
	sprintf(sSelect1, "%d", m_lpSelect1->getNodeID());
	
	//char sSelect2[15];	
	//sprintf(sSelect2, "%d", m_lpSelect2->getNodeID());
	
	char sNodeID[15];	
	sprintf(sNodeID, "%d", getNodeID());

	m_iNodeID = m_lpSelect1->getNodeID();
	
	if (lpJoinOutputs != NULL)
	{
		string sJoinProjection = lpJoinOutputs->getFirstProjection();
		if (m_sProjection.compare(sJoinProjection) == 0)
		{			
			string sOps = "//UNION level 1:\n";
			sOps = sOps  + "	((DataSource*) lp_ROS" + string(sSelect1) + ")->setPositionFilter(lpRosRos, 0);\n";
			//sOps = sOps  + "	((DataSource*) lp_ROS" + string(sSelect2) + ")->setPositionFilter(lpRosWos, 0);\n";
			//sOps = sOps  + "	Operator* lp_ROS" + string(sNodeID) + " = new Union(lp_ROS" + string(sSelect1) + ", lp_ROS" + string(sSelect2) + ", 1);\n";
			
			//cout << "NODE ID: " << getNodeID();

			lpOfstream->write(sOps.c_str(), sOps.length());
			return;
		}
		
		sJoinProjection = lpJoinOutputs->getSecondProjection();
		if (m_sProjection.compare(sJoinProjection) == 0)
		{			
			string sOps = "//UNION level 1:\n";
			sOps = sOps  + "	((DataSource*) lp_ROS" + string(sSelect1) + ")->setPositionFilter(lpRosRos, 1);\n";
			//sOps = sOps  + "	((DataSource*) lp_ROS" + string(sSelect2) + ")->setPositionFilter(lpWosRos, 1);\n";
			//sOps = sOps  + "	Operator* lp_ROS" + string(sNodeID) + " = new Union(lp_ROS" + string(sSelect1) + ", lp_ROS" + string(sSelect2) + ", 1);\n";
			
			//cout << "NODE ID: " << getNodeID();

			lpOfstream->write(sOps.c_str(), sOps.length());
			return;
		}
	}
}
		
// show what WOS operators this node will run
void SProjectUnionNode::showWOS(int* lpNodeID, ofstream* lpOfstream)
{
	// The operator exists
	if (m_bWOSHasShown)
	{
		return;
	}
	
	m_bWOSHasShown = 1;
	
	// Set nodeID to have different variable name for this node
	setNodeID(lpNodeID);
	
	// Get join outputs
	JoinOutputs* lpJoinOutputs = NULL;

	
	if (m_lpJoin != NULL) 
	{
		m_lpJoin->show(lpNodeID, lpOfstream);
		lpJoinOutputs = ( (BJoinNode*) m_lpJoin)->getJoinOutputs();
	}

	if (m_lpSelect1 != NULL) 
	{
		m_lpSelect1->showWOS(lpNodeID, lpOfstream);
	}
	
	if (m_lpSelect2 != NULL) 
	{
		m_lpSelect2->showWOS(lpNodeID, lpOfstream);
	}
	
	char sSelect1[15];	
	sprintf(sSelect1, "%d", m_lpSelect1->getNodeID());
	
	char sSelect2[15];	
	sprintf(sSelect2, "%d", m_lpSelect2->getNodeID());
	
	char sNodeID[15];	
	sprintf(sNodeID, "%d", getNodeID());
	
	if (lpJoinOutputs != NULL)
	{
		string sJoinProjection = lpJoinOutputs->getFirstProjection();
		if (m_sProjection.compare(sJoinProjection) == 0)
		{			
			string sOps = "//UNION level 1:\n";
			sOps = sOps  + "	((DataSource*) lp_WOS" + string(sSelect1) + ")->setPositionFilter(lpWosRos, 0);\n";
			sOps = sOps  + "	((DataSource*) lp_WOS" + string(sSelect2) + ")->setPositionFilter(lpWosWos, 0);\n";
			sOps = sOps  + "	Operator* lp_WOS" + string(sNodeID) + " = new Union(lp_WOS" + string(sSelect1) + ", lp_WOS" + string(sSelect2) + ", 1);\n";

			lpOfstream->write(sOps.c_str(), sOps.length());
			return;
		}
		
		sJoinProjection = lpJoinOutputs->getSecondProjection();
		if (m_sProjection.compare(sJoinProjection) == 0)
		{			
			string sOps = "//UNION level 1:\n";
			sOps = sOps  + "	((DataSource*) lp_WOS" + string(sSelect1) + ")->setPositionFilter(lpRosWos, 1);\n";
			sOps = sOps  + "	((DataSource*) lp_WOS" + string(sSelect2) + ")->setPositionFilter(lpWosWos, 1);\n";
			sOps = sOps  + "	Operator* lp_WOS" + string(sNodeID) + " = new Union(lp_WOS" + string(sSelect1) + ", lp_WOS" + string(sSelect2) + ", 1);\n";

			lpOfstream->write(sOps.c_str(), sOps.length());
			return;
		}
	}
}

// show merge operators
void SProjectUnionNode::showMerge(ofstream* lpOfstream)
{
	char sNodeID[15];	
	sprintf(sNodeID, "%d", m_iNodeID);
	
	string sOps = "//UNION level 2:\n";
	sOps = sOps + "	Operator* lp_Merge" + string(sNodeID) + " = new Union(lp_ROS" + string(sNodeID) + ", lp_WOS" + string(sNodeID) + ", 1); \n";
	
	lpOfstream->write(sOps.c_str(), sOps.length());
}

		

// show tree presentation
void SProjectUnionNode::showTree(string sTabs)
{
	cout << "showTree of SProjectUnionNode has not been implemented yet" << endl;
}


//---------------------------------------------------------------
// Memory management functions

// New a node and put it in the m_lpList
SProjectUnionNode* SProjectUnionNode::create()
{
	Log::writeToLog("SProjectUnionNode", 10, "Create without arguments");
		
	SProjectUnionNode* lpNode = new SProjectUnionNode();
	pushBack(lpNode);
	return lpNode;
}

// New a node and put it in the m_lpList
SProjectUnionNode* SProjectUnionNode::create(Node* lpJoin, EColumn* lpSelectionColumn)
{
	Log::writeToLog("SProjectUnionNode", 10, "Create with arguments: Node* lpJoin, EColumn* lpSelectionColumn");
		
	SProjectUnionNode* lpNode = new SProjectUnionNode(lpJoin, lpSelectionColumn);
	pushBack(lpNode);
	return lpNode;
}

