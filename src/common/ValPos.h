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
#ifndef VALPOS_H
#define VALPOS_H

#include "UnexpectedException.h"
#include "UnimplementedException.h"
#include "Constants.h"
#include <string.h>
#include <iostream>

class ValPos{
public:
  ValPos() : type(NOTYPE), position(0), value(NULL) {}
  //ValPos(unsigned int type_, unsigned int pos_, byte* value_);
  //ValPos(ValPos& valpos_);
  virtual ValPos* clone()=0;
  virtual ~ValPos();
  virtual void set(byte* value_)=0;
  virtual void set(unsigned int pos_, byte* value_)=0;
  virtual void set(unsigned int type_, unsigned int pos_, byte* value_)=0;
  virtual double getDouble() { throw new UnexpectedException("Only special instances of ValPos can implement this method");}
  virtual void printVal(std::ostream* o)=0;
  virtual unsigned short getSize()=0;
  unsigned int type;
  unsigned int position;
  byte* value;

  enum {
    NOTYPE = 0, 
    INTTYPE = 1, 
    FLOATTYPE = 2, 
    STRINGTYPE = 3, 
    LONGTYPE = 4,
    DOUBLETYPE = 5
  };

  //generic function overloads
  virtual bool operator > (ValPos*)=0;
  virtual bool operator < (ValPos*)=0;
  virtual bool operator >= (ValPos*)=0;
  virtual bool operator <= (ValPos*)=0;
  virtual bool operator == (ValPos*)=0;
  virtual bool operator != (ValPos*)=0;

  //specialized function overloads
  virtual bool operator > (int)=0;
  virtual bool operator < (int)=0;
  virtual bool operator >= (int)=0;
  virtual bool operator <= (int)=0;
  virtual bool operator == (int)=0;
  virtual bool operator != (int)=0;

};

#endif // VALPOS_H
