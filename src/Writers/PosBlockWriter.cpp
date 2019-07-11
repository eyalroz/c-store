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
#include "PosBlockWriter.h"

// Writes Position Bit Blocks
// Problems: migfer@mit.edu


PosBlockWriter::PosBlockWriter(Operator* blockSrc_, int colIndex_, unsigned int bfrSizeInBits_) : Writer(blockSrc_, colIndex_)
{
	init=true;
	isValRemaining=false;
	bbWriter=new BasicBlockWriter(blockSrc_, colIndex_);
	bfrSizeInBits=bfrSizeInBits_;
	writer=new BitWriter(bfrSizeInBits);
	writer->skipBits(3*8*sizeof(unsigned int));
	
	*((unsigned int*) writer->getBuffer())=1;
	startPosPtr=(unsigned int*) (writer->getBuffer()+sizeof(unsigned int));
	numValuesPtr=(unsigned int*) (writer->getBuffer()+2*sizeof(unsigned int));
	
	*startPosPtr=0;
	*numValuesPtr=0;
	outBlock=new PosBitBlock();
}


PosBlockWriter::~PosBlockWriter()
{
	delete writer;
	delete outBlock;
}


// Gets the next value block from the operator 
Block* PosBlockWriter::getNextValBlock(int colIndex_) {
	throw new UnexpectedException("Error: this is a position block writer");
}
	
// Gets the next position block (bitstring of positions) from the operator
PosBlock* PosBlockWriter::getNextPosBlock(int colIndex_) {
	if (colIndex_==0) {
		while (true) {
			if (init) {
				// initialize writer
				writer->clearBuffer();
				*((unsigned int*) writer->getBuffer())=1;
				*numValuesPtr=1;
				writer->skipBits(3*8*sizeof(unsigned int));

				if (isValRemaining) {
					oldPos=startPos; // previous startPos
					*startPosPtr=startPos;
					writer->writeBits(1,1);
					// cout << "ValRemain: Start Pos is: " << startPos << endl;
				}
				else {
					BasicBlock* block=(BasicBlock*) bbWriter->getNextValBlock(0);
					if (block==NULL)
						return NULL;
					else {
						writer->writeBits(1,1);
						
						startPos=block->getPosition();
						oldPos=startPos; // previous startPos
						*startPosPtr=startPos;
						// cout << "Start Pos is: " << startPos << endl;
					}
				}
				isValRemaining=false;
				init=false;				
			}
			else {			
				BasicBlock* block=(BasicBlock*) bbWriter->getNextValBlock(0);
				if (block==NULL) {
					init=true;
					isValRemaining=false;
					outBlock->setBuffer(writer->getBuffer(), bfrSizeInBits);
					// cout << "Num bits coded: " << outBlock->getSize() << endl;
					return outBlock;
				}
				else {
					unsigned int pos=block->getPosition();
					// cout << " Pos is: " << pos << "\t Writing: ";
					if (pos<=oldPos) throw new CodingException("Error: current position less than previous position");
					
					for (unsigned int i=1; i<(pos-oldPos);i++) {						
						if(!writer->writeBits(0,1)) {
							startPos=pos;
							isValRemaining=true;
							init=true;
							outBlock->setBuffer(writer->getBuffer(), bfrSizeInBits);
							// cout << endl << "Fail on 0: Num bits coded: " << outBlock->getSize() << endl;
							return outBlock;
						}
						// cout << "0";
						*numValuesPtr=*numValuesPtr+1;
					}
					
					
					if(!writer->writeBits(1,1)) {
						startPos=pos;
						isValRemaining=true;
						init=true;
						outBlock->setBuffer(writer->getBuffer(), bfrSizeInBits);
						// cout << endl << "Fail on 1: Num bits coded: " << outBlock->getSize() << endl;
						return outBlock;
					}
					else {
						// cout << "1" << endl;
						oldPos=pos;
						*numValuesPtr=*numValuesPtr+1;
					}
				}
			}
		}		
	}
	else {
		return NULL;	
	}
}
