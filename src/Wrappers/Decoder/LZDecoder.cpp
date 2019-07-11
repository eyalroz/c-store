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
// Decodes LZ pages
// Problems: dna@mit.edu

#include "LZDecoder.h"

LZDecoder::LZDecoder(bool valSorted_) : IntDecoder(valSorted_) {

}

LZDecoder::LZDecoder(LZDecoder& decoder_) : IntDecoder( decoder_) {

}
LZDecoder::~LZDecoder() {
	if (numIntsPtr!=NULL) delete numIntsPtr;
}

void LZDecoder::setBuffer(byte* buffer_) {
  //assume int type for now ... fix later
	if (buffer_==NULL) throw new CodingException("IntDecoder: Error, buffer passed in is NULL");
	numIntsPtr=new unsigned int;
	int sizeCompressed=*((unsigned int*) (buffer_));
	Log::writeToLog("LZDecoder", 1, "setBuffer(): buffer has sizeCompressed=", sizeCompressed);
	int numIntsPerPage=*((unsigned int*) (buffer_+sizeof(int)));
	*numIntsPtr=numIntsPerPage;
	Log::writeToLog("LZDecoder", 2, "setBuffer(): buffer has numIntsPerPage=", numIntsPerPage);
	startPosPtr=(unsigned int*) (buffer_+2*sizeof(int));
	Log::writeToLog("LZDecoder", 1, "setBuffer(): buffer has startPos=", *startPosPtr);
	
	byte* data=buffer_+3*sizeof(int);
	byte* decompressedBuffer = new byte[numIntsPerPage*sizeof(int)];
	// alright
	// for Dan pseudocode

	lzo_uint new_len = numIntsPerPage*sizeof(int);
	// decompressedBuffer=lzDecompress(data, sizeCompressed)
	lzo1x_decompress_safe(data, (*numIntsPtr) * sizeof(int),decompressedBuffer,&new_len,NULL);
	//lzo1x_decompress(b1,src_len,b2,&d,NULL);
	//lzo1x_1_compress(b1, src_len, b2, &dst_len, wrkmem.mem);

	*numIntsPtr=(new_len/sizeof(int));// write here the size of uncompressed data as number of ints
	Log::writeToLog("LZDecoder", 1, "setBuffer(): buffer has numInts=", *numIntsPtr);
	if (new_len==0) 
		throw new CodingException("SetBuffer: Error, size of decompressed page is 0");
	if ((new_len % (sizeof(int))) != 0) {
		Log::writeToLog("LZDecoder", 5, "setBuffer(): lzo error, new length%int!=0");
		throw new UnexpectedException("LZDecoder: setBuffer(): lzo error, new length%int!=");
	}

	Decoder::setBuffer(decompressedBuffer);		
	
	
	currPos=*startPosPtr;
	reader.setBuffer(decompressedBuffer, 8*(*numIntsPtr)*sizeof(int));

}

