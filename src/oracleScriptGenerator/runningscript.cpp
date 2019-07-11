/* 
 * Nga Tran - nga@brandeis.edu
 */

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>

using namespace std;
using std::cout;

#define LINE_LENGTH  1000

int main(int argc, char **argv) {

  if (argc != 3) {
    cout << "usage: ./<program> + <input file> + <output file>" << endl;
    exit (1);
  }

  FILE* ptrInputFile = NULL;
  FILE* ptrOutputFile = NULL;

  /* Open input and output file */

  if ((ptrInputFile = fopen(argv[1], "r")) == NULL) {
    cout << "Unable to open input file " << argv[2] << endl;
    exit (1);
  }
  if ((ptrOutputFile = fopen(argv[2], "w")) == NULL) {
    fclose(ptrInputFile); /* If error, close input file first! */
    cout << "Unable to create output file " << argv[3] << endl;
    exit (1);
  }

  // Read input file
  char line[LINE_LENGTH];
  string flushingCache;
  string sqlCommand;
  string scale;
  string suffix;
  string inFolder;
  string outFolder;

  // First line: execute file
    if (fgets(line, LINE_LENGTH, ptrInputFile) != NULL)
      {
	 flushingCache = string(strtok(line, " \n"));
      }
    else
      {
	cout << "Error - first line " << endl;
	return 0;
      }

    // second line
    if (fgets(line, LINE_LENGTH, ptrInputFile) != NULL)
      {
	sqlCommand = string(strtok(line, "\n\t#"));
      }
    else
      {
	cout << "Error - second line " << endl;
	return 0;
      }

    // third line
    if (fgets(line, LINE_LENGTH, ptrInputFile) != NULL)
      {
	scale = string(strtok(line, " \n"));
      }
    else
      {
	cout << "Error - third line " << endl;
	return 0;
      }

    // forth line
    if (fgets(line, LINE_LENGTH, ptrInputFile) != NULL)
      {
	suffix = string(strtok(line, " \n"));
      }
    else
      {
	cout << "Error - forth line " << endl;
	return 0;
      }

    // fifth line
    if (fgets(line, LINE_LENGTH, ptrInputFile) != NULL)
      {
	inFolder = string(strtok(line, " \n"));
      }
    else
      {
	cout << "Error - fifth line " << endl;
	return 0;
      }

    // sixth line
    if (fgets(line, LINE_LENGTH, ptrInputFile) != NULL)
      {
	outFolder = string(strtok(line, " \n"));
      }
    else
      {
	cout << "Error - sixth line " << endl;
	return 0;
      }



  /* Read from input file one line at a time */
  
  while (fgets(line, LINE_LENGTH, ptrInputFile) != NULL) {

    /* Get the parameter */

    string inFile = string(strtok(line, " \n"));
    
    string sInFile(inFolder);
    sInFile.append(inFile);

    string sOutFile(outFolder);
    sOutFile.append(inFile);
    sOutFile.append("_");
    sOutFile.append(scale);
    sOutFile.append("_out");
    sOutFile.append(suffix);

    string outLine(sqlCommand);
    outLine.append(" < ");
    outLine.append(sInFile);
    outLine.append(" > ");
    outLine.append(sOutFile);
    
    fputs(flushingCache.c_str(), ptrOutputFile);
    fputs("\n", ptrOutputFile);
    fputs(outLine.c_str(), ptrOutputFile);
    fputs("\n\n", ptrOutputFile);  
  }
  
  fclose(ptrInputFile);
  fclose(ptrOutputFile);

  return 0;
}
