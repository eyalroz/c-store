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
#ifndef _CATALOGINSTANCE_H_
#define _CATALOGINSTANCE_H_

#include <string>
#include <list>
#include <set>
#include <map>
#include <vector>
#include <db_cxx.h>
#include "../common/Constants.h"
#include "../parser/TVariable.h"
#include "ROSAM.h"
#include "WOSAM.h"
#include "WOSManager.h"
//#include "../parser/Projection.h"
//#include "../parser/constants.h"
#include "../common/Constants.h"
#include "../Paths.h"
#include "../plan/Node.h"
#include "../Util/BDBFile.h"

class WOSAM;
class ROSAM;

using namespace std;

class MetaColumn
{
 protected:
  string _name;
  int _compression;
  int _dataType;
  bool _sorted;
  string _fileName;
  string _fileNameWos;
  int _numIndexes;
  int _colPosition, _colCount;

  int _columnNumber;
  string _rosFileName;

  bool _isSortKey;

 public:
  MetaColumn( string colName, int dataType, int compType, int _colNum, bool isSK );
  //MetaColumn( char *col, int dataType, int compType );
  ~MetaColumn();

  string getName();
  int getDataType();
  bool isSorted();
  int getIndex();
  int getEncodingType();
  
  void setColPosition( int cp );
  void setColCount( int cc );

  int getColumnPosition() { return _colPosition; }

  bool isSortKey(){ return _isSortKey; }

  ROSAM *getROSAM();

  WOSAM *getWOSAM();

  string toString();
};

class MetaProjection
{
 protected:
  string _name;
  MetaColumn *_primaryColumn;
  map<string, MetaColumn*> _columns;
  vector<string> _columnNameInOrder;
  
  Db *_db_storage_key, *_db_sort_key;
  vector<char*> wos_data;
  int wos_size, curr_wos_page, wos_tuple_count;
  int num_fields, tuple_size;
  bool empty;
  int columnPosition;

    // NOTE: tuple MUST be of size (numfields+1)*sizeof(int), assuming
    // timestamp is int. For variable size fields, we store actual data
    // elsewhere, and pointer to it as an int.
  bool appendTuple( char *tuple );


 public:
  MetaProjection( string projName );
  ~MetaProjection();

  void addColumn( MetaColumn *mc );

  int columnCounter;

  vector<string> *getAllColumns();
  vector<string> *getAllColumnsInOrder();
  MetaColumn* getColumn( string name );
  string getPrimaryColumnName();
  vector<string> *getSortOrderColumns();
  
  string getName();

  string toString();
};


class CatalogInstance 
{
 protected:
  map<string, MetaProjection*> _projections;
  map<string, MetaColumn*> _allColumns;
  map<string, WOSManager*> _wosManagers;

  string m_sProjName;
  class Projection* m_projection;
  list<class TVariable*> m_listTuples;
  //string m_sAlias;

  void loadCatalogFromBDB();    //construct catalog from BDB file
  void loadCatalog();           //static catalog
  
  public:
   CatalogInstance(); //  class Projection* proj );
   ~CatalogInstance();
  
  list<class TVariable*> CatalogInstance::getColumns();
  class TVariable* getCol( string name );
  void setAlias( string alias );
  string getAlias();

  bool isSorted( string projName, string colName );
  int getColumnIndex( string projName, string colName );
  
  bool isSorted( string colName );
  int  getEncodingType( string projName, string colName );
  string getPrimaryColumnName( string projName );
  vector<string> *getSortOrderColumns( string projName );

  bool isValidProjection(string projName);
  bool isColumnOf(string projName, string colName);

  vector<string> *getColumnNames( string projName );
  vector<string> *getColumnNamesInOrder( string projName );
  static vector<AM*> *allocatedAMs;

  ROSAM *getROSAM( string projName, string colName);
  ROSAM *getROSAM( string colName);

  WOSAM *getWOSAM( string projName, string colName);
  WOSAM *getWOSAM( string colName);
  WOSAM *getWOSAM( string colName, int colIndex );
  WOSAM *getWOSAM( int colIndex );
  
  WOSManager *getWOSManager( string colName );

  DVAM *getDVAM( string colName, bool ros );

  static CatalogInstance* getCatalog();
  static bool getCat();

  int getColumnType( string colName );
  int getColumnType(string projName, string colName );

  void disposeOfWOS( WOSAM *wos );
  void disposeOfROS( ROSAM *ros );
  void disposeOfDV ( DVAM *dvam );

  static const string PROJECTIONS[25]; 
  static const string TYPE_NAMES[5]; 
  static const string COMPRESSION_NAMES[7];

  static CatalogInstance* catalog;
  static bool gottenCat;

  static DbEnv* dbEnv;

  static string stripOffDirectory( string fName );
  static void deallocAMs();

  string toString();
  
  // Nga: for plan node memory management
  static vector<Node*> *allocatedNodes;
  static void addAllocatedNode(Node* lpNode);
  static void deallocNodes();
};

#define MAX_NAME_LENGTH  128
typedef struct projectionRec {
  char          projectionName[MAX_NAME_LENGTH];                  /* projection name */
  unsigned int  projectionID;                                     /* projection ID */
  unsigned int  tableID;                                          /* belongs to this table*/
  unsigned int  primaryColumnID;                                  /* ID primary Column */
} PROJECTION_REC;

typedef struct columnRec {
  char          columnName[MAX_NAME_LENGTH];                      /* column name */
  unsigned int  columnID;                                         /* column ID */
  unsigned int  projectionID;                                     /* belongs to this projection */
  unsigned int  dataType;                                         /* data type */
  unsigned int  compressionType;                                  /* compression type */
  unsigned int  isStorageKey;                                     /* is this a storage key column  1=true */
} COLUMN_REC;

#endif /* _CATALOGINSTANCE_H_ */

