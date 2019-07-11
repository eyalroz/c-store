/**
 * DJD Experiments
 *
 * D1 Projections: 
 * C1  =  (l_shipdate | l_shipdate, l_suppkey)
 * C2  =  (l_suppkey  | l_shipdate, l_suppkey)
 *
 */

  ROSAM* am1 = new ROSAM("D1_UNCOMPRESSED_SHIPDATE" ,  // table
  		   2);                                 // num indexes
  
  ROSAM* am2 = new ROSAM("D1_l_orderkey_INT",
  	   1);
  
  ROSAM* am3 = new ROSAM("D1_l_suppkey_INT",
        1);

  // ROSAM* am4 = new ROSAM("D1_l_partkey_INT",
  //        1);

  // ROSAM* am5 = new ROSAM("D1_l_linenumber_INT",
  //      1);

  // ROSAM* am6 = new ROSAM("D1_l_quantity_INT",
  //      1);

  // ROSAM* am7 = new ROSAM("D1_l_extendedprice_INT",
  //      1);

  // ROSAM* am8 = new ROSAM("D1_l_returnflag_INT",
  //      1);

  
  Predicate* pred = new Predicate(Predicate::OP_GREATER_THAN);
  IntDataSource* ds1 = new IntDataSource(am1,
  			 true, // value sorted
  			 true); // isROS
  ds1->setPredicate(pred);
  ds1->changeRHSBinding(1);
  //ds1->printColumn();

  BCopyOperator* bc_ds1 = new BCopyOperator(ds1, 0, 2);
  
  IntDataSource* ds2 = new IntDataSource(am2,
    false, // value sorted
    true); // isROS
  ds2->setPositionFilter(bc_ds1, 0);

  IntDataSource* ds3 = new IntDataSource(am3,
  	  false, // value sorted
  	  true); // isROS
  
  ds3->setPositionFilter(bc_ds1, 1);

  //Count* agg = new Count((Operator*) ds, 0);
  
  Operator* srcs[3]={ds1, ds2, ds3};
  int numCols[3]={1, 1, 1};
  
  BlockPrinter* bPrint = new BlockPrinter(srcs,         // data sources
            numCols,      // num columns per source
            3,            // num sources
//  #ifdef CSTORE_PAPI
//        "/dev/null"); // no output
//  #else
        "DJD_QueryAny.out");// output filename
//  #endif
  bPrint->printColumns();
  
  delete ds1; delete am1;
  delete ds2; delete am2;
  delete ds3; delete am3;
  //delete agg;
  delete bPrint;
  //delete ds;
  //delete am;

