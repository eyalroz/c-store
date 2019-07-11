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
#include "BitReaderWriter.h"

BitReaderWriter::BitReaderWriter()
{}
BitReaderWriter::~BitReaderWriter()
{}

bool BitReaderWriter::test(bool funcOut, int result, int expected) {
	using namespace std;
	if (funcOut) {
		cout << "X: " << result << "\tE[X]: " << expected << " \t";
		if (result==expected) {
			cout << "SUCCESS" << endl;
			return true;
		}
		else {
			cout << "FAILED" << endl;
			return false;
		}
	}
	else {
		cout << "FAILED (function return failed)" <<  endl;
		return false;
	}
}

/*args param isn't really used, just keep the interface consistance*/
bool BitReaderWriter::run(Globals *g, const vector<string>& args) {
	using namespace std;
	bool success=true;

	cout << "*** BitWriter/Reader Exhaustive Cross Test ***" << endl;
	
	BitWriter* writer=new BitWriter(124300);
	BitReader* reader=new BitReader(writer->getBuffer(), writer->getNumBitsWritten());	
	
	success&=randomTest(reader, writer);
	writer->resetBuffer();
	success&=randomTest(reader, writer);		
	return success;
}


bool BitReaderWriter::randomTest(BitReader* reader, BitWriter* writer) {
	bool bitWriterSuccess=true;
	
	
	unsigned int nums[100000];
	unsigned int bits[100000];
	unsigned int count=0;
	unsigned int i, j;
	i=5886;
	j=14;
	
	nums[0]=i;
	bits[0]=j;
	

	while (writer->writeBits(i,j)) {
		j=((unsigned int) rand())%32+1;
		i=((unsigned int) rand());
		while (i>(0xFFFFFFFF>>(32-j))) {
			i=((unsigned int) rand())&0xFFFFFF;
		}

		count++;
		nums[count]=i;
		bits[count]=j;
	}
	reader->setBuffer(writer->getBuffer(), writer->getNumBitsWritten());	
	for (unsigned int i=0; i<count; i++) {
		unsigned int value;
		if (!reader->readBits(value, bits[i])) {
			cout << "FAILED, premature ending to read" << endl;
			bitWriterSuccess=false;
		}
		else if (nums[i]!=value) {
			cout << "FAILED, mismatch nums[i]=" << nums[i] << " val=" << value << endl;
			bitWriterSuccess=false;
		}
		
		// else cout << "Success on i=" << i << "\tnums[i]=" << nums[i] << "\t val=" << value << endl;
	}	
	if 	(bitWriterSuccess) {
		cout << "SUCCESS: #Coded ints is: " << count << " NumBits Written: " << writer->getNumBitsWritten() << endl;
	}
	return bitWriterSuccess;	
}
