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
#include "DoubleValPos.h"

DoubleValPos::DoubleValPos() {
  type=ValPos::DOUBLETYPE;
  position=0;
  doublevalue=0;
  value=(byte*)&doublevalue;
}

DoubleValPos::DoubleValPos(unsigned int pos_, double value_) {
  type = ValPos::DOUBLETYPE;
  position=pos_;
  doublevalue=value_;
  value=(byte*)&doublevalue;
}

DoubleValPos::DoubleValPos(DoubleValPos& valpos_) {
  assert(valpos_.type == ValPos::DOUBLETYPE);
  type=valpos_.type;	
  position=valpos_.position;
  doublevalue=valpos_.doublevalue;
  value=(byte*)&doublevalue;
}

ValPos* DoubleValPos::clone()
{
  ValPos* newVP = new DoubleValPos(*this);
  return newVP;
}

DoubleValPos::~DoubleValPos()
{
  value=NULL;
}

unsigned short DoubleValPos::getSize() {
  return sizeof(double);
}

void DoubleValPos::set(byte* value_) {
  doublevalue=*(double*)value_;
}

void DoubleValPos::set(unsigned int pos_, byte* value_) {
  position=pos_;
  doublevalue=*(double*)value_;
}

void DoubleValPos::set(unsigned int type_, unsigned int pos_, byte* value_) {
  assert(type==ValPos::DOUBLETYPE);
  type = type_;
  position=pos_;
  doublevalue=*(double*)value_;
}

void DoubleValPos::printVal(std::ostream* o) { 
  (*o) << doublevalue;
}

//generic function overloads
bool DoubleValPos::operator> (ValPos* vp) {
  if (vp->type != ValPos::DOUBLETYPE)
    throw new UnexpectedException("Incompatible types");
  return doublevalue > *(double*)vp->value;
}

bool DoubleValPos::operator>= (ValPos* vp) {
  if (vp->type != ValPos::DOUBLETYPE)
    throw new UnexpectedException("Incompatible types");
  return doublevalue >= *(double*)vp->value;
}

bool DoubleValPos::operator== (ValPos* vp) {
  if (vp->type != ValPos::DOUBLETYPE)
    throw new UnexpectedException("Incompatible types");
  return doublevalue == *(double*)vp->value;
}

bool DoubleValPos::operator< (ValPos* vp) {
  if (vp->type != ValPos::DOUBLETYPE)
    throw new UnexpectedException("Incompatible types");
  return doublevalue < *(double*)vp->value;
}

bool DoubleValPos::operator<= (ValPos* vp) {
  if (vp->type != ValPos::DOUBLETYPE)
    throw new UnexpectedException("Incompatible types");
  return doublevalue <= *(double*)vp->value;
}

bool DoubleValPos::operator!= (ValPos* vp) {
  if (vp->type != ValPos::DOUBLETYPE)
    throw new UnexpectedException("Incompatible types");
  return doublevalue != *(double*)vp->value;
}

//specialized function overloads
bool DoubleValPos::operator> (int vp) {
  return doublevalue > vp;
}

bool DoubleValPos::operator>= (int vp) {
  return doublevalue >= vp;
}

bool DoubleValPos::operator== (int vp) {
  return doublevalue == vp;
}

bool DoubleValPos::operator< (int vp) {
  return doublevalue < vp;
}

bool DoubleValPos::operator<= (int vp) {
  return doublevalue <= vp;
}

bool DoubleValPos::operator!= (int vp) {
  return doublevalue != vp;
}
