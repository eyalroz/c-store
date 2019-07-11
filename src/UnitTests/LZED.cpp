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
#include "LZED.h"

LZED::LZED()
{
}

LZED::~LZED()
{
}

bool LZED::run(Globals* g, const vector<string>& args) {
	bool success=true;
	cout << "LZEDTest: Simple two value test" << endl;
	byte* data = new byte[100000];
	byte* data2 = new byte[100000];
	for (int i = 0; i < 100000; i++)
	  data[i] = 2;
	data[13] = 14;
	byte* compressedData = new byte[100000 + 100000 / 64 + 16 + 3];
	lzo_byte *wrkmem = NULL;
	lzo_uint32 wrk_len = 0;
	wrk_len = LZO1X_1_MEM_COMPRESS;
	wrkmem = (lzo_bytep) lzo_malloc(wrk_len);
	int r = 0;
	printf("%d %d %d\n", data[11], data[12], data[13]);
	lzo_uint sizeCompressedData;		// write here the size of the compressed data (you will get this back in decoder)
	r = lzo1x_1_compress((const unsigned char*)data, 100000, compressedData, &sizeCompressedData, wrkmem);
	lzo_uint new_len = 100000;
	cout << "Compressed data size is: " << sizeCompressedData << endl;
	lzo1x_decompress_safe(compressedData, sizeCompressedData, data2, &new_len,NULL);
	cout << "new_len is: " << new_len << endl;
	success &= test("data[11] test", true, data[11], data2[11]);
	success &= test("data[12] test", true, data[12], data2[12]);
	success &= test("data[13] test", true, data[13], data2[13]);
	if (r != LZO_E_OK) {
	  Log::writeToLog("LZEncoder", 5, "gePage() should be ok but not ..");
	  cout << "Failing test ..." << endl;
	  success=false;
	}
	return success;	
	
}
