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
#ifndef OPERATOR_H
#define OPERATOR_H

#include "../common/Constants.h"
#include "../common/SBlock.h"
#include "../common/UnimplementedException.h"

class PosBlock;
class Block;

// Abstract class for implementaion of an Operator
class Operator{
public:
	// destructor
	virtual ~Operator();	
	
	// Gets the next value block from the operator 
	virtual	Block* getNextValBlock(int colIndex_)=0;

	// Gets the value block from the operator that contains position pos 
	virtual Block* getNextValBlockAtPos(int colIndex_, unsigned int pos) {
	  throw new UnimplementedException("Not implemented yet"); //Not sure if I want to implement this generally yet, for now I need it for some code in Filter.cpp. Maybe it will become more general later.
	}

	// Gets the next string value block from the operator
	// THIS IS UGLY ...FIX LATER
	virtual	SBlock* getNextSValBlock(int colIndex_){return NULL;};	
	
	// Gets the next position block (bitstring of positions) from the operator
	virtual PosBlock* getNextPosBlock(int colIndex_)=0;
	
	// Gets the maximum position in the data source
	virtual int getLastPosition()=0;

	
protected:
	Operator();

};

#endif // OPERATOR_H
