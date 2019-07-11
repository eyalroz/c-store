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
 *  InsertNode.h
 *  cstore
 *
 *  Created by Nga Tran on 6/6/05.
 * 
 *	Implementation of INSERT INTO projectionName VALUES(...);
 *	The name of this class should be changed to InsertProjectionNode
 */

#ifndef _INSERTNODE_H_
#define _INSERTNODE_H_

#include <string>
#include "../Node.h"
#include "../../parser/Projection.h"
#include "../../parser/Expression.h"
#include "../../parser/Expressions/EColumn.h"
#include "../../parser/Expressions/EString.h"
#include "../../parser/Expressions/ENumbers/EInteger.h"
#include "../../parser/Expressions/ENumbers/EFloat.h"
#include "../../Operators/InsertOperator.h"


using namespace std;

class InsertNode : public Node
{
	protected:
		Projection*			m_lpProjection;	// Projection to be inserted
		list<EColumn*>		m_listColumns;	// List of columns to be inserted
		list<Expression*>	m_listValues;	// List of coresponding values to be inserted
		
	protected:
		// Node's constructor is protected because they should not be new directly.
		// A node can be new by using static "create" function
		// This is for the purpose of memory management
		InsertNode();
		InsertNode(Projection* lpProjection, list<EColumn*>	 listColumns, list<Expression*> listValues);
		
	public:
		virtual ~InsertNode();
		
		// Run
		virtual Operator* run(bool rosOnly = 1);

		// show what operator this node will run
		virtual void show(int* lpNodeID, ofstream* lpOfstream, bool rosOnly = 1);
		
		// show tree presentation
		virtual void showTree(string sTabs);	
				
	//---------------------------------------------------------------
	// Memory management functions
		
	public:		
		// New a node and put it in the m_lpList
		static InsertNode* create();
		static InsertNode* create(Projection* lpProjection, list<EColumn*>	 listColumns, list<Expression*> listValues);
};

#endif // _INSERTNODE_H_

