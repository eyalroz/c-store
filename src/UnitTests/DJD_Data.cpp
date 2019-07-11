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

#include "DJD_Data.h"

DJD_Data::DJD_Data()
{
}

DJD_Data::~DJD_Data()
{
}

/* args isn't used here */
bool DJD_Data::run(Globals* g, const vector<string>& args) {

  bool success=true;
  /*
  string path=string(D1_COLUMN)+".suppkey";
  cout << " generate D1 suppkey INT..."<< endl << endl;
  generate_int_column(path, 1, "D1_l_suppkey_INT", 1, false);
  cout << " generate D1 suppkey RLE..."<< endl << endl;
  generate_rle_column(path, 1, "D1_l_suppkey_RLE", 1, false, (short)20, (unsigned int)16);


  string path2=string(D1_COLUMN)+".orderkey";
  cout << " generate D1 orderkey INT..."<< endl << endl;
  generate_int_column(path2, 1, "D1_l_orderkey_INT", 1, false);
  cout << " generate D1 orderkey RLE..."<< endl << endl;
  generate_rle_column(path2, 1, "D1_l_orderkey_RLE", 1, false, (short)28, (unsigned int)16);

  string path3=string(D1_COLUMN)+".partkey";
  cout << " generate D1 partkey INT..."<< endl << endl;
  generate_int_column(path3, 1, "D1_l_partkey_INT", 1, false);
  cout << " generate D1 partkey RLE..."<< endl << endl;
  generate_rle_column(path3, 1, "D1_l_partkey_RLE", 1, false, (short)24, (unsigned int)16);

  string path4=string(D1_COLUMN)+".linenumber";
  cout << " generate D1 linenumber INT..."<< endl << endl;
  generate_int_column(path4, 1, "D1_l_linenumber_INT", 1, false);
  cout << " generate D1 linenumber RLE..."<< endl << endl;
  generate_rle_column(path4, 1, "D1_l_linenumber_RLE", 1, false, (short)8, (unsigned int)16);

  string path5=string(D1_COLUMN)+".quantity";
  cout << " generate D1 quantity INT..."<< endl << endl;
  generate_int_column(path5, 1, "D1_l_quantity_INT", 1, false);
  cout << " generate D1 quantity RLE..."<< endl << endl;
  generate_rle_column(path5, 1, "D1_l_quantity_RLE", 1, false, (short)8, (unsigned int)16);

  string path6=string(D1_COLUMN)+".extendedprice";
  cout << " generate D1 extendedprice INT..."<< endl << endl;
  generate_int_column(path6, 1, "D1_l_extendedprice_INT", 1, false);
  cout << " generate D1 extendedprice RLE..."<< endl << endl;
  generate_rle_column(path6, 1, "D1_l_extendedprice_RLE", 1, false, (short)24, (unsigned int)16);

  string path7=string(D1_COLUMN)+".returnflag";
  cout << " generate D1 returnflag INT..."<< endl << endl;
  generate_int_column(path7, 1, "D1_l_returnflag_INT", 1, false);
  cout << " generate D1 returnflag RLE..."<< endl << endl;
  generate_rle_column(path7, 1, "D1_l_returnflag_RLE", 1, false, (short)8, (unsigned int) 32);

  cout << " returning success... " << endl;
  */
  return success;
}

bool DJD_Data::generate_int_column(string path, int columnIndex, string outputName, int numIndices, bool sorted)
{
  bool success=true;
  ColumnExtracter* ce = new ColumnExtracter(path,              // filename
                                              columnIndex,                 // column index
                                              true); // force rebuild flag
                        
  IntEncoder* encoder = new IntEncoder((Operator*) ce, // data source
                                          0,              // column index
                                          8*PAGE_SIZE);   // buffer size in bits
										 
	IntDecoder* decoder = new IntDecoder(false);         // value sorted
	PagePlacer* pagePlacer = new PagePlacer(encoder,
                                            decoder, 
                                            numIndices,       // num indexes
                                            true);  // position primary ???

  pagePlacer->placeColumn(outputName, // name
                            false,                   // split on value ???
                            sorted);                  // value sorted

  delete pagePlacer;
  delete ce;
  delete encoder;
  delete decoder;
  return success;
}

bool DJD_Data::generate_rle_column(string path, int columnIndex, string outputName, int numIndices, bool sorted, short valueSize, unsigned int repSize)
{
	bool success = true;
	
	ColumnExtracter* ce = new ColumnExtracter(path,              // filename
                                              columnIndex,       // column index
                                              true);             // force rebuild flag
    
	RLEEncoder* encoder = new RLEEncoder((Operator*) ce,         // data source
                                         0,                      // column index
                                         8*PAGE_SIZE,            // buffer size in bits
                                         (byte) INT_VALUE_TYPE,  // value type
                                         (short) valueSize,      // value size
                                         (unsigned int) 27,      // start pos size
                                         (unsigned int) repSize);     // reps size

  RLEDecoder* decoder = new RLEDecoder(true);      // value sorted
  PagePlacer* pagePlacer = new PagePlacer(encoder,
                                          decoder,
                                          numIndices,       // num indexes
                                          false);  // position primary ???
	 
  pagePlacer->placeColumn(outputName, // name
                          false,          // split on value
                          sorted);          // value sorted
  delete pagePlacer;
  delete ce;
  delete encoder;
  delete decoder;

  return success;	
}
