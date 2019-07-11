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

#include "ShortCutDictDelayedDecoder.h"

ShortCutDictDelayedDecoder::ShortCutDictDelayedDecoder(bool valSorted_) : IntDecoder(valSorted_) {
}

ShortCutDictDelayedDecoder::ShortCutDictDelayedDecoder(byte* buffer_, bool valSorted_) : IntDecoder(buffer_, valSorted_) {
  throw UnimplementedException("Not sure if I need the line below or not yet");
  setBuffer(buffer_);
}

ShortCutDictDelayedDecoder::ShortCutDictDelayedDecoder(ShortCutDictDelayedDecoder& decoder_) : IntDecoder(decoder_) { 
	fieldSizePtr=decoder_.fieldSizePtr;
} 

ShortCutDictDelayedDecoder::~ShortCutDictDelayedDecoder()
{

}
	
void ShortCutDictDelayedDecoder::setBuffer(byte* buffer_) { 
  //cout << "I was called." << endl;
  Decoder::setBuffer(buffer_);
  if (buffer==NULL) throw new CodingException("ShortCutDictDelayedDecoder: Error, buffer passed in is NULL");
  fieldSizePtr=(unsigned int*) buffer_;
  int fieldSize = *fieldSizePtr;

  if (fieldSize >16 || fieldSize == 0)
    throw new UnexpectedException("fieldSize must be <= 16 bits");
  if (fieldSize <=4)
    fieldBytes = 1;
  else fieldBytes = 2;

  numvalsperint = (fieldBytes*8) / fieldSize;
  if (fieldBytes == 1)
    numvalsperint*=4;
  else
    numvalsperint*=2;

  startPosPtr=(unsigned int*) (buffer_+sizeof(int));
  numIntsPtr=(unsigned int*) (buffer_+(2*sizeof(int)));
  int temp = (((*numIntsPtr)%numvalsperint) == 0)?0:1;
  realsize = (((*numIntsPtr)/numvalsperint) + temp);
  numIntsPtr=&realsize;
  currPos=*startPosPtr;
  Log::writeToLog("ShortCutDictDelayedDecoder", 1, "setBuffer(): buffer has numInts=", *numIntsPtr);
  Log::writeToLog("ShortCutDictDelayedDecoder", 1, "setBuffer(): buffer has startPos=", *startPosPtr);
  
  reader.setBuffer(buffer_+3*sizeof(int), getPageLengthBits()-8*3*sizeof(int));
}

int ShortCutDictDelayedDecoder::getPageLengthBits() { 
	Log::writeToLog("IntDecoder", 0, "getPageLength returning leng=", 8*(2+((unsigned int) *numIntsPtr))*sizeof(int));
	return 8*(3+((unsigned int) *numIntsPtr))*sizeof(int);
}

