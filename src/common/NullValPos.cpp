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
#include "NullValPos.h"

NullValPos::NullValPos() {
  type=ValPos::NOTYPE;
  position=0;
  value=NULL;
}

NullValPos::NullValPos(unsigned int pos_) {
  type = ValPos::NOTYPE;
  position=pos_;
  value=NULL;
}

NullValPos::NullValPos(NullValPos& valpos_) {
  assert(valpos_.type == ValPos::NOTYPE);
  type=valpos_.type;	
  position=valpos_.position;
  value=NULL;
}

ValPos* NullValPos::clone()
{
  ValPos* newVP = new NullValPos(*this);
  return newVP;
}

NullValPos::~NullValPos()
{
  value=NULL;
}

unsigned short NullValPos::getSize() {
  return 0;
}

void NullValPos::set(byte* value_) {
  assert(value_==NULL);
}

void NullValPos::set(unsigned int pos_, byte* value_) {
  assert(value_==NULL);
  position=pos_;
}

void NullValPos::set(unsigned int type_, unsigned int pos_, byte* value_) {
  assert(type==ValPos::NOTYPE);
  assert(value_==NULL);
  type = type_;
  position=pos_;
}

void NullValPos::printVal(std::ostream* o) {
  throw new UnexpectedException("Trying to print a Null value ...");
}


//generic function overloads
bool NullValPos::operator> (ValPos* vp) {
  return false;
}

bool NullValPos::operator>= (ValPos* vp) {
  return false;
}

bool NullValPos::operator== (ValPos* vp) {
  return false;
}

bool NullValPos::operator< (ValPos* vp) {
  return false;
}

bool NullValPos::operator<= (ValPos* vp) {
  return false;
}

bool NullValPos::operator!= (ValPos* vp) {
  return false;
}

//specialized function overloads
bool NullValPos::operator> (int vp) {
  return false;
}

bool NullValPos::operator>= (int vp) {
  return false;
}

bool NullValPos::operator== (int vp) {
  return false;
}

bool NullValPos::operator< (int vp) {
  return false;
}

bool NullValPos::operator<= (int vp) {
  return false;
}

bool NullValPos::operator!= (int vp) {
  return false;
}
