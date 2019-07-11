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
#include "StringValPos.h"

StringValPos::StringValPos() {
  type=ValPos::STRINGTYPE;
  valsize=0;
  position=0;
  value=NULL;
}

StringValPos::StringValPos(unsigned int pos_, unsigned short vs_) {
  type = ValPos::STRINGTYPE;
  valsize=vs_;
  position=pos_;
  value=new byte[vs_];
}

StringValPos::StringValPos(StringValPos& valpos_) {
  assert(valpos_.type == ValPos::STRINGTYPE);
  type=valpos_.type;	
  valsize=valpos_.valsize;
  position=valpos_.position;
  value=new byte[valpos_.valsize];
  set(valpos_.value);
}

ValPos* StringValPos::clone()
{
  ValPos* newVP = new StringValPos(*this);
  return newVP;
}

StringValPos::~StringValPos()
{
  delete[] value;
}

unsigned short StringValPos::getSize() {
  return valsize;
}

void StringValPos::set(byte* value_) {
  assert(valsize!=0);
  memcpy(value, value_, valsize);
}

void StringValPos::set(unsigned int pos_, byte* value_) {
  position=pos_;
  assert(valsize!=0);
  memcpy(value, value_, valsize);
}

void StringValPos::set(unsigned int type_, unsigned int pos_, byte* value_) {
  throw new UnexpectedException("Don't like this method for strings since we don't know valsize");
}

void StringValPos::printVal(std::ostream* o) {
  for (int i = 0; i < valsize; i++) {
    if (value[i])
      (*o) << value[i];
    else
      return;
  }
  
}

//generic function overloads
bool StringValPos::operator> (ValPos* vp) {
  if ((vp->type != ValPos::STRINGTYPE) || (vp->getSize()!=valsize))
    throw new UnexpectedException("Incompatible types");
  return memcmp(value, vp->value, valsize) > 0;
}

bool StringValPos::operator>= (ValPos* vp) {
  if ((vp->type != ValPos::STRINGTYPE) || (vp->getSize()!=valsize))
    throw new UnexpectedException("Incompatible types");
  return memcmp(value, vp->value, valsize) >= 0;
}

bool StringValPos::operator== (ValPos* vp) {
  if ((vp->type != ValPos::STRINGTYPE) || (vp->getSize()!=valsize))
    throw new UnexpectedException("Incompatible types");
  return memcmp(value, vp->value, valsize) == 0;
}

bool StringValPos::operator< (ValPos* vp) {
  if ((vp->type != ValPos::STRINGTYPE) || (vp->getSize()!=valsize))
    throw new UnexpectedException("Incompatible types");
  return memcmp(value, vp->value, valsize) < 0;
}

bool StringValPos::operator<= (ValPos* vp) {
  if ((vp->type != ValPos::STRINGTYPE) || (vp->getSize()!=valsize))
    throw new UnexpectedException("Incompatible types");
  return memcmp(value, vp->value, valsize) <= 0;
}

bool StringValPos::operator!= (ValPos* vp) {
  if ((vp->type != ValPos::STRINGTYPE) || (vp->getSize()!=valsize))
    throw new UnexpectedException("Incompatible types");
  return memcmp(value, vp->value, valsize) != 0;
}

//specialized function overloads
bool StringValPos::operator> (int vp) {
  throw new UnexpectedException("Incompatible types");
}

bool StringValPos::operator>= (int vp) {
  throw new UnexpectedException("Incompatible types");
}

bool StringValPos::operator== (int vp) {
  throw new UnexpectedException("Incompatible types");
}

bool StringValPos::operator< (int vp) {
  throw new UnexpectedException("Incompatible types");
}

bool StringValPos::operator<= (int vp) {
  throw new UnexpectedException("Incompatible types");
}

bool StringValPos::operator!= (int vp) {
  throw new UnexpectedException("Incompatible types");
}
