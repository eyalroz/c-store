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
#include "BitProfileTest.h"

BitProfileTest::BitProfileTest()
{
}

BitProfileTest::~BitProfileTest()
{
}


bool BitProfileTest::run(Globals* g, const vector<string>& args) {
	ofstream writerStream;
	writerStream.open("WriterProfile.csv");
	for (int i=0; i<=32; i++) {
		writerStream << " " << i << ",";
	}
	writerStream << endl;

	//bitWriterTest(writerStream);
	//bitWriterTest(writerStream);
	//bitWriterTest(writerStream);

	//byteWriterTest(writerStream);
	//byteWriterTest(writerStream);
	//byteWriterTest(writerStream);
	//intWriterTest(writerStream);
	//intWriterTest(writerStream);
	//intWriterTest(writerStream);
	ofstream readerStream;
	readerStream.open("ReaderProfile.csv");
	byte* bfr=new byte[20000*PAGE_SIZE];
	memset(bfr,2,20000*PAGE_SIZE);
	bitReaderTest(readerStream, bfr, 8 * 20000 * std::size_t(PAGE_SIZE));
	byteReaderTest(readerStream, bfr, 8 * 20000 * std::size_t(PAGE_SIZE));
	intReaderTest(readerStream, bfr, 8 * 20000 * std::size_t(PAGE_SIZE));
	return true;
}

void BitProfileTest::byteReaderTest(ofstream& readerStream, byte* bfr, std::size_t numBitsInBfr) {
	StopWatch clock;	
	double time;		
	ByteReader* reader=new ByteReader(bfr, numBitsInBfr);
	unsigned int value;
	for (int numBits=1; numBits<=32; numBits++) {
		if (numBits==8) {
			clock.start();
			while (reader->readByte((byte&) value));
			time=clock.stop();
			readerStream << time;
			reader->resetPos();		
		}
		else if (numBits==16) {
			clock.start();
			while (reader->readShort((unsigned short&) value));
			time=clock.stop();
			readerStream << time;
			reader->resetPos();		
		}
		else if (numBits==24) {
			clock.start();
			while (reader->readThree(value));
			time=clock.stop();
			readerStream << time;
			reader->resetPos();		
		}
		else if (numBits==32) {
			clock.start();
			while (reader->readInt(value));
			time=clock.stop();
			readerStream << time;
			reader->resetPos();			
		}
		readerStream << ", ";
	}
	readerStream << endl;		
}


void BitProfileTest::intReaderTest(ofstream& readerStream, byte* bfr, std::size_t numBitsInBfr) {
	double time;
	StopWatch clock;			
	IntReader* reader=new IntReader(bfr, numBitsInBfr);
	int value;
	for (int numBits=1; numBits<=32; numBits++) {
		if (numBits==32) {
			clock.start();
			while (reader->readInt(value));
			time=clock.stop();
			readerStream << time;
			reader->resetPos();			
		}
		readerStream << ", ";
	}
	readerStream << endl;		
}

void BitProfileTest::bitReaderTest(ofstream& readerStream, byte* bfr, std::size_t numBitsInBfr) {
	double time;
	StopWatch clock;			
	BitReader* reader=new BitReader(bfr, numBitsInBfr);
	unsigned int value;
	for (int numBits=1; numBits<=32; numBits++) {
		clock.start();
		while (reader->readBits(value, numBits));
		time=clock.stop();
		readerStream << time;
		reader->resetPos();		
		readerStream << ", ";
	}
	readerStream << endl;		
}

void BitProfileTest::byteWriterTest(ofstream& writerStream) { 
	StopWatch clock;			
	ByteWriter* writer=new ByteWriter(320000001); // max 20001 values
	double time;
	writerStream << "Byte, ";
	for (int numBits=1; numBits<=32; numBits++) {
		if (numBits==8) {
			clock.start();
			for (int val=0; val<10000000; val++) {
				writer->writeByte(1);
			} 
			time=clock.stop();
			writerStream << time;
			writer->resetBuffer();		
		}
		else if (numBits==16) {
			clock.start();
			for (int val=0; val<10000000; val++) {
				writer->writeShort(1);
			} 
			time=clock.stop();
			writerStream << time;
			writer->resetBuffer();		
		}
		else if (numBits==24) {
			clock.start();
			for (int val=0; val<10000000; val++) {
				writer->writeThree(1);
			} 
			time=clock.stop();
			writerStream << time;
			writer->resetBuffer();		
		}
		else if (numBits==32) {
			clock.start();
			for (int val=0; val<10000000; val++) {
				writer->writeInt(1);
			} 
			time=clock.stop();
			writerStream << time;
			writer->resetBuffer();			
		}
		writerStream << ", ";
	}
	writerStream << endl;	
	delete writer;	
}
void BitProfileTest::intWriterTest(ofstream& writerStream) {
	StopWatch clock;			
	IntWriter* writer=new IntWriter(320000001); // max 20001 values
	double time;
	writerStream << "Int, ";
	for (int numBits=1; numBits<=32; numBits++) {
		if (numBits==32) {
			clock.start();
			for (int val=0; val<10000000; val++) {
				writer->writeInt(1);
			} 
			time=clock.stop();
			writerStream << time;
			writer->resetBuffer();			
		}
		writerStream << ", ";
	}
	writerStream << endl;	
	delete writer;	
}


void BitProfileTest::bitWriterTest(ofstream& writerStream) {
	StopWatch clock;			
	BitWriter* writer=new BitWriter(320000001); // max 20001 values
	double time;
	writerStream << "Bit, ";
	for (int numBits=1; numBits<=32; numBits++) {
		clock.start();
		for (int val=0; val<10000000; val++) {
			writer->writeBits(1, numBits);
		} 
		time=clock.stop();
		writerStream << time << ", ";
		writer->clearBuffer();
	}
	writerStream << endl;	
	delete writer;	
}
