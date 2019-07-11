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
#ifndef _DELTAENCODER_H_
#define _DELTAENCODER_H_

#include "Encoder.h"
#include "../Util/BitWriter.h"
#include "../../common/Interfaces.h"
#include "../CodingException.h"
#include "../CompDataStructs/DeltaHeader.h"
#include "../../Writers/BasicBlockWriter.h"


// Generic delta encoder, codes on value sequence, this is further specialized as 
// either DeltaPos or DeltaVal, or BS encoding
class DeltaEncoder : public Encoder
{
public:
	DeltaEncoder(Operator* dataSrc_, int colIndex_, unsigned int deltaSize_, unsigned int bfrSize_, int headerValue_);
	virtual ~DeltaEncoder();
	virtual byte* getPage();
	unsigned int getBufferSize();
	bool writeVal(int val_, unsigned int pos_);
	void setHeaderValue(int headerValue_);
protected:
	// Pull state
	bool isValLeftOver;
	int valLeftOver; 
	BasicBlockWriter* bbWriter;
	BasicBlock* block;

	// Push methods and state
	void reset(int val, unsigned int pos);
	byte* getBuffer();	

	bool init;
	bool done;
	int headerValue;
	unsigned int currVal;
	unsigned int startVal;
	bool full;
		
	unsigned int size;
	unsigned int escapeInt;
	unsigned int deltaSize;
	BitWriter* writer;
};

#endif //_DELTAENCODER_H_
