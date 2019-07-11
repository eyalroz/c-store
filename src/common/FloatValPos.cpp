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
#include "FloatValPos.h"

FloatValPos::FloatValPos() {
  type=ValPos::FLOATTYPE;
  position=0;
  floatvalue=0;
  value=(byte*)&floatvalue;
}

FloatValPos::FloatValPos(unsigned int pos_, float value_) {
  type = ValPos::FLOATTYPE;
  position=pos_;
  floatvalue=value_;
  value=(byte*)&floatvalue;
}

FloatValPos::FloatValPos(FloatValPos& valpos_) {
  assert(valpos_.type == ValPos::FLOATTYPE);
  type=valpos_.type;	
  position=valpos_.position;
  floatvalue=valpos_.floatvalue;
  value=(byte*)&floatvalue;
}

ValPos* FloatValPos::clone()
{
  ValPos* newVP = new FloatValPos(*this);
  return newVP;
}

FloatValPos::~FloatValPos()
{
  value=NULL;
}

unsigned short FloatValPos::getSize() {
  return sizeof(float);
}

void FloatValPos::set(byte* value_) {
  floatvalue=*(float*)value_;
}

void FloatValPos::set(unsigned int pos_, byte* value_) {
  position=pos_;
  floatvalue=*(float*)value_;
}

void FloatValPos::set(unsigned int type_, unsigned int pos_, byte* value_) {
  assert(type==ValPos::FLOATTYPE);
  type = type_;
  position=pos_;
  floatvalue=*(float*)value_;
}

void FloatValPos::printVal(std::ostream* o) {
  (*o) << floatvalue;
}

//generic function overloads
bool FloatValPos::operator> (ValPos* vp) {
  if (vp->type != ValPos::FLOATTYPE)
    throw new UnexpectedException("Incompatible types");
  return floatvalue > *(float*)vp->value;
}

bool FloatValPos::operator>= (ValPos* vp) {
  if (vp->type != ValPos::FLOATTYPE)
    throw new UnexpectedException("Incompatible types");
  return floatvalue >= *(float*)vp->value;
}

bool FloatValPos::operator== (ValPos* vp) {
  if (vp->type != ValPos::FLOATTYPE)
    throw new UnexpectedException("Incompatible types");
  return floatvalue == *(float*)vp->value;
}

bool FloatValPos::operator< (ValPos* vp) {
  if (vp->type != ValPos::FLOATTYPE)
    throw new UnexpectedException("Incompatible types");
  return floatvalue < *(float*)vp->value;
}

bool FloatValPos::operator<= (ValPos* vp) {
  if (vp->type != ValPos::FLOATTYPE)
    throw new UnexpectedException("Incompatible types");
  return floatvalue <= *(float*)vp->value;
}

bool FloatValPos::operator!= (ValPos* vp) {
  if (vp->type != ValPos::FLOATTYPE)
    throw new UnexpectedException("Incompatible types");
  return floatvalue != *(float*)vp->value;
}

//specialized function overloads
bool FloatValPos::operator> (int vp) {
  return floatvalue > vp;
}

bool FloatValPos::operator>= (int vp) {
  return floatvalue >= vp;
}

bool FloatValPos::operator== (int vp) {
  return floatvalue == vp;
}

bool FloatValPos::operator< (int vp) {
  return floatvalue < vp;
}

bool FloatValPos::operator<= (int vp) {
  return floatvalue <= vp;
}

bool FloatValPos::operator!= (int vp) {
  return floatvalue != vp;
}
