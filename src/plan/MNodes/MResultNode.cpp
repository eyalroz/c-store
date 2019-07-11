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
 *  MResultNode.cpp
 *  cstore
 *
 *  Created by Nga Tran on 3/31/05.
 *  Copyright 2005 __MyCompanyName__. All rights reserved.
 *
 */

#include "MResultNode.h"

#include <iostream>


MResultNode::MResultNode() : MNode()
{
	m_bIsAggreggate = 0;
	m_bhasGroupby = 0;
	m_bDoMerge = 1;
}

MResultNode::MResultNode(list<Node*> lpChildren) : MNode(lpChildren)
{
	m_bIsAggreggate = 0;
	m_bhasGroupby = 0;
	m_bDoMerge = 1;
}

// Deallocate memory for its members
MResultNode::~MResultNode()
{
	 Log::writeToLog("MResultNode", 10, "~MResultNode"); 
}

// set if merge should be done
void MResultNode::setDoMerge(bool bDoMerge)
{
	m_bDoMerge = bDoMerge;
}

// There is aggragte in the results
void MResultNode::setAggregate()
{
	m_bIsAggreggate = 1;
}

// There is group by
void MResultNode::setGroupby()
{
	m_bhasGroupby = 1;
}

// Merge ROS and WOS operators
Operator* MResultNode::mergeROSandWOS()
{
	// Never used
	return MNode::mergeROSandWOS();
}
		
// Run the operator of this node
Operator* MResultNode::run(bool rosOnly)
{
	
	if (m_lpReturnOp != NULL)
	{
		return m_lpReturnOp;
	}
	
	int numCols = m_children.size();
	//Operator* srcsROS[numCols];
	//Operator* srcsWOS[numCols];
	Operator* srcs[numCols];
	int numColumns[numCols];

	// cout << "NGA: # nodes: " << numCols << endl;
	
	// Run all child nodes first
	list<Node*>::iterator it;
	int i = 0;
	for (it = m_children.begin(); it != m_children.end(); it++)
	{
		Node* lpNode = (*it);
		if (lpNode == NULL)
		  {
		    //cout << "NGA: NODE NULL \n";
		  }

		if (lpNode != NULL)
		{
			// ROS
			Operator* lpOpROS = NULL;
			lpOpROS = lpNode->runROS();
			
			if (lpOpROS == NULL)
			{
				// Invaid
				return m_lpReturnOp;
			}
			
			//srcsROS[i] = lpOpROS;
			cout << "	ROS " << i << " done"<< endl;
			
			
			if (!rosOnly)
			  {
			    // WOS
			    Operator* lpOpWOS = NULL;
			    lpOpWOS = lpNode->runWOS();
			  
			
			    if (lpOpWOS == NULL)
			      {
				// Invaid
				return m_lpReturnOp;
			      }
			
			    //srcsWOS[i] = lpOpWOS;
			    cout << "	WOS " << i << " done"<< endl;
			  }
			
			//Merge
			if (m_bDoMerge == 1)
			{
			  if (!rosOnly)
			    {
				Operator* lpOp = NULL;
				lpOp = lpNode->mergeROSandWOS();
				cout << "	Merge " << i << " done"<< endl;
			
				srcs[i] = lpOp;
			    }
			  else
			    {
			      srcs[i] = lpOpROS;

			      //cout << "NGA pointer " << i << ": " << lpOpROS << endl;
			    }
			
				if ( (m_bIsAggreggate) && (m_bhasGroupby) )
				{
					numColumns[i] = 2;
				}
				else
				{
					numColumns[i] = 1;
				}
			
				//cout << "NGA: numColumns[i]: " << numColumns[i] << endl;
				i++;
			}
		}
	}

	//cout << "NGA: REACH HERE XXX\n";
	
	if (m_bDoMerge == 1)
	{
		BlockPrinter* bPrint = new BlockPrinter(srcs, numColumns, numCols, QUERY_OUT);
	
		bPrint->printColumns();
		
		delete bPrint;
	}
	
	return m_lpROSReturnOp;
}


// Run ROS only
Operator* MResultNode::runROS()
{
	cout << "ROS only" << endl;

	if (m_lpROSReturnOp != NULL)
	{
		return m_lpROSReturnOp;
	}
	
	int numCols = m_children.size();
	Operator* srcs[numCols];
	int numColumns[numCols];
	
	// Run all child nodes first
	list<Node*>::iterator it;
	int i = 0;
	for (it = m_children.begin(); it != m_children.end(); it++)
	{
		Node* lpNode = (*it);
		if (lpNode != NULL)
		{
		
			Operator* lpOp = NULL;
			lpOp = lpNode->runROS();
			
			if (lpOp == NULL)
			{
				return m_lpROSReturnOp;
			}
			
			srcs[i] = lpOp;
			if ( (m_bIsAggreggate) && (m_bhasGroupby) )
			{
				numColumns[i] = 2;
			}
			else
			{
				numColumns[i] = 1;
			}
			
			i++;
		}
	}
	
	BlockPrinter* bPrint = new BlockPrinter(srcs, numColumns, numCols, "XXX.out");
	
	bPrint->printColumns();
	
	delete bPrint;
	
	return m_lpROSReturnOp;

}

// Run WOS only
Operator* MResultNode::runWOS()
{
	cout << "WOS only" << endl;
	
	if (m_lpWOSReturnOp != NULL)
	{
		return m_lpWOSReturnOp;
	}
	
	int numCols = m_children.size();
	Operator* srcs[numCols];
	int numColumns[numCols];
	
	// Run all child nodes first
	list<Node*>::iterator it;
	int i = 0;
	for (it = m_children.begin(); it != m_children.end(); it++)
	{
		Node* lpNode = (*it);
		if (lpNode != NULL)
		{
		
			Operator* lpOp = NULL;
			lpOp = lpNode->runWOS();
			
			if (lpOp == NULL)
			{
				return m_lpWOSReturnOp;
			}
			
			srcs[i] = lpOp;
			if ( (m_bIsAggreggate) && (m_bhasGroupby) )
			{
				numColumns[i] = 2;
			}
			else
			{
				numColumns[i] = 1;
			}
			
			i++;
		}
	}
	
	BlockPrinter* bPrint = new BlockPrinter(srcs, numColumns, numCols, "XXX.out");
	
	bPrint->printColumns();
	
	delete bPrint;
	
	return m_lpWOSReturnOp;

}

// show what operator this node will run
void MResultNode::show(int* lpNodeID, ofstream* lpOfstream, bool rosOnly)
{
	// The operator exists
	if (m_bROSHasShown)
	{
		return;
	}
	
	m_bROSHasShown = 1;
	
	// Set nodeID to have different variable name for this node
	setNodeID(lpNodeID);
	
	// Run all child nodes first
	int numCols = m_children.size();
	
	char sTmp[15];	
	sprintf(sTmp, "%d", numCols);
	
	string sSrcs = "	Operator* srcs[" + string(sTmp) + "] = {";
	string sNumCols = "	int numColumns[" + string(sTmp) + "] = {";
	int i = 0;
	
	list<Node*>::iterator it;
	for (it = m_children.begin(); it != m_children.end(); it++)
	{
		Node* lpNode = (*it);
		if (lpNode != NULL)
		{
			string sOps = "\n//ROS OPERATORS:\n";
			lpOfstream->write(sOps.c_str(), sOps.length());
			lpNode->showROS(lpNodeID, lpOfstream);
			
			if (!rosOnly)
			  {
			    sOps = "\n//WOS OPERATORS:\n";
			    lpOfstream->write(sOps.c_str(), sOps.length());
			    lpNode->showWOS(lpNodeID, lpOfstream);
			  }
			
			if (m_bDoMerge == 1)
			{
			  if (!rosOnly)
			    {
				sOps = "\n//MERGE ROS AND WOS:\n";
				lpOfstream->write(sOps.c_str(), sOps.length());
				lpNode->showMerge(lpOfstream);
			    }
			
				sprintf(sTmp, "%d", lpNode->getNodeID());
				if (rosOnly)
				  {
				    sSrcs = sSrcs + "lp_ROS" + string(sTmp);
				  }
				else
				  {
				    sSrcs = sSrcs + "lp_Merge" + string(sTmp);
				  }
				
			
				if ( (m_bIsAggreggate) && (m_bhasGroupby) )
				{
					sNumCols = sNumCols + "2";
				}
				else
				{
					sNumCols = sNumCols + "1";
				}
			
				i++;
			
				if (i < numCols) 
				{
					sSrcs = sSrcs + ", ";
					sNumCols = sNumCols + ", ";
				}
			}
		}
	}
	
	if (m_bDoMerge == 1)
	{
		sSrcs = sSrcs + "};\n";
		sNumCols = sNumCols + "};\n";
	
		char sCols[15];	
		sprintf(sCols, "%d", numCols);
	
		string sOps = "\n//OUPUT THE RESULTS:\n";
		sOps = sOps + sSrcs;
		sOps = sOps + sNumCols;
		sOps = sOps + "	BlockPrinter* bPrint = new BlockPrinter(srcs, numColumns, " + string(sCols) + ", \"" + QUERY_OUT + "\");\n";
		sOps = sOps + "	bPrint->printColumns();\n";
		
		lpOfstream->write(sOps.c_str(), sOps.length());
	}
}


// show merge operators
void MResultNode::showMerge(ofstream* lpOfstream)
{
	MNode::showMerge(lpOfstream);
}

// show tree presentation
void MResultNode::showTree(string sTabs)
{
	// Show its information first
	cout << endl;
	cout << sTabs << "CONCAT (Output the results)" << endl;
	
	MNode::showTree(sTabs);
}


//---------------------------------------------------------------
// Memory management functions

// New a node and put it in the m_lpList
MResultNode* MResultNode::create()
{
	Log::writeToLog("MResultNode", 10, "Create without arguments");
		
	MResultNode* lpNode = new MResultNode();
	pushBack(lpNode);
	return lpNode;
}

// New a node and put it in the m_lpList
MResultNode* MResultNode::create(list<Node*>	lpChildren)
{
	Log::writeToLog("MResultNode", 10, "Create with 1 argument: list of child nodes");
		
	MResultNode* lpNode = new MResultNode(lpChildren);
	pushBack(lpNode);
	return lpNode;
}
