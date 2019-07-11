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
 *  BJoinNode.cpp
 *  cstore
 *
 *  Created by Nga Tran on 3/23/05.
 * 
 *  Join node
 *  Join is a doing differently from other nodes because ROS and WOS have to join together
 */

#include "BJoinNode.h"

#include <iostream>

// Default
BJoinNode::BJoinNode() : BNode()
{
	m_iComparisonType = Predicate::OP_EQUAL;	// Default
	setPredicateType(PREDICATE_JOIN); // itself is a join node
	m_bIsRun = 0;
	m_lpJoinOutputs = NULL;
	m_lpLeftROS = NULL;
	m_lpRightROS = NULL;
}

/*
// Inputs are 2 nodes
BJoinNode::BJoinNode(Node* lpLeft, Node* lpRight, int iComparisonType) : BNode(lpLeft, lpRight)
{
	m_iComparisonType = Predicate::OP_EQUAL;	// Default
	setPredicateType(PREDICATE_JOIN); // itself is a join node
	m_bIsRun = 0;
	m_lpJoinOutputs = NULL;
}
*/

// Inputs are 2 column names
BJoinNode::BJoinNode(EColumn* lpLeft, EColumn* lpRight, int iComparisonType): BNode()
{
	m_iComparisonType = iComparisonType;
	setPredicateType(PREDICATE_JOIN); // itself is a join node
	m_bIsRun = 0;
	m_lpJoinOutputs = NULL;
	
	m_lpJoinOutputs = JoinOutputs::create();
	
	if (lpLeft == NULL)
	{
		cout << "INVALID JOIN ARGUMENTS. The results won't be correct" << cout;
		return;
	}
	else
	{
		m_lpJoinOutputs->setFirstProjection(lpLeft->getProjection());
		m_lpJoinOutputs->setFirstJoinColumn(lpLeft->toString());
	}
	
	if (lpRight == NULL)
	{
		cout << "INVALID JOIN ARGUMENTS. The results won't be correct" << cout;
		return;
	}
	else
	{
		m_lpJoinOutputs->setSecondProjection(lpRight->getProjection());
		m_lpJoinOutputs->setSecondJoinColumn(lpRight->toString());
	}
	
	// Get projection names				
	string sLeftProjection = lpLeft->getProjection();			
	string sRightProjection = lpRight->getProjection();
					
	m_lpLeft =  SelectNode::create(lpLeft->toString(), sLeftProjection, SELECT_FOR_JOIN);
	m_lpRight =  SelectNode::create(lpRight->toString(), sRightProjection, SELECT_FOR_JOIN);	
	m_lpLeftReplica =  SelectNode::create(lpLeft->toString(), sLeftProjection, SELECT_FOR_JOIN);
	m_lpRightReplica =  SelectNode::create(lpRight->toString(), sRightProjection, SELECT_FOR_JOIN);

	m_lpLeftROS = NULL;
	m_lpRightROS = NULL;
}


// Deallocate memory for its members
BJoinNode::~BJoinNode()
{
	Log::writeToLog("BJoinNode", 10, "~BJoinNode");
}


// Get 4 join outputs
JoinOutputs* BJoinNode::getJoinOutputs()
{
	return m_lpJoinOutputs;
}
		
		
// Run
Operator* BJoinNode::run(bool rosOnly)
{
  	// The join operator exists, do not run again
	if (m_bIsRun == 1)
	{
		return m_lpReturnOp;
	}
	
	m_bIsRun = 1;

  if (rosOnly)
    {
	// Generate the operator
	//Operator*	lpLeftROS = NULL;
	//Operator*	lpRightROS = NULL;

	
	// Run its chidren first
	if (m_lpLeft != NULL)
	{
	  //lpLeftROS = m_lpLeft->runROS();
	  m_lpLeftROS = m_lpLeft->runROS();

	}
	
	if (m_lpRight != NULL)
	{
	  //lpRightROS = m_lpRight->runROS();
	  m_lpRightROS = m_lpRight->runROS();
	}	
	
	if ( (m_lpLeftROS == NULL) || (m_lpRightROS == NULL) )
	{
		cout << "INVALID JOIN OPERATOR. The results won't be correct";
	}
	else
	{
		m_lpJoinOutputs->setRosRos(&m_lpLeftROS, 0, &m_lpRightROS, 0);
	}
    }
  else
    {
	
	
	// Generate the operator
	Operator*	lpLeftROS = NULL;
	Operator*	lpLeftWOS = NULL;
	Operator*	lpRightROS = NULL;
	Operator*	lpRightWOS = NULL;
	Operator*	lpLeftReplicaROS = NULL;
	Operator*	lpLeftReplicaWOS = NULL;
	Operator*	lpRightReplicaROS = NULL;
	Operator*	lpRightReplicaWOS = NULL;
	
	// Run its chidren first
	if (m_lpLeft != NULL)
	{
		lpLeftROS = m_lpLeft->runROS();
		lpLeftWOS = m_lpLeft->runWOS();
		lpLeftReplicaROS = m_lpLeftReplica->runROS();
		lpLeftReplicaWOS = m_lpLeftReplica->runWOS();
	}
	
	if (m_lpRight != NULL)
	{
		lpRightROS = m_lpRight->runROS();
		lpRightWOS = m_lpRight->runWOS();
		lpRightReplicaROS = m_lpRightReplica->runROS();
		lpRightReplicaWOS = m_lpRightReplica->runWOS();
	}	
	
	if ( (lpLeftROS == NULL) || (lpRightROS == NULL) || (lpLeftWOS == NULL) || (lpRightWOS == NULL) ||
		 (lpLeftReplicaROS == NULL) || (lpRightReplicaROS == NULL) || (lpLeftReplicaWOS == NULL) || (lpRightReplicaWOS == NULL) )
	{
		cout << "INVALID JOIN OPERATOR. The results won't be correct";
	}
	else
	{
		m_lpJoinOutputs->setRosRos(&lpLeftROS, 0, &lpRightROS, 0);
		m_lpJoinOutputs->setRosWos(lpLeftReplicaROS, 0, lpRightWOS, 0);
		m_lpJoinOutputs->setWosRos(lpLeftWOS, 0, lpRightReplicaROS, 0);
		m_lpJoinOutputs->setWosWos(lpLeftReplicaWOS, 0, lpRightReplicaWOS, 0);
	}
    }
	return m_lpReturnOp;

}


// show what operator this node will run
void  BJoinNode::show(int* lpNodeID, ofstream* lpOfstream, bool rosOnly)
{
	// The operator exists
	if (m_bHasShown)
	{
		return;
	}
	
	m_bHasShown = 1;
	
	// Set nodeID to have different variable name for this node
	setNodeID(lpNodeID);

	string sOps = "//JOINS\n";
	if (rosOnly)
	  {
	     // show its children first
	    if (m_lpLeft != NULL)
	      {
		m_lpLeft->showROS(lpNodeID, lpOfstream);
	      }
	
	    if (m_lpRight != NULL)
	      {
		m_lpRight->showROS(lpNodeID, lpOfstream);
	      }	
	
	    char sLeft[15];	
	    sprintf(sLeft, "%d", m_lpLeft->getNodeID());
	    
	    char sRight[15];	
	    sprintf(sRight, "%d", m_lpRight->getNodeID());
	
	    char sNodeID[15];	
	    sprintf(sNodeID, "%d", getNodeID());
	
	    sOps = sOps + "	int colval = 0;\n";
	    sOps =  sOps + "	Operator* lpRosRos = new HashJoin(&lp_ROS" + string(sLeft)  + ", &colval, " +   " &lp_ROS" + string(sRight)  + ", &colval, 1, 1, true);\n";
	  }
	else
	  {  

	    // show its children first
	    if (m_lpLeft != NULL)
	      {
		m_lpLeft->showROS(lpNodeID, lpOfstream);
		m_lpLeft->showWOS(lpNodeID, lpOfstream);
		m_lpLeftReplica->showROS(lpNodeID, lpOfstream);
		m_lpLeftReplica->showWOS(lpNodeID, lpOfstream);
	      }
	
	    if (m_lpRight != NULL)
	      {
		m_lpRight->showROS(lpNodeID, lpOfstream);
		m_lpRight->showWOS(lpNodeID, lpOfstream);
		m_lpRightReplica->showROS(lpNodeID, lpOfstream);
		m_lpRightReplica->showWOS(lpNodeID, lpOfstream);
	      }	
	
	    char sLeft[15];	
	    sprintf(sLeft, "%d", m_lpLeft->getNodeID());
	    
	    char sRight[15];	
	    sprintf(sRight, "%d", m_lpRight->getNodeID());
	
	    char sLeftReplica[15];	
	    sprintf(sLeftReplica, "%d", m_lpLeftReplica->getNodeID());
	
	    char sRightReplica[15];	
	    sprintf(sRightReplica, "%d", m_lpRightReplica->getNodeID());
	
	    char sNodeID[15];	
	    sprintf(sNodeID, "%d", getNodeID());
	
	    
	    sOps =  sOps + "	Operator* lpRosRos = new NLJoin(lp_ROS" + string(sLeft)  + ", 0, " +   " lp_ROS" + string(sRight)  + ", 0);\n";
	    sOps =  sOps + "	Operator* lpRosWos = new NLJoin(lp_ROS" + string(sLeftReplica)  + ", 0, " +   " lp_WOS" + string(sRight)  + ", 0);\n";
	    sOps =  sOps + "	Operator* lpWosRos = new NLJoin(lp_WOS" + string(sLeft)  + ", 0, " +   " lp_ROS" + string(sRightReplica)  + ", 0);\n";
	    sOps =  sOps + "	Operator* lpWosWos = new NLJoin(lp_WOS" + string(sLeftReplica)  + ", 0, " +   " lp_WOS" + string(sRightReplica)  + ", 0);\n";

	  }

	lpOfstream->write(sOps.c_str(), sOps.length());
}

// show tree presentation
void BJoinNode::showTree(string sTabs)
{
	// The operator exists
	if (m_bTreeHasShown)
	{
		cout << endl;
		cout << sTabs << "JOIN - The operator was created before" << endl;
	
		return;
	}
	
	m_bTreeHasShown = 1;
	
	// Show its information first
	cout << endl;
	cout << sTabs << "JOIN" << endl;
	
	cout << sTabs << "4 NLJoins" << endl;
	cout << sTabs << "	" << "ROS (ROS of first below), 0, ROS (ROS of second below), 0" << endl;
	cout << sTabs << "	" << "ROS (ROS of first below), 0, WOS (WOS of second below), 0" << endl;
	cout << sTabs << "	" << "WOS (WOS of first below), 0, ROS (ROS of second below), 0" << endl;
	cout << sTabs << "	" << "WOS (WOS of first below), 0, WOS (WOS of second below), 0" << endl;
		
	// Show child's information
	if (m_lpLeft != NULL)
	{
		m_lpLeft->showTree(sTabs);
	}
	
	if (m_lpRight != NULL)
	{
		m_lpRight->showTree(sTabs);
	}
}


// Check and set predicate
void BJoinNode::checkAndSetPredicate(SelectNode* lpNode)
{
	m_lpLeftReplica->checkAndSetPredicate(lpNode);
	m_lpRightReplica->checkAndSetPredicate(lpNode);
}

//---------------------------------------------------------------
// Memory management functions

// New a node and put it in the m_lpList
BJoinNode* BJoinNode::create()
{
	Log::writeToLog("BJoinNode", 10, "Create without arguments");
		
	BJoinNode* lpNode = new BJoinNode();
	pushBack(lpNode);
	return lpNode;
}

// New a node and put it in the m_lpList
BJoinNode* BJoinNode::create(EColumn* lpLeft, EColumn* lpRight, int iComparisonType)
{
	Log::writeToLog("BJoinNode", 10, "Create with 2 arguments: left node and right node");
		
	BJoinNode* lpNode = new BJoinNode(lpLeft, lpRight, iComparisonType);
	pushBack(lpNode);
	return lpNode;
}
