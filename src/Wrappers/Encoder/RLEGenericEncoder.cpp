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
// Encodes RLE Pages

#include "RLEGenericEncoder.h"

RLEGenericEncoder::RLEGenericEncoder(Operator* dataSrc_, int colIndex_, int bfrSizeInBits_, unsigned short valueType_, unsigned short fieldSize_) : Encoder(dataSrc_, colIndex_) {
  writer=new StringWriter(fieldSize_,bfrSizeInBits_);
  rleWriter=new RLEWriter(dataSrc_, colIndex_);
  bfrSizeInBits=bfrSizeInBits_;
  bfrSizeInBytes=bfrSizeInBits_/8;
  valueType=valueType_;
  fieldSize=fieldSize_;
  tripleSize=fieldSize_+(2*sizeof(int));
  Log::writeToLog("RLEGenericEncoder", 0, "Created buffer with size[bits]=",bfrSizeInBits_);
  dataSrc=rleWriter;
  initPage();
  block=(RLEBlock*) dataSrc->getNextValBlock(0);
}

RLEGenericEncoder::~RLEGenericEncoder() {	
  delete writer;
  delete rleWriter;
}
	
int RLEGenericEncoder::getNumBitsWritten() {
  return *pageLengthPtr;
}

int RLEGenericEncoder::getBufferSizeBytes() {
  int bufferSizeBytes=bfrSizeInBits/8;
  if ((bfrSizeInBits%8)!=0) bufferSizeBytes++;
  return bufferSizeBytes;
}
void RLEGenericEncoder::initPage() {
  writer->resetBuffer();
  bool headerSuccess=true;
  buffer=writer->getBuffer();

  *((short *) buffer)=fieldSize;
  pageLengthPtr=(int*) (buffer+sizeof(short));

  //byte* data=((byte*) pageLengthPtr)+sizeof(int);
  //*data=startPosSize;
  //*(data+1)=repsSize;
  //headerSuccess&=writer->skipBits(8*(sizeof(short)+sizeof(int)+2*sizeof(byte)));		
  headerSuccess&=writer->skipToBytePos(sizeof(short)+sizeof(int));

  *pageLengthPtr=0;	
	
  if (!headerSuccess) {
    Log::writeToLog("RLEGenericEncoder", 3, "RLEGenericEncoder: failed in initializing header, in initPage");
    throw new CodingException("RLEGenericEncoder: failed to write header in initPage()");
  }
}
byte* RLEGenericEncoder::getPage() {
  Log::writeToLog("RLEGenericEncoder", 0, "Called getPage()");
  initPage();
  if (block==NULL) {
    Log::writeToLog("RLEGenericEncoder", 2, "RLEGenericEncoder: got a NULL block, returning NULL");
    return NULL;	
  }
  while (block!=NULL && ((writer->getNumBytesWritten()+tripleSize)<=bfrSizeInBytes)) {
    RLETriple* triple=block->getTriple();
    if (triple==NULL) {
      Log::writeToLog("RLEGenericEncoder", 3, "Error, got a block with a NULL triple");
      throw CodingException("RLEGenericEncode: Error, block has a NULL triple");
    }
    bool success=true;
    //Log::writeToLog("RLEGenericEncoder", 0, "Trying to write block value=",*(int*)triple->value->value);
    //success&=writer->writeBits(*(int*)triple->value->value, valueSize);
    success&=writer->writeString((char*)triple->value->value);
    //Log::writeToLog("RLEGenericEncoder", 0, "Trying to write start=",triple->startPos);
    //success&=writer->writeBits(triple->startPos, startPosSize);
    success&=writer->writeString((char*)&triple->startPos, sizeof(int));
    //Log::writeToLog("RLEGenericEncoder", 0, "Trying to write reps=",triple->reps);
    //success&=writer->writeBits(triple->reps, repsSize);
    success&=writer->writeString((char*)&triple->reps, sizeof(int));
    if (success) {
      *pageLengthPtr+=(tripleSize*8);
      Log::writeToLog("RLEGenericEncoder", 0, "Wrote Block");
      block=(RLEBlock*) dataSrc->getNextValBlock(colIndex);			
    }
    else {
      Log::writeToLog("RLEGenericEncoder", 0, "Failed in writing triple");
      Log::writeToLog("RLEGenericEncoder", 0, "failed when numbyteswritten", writer->getNumBytesWritten());			
      throw new CodingException("RLEGenericEncoder: failed to write triple in RLEGenericEncoder");
    }
  }
  //if ((writer->getNumBitsWritten()+tripleSize)>bfrSizeInBits)
  //  success&=writer->writeBits(0,bfrSizeInBits - writer->getNumBitsWritten());
  Log::writeToLog("RLEGenericEncoder", 1, "Returning page");
  return buffer;
}

