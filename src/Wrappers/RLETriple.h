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
// RLE Triple
// Problems: migfer@mit.edu

#ifndef RLETRIPLE_H
#define RLETRIPLE_H
#include "CodingException.h"
#include "../common/ValPos.h"

class RLETriple {
	public:
		RLETriple() { value=NULL; startPos=0; reps=0;}
		RLETriple(ValPos* value_, unsigned int startPos_, unsigned int reps_) 
			: value(value_==NULL?NULL:value_->clone()), startPos(startPos_), reps(reps_) {
			if (reps==0)
				throw new CodingException("RLETriple: error, reps==0");
		}
		RLETriple(const RLETriple& trip_) {
			value=(trip_.value==NULL?NULL:trip_.value->clone());
			startPos=trip_.startPos;
			reps=trip_.reps;
		}
		virtual ~RLETriple(){delete value;};
		void setTriple(ValPos* val_, int pos_, int reps_) {
		  delete value;
			value=(val_==NULL?NULL:val_->clone());
			startPos=pos_;
			reps=reps_;
			if (reps==0)
				throw new CodingException("RLETriple: error, reps==0");
		}
		void setTriple(RLETriple* triple_) {
			if (triple_==NULL) 
				throw new CodingException("RLETriple: setTriple with NULL triple");
			else {
			  delete value;
				value=(triple_->value==NULL?NULL:triple_->value->clone());
				startPos=triple_->startPos;
				reps=triple_->reps;	
			}
		}

        void setNull() { value = NULL; startPos=0; reps=0;}

        bool isNull() {
        	return (value ==NULL && startPos==0 && reps ==0);
		}

		ValPos* value;
		unsigned int startPos;
		unsigned int reps;	
};
	
#endif // RLETRIPLE_H

