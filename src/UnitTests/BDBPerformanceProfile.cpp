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
#include "BDBPerformanceProfile.h"

BDBPerformanceProfile::BDBPerformanceProfile()
{
}

BDBPerformanceProfile::~BDBPerformanceProfile()
{
}
bool BDBPerformanceProfile::run(Globals* g, const vector<string>& args) {
  //system("rm BDBProfileTes*");	
	const int numPages=15000;
	byte* page=new byte[PAGE_SIZE];
	memset(page, 2, PAGE_SIZE);
	
	PageWriter* p = new PageWriter(2, sizeof(int), sizeof(int), ValPos::INTTYPE, ValPos::INTTYPE);
	p->initDB("BDBProfileTest", false);
	for (int i=0; i<numPages; i++) {
	  *((int*) page)=i;
	  p->placePage((char*) page, (char*)&i, (char*)&i);
	}
	cout << endl;
 	p->closeDB();
	system("mv BDBProfileTest* " RUNTIME_DATA);
	system("Data/flushCache");
	


	ROSAM* am=new ROSAM("BDBProfileTest", 2);
	int* order=new int[numPages];

	am->initCursors();
	StopWatch clock1;
	clock1.start();
	for (int i=0; i<numPages; i++) {
		int* page=(int*) am->getNextPage();
		if (page==NULL) cout << "Error, page gotten from BDB NULL" << endl;
		if (*page!=i) cout << "Error, page gotten from BDB invalid" << endl;
	}	
	cout << endl;
	cout << "Sequential scan of " << numPages << " pages took: " << clock1.stop() << " ms" << endl;	
	delete am;


	am=new ROSAM("BDBProfileTest", 2);		
	am->initCursors();	
	StopWatch clock2;
	clock2.start();

	for (int i=1; i<numPages; i++) {
		int* page=(int*) am->skipToPagePrimary((char*)&i);	
		if (page==NULL) cout << "Error, page gotten from BDB NULL" << endl;
		if (*page!=i) cout << "Error, page gotten from BDB invalid" << endl;
	}
	cout << "Sequential scan of primary index with " << numPages << " pages took: " << clock2.stop() << " ms" << endl;		
	delete am;

	system("Data/flushCache");

	am=new ROSAM("BDBProfileTest", 2);				
	am->initCursors();	
	StopWatch clock3;
	clock3.start();

	for (int i=1; i<numPages; i++) {
		int* page=(int*) am->skipToPageSecondary((char*)&i);	
		if (page==NULL) cout << "Error, page gotten from BDB NULL" << endl;
		if (*page!=i) cout << "Error, page gotten from BDB invalid" << endl;
	}
	cout << "Sequential scan of secondary index with " << numPages << " pages took: " << clock3.stop() << " ms" << endl;	
	delete am;

	system("Data/flushCache");
	
	getRandomOrder(order, numPages);

	am=new ROSAM("BDBProfileTest", 2);				
	am->initCursors();	
	StopWatch clock4;
	clock4.start();

	for (int i=1; i<numPages; i++) {
		int* page=(int*) am->skipToPagePrimary((char*)&order[i]);	
		if (page==NULL) cout << "Error, page gotten from BDB NULL" << endl;
		if (*page!=order[i]) cout << "Error, page gotten from BDB invalid" << endl;
	}
	cout << "Random scan of " << numPages << " pages on primary index took: " << clock4.stop() << " ms" << endl;		
	delete am;

	system("Data/flushCache");


	//system("rm BDBProfileTes*");

	StopWatch clock5;
	am=new ROSAM("BDBProfileTest", 2);				
	am->initCursors();	
	clock5.start();

	for (int i=1; i<numPages; i++) {
		int* page=(int*) am->skipToPageSecondary((char*)&order[i]);	
		if (page==NULL) cout << "Error, page gotten from BDB NULL" << endl;
		if (*page!=order[i]) cout << "Error, page gotten from BDB invalid" << endl;
	}
	cout << "Random scan of  " << numPages << " pages on secondary index took: " << clock5.stop() << " ms" << endl;		
	delete am;

	system("Data/flushCache");
	//getRandomOrder(order, numPages);

	
	return true;
}

void BDBPerformanceProfile::getRandomOrder(int* order, int numPages_) {
	vector<int> valVec;
	for (int i=0; i<numPages_; i++)
		valVec.push_back(i);
	
	for (int i=0; i<numPages_; i++) {
		int pos=((unsigned int) rand())%valVec.size();
		vector<int>::iterator iter=valVec.begin();
		iter+=pos;
		order[i]=*iter;
		valVec.erase(iter);
	}
}
