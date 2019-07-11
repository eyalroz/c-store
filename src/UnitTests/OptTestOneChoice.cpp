#include "OptTestOneChoice.h"

using namespace std;

OptTestOneChoice::OptTestOneChoice()
{
}

OptTestOneChoice::~OptTestOneChoice()
{
}

bool OptTestOneChoice::run(Globals* g, const vector<string>& args)
{

   CatalogInstance::getCatalog();
  while (1) {
    Parser* p = new Parser();
    PObject::initialize();
    
    cout << "Enter a query: " << endl; 
    int result = p->doParse();
    if (result == 0) {
      cout << "Query parsed properly" << endl;
      system("rm QueryX.out");
      
      QSelect* ptrQuery = (QSelect*) (p->getQuery());
      cout << "\n\nPrinting out ORIGINAL query from parser: \n";
      cout << ptrQuery->toString() << "\n\n\n";
      Opt_QueryGraph* ptrQueryGraph = new Opt_QueryGraph(ptrQuery);

      string sFactTableList = "";
      cout << "Enter the list of all fact tables separted by comma: ";
      getline(cin, sFactTableList);
      
      while (sFactTableList.find(",", 0) != string::npos) {
	string sNextTable = sFactTableList.substr(0, sFactTableList.find(",", 0));
	sFactTableList.erase(0, sFactTableList.find(",", 0)+1);
	ptrQueryGraph->addFactTable(sNextTable);
      }
      ptrQueryGraph->addFactTable(sFactTableList);
      ptrQueryGraph->setFactTable();
      
      cout << endl << "----------------------------------------------------------" << endl;
 
      cout << "INPUT: " << endl << endl;
      cout << ptrQueryGraph->getSQLQuery() << endl << endl;
      //cout << "FACT TABLES: " << endl;
  
  
      // PRODUCE SNOWFLAKES
  
      cout << "---------------------------------------------" << endl;
      cout << "OUTPUT: " << endl << endl;

      cout << "Enter output query name: " << endl;
      string sQueryName = "";
      cin >> sQueryName;

      StopWatch stopWatch;
      stopWatch.start();
  
      Opt_SnowFlakes* sf = new Opt_SnowFlakes(ptrQueryGraph);
      sf->produceSnowFlakeQueries();
      //cout << sf->toString();
           
      cout << sf->toSnowflakeSQLsOracle(sQueryName);

      cout << "Time to generate snowflake-based queries: " << stopWatch.stop() << " ms" << endl;
      
      delete ptrQueryGraph;
    }
    PObject::uninitialize();
    delete p;
  }
  
  return 1;
  /*

  // MAKE TEST DATA
  
  // Create tables
  Opt_Table* ptrLineItem = new Opt_Table("LineItem", "L");
  ptrLineItem->setRole(TABLE_ROLE_FACT);
  Opt_Table* ptrSupplier = new Opt_Table("Supplier", "S");
  Opt_Table* ptrCustomer = new Opt_Table("Customer", "C");
  Opt_Table* ptrNation = new Opt_Table("Nation", "N");
  Opt_Table* ptrRegion = new Opt_Table("Region", "R");
  Opt_Table* ptrOrders = new Opt_Table("Orders", "O");
  ptrOrders->setRole(TABLE_ROLE_FACT);
  
  // Region table
  Opt_String* ptrRegionNameValue = new  Opt_String("RR");
  Opt_Column* ptrRegionName = new Opt_Column("r_name", "Region");
  Opt_Comparison_Eq* ptrRegionNameComp = new Opt_Comparison_Eq(ptrRegionName, ptrRegionNameValue);
  ptrRegion->setNoneJoinPredicate(ptrRegionNameComp);
  
  // Nation Table
  Opt_Column* ptrNationName = new Opt_Column("n_name", "Nation");
  Opt_Column* ptrJoinNationRkey = new Opt_Column("n_regionkey", "Nation");
  Opt_Column* ptrJoinRegionkey = new Opt_Column("r_regionkey", "Region");
  Opt_Join* ptrJoinNR = new Opt_Join(ptrJoinNationRkey, ptrJoinRegionkey);
  ptrNation->addSelection(ptrNationName);
  ptrNation->addJoin(ptrJoinNR);
  
  // Suplier Table
  Opt_Column* ptrJoinSuppNkey = new Opt_Column("s_nationkey", "Supplier");
  Opt_Column* ptrJoinNationkey = new Opt_Column("n_nationkey", "Nation");
  Opt_Join* ptrJoinSN = new Opt_Join(ptrJoinSuppNkey, ptrJoinNationkey);
  ptrSupplier->addJoin(ptrJoinSN);
  
  // Customer Table
  Opt_Column* ptrJoinCusNkey = new Opt_Column("c_nationkey", "Customer");
  Opt_Column* ptrJoinSuppCusNkey = new Opt_Column("s_nationkey", "Supplier");
  Opt_Join* ptrJoinCS = new Opt_Join(ptrJoinCusNkey, ptrJoinSuppCusNkey);
  ptrCustomer->addJoin(ptrJoinCS);
  
  // Orders table
  Opt_String* ptrOrderDate1Value = new  Opt_String("D1");
  Opt_Column* ptrOrderDate1 = new Opt_Column("o_orderdate", "Orders");
  Opt_Comparison_Ge* ptrOrderDate1Comp = new Opt_Comparison_Ge(ptrOrderDate1, ptrOrderDate1Value);
  Opt_String* ptrOrderDate2Value = new  Opt_String("D2");
  Opt_Column* ptrOrderDate2 = new Opt_Column("o_orderdate", "Orders");
  Opt_Comparison_Lt* ptrOrderDate2Comp = new Opt_Comparison_Lt(ptrOrderDate2, ptrOrderDate2Value);
  Opt_Comparison_And* prtAndOrders = new Opt_Comparison_And(ptrOrderDate1Comp, ptrOrderDate2Comp);
  ptrOrders->setNoneJoinPredicate( prtAndOrders);
  
  Opt_Column* ptrJoinOrdersCkey = new Opt_Column("o_custkey", "Orders");
  Opt_Column* ptrJoinCustkey = new Opt_Column("c_custkey", "Customer");
  Opt_Join* ptrJoinOC = new Opt_Join(ptrJoinOrdersCkey, ptrJoinCustkey);
  ptrOrders->addJoin(ptrJoinOC);
  
  // LineItem table
  Opt_Agg_Sum* ptrSumPrice = new Opt_Agg_Sum("l_extendedprice", "LineItem");
  ptrLineItem->addAggregate(ptrSumPrice);
  
  Opt_Column* ptrJoinLOorderkey = new Opt_Column("l_orderkey", "LineItem");
  Opt_Column* ptrJoinOLorderkey = new Opt_Column("o_orderkey", "Orders");
  Opt_Join* ptrJoinLO = new Opt_Join(ptrJoinLOorderkey, ptrJoinOLorderkey);
  ptrLineItem->addJoin(ptrJoinLO);
  
  Opt_Column* ptrJoinLSsuppkey = new Opt_Column("l_suppkey", "LineItem");
  Opt_Column* ptrJoinSLsuppkey = new Opt_Column("s_suppkey", "Supplier");
  Opt_Join* ptrJoinLS = new Opt_Join(ptrJoinLSsuppkey, ptrJoinSLsuppkey);
  ptrLineItem->addJoin(ptrJoinLS);
  
  // List of table
  list<Opt_Table*> tableList;
  tableList.push_back(ptrNation);
  tableList.push_back(ptrLineItem);
  tableList.push_back(ptrSupplier);
  tableList.push_back(ptrCustomer);
  tableList.push_back(ptrRegion);
  tableList.push_back(ptrOrders);
  
  // Group by
  Opt_Column* ptrGroupNationName = new Opt_Column("n_name", "Nation");
  
  list<Opt_Column*> groupList;
  groupList.push_back(ptrGroupNationName);
  
  // Order by
  Opt_Agg_Sum* ptrOrderSumPrice = new Opt_Agg_Sum("l_extendedprice", "LineItem");
  
  list<Opt_Object*> orderList;
  orderList.push_back(ptrOrderSumPrice);

  

  // Create a query graph
  Opt_QueryGraph* ptrQueryGraph = new Opt_QueryGraph(tableList, groupList, orderList, NULL);
  
  cout << "INPUT: " << endl << endl;
  cout << ptrQueryGraph->getSQLQuery() << endl << endl;
  cout << "FACT TABLES: " << endl;
  cout << "LineItem, Orders" << endl << endl;
  
  
  // PRODUCE SNOWFLAKES
  
  cout << "---------------------------------------------" << endl;
  cout << "OUTPUT: " << endl << endl;
  
  Opt_SnowFlakes* sf = new Opt_SnowFlakes(ptrQueryGraph);
  sf->produceSnowFlakeQueries();
  cout << sf->toString();

  return 1;
  */

}
