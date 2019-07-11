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
#include "Block.h"
#include "../Wrappers/Type2Block.h"

Block::Block() {
}

Block::Block(const Block& block_) {
  valsize = block_.valsize;
}

Block::~Block() {
}

/*
bool Block::isSplittable() {
	return false;
}

Block* Block::split(int npositions) {
	throw UnexpectedException("unsupported");
}

*/

void Block::setType(short type_) {
  delete vp;
  delete utilityVP;
  switch (type_) {
  case ValPos::INTTYPE:
    vp = new IntValPos();
    break;
  case ValPos::FLOATTYPE:
    vp = new FloatValPos();
    break;
  case ValPos::DOUBLETYPE:
    vp = new DoubleValPos();
    break;
  case ValPos::LONGTYPE:
    vp = new LongValPos();
    break;
  case ValPos::STRINGTYPE:
    vp = new StringValPos();
    break;
  default:
    throw new UnexpectedException("Unknown type.");
  }
  utilityVP = vp->clone();
  valsize = vp->getSize();
}

void Block::setType(ValPos* vp_) {
  delete vp;
  delete utilityVP;
  vp = vp_->clone();
  utilityVP = vp_->clone();
  valsize = vp_->getSize();
}

bool Block::applyPredicate(Type2Block* t2b, Predicate* p) {
  ValPos* rhs = p->getRHS();
  if (rhs->type == ValPos::INTTYPE) {
    int rhsint = *(int*)rhs->value;
    //t2b = new Type2Block(false);
    ValPos* vp=getNext();
    //currBlock->resetBlock();
    //int value=pair->value;		
    p->setLHS(vp);  
    while ((!p->evalPredicate(rhsint)) && (hasNext())) {
      vp = getNext();		
      p->setLHS(vp); 
    }
    if (p->evalPredicate(rhsint)) { //we stopped while loop via 1st end condition
      t2b->initEmptyBuffer(vp->position, vp);
      t2b->addPosition(vp->position);
    }
    else return false;
    int currEndPos = vp->position;
    while (hasNext()) {
      vp=getNext();		
      p->setLHS(vp);
      if (p->evalPredicate(rhsint)) {
	t2b->addPosition(vp->position);
	currEndPos = vp->position;
      }
    }
    t2b->finishBufferEditing(currEndPos);
    return true;
  }
  else {
    throw new UnimplementedException("Deal with this case later");
  }
}
