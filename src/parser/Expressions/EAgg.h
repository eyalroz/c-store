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
#ifndef _EAGG_H_
#define _EAGG_H_

#include <string>

#include "../Expression.h"
#include "EColumn.h"
#include "../../plan/Nodes/SelectNode.h"
#include "../../plan/UNodes/UProjectNode.h"
#include "../../plan/BNodes/BProjectNode.h"


using namespace std;

// Variable
class EAgg : public Expression
{
   protected:
        EColumn*	m_lpRight;
        string		m_sOp;  
		Node*		m_lpAggNode;
		Node*		m_lpSelectNode;
	      
    protected:
		// PObject's constructor is protected because they should not be new directly.
		// A node can be new by using static "create" function
		// This is for the purpose of memory management
        EAgg();
		EAgg(EColumn* lpRight, string op);
		
	public:
        virtual ~EAgg();

		int	getType( ListProjections* lp );
		int	isAggregate();
		void	setRight(EColumn* lpRight);
		void	set(EColumn* lpRight, string op);
		EColumn*	getRight();
		string  getOp();

        virtual string	toString();
		
		// Translate the expression into a plan node
		virtual Node* translate();
		
		// Translate the expression into a plan node
		virtual Node* translate(bool bNewSelectNode, Node* lpNode, Node* lpGroupNode, int* lpCopyIndex);
	
	protected:
		// Get real aggregate node
		Node* getAggNode(bool bNewSelectNode, Node* lpNode, int* lpCopyIndex);

	//---------------------------------------------------------------
	// Memory management functions
		
	public:
		// New a PObject and put it in the m_lpList
		static EAgg* create();
		
		// New a PObject and put it in the m_lpList
		static EAgg* create(EColumn* lpRight, string op);
};

#endif //  _EAGG_H_
