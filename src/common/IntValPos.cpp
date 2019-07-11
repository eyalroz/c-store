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
#include "IntValPos.h"

IntValPos::IntValPos() {
  type=ValPos::INTTYPE;
  position=0;
  intvalue=0;
  value=(byte*)&intvalue;
}

IntValPos::IntValPos(unsigned int pos_, int value_) {
  type = ValPos::INTTYPE;
  position=pos_;
  intvalue=value_;
  value=(byte*)&intvalue;
}

IntValPos::IntValPos(IntValPos& valpos_) {
  assert(valpos_.type == ValPos::INTTYPE);
  type=valpos_.type;	
  position=valpos_.position;
  intvalue=valpos_.intvalue;
  value=(byte*)&intvalue;
}

ValPos* IntValPos::clone()
{
  ValPos* newVP = new IntValPos(*this);
  return newVP;
}

IntValPos::~IntValPos()
{
  value=NULL;
}

unsigned short IntValPos::getSize() {
  return sizeof(int);
}

void IntValPos::set(byte* value_) {
  intvalue=*(int*)value_;
}

void IntValPos::set(unsigned int pos_, byte* value_) {
  position=pos_;
  intvalue=*(int*)value_;
}

void IntValPos::set(unsigned int type_, unsigned int pos_, byte* value_) {
  assert(type==ValPos::INTTYPE);
  type = type_;
  position=pos_;
  intvalue=*(int*)value_;
}

void IntValPos::printVal(std::ostream* o) {
  (*o) << intvalue;
}

//generic function overloads
bool IntValPos::operator> (ValPos* vp) {
  if (vp->type != ValPos::INTTYPE)
    throw new UnexpectedException("Incompatible types");
  return intvalue > *(int*)vp->value;
}

bool IntValPos::operator>= (ValPos* vp) {
  if (vp->type != ValPos::INTTYPE)
    throw new UnexpectedException("Incompatible types");
  return intvalue >= *(int*)vp->value;
}

bool IntValPos::operator== (ValPos* vp) {
  if (vp->type != ValPos::INTTYPE)
    throw new UnexpectedException("Incompatible types");
  return intvalue == *(int*)vp->value;
}

bool IntValPos::operator< (ValPos* vp) {
  if (vp->type != ValPos::INTTYPE)
    throw new UnexpectedException("Incompatible types");
  return intvalue < *(int*)vp->value;
}

bool IntValPos::operator<= (ValPos* vp) {
  if (vp->type != ValPos::INTTYPE)
    throw new UnexpectedException("Incompatible types");
  return intvalue <= *(int*)vp->value;
}

bool IntValPos::operator!= (ValPos* vp) {
  //if (vp->type != ValPos::INTTYPE)
  //  throw new UnexpectedException("Incompatible types");
  return intvalue != *(int*)vp->value;
}

//specialized function overloads
bool IntValPos::operator> (int vp) {
  return intvalue > vp;
}

bool IntValPos::operator>= (int vp) {
  return intvalue >= vp;
}

bool IntValPos::operator== (int vp) {
  return intvalue == vp;
}

bool IntValPos::operator< (int vp) {
  return intvalue < vp;
}

bool IntValPos::operator<= (int vp) {
  return intvalue <= vp;
}

bool IntValPos::operator!= (int vp) {
  return intvalue != vp;
}
