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
#ifndef _BCOMPARISON_H_
#define _BCOMPARISON_H_

#include <string>

#include "../BExpression.h"
#include "../Expression.h"

#include "../../plan/Nodes/SelectNode.h"
#include "../../common/Predicate.h"
#include "../Expressions/EColumn.h"
#include "../constants.h"
#include "../Expressions/ENumbers/EFloat.h"
#include "../Expressions/ENumbers/EInteger.h"
#include "../Expressions/EString.h"
#include "../ListProjections.h"
#include "../../plan/BNodes/BJoinNode.h"


using namespace std;

// Variable
class BComparison : public BExpression
{
   protected:
		// PObject's constructor is protected because they should not be new directly.
		// A node can be new by using static "create" function
		// This is for the purpose of memory management
		BComparison();
		BComparison( Expression* lpLeft, Expression* lpRight, string op );
		
	public:
		virtual ~BComparison();

		void setLeft( Expression* lpLeft );
		void setRight( Expression* lpRight );
		Expression* getLeft();
		Expression* getRight();
		string getOp();
		void typeCheck( ListProjections* lp );

		// Translate the expression into a plan node
		virtual Node* translate(StringNodeMap* lpMapColumns);
	
		string	toString();

    protected:
		// Common translation for all BComparison nodes
		Node* commonTranslate(int iComparisonType, StringNodeMap* lpMapColumns);
	
		Expression* m_lpLeft;
		Expression* m_lpRight;
		string m_sOp;

	//---------------------------------------------------------------
	// Memory management functions
		
	public:
		// New a PObject and put it in the m_lpList
		static BComparison* create();
		
		// New a PObject and put it in the m_lpList
		static BComparison* create(Expression* lpLeft, Expression* lpRight, string op);
};

#endif //  _BCOMPARISON_H_
