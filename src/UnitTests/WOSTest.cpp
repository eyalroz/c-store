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
/**
 * WOSTest:
 *
 */
#include "WOSTest.h"
    

WOSTest::WOSTest() {
}

WOSTest::~WOSTest() {
}

/* args is not used here */
bool WOSTest::run(Globals* g, const vector<string>& args) 
{
  //default test file (splitted from D3, has 3 columns)
  string path=D8_DATA_WOS;
  
  //Log::writeToLog("WOSTest", 0, "WOSTest is starting...");
  bool success=true;
  
  system("rm Segment1.wos.*");
     /*
  // WOS with 5 int columns
  ROSWOSSegment *rwseg = new ROSWOSSegment( "Segment1", 5 );

  char *tuple  = new char[ 6 * sizeof(int) ];  // 5 columns + timestamp
  for (int i = 0; i < 20; i ++ )
    {
      int x = i;
      memcpy( tuple, &(x), sizeof(int));  // storage key, first 4 bytes
  
      int ii = ((i+1) % 5);  // to make sort key different and repeating.
      memcpy( tuple + sizeof(int), &ii, sizeof(int) ); // sort key, col 1.
      
      cout << " Append storage key " << i << " SortKey " << ii << endl;
      rwseg->appendTuple( tuple );
    }
  */

   
   // change number of columns acordingly if you us other data file 
  WOSAM* wosam = new WOSAM(path, 2, 3, 1);
  int key = 8035;

  exit( 1 );

  // Please not that the boolean says that the iterator should be set
  // to that value. Following the first call, all calls should specify false.
  // second paramter is the field that you want.
  // 0 being the storage key.

  //a = rwseg->getNextFieldBySortKey( key, 0, true );
  const void* pageret = wosam->skipToPagePrimary( (char*)&key );
  IntDecoder* decoder;
  Block* b;
  while (pageret != NULL) {
    decoder=new IntDecoder((byte*)pageret, false);
    
    while ((b=decoder->getNextBlock())) {
      cout << b->getStartPair()->value << " " << b->getStartPair()->position <<endl;
    }  
     pageret = wosam->getNextPagePrimary(  );
  }

/*memcpy( &field, a, sizeof(int ) );
  cout << " Set sort key to 4, 0th field (storage key): " << field << endl;

  while ( ( a = rwseg->getNextFieldBySortKey( key, 0, false )) != NULL )
    {
      memcpy( &field, a, sizeof(int ) );
      memcpy( &nextField, a+sizeof(int), sizeof(int ) );
      cout << " Fieldd is:  " << field << " sort key " << nextField << endl;
      }*/
  
  cout << " Now iterate through secondary (position) index, same column " << endl;
  
  key = 8;

  pageret = wosam->skipToPageSecondary( (char*)&key );

/*a = rwseg->getNextFieldBySortKey( key, 0, true );
  memcpy( &field, a, sizeof(int ) );
  cout << " Set storage key to 2, 0th field (storage key): " << field << endl;

  while ( ( a = rwseg->getNextFieldBySortKey( key, 0, false )) != NULL )
    {
      memcpy( &field, a, sizeof(int ) );
      memcpy( &nextField, a+sizeof(int), sizeof(int ) );
      cout << " Fielld is:  " << field << " sort key " << nextField << endl;
      }*/

  while (pageret != NULL) {
    decoder=new IntDecoder((byte*)pageret, false);
    while ((b=decoder->getNextBlock())) {
      if (b->getStartPair())
	cout << b->getStartPair()->value << " " << b->getStartPair()->position <<endl;
    } 
    pageret = wosam->getNextPageSecondary(  );
  }
  
  /*
  cout << "Now do query, just for fun" << endl;

  Predicate* pred=new Predicate(Predicate::OP_GREATER_THAN);

  IntDataSource* ds1=new IntDataSource(wosam,true,false);
  ds1->setPredicate(pred);
  ds1->changeRHSBinding(2);
  // ds1->printColumn();
  
  Sum* agg = new Sum((Operator*) ds1, 0, (Operator*) ds1, 0);

  Operator* srcs[1]={agg};
  int numCols[1]={2};
 
  
  BlockPrinter* bPrint=new BlockPrinter(srcs, numCols, 1, "WOSTest.out");*/
  /*  
  WOSAM* wosam2 = new WOSAM("Segment2", 2, 5, 1);
  WOSAM* wosam3 = new WOSAM("Segment3", 2, 5, 1);

  Predicate* pred = new Predicate(Predicate::OP_EQUAL);
  IntDataSource* ds1 = new IntDataSource(wosam, 
					 true, // value sorted
					 false); // isROS
  ds1->setPredicate(pred);
  ds1->changeRHSBinding(2);
  // ds1->printColumn();

  BCopyOperator* bc = new BCopyOperator(ds1, 0, 2);

  IntDataSource* ds2 = new IntDataSource(wosam2, 
					 true, // value sorted
					 false); // isROS
  ds2->setPositionFilter(bc,0);
  IntDataSource* ds3 = new IntDataSource(wosam3, 
					 true, // value sorted
					 false); // isROS
  ds3->setPositionFilter(bc,1);

  Max* agg = new Max((Operator*) ds2, 0, (Operator*) ds3, 0);

  Operator* srcs[1]={agg};
  int numCols[1]={2};
  
  BlockPrinter* bPrint = new BlockPrinter(srcs,         // data sources
					  numCols,      // num columns per source
					  1,            // num sources
					  "WOSTest.out");// output filename
  bPrint->printColumns();
  */
  
  
  cout << " DONE " <<  endl;

  delete wosam;
  return success;	
}
