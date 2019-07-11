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
 * DeleteOpTest:
 *
 */
#include "DeleteOpTest.h"
#include "stdlib.h"
    
DeleteOpTest::DeleteOpTest() {}

DeleteOpTest::~DeleteOpTest() {}

/* args is not used here */
bool DeleteOpTest::run(Globals* g, const vector<string>& args) 
{
  int success = true;
  StopWatch* stopWatch = new StopWatch();
  stopWatch->start();
      
  //DELETE FROM table_name
  //[ WHERE expression ]
 
  //example: 
  //DELETE FROM D7_DATA
  //WHERE orderdate > 9000
  //

  //run Query, generate before-deletion output  
      
  //predicate
  int PRED_FILTER = 9000;
  Predicate::PredicateType predicateType;
  predicateType = Predicate::OP_GREATER_THAN;
  
  //ROS data source
  ROSAM* amROS = new ROSAM("D7_DATA_ROS" , 3);
  Predicate* predROS=new Predicate(predicateType);
  RLEDataSource* dsROS=new RLEDataSource(amROS, true);
  dsROS->setPredicate(predROS);
  dsROS->changeRHSBinding(PRED_FILTER);
  
  //WOS data source 
  //ready to perform delete
  WOSManager* wosMgr = new WOSManager(D7_DATA_WOS,      // table name
                                      2,                // num indices
                                      3);               // num columns
   
  const int orderdateColumnIndex = 1;
  WOSAM* amWOS = wosMgr->getWOSAM(orderdateColumnIndex);    // field num
  
  IntDataSource *dsWOS = new IntDataSource(
      amWOS,
      true,              // value sorted
      false);            // isROS
  Predicate* predWOS=new Predicate(predicateType);
  dsWOS->setPredicate(predWOS);
  dsWOS->changeRHSBinding(PRED_FILTER);
  
  
  DeleteOperator* deleteOp = new DeleteOperator(wosMgr);
  deleteOp->deleteTuple((Operator*) dsROS, true);
  deleteOp->deleteTuple((Operator*) dsWOS, false);
      
  //run Query, generate after-deletion output
      
  cout << "DeleteOpTest took: " << stopWatch->stop() << " ms" <<  endl;
  delete stopWatch;

  return success;	
}

/*  int PRED_FILTER = 9000;
  //run Query, generate before-deletion output
  
  //delete vector column
  //DeleteOperator(WOSAM* wosamToDV);
  //DeleteOperator(ROSAM* rosamToDV);
  //bool markDeleted(unsigned int pos);  
  //bool markDeleted(Operator* positionStream);
  
  string tableDV=D6_DATA_MINI_DV;
  WOSAM* wosam = new WOSAM(tableDV, 2, 2, 1);
  DeleteOperator doWOS = new DeleteOperator(wosam);
  doWOS->markAsDeleted(dsWOS);
  
  ROSAM* rosam = new ROSAM(tableDV, 2);
  DeleteOperator doROS = new DeleteOperator(rosam);
  doWOS->markDeleted(dsROS);
  
  wosam->rwseg->writeTuplesToDisk();
*/
