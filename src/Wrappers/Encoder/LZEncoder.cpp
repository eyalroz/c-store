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
// Encodes LZ Pages
// Problems: dna@mit.edu

#include "LZEncoder.h"

LZEncoder::LZEncoder(Operator* dataSrc_, int colIndex_, int numIntsPerPage_) : Encoder(dataSrc_, colIndex_)
{
	numIntsPerPage=numIntsPerPage_;
	page=new byte[PAGE_SIZE];
	memset(page, 0, PAGE_SIZE);
	buffer=new int[numIntsPerPage_];
	writer=new BasicBlockWriter(dataSrc_, colIndex_);
}

LZEncoder::~LZEncoder()
{
}
byte* LZEncoder::getPage() {
	int count=0;
	unsigned int startPos;
	memset(page, 0, PAGE_SIZE);
	memset(buffer, 0, numIntsPerPage*sizeof(int));
	BasicBlock* block=(BasicBlock*) writer->getNextValBlock(0);	
	if (block!=NULL) 
		startPos=block->getStartPair()->position;
	else 
		return NULL;

	//assume type int ... fix later (will be easy to fix)
	assert(block->getStartPair()->type == ValPos::INTTYPE);
	while ((count<numIntsPerPage) && (block!=NULL)) {
		buffer[count]=*(int*)block->getStartPair()->value; // basic block only one pair

		count++;
		if ((count) == numIntsPerPage)
		  break;
		block=(BasicBlock*) writer->getNextValBlock(0);
	}	
	
	byte* compressedData = new byte[numIntsPerPage*4 + numIntsPerPage*4 / 64 + 16 + 3];
	lzo_byte *wrkmem = NULL;
	lzo_uint32 wrk_len = 0;
	wrk_len = LZO1X_1_MEM_COMPRESS;
	wrkmem = (lzo_bytep) lzo_malloc(wrk_len);
	int r = 0;
	
	// alright dan: at this point you have
	//		1. A buffer, with a maximum size of numIntsPerPage (constructor argument)
	//		2. A count, with the number of ints in that buffer
		
	lzo_uint sizeCompressedData;		// write here the size of the compressed data (you will get this back in decoder)
	//cout << "sizein" << ((count)) << endl;
	r = lzo1x_1_compress((const unsigned char*)buffer, (count)*sizeof(int), compressedData, &sizeCompressedData, wrkmem);
	  if (r != LZO_E_OK) {
	    Log::writeToLog("LZEncoder", 5, "gePage() should be ok but not ..");
	    throw new UnexpectedException("LZEncoder: Dan's comment: should be ok but not ..");
	  }
	
	
	// Writing header for decoder to use
	*((int*) page)=sizeCompressedData;
	*((int*) (page+sizeof(int)))=numIntsPerPage;
	*((int*) (page+2*sizeof(int)))=startPos;
    Log::writeToLog("LZEncoder", 0, "getPage() returning page, sizeCompressed", sizeCompressedData);
    Log::writeToLog("LZEncoder", 0, "getPage() returning page, startPos", startPos);    

	if (sizeCompressedData>PAGE_SIZE-3*sizeof(int)) 
		throw CodingException("LZEncoder: Error, compressed data larger than what we can fit on page");
		
	memcpy(page+3*sizeof(int), compressedData, sizeCompressedData);
	delete[] compressedData;
	lzo_free(wrkmem);
	return page;
}
