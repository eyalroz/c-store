%{
#include <iostream>
#include <string>
#include <stack>
#include <list>
#include "PObject.h"

#define YYSTYPE PObject*

#include "ListProjections.h"
#include "ListSelections.h"
#include "ListOrdering.h"
#include "ListCols.h"
#include "ListExps.h"
#include "ListColumnValues.h"
#include "TVariable.h"
#include "Expression.h"
#include "Query.h"
#include "ColOrder.h"
#include "EHolder.h"
#include "PIdent.h"
#include "Parser.h"
#include "../AM/CatalogInstance.h"

#include "Expressions/EColumn.h"
#include "Expressions/EString.h"
#include "Expressions/ENumber.h"
#include "Expressions/ENumbers/EInteger.h"
#include "Expressions/ENumbers/EFloat.h"

#include "Expressions/EArithmetic.h"
#include "Expressions/EArithmetics/EPlus.h"
#include "Expressions/EArithmetics/EMinus.h"
#include "Expressions/EArithmetics/EMult.h"
#include "Expressions/EArithmetics/EDiv.h"

#include "Expressions/EAgg.h"
#include "Expressions/EAgg/EAggCount.h"
#include "Expressions/EAgg/EAggAvg.h"
#include "Expressions/EAgg/EAggMax.h"
#include "Expressions/EAgg/EAggMin.h"
#include "Expressions/EAgg/EAggSum.h"

#include "BExpression.h"
#include "BExpressions/BBool.h"
#include "BExpressions/BIsNull.h"

#include "BExpressions/BComparison.h"
#include "BExpressions/BComparison/BEq.h"
#include "BExpressions/BComparison/BNe.h"
#include "BExpressions/BComparison/BLt.h"
#include "BExpressions/BComparison/BGt.h"
#include "BExpressions/BComparison/BLe.h"
#include "BExpressions/BComparison/BGe.h"

#include "BExpressions/BBinaryLogic.h"
#include "BExpressions/BBinaryLogic/BConjunction.h"
#include "BExpressions/BBinaryLogic/BDisjunction.h"
#include "BExpressions/BNegation.h"

#include "Queries/QCommit.h"
#include "Queries/QDelete.h"
#include "Queries/QInsert.h"
#include "Queries/QSelect.h"
#include "Queries/QMerge.h"

using namespace std;
 
int yylex( void );

/* These are needed as placeholders to keep it happy */

extern "C" void yyerror( const char *str ) {
	parser_error( "Syntax error" );
}

extern "C" int yywrap( void ) {
        return 1;
}

#if __cplusplus >= 201103L
[[noreturn]]
#endif
void parser_error( std::string message ) {
	cerr << "error: " << message << endl;
	exit( 1 );
}

extern Parser* thisptr;

%}

	/* operators */

%left OR
%left AND
%left NOT
/* %left <subtok> COMPARISON  = <> < > <= >= */
%left OP_EQ OP_NE OP_LT OP_GT OP_LE OP_GE
%left '+' '-'
%left '*' '/'
%nonassoc UMINUS

	/* literal keyword tokens */

%token AND AVG MIN MAX SUM COUNT AS ASC BETWEEN BY DESC DISTINCT FROM GROUP
%token HAVING IN IS NOT NULLX OR ORDER SELECT WHERE BOOL_TRUE BOOL_FALSE
%token AGG_COUNT AGG_MIN AGG_MAX AGG_SUM AGG_AVG
%token STRING FLOATNUM INTNUM NAME
        /* Tien's */
%token INSERT INTO VALUES DELETE COMMIT LIMIT OFFSET MERGE
//%token USER PARAMETER INDICATOR 

%%

sql:
	statement ';'	{
		thisptr->setQuery( dynamic_cast<Query*>( $1 ) );
		YYACCEPT;
	}

statement:
                merge_statement { $$ = $1; }
                |
                commit_statement { $$ = $1; }
                |
                delete_statement_searched { $$ = $1; }
                |
                insert_statement { $$ = $1; }
                |
		query_spec { $$ = $1; }
	;

merge_statement:
		MERGE proj_exp {
		  Query* q = QMerge::create();
		 
		  Projection* cl_projection = dynamic_cast<Projection*> ($2);
		  ( (QMerge*) q )->setProjection(cl_projection);
		  
		  $$ = q;
		};

commit_statement: 
		COMMIT {
		  Query* q = QCommit::create();
		 		  
		  $$ = q;
		};

delete_statement_searched:
		DELETE FROM projection_list opt_where_clause {
		  ListProjections* cl_from = dynamic_cast<ListProjections*>( $3 );
		  BExpression*  cl_where = dynamic_cast<BExpression*>( $4 );
		  
		  Query* q;
		  //cl_from check type .......?????
		  cl_where->typeCheck(cl_from);
		  q = QDelete::create();
		  
		  list<Projection*> projections = cl_from->getProjections();
		
		  if (projections.size() > 0)
		  {
			( (QDelete*) q)->setProjection(projections.front());
		  }
		  ( (QDelete*) q)->setWherePredicate(cl_where);
		  
		  $$ = q;
		};

insert_statement:
		INSERT INTO proj_exp opt_column_commalist
		VALUES '(' insert_atom_commalist ')' {
		  Projection* cl_into = dynamic_cast<Projection*>($3);
		  ListCols* cl_columns = dynamic_cast<ListCols*>($4);
		  ListExps* cl_values = dynamic_cast<ListExps*>($7);
		  ListColumnValues* cl_column_values = ListColumnValues::create();
		 
		  CatalogInstance* lpCatalog = NULL;
		  lpCatalog = CatalogInstance::getCatalog();
		  if (lpCatalog == NULL)
		    parser_error("Unable to create Catalog.");
		  
		  // dealing with empty opt_column_commalist
		  list<EColumn*> columns = cl_columns->get();
		  if (columns.size() == 0) {
		    vector<string>* lpListColumnNames = lpCatalog->getColumnNames(cl_into->get());
		    for(vector<string>::iterator i = lpListColumnNames->begin();
			i != lpListColumnNames->end(); ++i) {
		      EColumn* col = EColumn::create(cl_into->get(), *i); 
			  
		      cl_columns->add(col);
		    }
		  }
		  // check if all fields in cl_columns are indeed in cl_into
		  ListProjections* cl_into_list = ListProjections::create();
		 
		  cl_into_list->add(cl_into);
		  cl_columns->typeCheck(cl_into_list, NULL);
		  // cl_olumn_values and cl_columns check
		  cl_column_values->setColumns(cl_columns);
		  cl_column_values->setValues(cl_values);
		  cl_column_values->typeCheck(cl_into);
		  
		  Query* q = QInsert::create();
		  		  
		  ( (QInsert*) q)->setProjection(cl_into);
		  ( (QInsert*) q)->setColumnValues(cl_column_values);
		  //delete lpCatalog;
		  $$ = q;
		}
                |
		INSERT INTO proj_exp opt_column_commalist
		query_spec {
		  Projection* cl_into = dynamic_cast<Projection*>($3);
		  ListCols* cl_columns = dynamic_cast<ListCols*>($4);
		  Query* cl_query = dynamic_cast<Query*>($5);
		  ListColumnValues* cl_column_values = ListColumnValues::create();
		  
		  CatalogInstance* lpCatalog = NULL;
		  lpCatalog = CatalogInstance::getCatalog();
		  if (lpCatalog == NULL)
		    parser_error("Unable to create Catalog.");

		  // dealing with empty opt_column_commalist
		  list<EColumn*> columns = cl_columns->get();
		  if (columns.size() == 0) {
		    vector<string>* lpListColumnNames = lpCatalog->getColumnNames(cl_into->get());
		    for(vector<string>::iterator i = lpListColumnNames->begin();
			i != lpListColumnNames->end(); ++i) {
		      EColumn* col = EColumn::create(cl_into->get(), *i); 
			  
		      cl_columns->add(col);
		    }
		  }
		  // check if all fields in cl_columns are indeed in cl_into
		  ListProjections* cl_into_list = ListProjections::create();
		 
		  cl_into_list->add(cl_into);
		  cl_columns->typeCheck(cl_into_list, NULL);

		  // checking for types
		  cl_column_values->setColumns(cl_columns);
		  ListSelections* cl_selections = ( (QSelect*) cl_query)->getSelections();
		  list<EHolder*> list_selections = cl_selections->getSelections();
		  list<EColumn*> list_columns = cl_columns->get();
		  if (list_columns.size() != list_selections.size())
		    parser_error("Invalid INSERT statement: sizes are not equal.");
		  list<EHolder*>::iterator i = list_selections.begin();
		  list<EColumn*>::iterator j = list_columns.begin();
		  for (; i != list_selections.end();) {
		    EHolder* eholder = *i;
		    Expression* expression = eholder->getExpression();
		    EColumn* column = *j;
		    if (lpCatalog->getColumnType(column->getName()) != expression->getType(cl_into_list)) {
		      parser_error("Invalid INSERT statement: types are not equivalent.");
		    }
		    i++;j++;
		  }

		  Query* q = QInsert::create();
				  
		  ( (QInsert*) q)->setProjection(cl_into);
		  ( (QInsert*) q)->setColumnValues(cl_column_values);
		  ( (QInsert*) q)->setSelectQuery(cl_query);
		  //delete lpCatalog;
		  $$ = q;
		};

opt_column_commalist:
		/* empty */ {
		  ListCols* lc = ListCols::create();
		  $$ = lc;
		}
		|
		'(' column_commalist ')' {
		  // ListCols* lc = dynamic_cast<ListCols*>( $2 );
		  $$ = $2;
		};

column_commalist:
		column {
		  ListCols* lc =  ListCols::create();
		  
		  EColumn* c = dynamic_cast<EColumn*>($1);
		  lc->add(c);
		  
		  $$ = lc;
		}
		|
		column_commalist ',' column {
		  ListCols* lc = dynamic_cast<ListCols*>($1);
		  EColumn* c = dynamic_cast<EColumn*>($3);
		  lc->add(c);

		  $$ = lc;
 		};

column:
		NAME {
			EColumn* lp=  EColumn::create($1->toString());
			$$ = lp;
			
		};


insert_atom_commalist:
		insert_atom {
		  ListExps* cl_listexps = ListExps::create();
		  Expression* cl_expression = dynamic_cast<Expression*>($1);
		  cl_listexps->add(cl_expression);
		  $$ = cl_listexps;
		}
		|
		insert_atom_commalist ',' insert_atom {
		  ListExps* cl_listexps = dynamic_cast<ListExps*>($1);
		  Expression* cl_expression = dynamic_cast<Expression*>($3);
		  cl_listexps->add(cl_expression);
		  $$ = cl_listexps;
		};
		
insert_atom:
		atom {
		  $$ = $1;
		}
		|
		NULLX {
			EString* lp = EString::create("NULL");
			$$ = lp;
			
		};

atom:
		literal {
		  $$ = $1;
		};

query_spec:
		SELECT selection FROM projection_list opt_where_clause opt_ordering {
			ListSelections* cl_select = dynamic_cast<ListSelections*>( $2 );
			ListProjections* cl_from = dynamic_cast<ListProjections*>( $4 );
			BExpression* cl_where = dynamic_cast<BExpression*>( $5 );
			ListOrdering* cl_order_by = dynamic_cast<ListOrdering*>( $6 );
			
			Query* q;
			cl_select->setProjectionList( cl_from );
			cl_select->typeCheck();
			cl_order_by->typeCheck( cl_from );
			//cl_order_by->isValid(cl_select, cl_from);
			cl_where->typeCheck( cl_from );
			q = QSelect::create();
			( (QSelect*) q)->setQueryComponents(cl_select, cl_from, cl_where, NULL, NULL, cl_order_by);
			// Adds one predicate "AND deleted = 0"
			( (QSelect*) q)->addNotDeletedPredicate();
			
			$$ = q;
		}
                |	
		SELECT selection FROM projection_list opt_where_clause GROUP BY group_by_clause opt_having_clause opt_ordering {
			ListSelections* cl_select = dynamic_cast<ListSelections*>( $2 );
			ListProjections* cl_from = dynamic_cast<ListProjections*>( $4 );
			BExpression* cl_where = dynamic_cast<BExpression*>( $5 );
			ListCols* cl_group_by = dynamic_cast<ListCols*>( $8 );
			BExpression* cl_having = dynamic_cast<BExpression*>( $9 );
			ListOrdering* cl_order_by = dynamic_cast<ListOrdering*>( $10 );

			Query* q;
			cl_select->setProjectionList( cl_from );
			cl_select->typeCheck();
			cl_order_by->typeCheck( cl_from );
			//cl_order_by->isValid(cl_select, cl_from);
			cl_having->typeCheck( cl_from );
			cl_group_by->typeCheck( cl_from, cl_select );
			//cl_group_by->isValid(cl_select, cl_from);
			cl_where->typeCheck( cl_from );
			q = QSelect::create();
			( (QSelect*) q)->setQueryComponents(cl_select, cl_from, cl_where, cl_group_by, cl_having, cl_order_by);
			// Adds one predicate "AND deleted = 0"
			( (QSelect*) q)->addNotDeletedPredicate();
			
			$$ = q;
		};

selection:
		scalar_exp_commalist	{ $$ = $1; }
	|	'*'		{
			ListSelections* ls = ListSelections::create();
			ls->setSelectAll( );
			$$ = ls;
		}
	;

scalar_exp_commalist:
		expression	{
			ListSelections* ls = ListSelections::create();
			Expression* l = dynamic_cast<Expression*>($1);
			EHolder* eh = EHolder::create( l );
			
			if( l->isAggregate() ) {
				ls->addAggregate( eh );
			} else {
				ls->addSelection( eh );
			}
			$$ = ls;
		}
	|	expression AS NAME	{
			ListSelections* ls = ListSelections::create();
			Expression* l = dynamic_cast<Expression*>($1);
			EHolder* eh = EHolder::create( l, dynamic_cast<PIdent*>( $3 )->toString() );			
			if( l->isAggregate() ) {
				ls->addAggregate( eh );
			} else {
				ls->addSelection( eh );
			}
			$$ = ls;
		}
	|	scalar_exp_commalist ',' expression	{
			ListSelections* ls = dynamic_cast<ListSelections*>($1);
			Expression* l = dynamic_cast<Expression*>($3);
			EHolder* eh = EHolder::create( l );
			
			if( l->isAggregate() ) {
				ls->addAggregate( eh );
			} else {
				ls->addSelection( eh );
			}
			$$ = $1;
		}
	|	scalar_exp_commalist ',' expression AS NAME	{
			ListSelections* ls = dynamic_cast<ListSelections*>($1);
			Expression* l = dynamic_cast<Expression*>($3);
			PIdent* alias = dynamic_cast<PIdent*>($5);
			EHolder* eh = EHolder::create( l, alias->toString() );
			
			
			
			
			if( l->isAggregate() ) {
				ls->addAggregate( eh );
			} else {
				ls->addSelection( eh );
			}
			$$ = $1;
		}
	;

expression:
		scalar_exp { $$ = $1; }
	|	aggregate_function { $$ = $1; }
	;

scalar_exp:
		scalar_exp '+' scalar_exp	{
							Expression* l = dynamic_cast<Expression*>( $1 );
							Expression* r = dynamic_cast<Expression*>( $3 );
							EPlus* lp = EPlus::create( l, r );
							
							$$ = lp;
							
						}
	|	scalar_exp '-' scalar_exp	{
							Expression* l = dynamic_cast<Expression*>( $1 );
							Expression* r = dynamic_cast<Expression*>( $3 );
							EMinus* lp = EMinus::create( l, r );
							
							$$ = lp;
							
							
						}
	|	scalar_exp '*' scalar_exp	{
							Expression* l = dynamic_cast<Expression*>( $1 );
							Expression* r = dynamic_cast<Expression*>( $3 );
							EMult* lp = EMult::create( l, r );
							
							$$ = lp;
							
							
						}
	|	scalar_exp '/' scalar_exp	{
							Expression* l = dynamic_cast<Expression*>( $1 );
							Expression* r = dynamic_cast<Expression*>( $3 );
							EDiv* lp = EDiv::create( l, r );
							
							$$ = lp;
							
							
						}
	|	'+' scalar_exp %prec UMINUS	{ $$ = $2; }
	|	'-' scalar_exp %prec UMINUS	{
							Expression* r = dynamic_cast<Expression*>($2);
							EInteger* eint = EInteger::create( 0 );
							EMinus* lp = EMinus::create(eint , r );
							
							$$ = lp;
							
							
						}
	|	literal				{ $$ = $1; }
	|	column_ref			{ $$ = $1; }
	|	'(' scalar_exp ')'		{ $$ = $2; }
	;

literal:
		STRING		{ $$ = $1; }
	|	INTNUM		{ $$ = $1; }
	|	FLOATNUM	{ $$ = $1; }
	;

column_ref:
		NAME		{ 
						EColumn* lp = EColumn::create( $1->toString() );
						
						 $$ = lp;
						
					}
	|	NAME '.' NAME	{ 
							EColumn* lp = EColumn::create( $1->toString(), $3->toString() ); 
							$$ = lp;
							
						}
	;

aggregate_function:
		AGG_COUNT '(' '*' ')'	{ 
									EAggCount* lp = EAggCount::create( NULL );
									$$ = lp;
									
								}
	|	AGG_COUNT '(' column_ref ')' { 
										EAggCount* lp = EAggCount::create( dynamic_cast<EColumn*>($3) );
										$$ = lp;
										
									}
	|	AGG_MIN '(' column_ref ')' { 
										EAggMin* lp = EAggMin::create( dynamic_cast<EColumn*>($3) );
										$$ = lp;
										
									}
	|	AGG_MAX '(' column_ref ')' { 
										EAggMax* lp = EAggMax::create( dynamic_cast<EColumn*>($3) );
										$$ = lp;
										
									}
	|	AGG_SUM '(' column_ref ')' { 
										EAggSum* lp = EAggSum::create( dynamic_cast<EColumn*>($3) ); 
										$$ = lp;
										
									}
	|	AGG_AVG '(' column_ref ')' { 
										EAggAvg* lp = EAggAvg::create( dynamic_cast<EColumn*>($3) );
										$$ = lp;
										
									}

boolean_literal:
		BOOL_TRUE {
					BBool* lp = BBool::create( 1 ); 
					$$ = lp;
					 
					}
	|	BOOL_FALSE { 
					BBool* lp = BBool::create( 0 );  
					$$ = lp;
					
					}
	;

projection_list:
		proj_exp {
			ListProjections* lp = ListProjections::create();
			
			lp->add( dynamic_cast<Projection*>($1) );
			$$ = lp;
		}
	|	projection_list ',' proj_exp {
			dynamic_cast<ListProjections*>($1)->add( dynamic_cast<Projection*>($3) );
			$$ = $1;
		}

proj_exp:
		NAME		{ 
						Projection* lp = Projection::create( $1->toString() ); 
						$$ = lp;
						
					}
        |       NAME AS NAME    { 
						Projection* lp = Projection::create( $1->toString(), $3->toString() );
						$$ = lp;
						
						 }
        |	NAME NAME	{ 
							Projection* lp = Projection::create( $1->toString(), $2->toString() );
							$$ = lp;
							
						} /* Rename projection  */
	;

opt_where_clause:
		/* empty */ {
			BBool* lp = BBool::create( 1 );
			$$ = lp;
			
		}
	|	where_clause { $$ = $1; }
	;

where_clause:
		WHERE search_condition { $$ = $2; }
	;

search_condition:
	|	search_condition OR search_condition	{
								BExpression* l = dynamic_cast<BExpression*>($1);
								BExpression* r = dynamic_cast<BExpression*>($3);
								BDisjunction* lp = BDisjunction::create( l, r );
								$$ = lp;
								
							}
	|	search_condition AND search_condition	{
								BExpression* l = dynamic_cast<BExpression*>($1);
								BExpression* r = dynamic_cast<BExpression*>($3);
								BConjunction* lp = BConjunction::create( l, r );
								$$ = lp;
								
							}
	|	NOT search_condition			{
								BExpression* r = dynamic_cast<BExpression*>($2);
								BNegation* lp = BNegation::create( r );
								$$ = lp;
								
							}
	|	boolean_literal { $$ = $1; }
	|	'(' search_condition ')' { $$ = $2; }
	|	predicate { $$ = $1; }
	;

predicate:
		comparison_predicate { $$ = $1; }
	|	between_predicate { $$ = $1; }
	|	test_for_null { $$ = $1; }
	;

comparison_predicate:
		expression OP_EQ expression	{
				Expression* l = dynamic_cast<Expression*>( $1 );
				Expression* r = dynamic_cast<Expression*>( $3 );
				BEq* lp =  BEq::create( l, r );
				$$ = lp;
				
			}
	|	expression OP_NE expression	{
				Expression* l = dynamic_cast<Expression*>( $1 );
				Expression* r = dynamic_cast<Expression*>( $3 );
				BNe* lp =  BNe::create( l, r );
				$$ = lp;
				
			}
	|	expression OP_LT expression	{
				Expression* l = dynamic_cast<Expression*>( $1 );
				Expression* r = dynamic_cast<Expression*>( $3 );
				BLt* lp =  BLt::create( l, r );
				$$ = lp;
				
			}
	|	expression OP_GT expression	{
				Expression* l = dynamic_cast<Expression*>( $1 );
				Expression* r = dynamic_cast<Expression*>( $3 );
				BGt* lp =  BGt::create( l, r );
				$$ = lp;
				
			}
	|	expression OP_LE expression	{
				Expression* l = dynamic_cast<Expression*>( $1 );
				Expression* r = dynamic_cast<Expression*>( $3 );
				BLe* lp =  BLe::create( l, r );
				$$ = lp;
				
			}
	|	expression OP_GE expression	{
				Expression* l = dynamic_cast<Expression*>( $1 );
				Expression* r = dynamic_cast<Expression*>( $3 );
				BGe* lp =  BGe::create( l, r );
				$$ = lp;
				
			}
	;

between_predicate:
	expression NOT BETWEEN expression AND expression {
	  Expression* a = dynamic_cast<Expression*>( $1 );
	  Expression* b = dynamic_cast<Expression*>( $4 );
	  Expression* c = dynamic_cast<Expression*>( $6 );
	  BGt* gt = BGt::create( a, b );
	  BLt* lt = BLt::create( a, c );
	  BConjunction* conj = BConjunction::create( gt, lt );
	  BNegation* bneg = BNegation::create( conj );
	  $$ = bneg;
	}
	|
	expression BETWEEN expression AND expression {
	  Expression* a = dynamic_cast<Expression*>( $1 );
	  Expression* b = dynamic_cast<Expression*>( $3 );
	  Expression* c = dynamic_cast<Expression*>( $5 );
	  BGt* gt = BGt::create( a, b );
	  BLt* lt = BLt::create( a, c );
	  BConjunction* conj = BConjunction::create( gt, lt );
	  $$ = conj;
	};

	/* NULLX used because other things use the constant NULL */

test_for_null:
		column_ref IS NOT NULLX	{
						EColumn* col = dynamic_cast<EColumn*>( $1 );
						BIsNull* bIsNull = BIsNull::create( col );
						BNegation* lpNot = BNegation::create( bIsNull );
						$$ = lpNot;
					}
	|	column_ref IS NULLX	{
						EColumn* col = dynamic_cast<EColumn*>( $1 );
						BIsNull* lpNull = BIsNull::create( col );
						$$ = lpNull;
					}
	;

group_by_clause:
		column_ref	{
			ListCols* group = ListCols::create();
			group->add( dynamic_cast<EColumn*>( $1 ) );
			$$ = group;
		}
	|	group_by_clause ',' column_ref {
			dynamic_cast<ListCols*>($1)->add( dynamic_cast<EColumn*>( $3 ) );
			$$ = $1;
		}
	;

opt_having_clause:
		/* empty */	{ 
						BBool* lp = BBool::create( 1 );  
						$$ = lp;
						
					}
	|	HAVING search_condition { $$ = $2; }
	;

opt_ordering:
		/* empty */	{ 
						ListOrdering* lp = ListOrdering::create(); 
						$$ = lp;
						
					}
	|	ORDER BY ordering_spec_commalist { $$ = $3; }
	;

ordering_spec_commalist:
		ordering_spec	{
			ListOrdering* lo = ListOrdering::create();
			lo->add( dynamic_cast<ColOrder*>($1) );
			$$ = lo;
		}
	|	ordering_spec_commalist ',' ordering_spec {
			dynamic_cast<ListOrdering*>($1)->add( dynamic_cast<ColOrder*>($3) );
			$$ = $1;
		}
	;

ordering_spec:
		column_ref { 
						ColOrder* lp = ColOrder::create( dynamic_cast<EColumn*>( $1 ), 0 ); 
						$$ = lp;
						
					}
	|	column_ref ASC { 
							ColOrder* lp = ColOrder::create( dynamic_cast<EColumn*>( $1 ), 0 );
							$$ = lp;
							
						}
	|	column_ref DESC { 
							ColOrder* lp = ColOrder::create( dynamic_cast<EColumn*>( $1 ), 1 );
							$$ = lp;
							
						}
	;
	
%%
