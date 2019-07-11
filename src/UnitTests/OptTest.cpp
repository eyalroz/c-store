/* Test File for Opt_Plan/
 * 
 * Email questions to {nga, hoangt}@cs.brandeis.edu
 *
 */

#include "OptTest.h"

OptTest::OptTest() {}

OptTest::~OptTest() {}

bool OptTest::run(Globals* g, const vector<string>& args) {
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
      
      char FactTables[2000]; FactTables[0] = '\0';
      char *ptrFTable;
      char QueryName[256]; QueryName[0] = '\0';
      list<char*> FList;
      
      cout << "Enter a list of fact tables: ";
      cin.getline(FactTables, 2000);
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

      // Decompse the original
      decompose(ptrQueryGraph, FList, QueryName);

      // Get replica if any
      Opt_QueryGraph* ptrReplica = ptrQueryGraph->getReplica();
      if (ptrReplica != NULL)
	{
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

	  /*
	  list<char*>::iterator factIt;
	  cout << "New fact tables:\n";
	  for (factIt = FList.begin(); factIt != FList.end(); factIt++)
	    {
	      cout << (*factIt) << ",";
	    }
	  cout << "\n";
	  */

	  // decompose the replica	
	  strcat(QueryName, "R");
          decompose(ptrReplica, FList, QueryName);
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

// Decompose a given query
void OptTest::decompose(Opt_QueryGraph* ptrQueryGraph, list<char*> factTables, char queryOutputName[256])
{
     if (factTables.size() >= 1) {

	/* List of fact tables will be from 2 */

	for (unsigned int i = 1; i <= factTables.size(); i++) {
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
	  unsigned int t = 1;
	  for (list<char*>::iterator iter = factTables.begin(); t <= i; t++, iter++) {
	    (ptrQueryGraph)->addFactTable(*iter);
	    //cout << *iter << ",";
	  }
	  //cout <<endl;

	  /* Set the fact tables for this given Query Graph */
	  (ptrQueryGraph)->setFactTable();

	  /* SnowFlake Query data structure is created for every i */
	  Opt_SnowFlakes* sf = new Opt_SnowFlakes(ptrQueryGraph);
	  assert(sf);
	  
	  sf->produceSnowFlakeQueries();
	  sf->toSnowflakeSQLsOracle(thisQueryName);

	  delete sf;
	}
      }
}
