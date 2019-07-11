/* Test File for Opt_Plan/
 * 
 * Email questions to nga@cs.brandeis.edu
 *
 */

#include "OptPlanTest.h"

OptPlanTest::OptPlanTest() {}

OptPlanTest::~OptPlanTest() {}


bool OptPlanTest::run(Globals* g, const vector<string>& args) {
  CatalogInstance::getCatalog();

  while (1) {
    
    /* Initilize parser and objects */
    
    Parser* p = new Parser();
    PObject::initialize();
    
    /* Parsing a query */

    cout << "Enter a query: " << endl; 
    int result = p->doParse();

    if (result == 0) { /* If query parsed successfully */
      cout << "\nQuery parsed properly!" << endl;
            
      QSelect* ptrQuery = (QSelect*) (p->getQuery());

      Opt_QueryGraph* ptrQueryGraph = new Opt_QueryGraph(ptrQuery);

      // Get many to many joins
       while(1)
      {
	cout << "Enter many to many join: table,table (Return when done):" << endl;
	char manyToManyJoin[2000]; manyToManyJoin[0] = '\0'; 
	char delims[] = ",";
	char* token = NULL;
	
	cin.getline(manyToManyJoin, 2000);
	if (strlen(manyToManyJoin) == 0)
	  break;
	
	token = strtok(manyToManyJoin, delims);
	string sFromTableName(token);
	
	token = strtok(NULL, delims);
	string sToTableName(token);
       
	cout << "Many to Many: " << sFromTableName << " " << sToTableName << endl;

	ptrQueryGraph->addManyToManyJoin(sFromTableName, sToTableName);
	

      }

      // Get # row and cardinality
      while(1)
      {
	cout << "Enter table_name,number_of_rows,cardinality (Return when done)" << endl;
	char tableInfo[2000]; tableInfo[0] = '\0'; 
	char delims[] = ",";
	char* token = NULL;
	
	cin.getline(tableInfo, 2000);
	if (strlen(tableInfo) == 0)
	  break;
	
	token = strtok(tableInfo, delims);
	string sTableName(token);
	
	token = strtok(NULL, delims);
	long lRowNum = atol(token);
       
	token = strtok(NULL, delims);
	long lCardinality = atol(token);

	cout << "INFO: " << sTableName << " " << lRowNum << " " << lCardinality << endl;

	ptrQueryGraph->setTableInfo(sTableName, lRowNum, lCardinality);
	

      }


      
      char FactTables[2000]; FactTables[0] = '\0';
      char *ptrFTable;
      char QueryName[256]; QueryName[0] = '\0';
      list<char*> FList;
      
      cout << "Enter a list of fact tables: ";
      cin.getline(FactTables, 2000);

      cout << "Enter number of best plans you want: ";
      char sNumber[2000]; sNumber[0] = '\0';
      cin.getline(sNumber, 2000);
      unsigned int numPlan = atoi(sNumber);

      cout << "Enter output file name: ";
      cin.getline(QueryName, 256);

      StopWatch stopWatch;
      stopWatch.start();


      ptrFTable = strtok(FactTables, " .,;\n");
      while (ptrFTable != NULL) {
	char* _table;
	_table = (char*) malloc (sizeof(char) * 30);
	assert(_table);
	strcpy(_table, ptrFTable);
	FList.push_back(_table);
	ptrFTable = strtok(NULL, " .,;\n");
      }

      // decompose and make plan
      //getPlan(ptrQueryGraph, FList, QueryName);
      //Opt_Plan* plan = getBestPlan(ptrQueryGraph, FList, QueryName);
      list<Opt_Plan*> plans = getPlans(ptrQueryGraph, FList, QueryName);
      
      
      
      //Opt_Plan* replicaPlan = NULL;

      // Get replica if any
      //Opt_QueryGraph* ptrReplica = ptrQueryGraph->getReplica();
 
      list<Opt_Plan*> replicaPlans;
      /*
      if (ptrReplica != NULL)
	{
	  cout << endl << "SSSSSSSSSSSSSSSSSSSSS" << endl;      
	  cout << ptrReplica->getSQLQuery();      
	  cout << endl << "SSSSSSSSSSSSSSSSSSSSS" << endl;

	  //cout << "Replica: \n" << ptrReplica->getSQLQuery();
	  //cout << "------------\n";

  
	  // Get replica table names
	  list<ReplicaTable*> replicaTables = ptrReplica->getReplicaTables();

	  // Add these replica into fact table list, right before its's original
	  list<ReplicaTable*>::iterator replicaIt;
	  for (replicaIt = replicaTables.begin(); replicaIt != replicaTables.end(); replicaIt++)
	  {
	    char* replicaTableName = (char*) (*replicaIt)->m_sName.c_str();
	    int numReplica = (*replicaIt)->m_iNumReplica;

	    //cout << "Replica: " << replicaTableName << "\n";	    

	    // Insert this replica tables in the right fact position
	    list<char*>::iterator factIt;
	    for (factIt = FList.begin(); factIt != FList.end(); factIt++)
	    {
	      //cout << "         " << (*factIt) << "\n";

	      if (strcmp(replicaTableName, (*factIt)) == 0)
	      {
		for (int i = 0; i < numReplica; i++)
		{
		  char s[300];
		  s[0] = '\0';
		  strcpy(s,replicaTableName);
		  strcat(s,"_");
		  char num[10];
		  sprintf(num, "%d", i+1);
		  strcat(s,num);

		  char* _table;
		  _table = (char*) malloc (sizeof(char) * 30);
		  assert(_table);
		  strcpy(_table, s);

		   FList.insert(factIt, _table);
		}
		break;
	      }
	    }
	  }

	  
	  //list<char*>::iterator factIt;
	  //cout << "New fact tables:\n";
	 // for (factIt = FList.begin(); factIt != FList.end(); factIt++)
	  //  {
	   //   cout << (*factIt) << ",";
	//    }
	//  cout << "\n";
	  //

	  // decompose and get plans the replica	
	  //getPlan(ptrReplica, FList, QueryName);
          //replicaPlan = getBestPlan(ptrReplica, FList, QueryName);
	  replicaPlans = getPlans(ptrReplica, FList, QueryName);
	}
      
      
      //cout << endl<< "NON_REPLICA PLAN: " << endl;
      //plan->print("");

      //if (replicaPlan != NULL) 
	//{
	  //cout << endl<< "REPLICA PLAN: " << endl;
	//    replicaPlan->print("");  
	// }
	*/
      
      
      /*
	// BEST PLAN ONLY
      cout << endl << "BEST PREDICTED PLAN:" << endl;
      
      if ( (replicaPlan != NULL)  &&  (plan->getCost() > replicaPlan->getCost()) )
	{
	        replicaPlan->print("");
	        cout << "COST: " << replicaPlan->getCost() << endl;
	         cout << "SQL --------------" << endl;
	       cout << replicaPlan->getSQL();
	  
	  delete replicaPlan;
	}
      else
	{
	  plan->print("");
	   cout << "COST: " << plan->getCost() << endl;
	   cout << "SQL --------------" << endl;
	   cout << plan->getSQL();
	}

	cout << endl ;
	
	delete plan;
      */

      // n Best plan --> n text files
      plans.merge(replicaPlans);
      list<Opt_Plan*> sortedPlans = Opt_Plan::sortPlans(plans);
      if (numPlan > sortedPlans.size())
	{
	  numPlan =  sortedPlans.size();
	}

      list<Opt_Plan*>::iterator planIt = sortedPlans.begin();
      for (unsigned int i = 0; i < numPlan; i++)
	{
	  string sPlanFileName(QueryName);
	  sPlanFileName.append("_");
	  char num[10]; 
	  sprintf(num, "%d", i);
	  sPlanFileName.append(num);
	  sPlanFileName.append(".sql");
	  (*planIt)->toFile(sPlanFileName);
	  planIt++;
	}



      
      
      

      /* Memory de-allocation */

      for (list<char*>::iterator iter = FList.begin(); iter != FList.end(); iter++) {
	free (*iter);
      }
      delete ptrQueryGraph;

      cout << "[done]" << endl;
    
	    cout << "Running time: " << stopWatch.stop() << " ms" << endl;
    } 
  
    PObject::uninitialize();
    delete p;
  }
  
  return 1;

}

// Get plan for a given query
void OptPlanTest::getPlan(Opt_QueryGraph* ptrQueryGraph, list<char*> factTables, char queryOutputName[256])
{
     if (factTables.size() >= 1) {

	for (unsigned int i = 0; i < factTables.size(); i++) {
	  char thisQueryName[300]; thisQueryName[0] = '\0';
	  char num[10]; sprintf(num, "%d", i); /* Convert from int to char */

	  /* Formatting query's name */

	  strcpy(thisQueryName, queryOutputName);
	  strcat(thisQueryName, "_");
	  strcat(thisQueryName, num);
	  strcat(thisQueryName, "F");

	  /* The ptrQueryGraph is kept and only its list of fact tables is reset */
	  (ptrQueryGraph)->resetFactTable();

	  /* The first i table in the list of tables will fact tables */
	  // cout << "FACTS: ";
	  unsigned int t = 0;
	  for (list<char*>::iterator iter = factTables.begin(); t <= i; t++, iter++) {
	    (ptrQueryGraph)->addFactTable(*iter);
	    //cout << *iter << ",";
	  }
	  //cout <<endl;

	  /* Set the fact tables for this given Query Graph */
	  (ptrQueryGraph)->setFactTable();

	  cout << endl << "PLAN " << i+1 << " fact tables //////////////////////////" << endl << endl;

	  if (i == 0)
	  {
	    Opt_Plan* plan = new Opt_Plan();
	    plan->makePlan(ptrQueryGraph, ptrQueryGraph);
	    plan->print("   ");
	    delete plan;
	  }
	  else
	  {
	    /* SnowFlake Query data structure is created for every i */
	    Opt_SnowFlakes* sf = new Opt_SnowFlakes(ptrQueryGraph);
	    assert(sf);
	  
	    sf->produceSnowFlakeQueries();
	    list<SnowflakeBasedQuery*> sfb = sf->getSnowFlakeBasedQueryList();
	    list<SnowflakeBasedQuery*>::iterator sfbIt;
	    for (sfbIt = sfb.begin(); sfbIt != sfb.end(); sfbIt++)
	    {
	      list<Opt_QueryGraph*> queries = (*sfbIt)->mSnowFlakeQueryList;
	    
	      list<Opt_QueryGraph*>::iterator it;
	      for (it = queries.begin(); it != queries.end(); it++)
		{
		  cout << endl << "SUBPLAN "  << "---------------" << endl << endl;
		  Opt_Plan* plan = new Opt_Plan();
		  plan->makePlan(*it, ptrQueryGraph);
		  plan->print("   ");
		  delete plan;
		}
	      cout << "---------------------------" << endl;
	    }
	   
	    delete sf;
	  }

	}
      }
}


Opt_Plan*  OptPlanTest::getBestPlan(Opt_QueryGraph* ptrQueryGraph, list<char*> factTables, char queryOutputName[256])
{
  Opt_Plan* bestPlan = NULL;
  list<Opt_Plan*> plans;

  if (factTables.size() >= 1) {

	for (unsigned int i = 0; i < factTables.size(); i++) {
	  char thisQueryName[300]; thisQueryName[0] = '\0';
	  char num[10]; sprintf(num, "%d", i); /* Convert from int to char */

	  /* Formatting query's name */

	  strcpy(thisQueryName, queryOutputName);
	  strcat(thisQueryName, "_");
	  strcat(thisQueryName, num);
	  strcat(thisQueryName, "F");

	  /* The ptrQueryGraph is kept and only its list of fact tables is reset */
	  (ptrQueryGraph)->resetFactTable();

	  /* The first i table in the list of tables will fact tables */
	  // cout << "FACTS: ";
	  unsigned int t = 0;
	  for (list<char*>::iterator iter = factTables.begin(); t <= i; t++, iter++) {
	    (ptrQueryGraph)->addFactTable(*iter);
	    //cout << *iter << ",";
	  }
	  //cout <<endl;

	  /* Set the fact tables for this given Query Graph */
	  (ptrQueryGraph)->setFactTable();

	  //cout << endl << "PLAN " << i+1 << " fact tables //////////////////////////" << endl << endl;

	  if (i == 0)
	  {
	    Opt_Plan* plan = new Opt_Plan();
	    plan->makePlan(ptrQueryGraph, ptrQueryGraph);
	    // plan->print("   ");
	    plans.push_back(plan);
	  }
	  else
	  {
	    /* SnowFlake Query data structure is created for every i */
	    Opt_SnowFlakes* sf = new Opt_SnowFlakes(ptrQueryGraph);
	    assert(sf);
	  
	     plans.push_back(sf->getPlan());	    
	   }
	   
	  //delete sf;
	}

  }
  
  cout << "PLAN NUM: " << plans.size() << endl;
  if (plans.size() > 0)
    {
      bestPlan = plans.front();
      plans.pop_front();

      list<Opt_Plan*>::iterator it;
      for (it = plans.begin(); it != plans.end(); it++)
	{
	  long double bc = bestPlan->getCost();
	  long double c = (*it)->getCost();
	  if (bc > c)
	    bestPlan = *it;
	}
    }

  return bestPlan;
}


///////////
list<Opt_Plan*>  OptPlanTest::getPlans(Opt_QueryGraph* ptrQueryGraph, list<char*> factTables, char queryOutputName[256])
{
  list<Opt_Plan*> plans;

  if (factTables.size() >= 1) {

	for (unsigned int i = 0; i < factTables.size(); i++) {
	  char thisQueryName[300]; thisQueryName[0] = '\0';
	  char num[10]; sprintf(num, "%d", i); /* Convert from int to char */

	  /* Formatting query's name */

	  strcpy(thisQueryName, queryOutputName);
	  strcat(thisQueryName, "_");
	  strcat(thisQueryName, num);
	  strcat(thisQueryName, "F");

	  /* The ptrQueryGraph is kept and only its list of fact tables is reset */
	  (ptrQueryGraph)->resetFactTable();

	  /* The first i table in the list of tables will fact tables */
	  // cout << "FACTS: ";
	  unsigned int t = 0;
	  for (list<char*>::iterator iter = factTables.begin(); t <= i; t++, iter++) {
	    (ptrQueryGraph)->addFactTable(*iter);
	    //cout << *iter << ",";
	  }
	  //cout <<endl;

	  /* Set the fact tables for this given Query Graph */
	  (ptrQueryGraph)->setFactTable();

	  //cout << endl << "PLAN " << i+1 << " fact tables //////////////////////////" << endl << endl;

	  if (i == 0)
	  {
	    Opt_Plan* plan = new Opt_Plan();
	    plan->makePlan(ptrQueryGraph, ptrQueryGraph);
	    // plan->print("   ");
	    plans.push_back(plan);
	  }
	  else
	  {
	    /* SnowFlake Query data structure is created for every i */
	    Opt_SnowFlakes* sf = new Opt_SnowFlakes(ptrQueryGraph);
	    assert(sf);
	  
	    list<Opt_Plan*> sfPlans = sf->getPlans();
	    plans.merge(sfPlans);
	   }
	   
	  //delete sf;
	}

  }
  
  return plans;
}
