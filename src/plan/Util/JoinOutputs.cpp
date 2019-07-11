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
 *  JoinOuputs.cpp
 *  cstore
 *
 *  Created by Nga Tran on 6/22/05.
 *  Copyright 2005 __MyCompanyName__. All rights reserved.
 *
 */

#include "JoinOutputs.h"

JoinOutputs::JoinOutputs()
{
	m_lpRosRos = NULL;
	m_lpRosWos = NULL;
	m_lpWosRos = NULL;
	m_lpWosWos = NULL;
	m_ptrColval = new int();
	*m_ptrColval = 0;

	//m_lpLeftROS = NULL;
	//m_lpRightROS = NULL;
}

JoinOutputs::~JoinOutputs()
{
  /*
	cout << "Deallocate memory at JoinOutputs" << endl; 
	
	if (m_lpRosRos != NULL)
	{
		delete m_lpRosRos;
		m_lpRosRos = NULL;
	}
		
	if (m_lpRosWos != NULL)
	{
		delete m_lpRosWos;
		m_lpRosWos = NULL;
	}
	
	if (m_lpWosRos != NULL)
	{
		delete m_lpWosRos;
		m_lpWosRos = NULL;
	}
	
	if (m_lpWosWos != NULL)
	{
		delete m_lpWosWos;
		 m_lpWosWos = NULL;
	}
	*/
}
		
void JoinOutputs::setFirstProjection(string sFirstProjection)
{
	m_sFirstProjection = sFirstProjection;
}

string JoinOutputs::getFirstProjection()
{
	return m_sFirstProjection;
}

void JoinOutputs::setSecondProjection(string sSecondProjection)
{
	m_sSecondProjection = sSecondProjection;
}

string JoinOutputs::getSecondProjection()
{
	return m_sSecondProjection;
}

		
void JoinOutputs::setFirstJoinColumn(string sFirstJoinColumn)
{
	m_sFirstJoinColumn = sFirstJoinColumn;
}

string JoinOutputs::getFirstJoinColumn()
{
	return m_sFirstJoinColumn;
}

		
void JoinOutputs::setSecondJoinColumn(string sSecondJoinColumn)
{
	m_sSecondJoinColumn = sSecondJoinColumn;
}


string JoinOutputs::getSecondJoinColumn()
{
	return m_sSecondJoinColumn;
}

		
void JoinOutputs::setRosRos(Operator** lpLeft, int iLeftColIndex, Operator** lpRight, int iRightColIndex)
{
  
  //m_lpRosRos = new NLJoin(lpLeft, iLeftColIndex, lpRight, iRightColIndex);
  //m_lpRosRos = new HashJoin(&lpLeft, &m_icolval, &lpRight, &m_icolval, 1, 1, true);
  //m_lpRosRos = new HashJoin(&lpLeft, m_ptrColval, &lpRight, m_ptrColval, 1, 1, true);

  //m_lpLeftROS = *lpLeft;
  //m_lpRightROS = *lpRight;

  m_lpRosRos = new HashJoin(lpLeft, m_ptrColval, lpRight, m_ptrColval, 1, 1, true);
  
  //cout << "NGA:  lpROSROS A: " << m_lpRosRos << endl;
}

Operator* JoinOutputs::getRosRos()
{
	return m_lpRosRos;
}

		
void JoinOutputs::setRosWos(Operator* lpLeft, int iLeftColIndex, Operator* lpRight, int iRightColIndex)
{
	m_lpRosWos = new NLJoin(lpLeft, iLeftColIndex, lpRight, iRightColIndex);;
}

Operator* JoinOutputs::getRosWos()
{
	return m_lpRosWos;
}

		
void JoinOutputs::setWosRos(Operator* lpLeft, int iLeftColIndex, Operator* lpRight, int iRightColIndex)
{
	m_lpWosRos = new NLJoin(lpLeft, iLeftColIndex, lpRight, iRightColIndex);;
}

Operator* JoinOutputs::getWosRos()
{
	return m_lpWosRos;
}

		
void JoinOutputs::setWosWos(Operator* lpLeft, int iLeftColIndex, Operator* lpRight, int iRightColIndex)
{
	m_lpWosWos = new NLJoin(lpLeft, iLeftColIndex, lpRight, iRightColIndex);;
}

Operator* JoinOutputs::getWosWos()
{
	return m_lpWosWos;
}


//---------------------------------------------------------------
// Memory management functions
		
	
// New a node and put it in the m_lpList
JoinOutputs* JoinOutputs::create()
{
	Log::writeToLog("JoinOutputs", 10, "Create without arguments");
		
	JoinOutputs* lpNode = new JoinOutputs();
	pushBack(lpNode);
	return lpNode;
}
