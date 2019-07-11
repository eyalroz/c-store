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
#include "LongValPos.h"

LongValPos::LongValPos() {
  type=ValPos::LONGTYPE;
  longvalue = 0;
  position=0;
  value=(byte*)&longvalue;
}

LongValPos::LongValPos(unsigned int pos_, long long value_) {
  type = ValPos::LONGTYPE;
  position=pos_;
  longvalue=value_;
  value=(byte*)&longvalue;
}

LongValPos::LongValPos(LongValPos& valpos_) {
  assert(valpos_.type == ValPos::LONGTYPE);
  type=valpos_.type;
  position=valpos_.position;
  longvalue=valpos_.longvalue;
  value=(byte*)&longvalue;
}

ValPos* LongValPos::clone()
{
  ValPos* newVP = new LongValPos(*this);
  return newVP;
}

LongValPos::~LongValPos()
{
  value=NULL;
}

unsigned short LongValPos::getSize() {
  return sizeof(long long);
}

void LongValPos::set(byte* value_) {
  longvalue=*(long long*)value_;
}

void LongValPos::set(unsigned int pos_, byte* value_) {
  position=pos_;
  longvalue=*(long long*)value_;
}

void LongValPos::set(unsigned int type_, unsigned int pos_, byte* value_) {
  assert(type==ValPos::LONGTYPE);
  type = type_;
  position=pos_;
  longvalue=*(long long*)value_;
}

void LongValPos::printVal(std::ostream* o) {
  (*o) << longvalue;
}

//generic function overloads
bool LongValPos::operator> (ValPos* vp) {
  if (vp->type != ValPos::LONGTYPE)
    throw new UnexpectedException("Incompatible types");
  return longvalue > *(long long*)vp->value;
}

bool LongValPos::operator>= (ValPos* vp) {
  if (vp->type != ValPos::LONGTYPE)
    throw new UnexpectedException("Incompatible types");
  return longvalue >= *(long long*)vp->value;
}

bool LongValPos::operator== (ValPos* vp) {
  if (vp->type != ValPos::LONGTYPE)
    throw new UnexpectedException("Incompatible types");
  return longvalue == *(long long*)vp->value;
}

bool LongValPos::operator< (ValPos* vp) {
  if (vp->type != ValPos::LONGTYPE)
    throw new UnexpectedException("Incompatible types");
  return longvalue < *(long long*)vp->value;
}

bool LongValPos::operator<= (ValPos* vp) {
  if (vp->type != ValPos::LONGTYPE)
    throw new UnexpectedException("Incompatible types");
  return longvalue <= *(long long*)vp->value;
}

bool LongValPos::operator!= (ValPos* vp) {
  if (vp->type != ValPos::LONGTYPE)
    throw new UnexpectedException("Incompatible types");
  return longvalue != *(long long*)vp->value;
}

//specialized function overloads
bool LongValPos::operator> (int vp) {
  return longvalue > vp;
}

bool LongValPos::operator>= (int vp) {
  return longvalue >= vp;
}

bool LongValPos::operator== (int vp) {
  return longvalue == vp;
}

bool LongValPos::operator< (int vp) {
  return longvalue < vp;
}

bool LongValPos::operator<= (int vp) {
  return longvalue <= vp;
}

bool LongValPos::operator!= (int vp) {
  return longvalue != vp;
}
