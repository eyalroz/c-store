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
#ifndef PREDICATE_H
#define PREDICATE_H

#include "IntValPos.h"

class Predicate {
public:

	// Enumeration that describes the type of operation this predicate performs
	enum PredicateType {
		OP_LESS_THAN,
		OP_LESS_THAN_OR_EQUAL,
		OP_EQUAL,
		OP_GREATER_THAN_OR_EQUAL,
		OP_GREATER_THAN,
		//OP_EVEN,
		//OP_ODD	
	};
	
	// Instatiate a predicate with nothing bound
	Predicate(PredicateType type_);
	
	// Instatiate a predicate with the rhs bound (useful for > k style predicates)
	Predicate(PredicateType type_, ValPos* rhs_);	

	// Copy constructor
	Predicate(const Predicate& predicate_);	
	
	// Destructor
	virtual ~Predicate();	

	// Bind the RHS of operator	
	void setRHS(ValPos* rhs_);

	// Bind the RHS of operator	
	void setRHS(int rhsint);
	
	// Bind the lhs of operator	
	void setLHS(ValPos* lhs_);
	
	// Get the value of the lhs of operator
	ValPos* getLHS();
	
	// Get the value of the rhs of operator
	ValPos* getRHS();
	
	// Get the type of predicate
	PredicateType getPredType();
	
	// Evaluate the predicate
	bool evalPredicate();
	
	// Evaluate the predicate with the special case that the rhs is an int
	bool evalPredicate(int rhs);
	
protected:
	ValPos* lhsVal;
	ValPos* rhsVal;
	bool isBinary;
	bool isBoundLHS;
	bool isBoundRHS;
	PredicateType predType;
	ValPos* vp;
};

#endif // PREDICATE_H
