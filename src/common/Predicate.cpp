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
#include "Predicate.h"

Predicate::Predicate(PredicateType type_) {
  predType=type_;
  if ((predType==OP_LESS_THAN) ||
      (predType==OP_LESS_THAN_OR_EQUAL) ||
      (predType==OP_EQUAL) ||
      (predType==OP_GREATER_THAN_OR_EQUAL) ||
      (predType==OP_GREATER_THAN))
    isBinary=true;
  else 
    isBinary=false;
  
  isBoundLHS=false;
  isBoundRHS=false;
  vp = NULL;
}

Predicate::Predicate(PredicateType type_, ValPos* rhs_) {
  (Predicate(type_));
  isBoundRHS=true;
  rhsVal=rhs_;
  vp = NULL;
}

Predicate::Predicate(const Predicate& pred) {
  this->lhsVal=pred.lhsVal->clone();
  this->rhsVal=pred.rhsVal->clone();
  this->isBinary=pred.isBinary;
  this->isBoundLHS=pred.isBoundLHS;
  this->isBoundRHS=pred.isBoundRHS;
  this->predType=pred.predType;	
  vp = NULL;
}

Predicate::~Predicate() {
  delete vp;
}

void Predicate::setRHS(ValPos* rhs_) {
  rhsVal=rhs_;
  isBoundRHS=true;	
}
void Predicate::setRHS(int rhsint) {
  vp = new IntValPos(0, rhsint);
  rhsVal=vp;
  isBoundRHS=true;	
}
void Predicate::setLHS(ValPos* lhs_) {
  lhsVal=lhs_;
  isBoundLHS=true;	
}
ValPos* Predicate::getLHS() {
  // TODO Throw exception if not bound
  return lhsVal;
}
ValPos* Predicate::getRHS() {
  // TODO Throw exception if not bound
  return rhsVal;
}

Predicate::PredicateType Predicate::getPredType() {
  return this->predType;
}	

bool Predicate::evalPredicate() {
  // TODO check arguements are bound	
  switch (predType) {
  case OP_LESS_THAN:
    return (*lhsVal<rhsVal);
  case OP_LESS_THAN_OR_EQUAL:
    return (*lhsVal<=rhsVal);			
  case OP_EQUAL:
    return (*lhsVal==rhsVal);			
  case OP_GREATER_THAN_OR_EQUAL:
    return (*lhsVal>=rhsVal);
  case OP_GREATER_THAN:
    return (*lhsVal>rhsVal);			
    //case OP_EVEN:
    //return (lhsVal%2==0);			
    //case OP_ODD:
    //return (lhsVal%2==1);			
  default:
    // TODO throw exception
    return false;
  }
  
}

bool Predicate::evalPredicate(int val) {
  // TODO check arguements are bound	
  switch (predType) {
  case OP_LESS_THAN:
    return (*lhsVal<val);
  case OP_LESS_THAN_OR_EQUAL:
    return (*lhsVal<=val);			
  case OP_EQUAL:
    return (*lhsVal==val);			
  case OP_GREATER_THAN_OR_EQUAL:
    return (*lhsVal>=val);
  case OP_GREATER_THAN:
    return (*lhsVal>val);			
    //case OP_EVEN:
    //return (lhsVal%2==0);			
    //case OP_ODD:
    //return (lhsVal%2==1);			
  default:
    // TODO throw exception
    return false;
  }
  
}
