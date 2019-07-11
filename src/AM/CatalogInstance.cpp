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
#include <iostream>
#include "CatalogInstance.h"
#include "CatalogInstanceBDB.h"

const string CatalogInstance::PROJECTIONS[ /*25*/ ]
      = { "lineitem", "orders", "customers", "typetest", 
	  "Q1_projection", "Q2_projection", "Q3_projection", 
	  "Q4_projection", "Q5_projection", "Q6_projection", "Q7_projection",
	  "D1.data", "D1.shipdate", "D1.suppkey", "D2.data", "D3.data",
	  "D3.custkey", "D4.data", "D5.data", "D6.data", "D7.data",
	  "D9.data", "D8_projection", "D10_projection",
	  "MO_D6.data" };

const string CatalogInstance::TYPE_NAMES[ /*5*/ ]
      = { "", "INTEGER", "FLOAT", "STRING", "STAR" };

const string CatalogInstance::COMPRESSION_NAMES[ /*7*/ ]
      = { "", "COMPRESSION_TYPE1", "COMPRESSION_TYPE1A", 
	  "COMPRESSION_TYPE2", "COMPRESSION_TYPE3", 
	  "COMPRESSION_TYPE3A", "COMPRESSION_TYPE4" };

using namespace std;

CatalogInstance* CatalogInstance::catalog = NULL;
bool CatalogInstance::gottenCat = false;
DbEnv* CatalogInstance::dbEnv = NULL;
vector<AM*> *CatalogInstance::allocatedAMs = new vector<AM*>();
vector<Node*> *CatalogInstance::allocatedNodes = new vector<Node*>();

string MetaColumn::toString()
{
  string mcs = "";
  mcs += ("\tColumn: " + _name + ( isSorted() ? " is sorted " : " is NOT sorted" ) + "  compression: " + CatalogInstance::COMPRESSION_NAMES[ _compression ] + "  dataType: " + CatalogInstance::TYPE_NAMES[_dataType] + " indexes " + ( _numIndexes == 2 ? "2" : "1" ) + " fileName " + _fileName + " filenamWOS " + _fileNameWos + "\n");

  return mcs;
}

MetaColumn::MetaColumn( string colName, int dataType, int compType, 
			int colNum, bool isSK )
{
  _name = colName; //string(colName.c_str());
  _compression = compType;
  _dataType = dataType;
  
  _columnNumber = colNum;

  //_fileName = colName + "_mini.dat";
  _fileName = colName;
  _rosFileName = colName + "_full.ros";
  //_fileNameWos = colName + "_full.wos";

  _colPosition = colNum;

  _isSortKey = isSK;

  if ( _compression == COMPRESSION_TYPE2 || 
       _compression == COMPRESSION_TYPE4 )
    _sorted = false;
  else
    _sorted = true;

  if ( _sorted )
    _numIndexes = 2;
  else
    _numIndexes = 1;
}

void MetaColumn::setColCount( int cc )
{
  _colCount = cc;
}

bool MetaProjection::appendTuple( char *tuple )
{
  empty = false;
  // curr_wos_page doubles up as page counter
  int curr_offset = tuple_size*wos_tuple_count - 
    (WOS_PAGE_SIZE / tuple_size)*curr_wos_page * tuple_size; 

  //cout << " space " << tuple_size*wos_tuple_count << " WOS PAGE blah " << (WOS_PAGE_SIZE / tuple_size)*curr_wos_page << endl;

  if ( curr_offset > WOS_PAGE_SIZE - tuple_size ) // spill into new page
    {
      curr_wos_page++;

      /*if ( _col )
	{
	  for ( int i = 0; i < num_fields+1; i++ )
	    wos_col_data[ i ].push_back( (char*)malloc( WOS_PAGE_SIZE ) );
	}
	else*/	  
	wos_data.push_back( (char*)malloc( WOS_PAGE_SIZE ) ); //new char( WOS_PAGE_SIZE ) );
	//new_page_counter++;
      curr_offset = 0;
    }
  //else
    {
      //memcpy( fudge+ curr_offset, tuple, 4 );

      /*if ( _col )
	for ( int i = 0; i < num_fields+1; i++ )
	  {
	    //cout << " TRYING FOR i = " << i << endl;
	  memcpy( (char*)(wos_col_data[i][ curr_wos_page ])+curr_offset, tuple+i*sizeof(int), tuple_size );
	  }
	  else*/
	memcpy( (char*)(wos_data[ curr_wos_page ])+curr_offset, tuple, tuple_size );
    }

  //char *blah = new char[ 100 ];
  //char *bleh = new char[ 4 ];
  //memcpy( bleh, &wos_tuple_count, 4 );
  // update indices on timestamp and sort key
  // current version will assume that first column (second value) is the skey
  PageWriter::placeRecord( _db_storage_key, tuple, sizeof(int), 
			   (char*)&wos_tuple_count, sizeof(int) );

  PageWriter::placeRecord( _db_sort_key, tuple+sizeof(int), sizeof(int), 
  			   (char*)&wos_tuple_count, sizeof(int) );

  wos_tuple_count++;

  int z;
  memcpy(&z, tuple+sizeof(int), sizeof(int) );
  //cout << " second value " << z << " Placed " << wos_tuple_count << "th tuple " << " page is " << curr_wos_page << " offset " << curr_offset << " page size " << WOS_PAGE_SIZE << endl<< endl;
  
  return true;
}



void MetaColumn::setColPosition( int cp )
{
  _colPosition = cp;
}
/*
MetaColumn::MetaColumn( char *col, int dataType, int compType ) 
{w w
  cout << "This constructor is active " << endl;
  string s(col);
  //_name = s;
  
  MetaColumn::MetaColumn( s, dataType, compType );
  
  //cout << " Ended constructor " << endl << " name? " << _name << " s? " << s << " col " << col << endl << endl;
}
*/
MetaColumn::~MetaColumn()
{
}

string MetaColumn::getName()
{
  return _name;
}

bool MetaColumn::isSorted()
{
  return _sorted;
}

int MetaColumn::getIndex()
{
  return _columnNumber;
}

int MetaColumn::getEncodingType()
{
  return _compression;
}

int MetaColumn::getDataType()
{
  return _dataType;
}

MetaProjection::MetaProjection( string projName )
{
  //cout << " MetaProjection " << _name << " created " << endl;
  _name = RUNTIME_DATA + projName;

  columnCounter = 0;
  _primaryColumn = NULL;

  // Moving shared BDB items.
  // This is shared stuff for different WOS cursors.
  // ROS BDB files are a separate issue!
  _db_storage_key = new Db( NULL, 0 );
  _db_sort_key = new Db( NULL, 0 );
	
  // Sort key is not guaranteed to be unique. StorageKey has to be.
  _db_sort_key->set_flags( DB_DUP );

  // assume integers
  _db_storage_key->set_bt_compare( ROSAM::compare_int );
  _db_storage_key->set_error_stream( &cout );
  
  // do not assume integers on sort key because key is multiple
  // fields mended together.
  // Actually, we do assume that sort key is now an integer
  // but that will change, so segment should ask the catalog and
  // decide on this here.
  _db_sort_key->set_bt_compare( ROSAM::compare_int );
  _db_sort_key->set_error_stream( &cout );
  
  _db_storage_key->set_cachesize( 0, SMALL_CACHE_SIZE, 1 ); // 5Mb cache
  //this is very much an open question. There is certainly no need
  // to use a large page because we're only indexing storagekey to
  // position. Data is stored outside of BDB.
  _db_storage_key->set_pagesize( 4096 );
  
  _db_sort_key->set_cachesize( 0, SMALL_CACHE_SIZE, 1 ); // 5Mb cache
  _db_sort_key->set_pagesize( 4096 );

  string store_k = (_name + ".storage_key");
  
  if ( (_db_storage_key->open( NULL, (store_k.c_str()),NULL,
			       DB_BTREE, DB_CREATE, 0664) != 0 ))
    cerr << "Failed to open table " << (store_k) << endl;

  string sort_k = (_name + ".sort_key");

  if ((_db_sort_key->open( NULL, (sort_k.c_str()), NULL, 
			   DB_BTREE, DB_CREATE, 0664) != 0 ) )
    cerr << "Failed to open table " << (sort_k) << endl;
}

ROSAM *MetaColumn::getROSAM()
{
  // ROSAM
  cout << " Create and return a ROSAM " << _fileName << " indexes " << _numIndexes << endl;
  ROSAM *rosam = new ROSAM( _fileName+
			    (_fileName.rfind( ".ros" ) == string::npos ? ".ros" : ""),
			    _numIndexes);
  CatalogInstance::allocatedAMs->push_back( rosam );
  return ( rosam );
}

WOSAM *MetaColumn::getWOSAM()
{
  // maybe can use _numIndices, but not about to check.
  //
  cout << " Parameters are (PLEASE check): total columns " << _colCount << " field " << _colPosition << endl;
  WOSAM *wosam = new WOSAM( _fileNameWos, 2, _colCount, _colPosition );
  CatalogInstance::allocatedAMs->push_back( wosam );

  return (wosam);
}

MetaProjection::~MetaProjection()
{
  for ( map<string, MetaColumn*>::iterator iter = _columns.begin();
	iter != _columns.end(); iter++ )
    delete iter->second;
}

void MetaProjection::addColumn( MetaColumn *mc )
{
  /*
    assert ( _columns.find( mc->getName() ) ==
	   _columns.end() );
  */

  if ( !_primaryColumn )
    _primaryColumn = mc;

  // This assumes that columns are added in the order of
  // the schema!
  columnPosition = columnCounter;
  columnCounter++;

  _columns[ mc->getName() ] = mc;
  _columnNameInOrder.push_back(mc->getName());
}

vector<string> *MetaProjection::getAllColumns()
{

          vector<string> *cols = new vector<string>();

          for ( map<string, MetaColumn*>::iterator iter = _columns.begin();
                iter != _columns.end(); iter++ ){
            cols->push_back( (iter->second)->getName() );
          }
        
          return cols;
}
vector<string> *MetaProjection::getAllColumnsInOrder()
{
        vector<string> *cols = new vector<string>;
  for (int i=0; i<(int) _columnNameInOrder.size(); i++)
    {
	cols->push_back(_columnNameInOrder[i]);
    }
        
        
  return cols;
}

vector<string> *MetaProjection::getSortOrderColumns()
{
  vector<string> *cols = new vector<string>();
  /*
  for ( map<string, MetaColumn*>::iterator iter = _columns.begin();
	iter != _columns.end(); iter++ )
    {
      if ( (iter->second)->isSortKey() )
	cols->push_back( (iter->second)->getName() );
    }
  */
  for (int i=0; i<(int) _columnNameInOrder.size(); i++)
    {
      if ( _columns[_columnNameInOrder[i]]->isSortKey() )
	cols->push_back(_columnNameInOrder[i]);
    }
  
  return cols;
}

MetaColumn* MetaProjection::getColumn( string name )
{
  // This is inefficient and should change.
  // column count is set every time getColumn is called.
  //_columns[ name ]->setColCount( columnCounter );


  // Again, hacky. But will only do this once on a get:
  
  /*  if ( !wos_data.size() )
    {
      int num_fields = columnCounter;
      tuple_size = (num_fields+1)*sizeof(int);


      //cout << "CatalogInstance:: Temporary message, col count is? " << num_fields << endl;
      wos_data.push_back( (char*)malloc( WOS_PAGE_SIZE ) );
      
      // Load pre-existing WOS. 
      // opening file for reading fails miserably if the file
      // does not exist. So instead of non-existing file will
      // make an empty one by touch.
      system( ("touch " + _name + ".wos.TUPLES").c_str() );
      FILE *wosfd = fopen((_name + ".wos.TUPLES").c_str(), "r");
      char tuple[ (num_fields+1)*sizeof(int) ];
      
      int counter = 0;
      
      while (fread(tuple, 1, (num_fields+1)*sizeof(int), wosfd))
	{
	  //cout << " READ TUPLE from file, count " << a  << " REATD " << b << endl;
	  appendTuple( tuple );
	  counter++;
	}
      //if ( counter > 0 )
	cout << " Loaded " << counter << " tuples from " + _name << endl;
    }
  */
  return _columns[ name ];
}

string MetaProjection::getPrimaryColumnName()
{
  return _primaryColumn->getName();
}

string MetaProjection::getName()
{
  return _name;
}

string MetaProjection::toString()
{
  cout << " To String in MetaProjection " << endl;
  //_name = NULL;
  cout << " NAME " << _name << endl;
  string mps = ("Projection: " + _name + ":\n\n");

  for ( map<string, MetaColumn*>::iterator iter = _columns.begin();
	iter != _columns.end(); iter++ )
    mps += iter->second->toString();

  return (mps + "\n");
}

CatalogInstance::CatalogInstance()
{
  //original constructor loads projections/columns from static arrays
  //now replaced with method getCatalogInstanceFromBDB()
  //+
  cout << "\tloading catalog from BDB files..." << endl;
  this->loadCatalogFromBDB();
  //-
  //this->loadCatalog();
}

void CatalogInstance::loadCatalogFromBDB()
{
  string bdbProjection = RUNTIME_DATA + CSTORE_SYSTABLE_PROJECTION;
  PROJECTION_REC projRec;
  BDBFile* f1 = new BDBFile(bdbProjection);
  int rtrnCode = f1->open();
  if (rtrnCode != 0) throw UnexpectedException("CatalogInstance: open projection file error");
  void* v = f1->readFirstRec(&projRec.projectionID, sizeof(projRec.projectionID));
  while (v != NULL) {
    memcpy(&projRec, v, sizeof(PROJECTION_REC));
    /*
    cout.fill(' ');
    cout.width(25);
    cout.setf(ios_base::left  , ios_base::adjustfield);
    cout.setf(ios_base::dec, ios_base::basefield);
    cout  << projRec.projectionName
        << projRec.projectionID
        << projRec.tableID
        << projRec.primaryColumnID << endl;
    */
    string sProj = projRec.projectionName;
    _projections[ sProj ] = new MetaProjection( sProj );
    v = f1->readNextRec();
  }
  int totProjection = f1->getRecordCount();
  
  //----- loading column -----
  COLUMN_REC colRec;
  string bdbColumn = RUNTIME_DATA + CSTORE_SYSTABLE_COLUMN;
  BDBFile* f2 = new BDBFile(bdbColumn, true);
  
  //secondary index
  string bdbColumnAK = RUNTIME_DATA + CSTORE_SYSTABLE_COLUMN_AK;
  BDBFile* f3 = new BDBFile(bdbColumnAK, false);
  
  rtrnCode = f2->dbHandle->associate(NULL, f3->dbHandle, altKeyCallBackProjectionID, 0);
  if (rtrnCode != 0) throw UnexpectedException("CatalogInstance: associate error");
  
  rtrnCode = f2->open();
  if (rtrnCode != 0) throw UnexpectedException("CatalogInstance: open column file error");
  
  rtrnCode = f3->open();
  if (rtrnCode != 0) throw UnexpectedException("CatalogInstance: open column file error");
  
  for (int i = 1; i <= totProjection; ++i)  {
    //cout << "\n\nread column(s) with projectionID " << i << endl;
    int* pI = &i;
    v = f3->readRec(pI, sizeof(int));
    while (v != NULL) {
      memcpy(&colRec, v, sizeof(COLUMN_REC));
      /*
      cout.fill(' ');
      cout.width(25);
      cout.setf(ios_base::left, ios_base::adjustfield);
      cout.setf(ios_base::dec, ios_base::basefield);
      cout << colRec.columnName;
      cout <<  colRec.columnID;
      cout << colRec.projectionID;
      cout << colRec.dataType;
      cout << colRec.compressionType; 
      cout << colRec.isStorageKey << endl;
      */
      //get projection name
      void* vdata = f1->readRec(pI, sizeof(projRec.projectionID));
      if (vdata != NULL) {
        memcpy(&projRec, vdata, sizeof(PROJECTION_REC));
      } else {
        throw UnexpectedException("CatalogInstance: read projection name error");      
      }
      string s = projRec.projectionName;
      //adding column to the container
      _projections[ s ]->addColumn ( new MetaColumn(
              colRec.columnName,
              colRec.dataType,
              colRec.compressionType,
              colRec.columnID,            
              colRec.isStorageKey) );          
      v = f3->readNextRec(pI, sizeof(int));
    }
  }
    
  //WOSManager* wm = new WOSManager( string(D6_DATA_WOS)+string(".dummy.dummy"), 2, 8 );
  WOSManager* wm = new WOSManager( string(D6_DATA_WOS), 2, 8 );
  //WOSManager* wm = new WOSManager( "RuntimeData/D10_dummy.wos", 2, 8 );
  _wosManagers[ wm->getFileName() ] = wm;
  _wosManagers[ "D6.data" ] = wm;
  _wosManagers[ "Q1_projection" ] = wm;

  _wosManagers[ "Q1_l_shipdate" ] = wm;  // This is a slight naming hack
  // a getWOS call that comes for Q1_l_shipdate column will return same
  // as  D6_DATA_WOS, because it is the same. Naming should be general
  // most likely, the calling code should be fixed to substitute the
  // projection name (?)

  _wosManagers[ "Q1_l_shipdate" ] = wm; 
  _wosManagers[ "Q1_l_suppkey" ] = wm; 
  _wosManagers[ "Q1_l_orderkey" ] = wm; 
  _wosManagers[ "Q1_l_partkey" ] = wm; 
  _wosManagers[ "Q1_l_linenumber" ] = wm; 
  _wosManagers[ "Q1_l_quantity" ] = wm; 
  _wosManagers[ "Q1_l_extendedprice" ] = wm; 
  _wosManagers[ "Q1_l_returnflag" ] = wm; 

  _wosManagers[ "D6_l_shipdate" ] = wm;

  _wosManagers[ "Q2_projection" ] = wm;

  _wosManagers[ "Q2_l_suppkey" ] = wm;
  _wosManagers[ "Q2_l_shipdate" ] = wm;
  _wosManagers[ "Q2_l_orderkey" ] = wm; 
  _wosManagers[ "Q2_l_partkey" ] = wm; 
  _wosManagers[ "Q2_l_linenumber" ] = wm; 
  _wosManagers[ "Q2_l_quantity" ] = wm; 
  _wosManagers[ "Q2_l_extendedprice" ] = wm; 
  _wosManagers[ "Q2_l_returnflag" ] = wm; 

  _wosManagers[ "Q3_l_suppkey" ] = wm;
  _wosManagers[ "Q3_l_shipdate" ] = wm;
  _wosManagers[ "Q3_l_orderkey" ] = wm; 
  _wosManagers[ "Q3_l_partkey" ] = wm; 
  _wosManagers[ "Q3_l_linenumber" ] = wm; 
  _wosManagers[ "Q3_l_quantity" ] = wm; 
  _wosManagers[ "Q3_l_extendedprice" ] = wm; 
  _wosManagers[ "Q3_l_returnflag" ] = wm; 


  _wosManagers[ "D6_l_suppkey" ] = wm;
  _wosManagers[ "D6_l_shipdate" ] = wm;

  /*
    D7.data.ros/wos:
  (o_orderdate, l_suppkey, l_shipdate | o_orderdate, l_suppkey)
    
  ~= D2 from paper
  */

  wm = new WOSManager( (string(D7_DATA_WOS)), 2, 3 );
  _wosManagers[ wm->getFileName() ] = wm;
  _wosManagers[ "D7.data" ] = wm;

  _wosManagers[ "Q4_o_orderdate" ] = wm;
  _wosManagers[ "Q5_o_orderdate" ] = wm;
  _wosManagers[ "Q6_o_orderdate" ] = wm;

  _wosManagers[ "Q4_l_shipdate" ] = wm;
  _wosManagers[ "Q5_l_shipdate" ] = wm;
  _wosManagers[ "Q6_l_shipdate" ] = wm;

  _wosManagers[ "Q5_l_suppkey" ] = wm;
  _wosManagers[ "Q6_l_suppkey" ] = wm;
  
  wm = new WOSManager( D9_DATA_WOS, 2, 3 );
  _wosManagers[ wm->getFileName() ] = wm;
  _wosManagers[ "D9.data" ] = wm;
  _wosManagers[ "Q7_c_nationkey" ] = wm;
  _wosManagers[ "Q7_l_returnflag" ] = wm;
  _wosManagers[ "Q7_l_extendedprice" ] = wm;
  
  //wm = new WOSManager( "RuntimeData/D8_dummy.wos", 2, 8 );
  wm = new WOSManager( D8_DATA_WOS, 2, 3 );
  _wosManagers[ wm->getFileName() ] = wm;
  _wosManagers[ "D8_projection" ] = wm;
  _wosManagers[ "D8_orderdate" ] = wm;
  _wosManagers[ "D8_custkey" ] = wm;

  //wm = new WOSManager( "RuntimeData/D10_dummy.wos", 2, 2 );
  wm = new WOSManager( D10_DATA_WOS, 2, 2 );
  _wosManagers[ wm->getFileName() ] = wm;
  _wosManagers[ "D10_projection" ] = wm;
  _wosManagers[ "D10_nationkey" ] = wm;
  _wosManagers[ "D10_custkey" ] = wm;

  wm = new WOSManager( RUNTIME_DATA "MO_D6.data.wos", 2, 8 );
  _wosManagers[ wm->getFileName() ] = wm;
  _wosManagers[ "MO_D6.data" ] = wm;

  _wosManagers[ "MO_D6_l_suppkey" ] = wm;
  _wosManagers[ "MO_D6_l_shipdate" ] = wm;
  _wosManagers[ "MO_D6_l_orderkey" ] = wm;
  _wosManagers[ "MO_D6_l_partkey" ] = wm;
  _wosManagers[ "MO_D6_l_linenumber"] = wm;
  _wosManagers[ "MO_D6_l_returnflag" ] = wm;
  _wosManagers[ "MO_D6_l_extendedprice"] = wm;
  _wosManagers[ "MO_D6_l_quantity" ] = wm;
  
  vector<string> *vec = NULL;
  v = f1->readFirstRec(projRec.projectionName, sizeof(projRec.projectionName));
  while (v != NULL) {
    memcpy(&projRec, v, sizeof(PROJECTION_REC));
    string s = projRec.projectionName;
    vec = _projections[ s ]->getAllColumns();
    for ( vector<string>::iterator iter = vec->begin(); iter != vec->end(); iter++ )    
    {
      _allColumns[ (*iter) ] = _projections[ s ]->getColumn( (*iter) );
    }
    delete vec;
    v = f1->readNextRec();
  }
  
  f1->close();
  f3->close();
  f2->close();
  delete f1;
  delete f2;
  delete f3;    
}

//original constructor load catalog from static arrays
void CatalogInstance::loadCatalog()
{
  for ( unsigned int i = 0; i < sizeof( PROJECTIONS )/sizeof( string ); i ++ )
  {
      //cout << " PROJE " << PROJECTIONS[i] << endl;
    _projections[ PROJECTIONS[i] ] = new MetaProjection( PROJECTIONS[i] );
  }
  _projections[ "lineitem" ]->addColumn
      ( new MetaColumn( "l_shipdate", TYPE_INTEGER, COMPRESSION_TYPE1, 1, true) );
  _projections[ "lineitem" ]->addColumn
      ( new MetaColumn( "l_orderkey", TYPE_INTEGER, COMPRESSION_TYPE4, 2, false ) );
  _projections[ "lineitem" ]->addColumn
      ( new MetaColumn( "l_returnflag", TYPE_STRING, COMPRESSION_TYPE4, 3, false ) );
  _projections[ "lineitem" ]->addColumn
      ( new MetaColumn( "l_extendedprice", TYPE_INTEGER, COMPRESSION_TYPE4, 4, false ) );
  _projections[ "lineitem" ]->addColumn
      ( new MetaColumn( "l_suppkey", TYPE_INTEGER, COMPRESSION_TYPE4, 5, false ) );

  _projections[ "orders" ]->addColumn
      ( new MetaColumn( "o_orderdate", TYPE_INTEGER, COMPRESSION_TYPE1, 1, true) );
  _projections[ "orders" ]->addColumn
      ( new MetaColumn( "o_orderkey", TYPE_INTEGER, COMPRESSION_TYPE4, 2, false ) );
  _projections[ "orders" ]->addColumn
      ( new MetaColumn( "o_custkey", TYPE_INTEGER, COMPRESSION_TYPE4, 3, false ) );

  _projections[ "customers" ]->addColumn
      ( new MetaColumn( "c_nationkey", TYPE_INTEGER, COMPRESSION_TYPE1, 1, true  ) );
  _projections[ "customers" ]->addColumn
      ( new MetaColumn( "c_custkey", TYPE_INTEGER, COMPRESSION_TYPE4, 2,false  ) );
    
  _projections[ "typetest" ]->addColumn
      ( new MetaColumn( "t_int", TYPE_INTEGER, COMPRESSION_TYPE4, 1, true ));
  _projections[ "typetest" ]->addColumn
      ( new MetaColumn( "t_float", TYPE_FLOAT, COMPRESSION_TYPE4, 2, false  ));
  _projections[ "typetest" ]->addColumn
      ( new MetaColumn( "t_string", TYPE_STRING, COMPRESSION_TYPE4, 3, false   ) );

  _projections[ "Q1_projection" ]->addColumn
      ( new MetaColumn( "Q1_l_shipdate", TYPE_INTEGER, COMPRESSION_TYPE1, 1, true  ) );
  _projections["Q1_projection"]->addColumn
      ( new MetaColumn( "Q1_l_suppkey", TYPE_INTEGER, COMPRESSION_TYPE4,2, true  ) );
  _projections["Q1_projection"]->addColumn
      ( new MetaColumn( "Q1_l_orderkey", TYPE_INTEGER, COMPRESSION_TYPE4,3, false  ) );
  _projections["Q1_projection"]->addColumn
      ( new MetaColumn( "Q1_l_partkey", TYPE_INTEGER, COMPRESSION_TYPE4,4, false  ) );
  _projections["Q1_projection"]->addColumn
      ( new MetaColumn( "Q1_l_linenumber", TYPE_INTEGER, COMPRESSION_TYPE4,5, false  ) );
  _projections["Q1_projection"]->addColumn
      ( new MetaColumn( "Q1_l_quantity", TYPE_INTEGER, COMPRESSION_TYPE4,6, false  ) );
  _projections["Q1_projection"]->addColumn
      ( new MetaColumn( "Q1_l_extendedprice",TYPE_INTEGER, COMPRESSION_TYPE4,7, false ));
  _projections["Q1_projection"]->addColumn
      ( new MetaColumn( "Q1_l_returnflag", TYPE_STRING, COMPRESSION_TYPE4,8, false  ) );


  _projections["Q2_projection"]->addColumn
      ( new MetaColumn("Q2_l_shipdate", TYPE_INTEGER, COMPRESSION_TYPE1, 1, true ) );
  _projections["Q2_projection"]->addColumn
      ( new MetaColumn("Q2_l_suppkey", TYPE_INTEGER, COMPRESSION_TYPE4, 2, false  ) );
  _projections["Q2_projection"]->addColumn
      ( new MetaColumn( "Q2_l_orderkey", TYPE_INTEGER, COMPRESSION_TYPE4,3, false  ) );

  _projections["Q2_projection"]->addColumn
      ( new MetaColumn( "Q2_l_partkey", TYPE_INTEGER, COMPRESSION_TYPE4,4, false  ) );
  _projections["Q2_projection"]->addColumn
      ( new MetaColumn( "Q2_l_linenumber", TYPE_INTEGER, COMPRESSION_TYPE4,5, false  ) );
  _projections["Q2_projection"]->addColumn
      ( new MetaColumn( "Q2_l_quantity", TYPE_INTEGER, COMPRESSION_TYPE4,6, false  ) );
  _projections["Q2_projection"]->addColumn
      ( new MetaColumn( "Q2_l_extendedprice",TYPE_INTEGER, COMPRESSION_TYPE4,7, false ));
  _projections["Q2_projection"]->addColumn
      ( new MetaColumn( "Q2_l_returnflag", TYPE_STRING, COMPRESSION_TYPE4,8, false  ) );


  _projections["Q3_projection"]->addColumn
      ( new MetaColumn("Q3_l_shipdate", TYPE_INTEGER, COMPRESSION_TYPE1,1, true) );
  _projections["Q3_projection"]->addColumn
      ( new MetaColumn("Q3_l_suppkey", TYPE_INTEGER, COMPRESSION_TYPE4,2, true ) );
  _projections["Q3_projection"]->addColumn
      ( new MetaColumn( "Q3_l_orderkey", TYPE_INTEGER, COMPRESSION_TYPE4,3, false  ) );
  _projections["Q3_projection"]->addColumn
      ( new MetaColumn( "Q3_l_partkey", TYPE_INTEGER, COMPRESSION_TYPE4,4, false  ) );
  _projections["Q3_projection"]->addColumn
      ( new MetaColumn( "Q3_l_linenumber", TYPE_INTEGER, COMPRESSION_TYPE4,5, false  ) );
  _projections["Q3_projection"]->addColumn
      ( new MetaColumn( "Q3_l_quantity", TYPE_INTEGER, COMPRESSION_TYPE4,6, false  ) );
  _projections["Q3_projection"]->addColumn
      ( new MetaColumn( "Q3_l_extendedprice",TYPE_INTEGER, COMPRESSION_TYPE4,7, false ));
  _projections["Q3_projection"]->addColumn
      ( new MetaColumn( "Q3_l_returnflag", TYPE_STRING, COMPRESSION_TYPE4,8, false  ) );


  _projections["Q4_projection"]->addColumn
      ( new MetaColumn("Q4_o_orderdate", TYPE_INTEGER, COMPRESSION_TYPE1,1, true  ) );
  // not used in the query, inserted for continuity. Can probably not have
  // this entry at all (second column)
  // Big mistake. Q4_l_shipdate is in fact 2nd column in the data file!
  // Never mind, this was fixed.
  _projections["Q4_projection"]->addColumn
      ( new MetaColumn("Q4_l_suppkey", TYPE_INTEGER, COMPRESSION_TYPE4,2, false ));
  _projections["Q4_projection"]->addColumn
      ( new MetaColumn("Q4_l_shipdate", TYPE_INTEGER, COMPRESSION_TYPE4,3, false   ) );

  _projections["Q5_projection"]->addColumn
      ( new MetaColumn("Q5_o_orderdate", TYPE_INTEGER, COMPRESSION_TYPE1,1, true  ) );
  _projections["Q5_projection"]->addColumn
      ( new MetaColumn("Q5_l_suppkey", TYPE_INTEGER, COMPRESSION_TYPE4,2, true ));
  _projections["Q5_projection"]->addColumn
      ( new MetaColumn( "Q5_l_shipdate", TYPE_INTEGER, COMPRESSION_TYPE4,3, false   ) );

  _projections["Q6_projection"]->addColumn
      ( new MetaColumn("Q6_o_orderdate", TYPE_INTEGER, COMPRESSION_TYPE1,1, true ) );
  _projections["Q6_projection"]->addColumn
      ( new MetaColumn("Q6_l_suppkey", TYPE_INTEGER, COMPRESSION_TYPE4,2, true  ) );
  _projections["Q6_projection"]->addColumn
      ( new MetaColumn("Q6_l_shipdate" , TYPE_INTEGER, COMPRESSION_TYPE4,3, false  ) );

  _projections["Q7_projection"]->addColumn
      ( new MetaColumn("Q7_l_returnflag", TYPE_INTEGER, COMPRESSION_TYPE1,1, true  ) );
  _projections["Q7_projection"]->addColumn
      ( new MetaColumn("Q7_l_extendedprice",TYPE_INTEGER, COMPRESSION_TYPE4, 2, false ));
  _projections["Q7_projection"]->addColumn
      ( new MetaColumn("Q7_c_nationkey", TYPE_INTEGER, COMPRESSION_TYPE4, 3, false ) );

  _projections["D1.data"]->addColumn
      ( new MetaColumn("Q7_l_returnflag", TYPE_INTEGER, COMPRESSION_TYPE1,1, true  ) );

  /*
  These files can be found in the
    wosrosplay 
  directory on sorrel.
    D6.data.ros/wos:
  (l_shipdate, l_suppkey, l_orderkey, l_partkey, l_linenumber, l_quantity,
  l_extendedprice, l_returnflag | l_shipdate, l_suppkey)
  */
  
  _projections["D6.data"]->addColumn
      ( new MetaColumn( "D6_l_shipdate", TYPE_INTEGER, COMPRESSION_TYPE1,1, true ) );
  _projections["D6.data"]->addColumn
      ( new MetaColumn( "D6_l_suppkey", TYPE_INTEGER, COMPRESSION_TYPE4,2, true  ) );
  _projections["D6.data"]->addColumn
      ( new MetaColumn( "D6_l_orderkey", TYPE_INTEGER, COMPRESSION_TYPE4,3, false  ) );

  _projections["D6.data"]->addColumn
      ( new MetaColumn( "D6_l_partkey", TYPE_INTEGER, COMPRESSION_TYPE4,4, false  ) );
  _projections["D6.data"]->addColumn
      ( new MetaColumn( "D6_l_linenumber", TYPE_INTEGER, COMPRESSION_TYPE4,5, false  ) );
  _projections["D6.data"]->addColumn
      ( new MetaColumn( "D6_l_quantity", TYPE_INTEGER, COMPRESSION_TYPE4,6, false  ) );
  _projections["D6.data"]->addColumn
      ( new MetaColumn( "D6_l_extendedprice",TYPE_INTEGER, COMPRESSION_TYPE4,7, false ));
  _projections["D6.data"]->addColumn
      ( new MetaColumn( "D6_l_returnflag", TYPE_STRING, COMPRESSION_TYPE4,8, false  ) );

  //WOSManager* wm = new WOSManager( string(D6_DATA_WOS)+string(".dummy.dummy"), 2, 8 );
  WOSManager* wm = new WOSManager( string(D6_DATA_WOS), 2, 8 );
  //WOSManager* wm = new WOSManager( "RuntimeData/D10_dummy.wos", 2, 8 );
  _wosManagers[ wm->getFileName() ] = wm;
  _wosManagers[ "D6.data" ] = wm;
  _wosManagers[ "Q1_projection" ] = wm;

  _wosManagers[ "Q1_l_shipdate" ] = wm;  // This is a slight naming hack
  // a getWOS call that comes for Q1_l_shipdate column will return same
  // as  D6_DATA_WOS, because it is the same. Naming should be general
  // most likely, the calling code should be fixed to substitute the
  // projection name (?)

  _wosManagers[ "Q1_l_shipdate" ] = wm; 
  _wosManagers[ "Q1_l_suppkey" ] = wm; 
  _wosManagers[ "Q1_l_orderkey" ] = wm; 
  _wosManagers[ "Q1_l_partkey" ] = wm; 
  _wosManagers[ "Q1_l_linenumber" ] = wm; 
  _wosManagers[ "Q1_l_quantity" ] = wm; 
  _wosManagers[ "Q1_l_extendedprice" ] = wm; 
  _wosManagers[ "Q1_l_returnflag" ] = wm; 

  _wosManagers[ "D6_l_shipdate" ] = wm;

  _wosManagers[ "Q2_projection" ] = wm;

  _wosManagers[ "Q2_l_suppkey" ] = wm;
  _wosManagers[ "Q2_l_shipdate" ] = wm;
  _wosManagers[ "Q2_l_orderkey" ] = wm; 
  _wosManagers[ "Q2_l_partkey" ] = wm; 
  _wosManagers[ "Q2_l_linenumber" ] = wm; 
  _wosManagers[ "Q2_l_quantity" ] = wm; 
  _wosManagers[ "Q2_l_extendedprice" ] = wm; 
  _wosManagers[ "Q2_l_returnflag" ] = wm; 

  _wosManagers[ "Q3_l_suppkey" ] = wm;
  _wosManagers[ "Q3_l_shipdate" ] = wm;
  _wosManagers[ "Q3_l_orderkey" ] = wm; 
  _wosManagers[ "Q3_l_partkey" ] = wm; 
  _wosManagers[ "Q3_l_linenumber" ] = wm; 
  _wosManagers[ "Q3_l_quantity" ] = wm; 
  _wosManagers[ "Q3_l_extendedprice" ] = wm; 
  _wosManagers[ "Q3_l_returnflag" ] = wm; 


  _wosManagers[ "D6_l_suppkey" ] = wm;
  _wosManagers[ "D6_l_shipdate" ] = wm;

  /*
    D7.data.ros/wos:
  (o_orderdate, l_suppkey, l_shipdate | o_orderdate, l_suppkey)
    
  ~= D2 from paper
  */

  _projections[ "D7.data" ]->addColumn
      ( new MetaColumn( "D7_o_orderdate", TYPE_INTEGER, COMPRESSION_TYPE1,1, true ));
  _projections[ "D7.data" ]->addColumn
      ( new MetaColumn( "D7_l_suppkey", TYPE_INTEGER, COMPRESSION_TYPE4,2, true ));
  _projections[ "D7.data" ]->addColumn
      ( new MetaColumn( "D7_l_shipdate", TYPE_INTEGER, COMPRESSION_TYPE4,3, false  ));

  wm = new WOSManager( (string(D7_DATA_WOS)), 2, 3 );
  _wosManagers[ wm->getFileName() ] = wm;
  _wosManagers[ "D7.data" ] = wm;

  _wosManagers[ "Q4_o_orderdate" ] = wm;
  _wosManagers[ "Q5_o_orderdate" ] = wm;
  _wosManagers[ "Q6_o_orderdate" ] = wm;

  _wosManagers[ "Q4_l_shipdate" ] = wm;
  _wosManagers[ "Q5_l_shipdate" ] = wm;
  _wosManagers[ "Q6_l_shipdate" ] = wm;

  _wosManagers[ "Q5_l_suppkey" ] = wm;
  _wosManagers[ "Q6_l_suppkey" ] = wm;

  _projections[ "D9.data" ]->addColumn
      ( new MetaColumn( "D9_o_orderdate", TYPE_INTEGER, COMPRESSION_TYPE1,1, true ));
  _projections[ "D9.data" ]->addColumn
      ( new MetaColumn( "D9_l_suppkey", TYPE_INTEGER, COMPRESSION_TYPE4,2, true  ));
  _projections[ "D9.data" ]->addColumn
      ( new MetaColumn( "D9_l_shipdate", TYPE_INTEGER, COMPRESSION_TYPE4,3, false  ));

  wm = new WOSManager( D9_DATA_WOS, 2, 3 );
  _wosManagers[ wm->getFileName() ] = wm;
  _wosManagers[ "D9.data" ] = wm;
  _wosManagers[ "Q7_c_nationkey" ] = wm;
  _wosManagers[ "Q7_l_returnflag" ] = wm;
  _wosManagers[ "Q7_l_extendedprice" ] = wm;

  _projections[ "D8_projection" ]->addColumn
      ( new MetaColumn( "D8_orderdate", TYPE_INTEGER, COMPRESSION_TYPE4,1, true  ));
  _projections[ "D8_projection" ]->addColumn
      ( new MetaColumn( "D8_custkey", TYPE_INTEGER, COMPRESSION_TYPE4,2, false ));

  _projections[ "D10_projection" ]->addColumn
      ( new MetaColumn( "D10_custkey", TYPE_INTEGER, COMPRESSION_TYPE4,1, true  ));
  _projections[ "D10_projection" ]->addColumn
      ( new MetaColumn( "D10_nationkey", TYPE_INTEGER, COMPRESSION_TYPE4,2, false ));

  
  //wm = new WOSManager( "RuntimeData/D8_dummy.wos", 2, 8 );
  wm = new WOSManager( D8_DATA_WOS, 2, 3 );
  _wosManagers[ wm->getFileName() ] = wm;
  _wosManagers[ "D8_projection" ] = wm;
  _wosManagers[ "D8_orderdate" ] = wm;
  _wosManagers[ "D8_custkey" ] = wm;

  //wm = new WOSManager( "RuntimeData/D10_dummy.wos", 2, 2 );
  wm = new WOSManager( D10_DATA_WOS, 2, 2 );
  _wosManagers[ wm->getFileName() ] = wm;
  _wosManagers[ "D10_projection" ] = wm;
  _wosManagers[ "D10_nationkey" ] = wm;
  _wosManagers[ "D10_custkey" ] = wm;


  /*
  MERGE OUT STUFF
  */
  _projections["MO_D6.data"]->addColumn
      ( new MetaColumn( "MO_D6_l_shipdate", TYPE_INTEGER, COMPRESSION_TYPE1,1, true ) );
  _projections["MO_D6.data"]->addColumn
      ( new MetaColumn( "MO_D6_l_suppkey", TYPE_INTEGER, COMPRESSION_TYPE4,2, true  ) );
  _projections["MO_D6.data"]->addColumn
      ( new MetaColumn( "MO_D6_l_orderkey", TYPE_INTEGER, COMPRESSION_TYPE4,3, false  ) );

  _projections["MO_D6.data"]->addColumn
      ( new MetaColumn( "MO_D6_l_partkey", TYPE_INTEGER, COMPRESSION_TYPE4,4, false  ) );
  _projections["MO_D6.data"]->addColumn
      ( new MetaColumn( "MO_D6_l_linenumber", TYPE_INTEGER, COMPRESSION_TYPE4,5, false  ) );
  _projections["MO_D6.data"]->addColumn
      ( new MetaColumn( "MO_D6_l_quantity", TYPE_INTEGER, COMPRESSION_TYPE4,6, false  ) );
  _projections["MO_D6.data"]->addColumn
      ( new MetaColumn( "MO_D6_l_extendedprice",TYPE_INTEGER, COMPRESSION_TYPE4,7, false ));
  _projections["MO_D6.data"]->addColumn
      ( new MetaColumn( "MO_D6_l_returnflag", TYPE_STRING, COMPRESSION_TYPE4,8, false  ) );

  wm = new WOSManager( RUNTIME_DATA "MO_D6.data.wos", 2, 8 );
  _wosManagers[ wm->getFileName() ] = wm;
  _wosManagers[ "MO_D6.data" ] = wm;

  _wosManagers[ "MO_D6_l_suppkey" ] = wm;
  _wosManagers[ "MO_D6_l_shipdate" ] = wm;
  _wosManagers[ "MO_D6_l_orderkey" ] = wm;
  _wosManagers[ "MO_D6_l_partkey" ] = wm;
  _wosManagers[ "MO_D6_l_linenumber"] = wm;
  _wosManagers[ "MO_D6_l_returnflag" ] = wm;
  _wosManagers[ "MO_D6_l_extendedprice"] = wm;
  _wosManagers[ "MO_D6_l_quantity" ] = wm;

  vector<string> *vec = NULL;
  for ( unsigned int i = 0; i < sizeof( PROJECTIONS )/sizeof( string ); i ++ )
  {
    vec = _projections[ PROJECTIONS[i] ]->getAllColumns();
    for ( vector<string>::iterator iter = vec->begin(); iter != vec->end(); iter++ )    
    {
      _allColumns[ (*iter) ] = _projections[ PROJECTIONS[i] ]->getColumn( (*iter) );
    }
    delete vec;
  }    
}

CatalogInstance::~CatalogInstance() 
{
  for ( map<string, MetaProjection*>::iterator iter = _projections.begin();
	iter != _projections.end(); iter++ )
    {
      if ( iter->second == NULL )
      delete iter->second;
    }
}

bool CatalogInstance::isSorted( string projName, string colName )
{
  if ( _projections.find( projName ) == _projections.end() )
    return false;

  if ( _allColumns.find( colName ) == _allColumns.end() )
    {
      // throw exception
      return false;
    }
  
  return _projections[ projName ]->getColumn( colName )->isSorted();
}

int CatalogInstance::getColumnIndex( string projName, string colName )
{
  if ( _projections.find( projName ) == _projections.end() )
    return -1;

  if ( _allColumns.find( colName ) == _allColumns.end() )
    {
      // throw exception
      cerr << "Column " << colName << " does not seem to be a column " << endl;
      return -1;
    }
  
  return _projections[ projName ]->getColumn( colName )->getIndex();
}

bool CatalogInstance::isSorted( string colName )
{
  if ( _allColumns.find( colName ) == _allColumns.end() )
    {
      // throw exception
      return false;
    }
  
  return _allColumns[ colName ]->isSorted();
}

bool CatalogInstance::isValidProjection(string projName)
{
  return ( _projections.find( projName ) != _projections.end() );
}

bool CatalogInstance::isColumnOf(string projName, string colName)
{
  if ( isValidProjection( projName ) )
    return ( _allColumns.find( colName ) != _allColumns.end() );

  return false;
}

vector<string> *CatalogInstance::getSortOrderColumns( string projName )
{
  return _projections[ projName ]->getSortOrderColumns();
}

string CatalogInstance::getPrimaryColumnName( string projName ) 
{
  if ( _projections.find( projName ) == _projections.end() )
    return NULL;

  return _projections[ projName ]->getPrimaryColumnName();
}

int CatalogInstance::getEncodingType( string projName, string colName )
{
  if ( _projections.find( projName ) == _projections.end() )
    return -1;

  if ( _allColumns.find( colName ) == _allColumns.end() )
    {
      // throw exception
      return -1;
    }
  
  return _projections[ projName ]->getColumn( colName )->getEncodingType();
}


/*void CatalogInstance::setAlias( string alias ) {
	m_sAlias = alias;
}
*/
string CatalogInstance::toString()
{
  string cis = "";

  for ( map<string, MetaProjection*>::iterator iter = _projections.begin();
	iter != _projections.end(); iter++ )
    cis += iter->second->toString();
  
  return cis;
}

ROSAM *CatalogInstance::getROSAM( string projName, string colName)
{
  if ( _projections.find( projName ) == _projections.end() )
    {
      cerr << " Looking for proj (and not finding it) " << projName << " name " << colName << endl;
      return NULL;
    }

  if ( _allColumns.find( colName ) == _allColumns.end() )
    {
      // throw exception
      cerr << " Looking for proj (and not finding it) " << projName << " name " << colName << endl;
      return NULL;
    }
  
  return _projections[ projName ]->getColumn( colName )->getROSAM();
}

ROSAM *CatalogInstance::getROSAM( string colName)
{
  //cout << "CATALOG: get ROSAM for " << colName << endl;
  
  if ( _allColumns.find( colName ) == _allColumns.end() )
    {
      cerr << "Catalog: no such column: " << colName << endl;
      // throw exception
      return NULL;
    }
  
  return _allColumns[ colName ]->getROSAM();
}


WOSAM *CatalogInstance::getWOSAM( string projName, string colName)
{
  // cout << " CALL on " << projName << " col name " << colName << " position " << _allColumns[ colName ]->getColumnPosition() << endl;

  return _wosManagers[ (CatalogInstance::stripOffDirectory( projName )/* + string(".wos")*/) ]->getWOSAM( _allColumns[ colName ]->getColumnPosition() );

  /*if ( _projections.find( projName ) == _projections.end() )
    return NULL;

  if ( _allColumns.find( colName ) == _allColumns.end() )
    {
      // throw exception
      return NULL;
    }
  
  //cout << "Temporary Notice: getting proj " << projName << " column " << colName << endl;
  return _projections[ projName ]->getColumn( colName )->getWOSAM();
  */
}

WOSAM *CatalogInstance::getWOSAM( string colName)
{
  
  if ( _allColumns.find( colName ) == _allColumns.end() )
    {
      // throw exception
      return NULL;
    }
  
  //cout << "Temporary Notice: getting proj column " << colName << endl;
  return _allColumns[ colName ]->getWOSAM();
}

WOSAM *CatalogInstance::getWOSAM( string colName, int colIndex )
{
  //cout << "Temporary Notice: getting proj column " << (CatalogInstance::stripOffDirectory( colName )) << " from " << colName << endl;
  return _wosManagers[ (CatalogInstance::stripOffDirectory( colName ) /*+ string(".wos")*/) ]->getWOSAM( colIndex );
}

DVAM *CatalogInstance::getDVAM( string colName, bool ros )
{
  //cout << "Temporary Notice: getting proj column " << (CatalogInstance::stripOffDirectory( colName ) + string(".wos")) << endl;
  return _wosManagers[ (CatalogInstance::stripOffDirectory( colName ) /*+ string(".wos")*/) ]->getDVAM( ros );
}

WOSManager *CatalogInstance::getWOSManager( string colName )
{
  return _wosManagers[  (CatalogInstance::stripOffDirectory( colName ) /*+ string(".wos")*/) ];
}

WOSAM *CatalogInstance::getWOSAM( int colIndex )
{
  return _wosManagers[ "D6.data.wos" ]->getWOSAM( colIndex );
}

int CatalogInstance::getColumnType( string colName )
{
  if ( _allColumns.find( colName ) == _allColumns.end() )
    {
      throw UnexpectedException("No such column exists");
    }
  
  return _allColumns[ colName ]->getDataType();
}


int CatalogInstance::getColumnType( string projName, string colName )
{
  if ( _projections.find( projName ) == _projections.end() )
    {
            return 0;
    }

  return _projections[ projName ]->getColumn(colName)->getDataType();
}

string CatalogInstance::stripOffDirectory( string fName )
{
  string s = fName;
  int loc = s.rfind("/", string::npos);
  if(s.rfind("/", string::npos) != string::npos)
      return s.substr(loc+1); 
  else
      return fName;    
}

static bool getCat() {
  return CatalogInstance::gottenCat;
}

static CatalogInstance* getCatalog()
{
  if ( !CatalogInstance::catalog ) {
    CatalogInstance::catalog = new CatalogInstance();
    CatalogInstance::gottenCat=true;
  }

    return CatalogInstance::catalog;
}

vector<string> *CatalogInstance::getColumnNames( string projName )
{
  if ( _projections.find( projName ) == _projections.end() )
    {
      cout << " Failed to find column names for " << projName << endl;
      return new vector<string>();
    }

  return _projections[ projName ]->getAllColumns();
}

vector<string> *CatalogInstance::getColumnNamesInOrder( string projName )
{
  if ( _projections.find( projName ) == _projections.end() )
    {
      cout << " Failed to find column names for " << projName << endl;
      return new vector<string>();
    }

  return _projections[ projName ]->getAllColumnsInOrder();
}


void CatalogInstance::disposeOfWOS( WOSAM *wos )
{
  cout << " Dispose of " << wos->getTableName() << endl;
  //this migth become more complicated eventually.
  if ( wos )
    delete wos;
	
	cout << "End Dispose" << endl;
}

void CatalogInstance::disposeOfROS( ROSAM *ros )
{
  cout << " Dispose of " << ros->getTableName() << endl;
  if ( ros )
    delete ros;
	
	cout << "End Dispose" << endl;
}

void CatalogInstance::disposeOfDV ( DVAM *dvam )
{
  cout << " Dispose of " << dvam->getTableName() << endl;

  /*if ( dvam )
    delete dvam;*/
}

list<TVariable*> CatalogInstance::getColumns() 
{
  vector<string> *cols;
  
  if( m_listTuples.empty() ) 
    {
      cols = _projections[ m_sProjName ]->getAllColumns();
      for ( vector<string>::iterator iter = cols->begin(); 
	    iter != cols->end(); iter++ )
	m_listTuples.push_front( getCol( *iter ) );
      delete cols;
    }
  
  return m_listTuples;
}

CatalogInstance* CatalogInstance::getCatalog()
{
  if ( !CatalogInstance::catalog ) 
    {
      CatalogInstance::catalog = new CatalogInstance();
      CatalogInstance::gottenCat=true;
      /*
      try
	{
	  CatalogInstance::dbEnv = new DbEnv( 0 );
	  
	  CatalogInstance::dbEnv->open( ".DbCache", 
					DB_INIT_MPOOL | DB_CREATE| DB_PRIVATE,
					0644 ); // DB_THREAD Not necessary yet?
	  //CatalogInstance::dbEnv->set_verbose( DB_VERB_REPLICATION, true );
	  //CatalogInstance::dbEnv->set_cachesize( 0, (10*CACHE_SIZE), 10 );
	  CatalogInstance::dbEnv->set_cachesize(2, 524288000, 3 );
	  CatalogInstance::dbEnv->set_data_dir( "." );
	  CatalogInstance::dbEnv->set_tmp_dir( ".tempDB" );
	  
	  // DB and other code goes here
	}
      catch(DbException &e)
	{
	  // DB error handling goes here
	  cout << "CatalogInstance::getCatalog(), exception: -" << e.what() << " -" <<  e.get_errno() << "-" << endl;
	  //char *a = new char[ 100 ];
	  //bzero( a, 100 );
	  //CatalogInstance::dbEnv->get_home( (const char **)&a );
	  //cout << " DB " <<   string( a ) << endl;
	  }
      */
  }
    return CatalogInstance::catalog;
}

void CatalogInstance::deallocAMs()
{
  for ( vector<AM*>::iterator iter = CatalogInstance::allocatedAMs->begin();
	iter != CatalogInstance::allocatedAMs->end(); iter++ )
    {
    //uncomment this line will cause exception if you're deleting DVAM 
    //cout << " deleting AM " << (*iter)->getColumnName() << endl;
    delete (*iter);
    }

  CatalogInstance::allocatedAMs->clear();
}

bool CatalogInstance::getCat() {
  return CatalogInstance::gottenCat;
}

TVariable* CatalogInstance::getCol( string name ) 
{
  TVariable* lpTVariable = TVariable::create( m_projection, name, _projections[ m_sProjName ]->getColumn( name )->getDataType() );

  return lpTVariable;
}


// Nga: for plan node memory management
void CatalogInstance::addAllocatedNode(Node* lpNode)
{
	CatalogInstance::allocatedNodes->push_back(lpNode);
}

void CatalogInstance::deallocNodes()
{
	for ( vector<Node*>::iterator iter = CatalogInstance::allocatedNodes->begin();
	iter != CatalogInstance::allocatedNodes->end(); iter++ )
    {
		cout << " deleting Node " << (*iter)->getNodeID() << endl;
		delete (*iter);
    }

  CatalogInstance::allocatedNodes->clear();
}

