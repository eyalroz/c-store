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
#include "BDBReadWrite.h"

BDBReadWrite::BDBReadWrite()
{}
BDBReadWrite::~BDBReadWrite()
{}


bool BDBReadWrite::run(Globals* g, const vector<string>& args) {
  using namespace std;
  cout << "*** Running BDBReadWriteTest ****" << endl;
  const void* x;
  bool ret = true;
  char * dna = new char[60];

  sprintf(dna, "Hey Dan!");
  char* toread;
  PageWriter* p = new PageWriter(1, sizeof(int), 0, ValPos::INTTYPE, ValPos::NOTYPE);
  p->initDB("dantheman", false);
  int temp = 2;
  p->placePage(dna, (char*)&temp);
  p->closeDB();
  system("mv dantheman* " RUNTIME_DATA);
  ROSAM* dtm = new ROSAM( "dantheman" , 1);
  dtm->initCursors();
  cout << "Last primary index key: " << *(int*)dtm->getLastIndexValuePrimary() << endl;
  try {
    cout << "Last secondary index key: " << *(int*)dtm->getLastIndexValueSecondary() << endl;
  } 
  catch( AccessException* ) {
    cout << "No secondary index excepton caught properly" << endl;
  }

  x = dtm->skipToPagePrimary((char*)&temp);
  //x = dtm->getNextPagePrimary();
  if (x == NULL)
    return false;
  toread = (char *)x;
  ret &= test(dna, toread);
  dtm->initCursors();
  x = dtm->getNextPagePrimary();
  toread = (char *)x;
  ret &= test(dna, toread);
  //system ("rm dantheman*");

  char * dna2 = new char[60];
  char * dna3 = new char[60];
  sprintf(dna, "Hey Dan2");
  sprintf(dna2, "Hey Dan, Miguel found bug");
  sprintf(dna3, "Hey Dan, It's all good.");
  p = new PageWriter(2, sizeof(int), sizeof(int), ValPos::INTTYPE, ValPos::INTTYPE);
  p->initDB("dantheman", false);
  temp = 2;
  int temp2 = 10000;
  p->placePage(dna, (char*)&temp, (char*)&temp2);
  temp = 3;
  temp2 = 10200;
  p->placePage(dna2, (char*)&temp, (char*)&temp2);
  temp = 4;
  temp2 = 10400;
  p->placePage(dna3, (char*)&temp, (char*)&temp2);
  p->closeDB();
  system("mv dantheman* " RUNTIME_DATA);
  dtm = new ROSAM( "dantheman" , 2);
  dtm->initCursors();
  cout << "Last primary index key: " << *(int*)dtm->getLastIndexValuePrimary() << endl;
  cout << "Last secondary index key: " << *(int*)dtm->getLastIndexValueSecondary() << endl;
  temp2 = 10200;
  x = dtm->skipToPageSecondary((char*)&temp2);
  x = dtm->getNextPageSecondary();
  //x = dtm->getNextPagePrimary();
  if (x == NULL)
    return false;
  toread = (char *)x;
  ret &= test(dna3, toread);
  dtm->initCursors();
  x = dtm->getNextPagePrimary();
  toread = (char *)x;
  ret &= test(dna, toread);
  delete[] dna;

  //system ("rm dantheman*");

  cout << "*** Finished running BDBReadWrite Test: ";
  if (ret) cout << "Success *****" << endl;
  else cout << "FAILED *****" << endl;

  return ret;
  
}


bool BDBReadWrite::test(char* x1, char* x2) {
	using namespace std;
	cout << "test " << x1 << " " << x2 << endl;
	if (strcmp(x1, x2) == 0)
	  return true;
	else {
	  cout << "failed " << x1 << " != " << x2 << endl;
	  return false;
	}	
}

