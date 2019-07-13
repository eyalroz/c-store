%option nounput

%{

#define YYSTYPE PObject*

#include "PObject.h"
#include "grammar.h"
#include <string>
#include <iostream>

#include "PIdent.h"
#include "Expressions/EString.h"
#include "Expressions/ENumbers/EInteger.h"
#include "Expressions/ENumbers/EFloat.h"

using namespace std;

int lineno = 1;
extern "C" void yyerror(const char *s);

%}

%%

	/* literal keyword tokens */

AND		{ return AND; }
AVG		{ return AGG_AVG; }
MIN		{ return AGG_MIN; }
MAX		{ return AGG_MAX; }
SUM		{ return AGG_SUM; }
COUNT		{ return AGG_COUNT; }
AS		{ return AS; }
ASC		{ return ASC; }
BETWEEN		{ return BETWEEN; }
BY		{ return BY; }
DESC		{ return DESC; }
DISTINCT	{ return DISTINCT; }
FROM		{ return FROM; }
GROUP		{ return GROUP; }
HAVING		{ return HAVING; }
IN		{ return IN; }
IS		{ return IS; }
NOT		{ return NOT; }
NULL		{ return NULLX; }
OR		{ return OR; }
ORDER		{ return ORDER; }
SELECT		{ return SELECT; }
WHERE		{ return WHERE; }
TRUE		{ return BOOL_TRUE; }
FALSE		{ return BOOL_FALSE; }
        /* Tien's: INSERT INTO and DELETE FROM */
INSERT          { return INSERT; }
INTO            { return INTO; }
VALUES          { return VALUES; }
DELETE          { return DELETE; }      
COMMIT          { return COMMIT; }
LIMIT           { return LIMIT; }
OFFSET          { return OFFSET; }
MERGE           { return MERGE; }

	/* punctuation */

"="		{ return OP_EQ; }
"<>" 		{ return OP_NE; }
"<"		{ return OP_LT; }
">"		{ return OP_GT; }
"<="		{ return OP_LE; }
">="		{ return OP_GE; }

[-+*/:(),;]	{ return yytext[0]; }

	/* names */

[A-Za-z]["."A-Za-z0-9_]*       { yylval = PIdent::create( yytext ) ; return NAME; }

	/* numbers */

[0-9]+		{ yylval = EInteger::create( atoi( yytext ) ); return INTNUM; }


[0-9]+"."[0-9]* |
"."[0-9]*	{ yylval = EFloat::create( atof( yytext ) ); return FLOATNUM; }

	/* strings */

'[^'\n]*'	{
			if( yytext[yyleng-2] == '\\' ) {
				yyless(yyleng-1);
				yymore();
			} else {
				yylval = EString::create( yytext );
				return STRING;
			}
		}
		
'[^'\n]*$	{ yyerror("Unterminated string"); }

\n		lineno++;

[ \t\r]+	;	/* white space */

"--".*$		;	/* comment */

%%
int yy_set_yyin(const char* inputFileName)
{
  yyin = fopen(inputFileName, "r");
  if (!yyin) 
    return 0;  
  else
    return 1;  
}
int yy_reset_yyin()
{
  yyin = (FILE*)0;
  return 0;
}
