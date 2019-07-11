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
 *  DeletePlan.h
 *  cstore
 *
 *  Created by Nga Tran on 6/21/05.
 *  
 *  Plan for delete query
 */



#ifndef _DELETEPLAN_H_
#define _DELETEPLAN_H_

class QDelete;

#include "../Plan.h"
#include "../MNodes/MResultNode.h"
#include "../Nodes/DeleteProjectionNode.h"
#include "../UNodes/UDeleteProjectionNode.h"
#include "../UNodes/UNegationNode.h"
#include "../../parser/BExpression.h"
#include "../../parser/Projection.h"
#include "../../parser/Query.h"
#include "../../parser/Queries/QDelete.h"


class DeletePlan : public Plan
{
	protected:
	
		// Variables for the purpose of allocate/deallocate memory
		Node* m_lpDeleteProjection;
		bool  m_bIsDeletedAll;

	protected:
		// Node's constructor is protected because they should not be new directly.
		// A node can be new by using static "create" function
		// This is for the purpose of memory management
		DeletePlan();

	public:
		virtual ~DeletePlan();
		
		// Generate a plan
		void makePlan(Query* lpQuery);
		
	protected:
		// Parse DELETE query into specific lists
		void parseQuery(
			Query* lpQuery, Projection** lpProjection, BExpression** lpWherePredicate);
			
	//---------------------------------------------------------------
	// Memory management functions
		
	public:		
		// New a node and put it in the m_lpList
		static DeletePlan* create();

};


#endif //  _DELETEPLAN_H_


