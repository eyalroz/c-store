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
#include "ValPos.h"
/*
ValPos::ValPos(unsigned int type_, unsigned int pos_, byte* value_) {
  type = type_;
  position=pos_;
  switch (type) {
  case NOTYPE: 
    throw new UnexpectedException("Type must be defined");
    break;
  case INTTYPE:
    value = new byte[sizeof(int)];
    memcpy(value, value_, sizeof(int));
    break;
  case FLOATTYPE:
    value = new byte[sizeof(float)];
    memcpy(value, value_, sizeof(float));
    break;
  case STRINGTYPE:
    value=value_;
    break;
  case LONGTYPE:
    value = new byte[sizeof(long long)];
    memcpy(value, value_, sizeof(long long));
    break;
  default:
    throw new UnexpectedException("Unknown type.");
    break;
  } 
}

ValPos::ValPos(ValPos& valpos_) {
  type=valpos_.type;	
  position=valpos_.position;
  switch (type) {
  case NOTYPE: 
    throw new UnexpectedException("Type must be defined");
    break;
  case INTTYPE:
    value = new byte[sizeof(int)];
    memcpy(value, valpos_.value, sizeof(int));
    break;
  case FLOATTYPE:
    value = new byte[sizeof(float)];
    memcpy(value, valpos_.value, sizeof(float));
    break;
  case STRINGTYPE:
    value=valpos_.value;
    break;
  case LONGTYPE:
    value = new byte[sizeof(long long)];
    memcpy(value, valpos_.value, sizeof(long long));
    break;
  default:
    throw new UnexpectedException("Unknown type.");
    break;
  } 	
}

ValPos* ValPos::clone()
{
  ValPos* newVP = new ValPos(*this);
  return newVP;
}
*/
ValPos::~ValPos()
{
  if (value && type != STRINGTYPE)
    delete[] value;
}
/*
void ValPos::set(byte* value_) {
  switch (type) {
  case NOTYPE: 
    throw new UnexpectedException("Type must be defined");
    break;
  case INTTYPE:
    value = new byte[sizeof(int)];
    memcpy(value, value_, sizeof(int));
    break;
  case FLOATTYPE:
    value = new byte[sizeof(float)];
    memcpy(value, value_, sizeof(float));
    break;
  case STRINGTYPE:
    value=value_;
    break;
  case LONGTYPE:
    value = new byte[sizeof(long long)];
    memcpy(value, value_, sizeof(long long));
    break;
  default:
    throw new UnexpectedException("Unknown type.");
    break;
  } 
}

void ValPos::set(unsigned int pos_, byte* value_) {
  position=pos_;
  switch (type) {
  case NOTYPE: 
    throw new UnexpectedException("Type must be defined");
    break;
  case INTTYPE:
    value = new byte[sizeof(int)];
    memcpy(value, value_, sizeof(int));
    break;
  case FLOATTYPE:
    value = new byte[sizeof(float)];
    memcpy(value, value_, sizeof(float));
    break;
  case STRINGTYPE:
    value=value_;
    break;
  case LONGTYPE:
    value = new byte[sizeof(long long)];
    memcpy(value, value_, sizeof(long long));
    break;
  default:
    throw new UnexpectedException("Unknown type.");
    break;
  } 
}

void ValPos::set(unsigned int type_, unsigned int pos_, byte* value_) {
  type = type_;
  position=pos_;
  switch (type) {
  case NOTYPE: 
    throw new UnexpectedException("Type must be defined");
    break;
  case INTTYPE:
    value = new byte[sizeof(int)];
    memcpy(value, value_, sizeof(int));
    break;
  case FLOATTYPE:
    value = new byte[sizeof(float)];
    memcpy(value, value_, sizeof(float));
    break;
  case STRINGTYPE:
    value=value_;
    break;
  case LONGTYPE:
    value = new byte[sizeof(long long)];
    memcpy(value, value_, sizeof(long long));
    break;
  default:
    throw new UnexpectedException("Unknown type.");
    break;
  } 
}

void ValPos::printVal(std::ostream* o) {
  throw new UnimplementedException("Not done yet; maybe don't need to.");
}

//generic function overloads
bool ValPos::operator> (ValPos* vp) {
  throw new UnimplementedException("Not done yet; maybe don't need to.");
}

bool ValPos::operator>= (ValPos* vp) {
  throw new UnimplementedException("Not done yet; maybe don't need to.");
}

bool ValPos::operator== (ValPos* vp) {
  throw new UnimplementedException("Not done yet; maybe don't need to.");
}

bool ValPos::operator< (ValPos* vp) {
  throw new UnimplementedException("Not done yet; maybe don't need to.");
}

bool ValPos::operator<= (ValPos* vp) {
  throw new UnimplementedException("Not done yet; maybe don't need to.");
}

bool ValPos::operator!= (ValPos* vp) {
  throw new UnimplementedException("Not done yet; maybe don't need to.");
}

//specialized function overloads
bool ValPos::operator> (int vp) {
  throw new UnimplementedException("Not done yet; maybe don't need to.");
}

bool ValPos::operator>= (int vp) {
  throw new UnimplementedException("Not done yet; maybe don't need to.");
}

bool ValPos::operator== (int vp) {
  throw new UnimplementedException("Not done yet; maybe don't need to.");
}

bool ValPos::operator< (int vp) {
  throw new UnimplementedException("Not done yet; maybe don't need to.");
}

bool ValPos::operator<= (int vp) {
  throw new UnimplementedException("Not done yet; maybe don't need to.");
}

bool ValPos::operator!= (int vp) {
  throw new UnimplementedException("Not done yet; maybe don't need to.");
}
*/
