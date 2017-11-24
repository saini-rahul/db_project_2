%{
#include<stdio.h>
#include "db_manager.h"
extern int  yyparse();
extern int yylex_destroy(void);

int yylex(void);
Statement_list *root;
void yyerror(const char *s);

%}

%union{
    int d;
    char *sttr;
    char *sttr1;
    Statement_list *st_ls;
    Statement *st;
    Create_statement *cr_st;
    Attribute_type_list *at_tp_ls;
    Drop_statement *dr_st;
    Select_statement *sl_st;
    Select_statement_rest *sl_rs;
    Select_list *sl_li;
    Select_sublist *sl_sli;
    Table_list *tb_st;
    Delete_statement *dl_st;
    Insert_statement *is_st;
    Attribute_list *at_ls;
    Search_condition *sr_cd;
    Boolean_term *bl_tr;
    Boolean_factor *bl_fc;
    Expression *exp;
    Term *tr;
    Insert_touples *is_tp;
    Value *vl;
    Value_list *vl_ls;
    Comp_op *cm_op;
    Table_name *tb_nm;
    Attribute_name * att_nm;
    Column_name *cl_nm;
    Data_type *dt_tp;
    
    };

%token <d> INTEGER
%token <sttr> NAME
%token <sttr1> LITERAL
%token EE GE LE NE AA OO NEWLINE
%token CREATE TABLE WHERE DELETE FROM INTO INSERT SELECT
%token VALUES DROP ORDER BY INT_L STR20_L AND OR DISTINCT NUL

%type<st_ls>statement_list
%type<st>statement
%type<cr_st>create_statement
%type<at_tp_ls>attribute_type_list
%type<dr_st>drop_statement
%type<sl_st>select_statement
%type<sl_rs>select_statement_rest
%type<sl_li>select_list
%type<sl_sli>select_sublist
%type<tb_st>table_list
%type<dl_st>delete_statement
%type<is_st>insert_statement
%type<at_ls>attribute_list
%type<sr_cd>search_condition
%type<bl_tr>boolean_term
%type<bl_fc>boolean_factor
%type<exp>expression
%type<tr>term
%type<is_tp>insert_touples
%type<vl>value
%type<vl_ls>value_list
%type<cm_op>comp_op
%type<tb_nm>table_name
%type<att_nm>attribute_name
%type<cl_nm>column_name
%type<dt_tp>data_type

%%

statement_list:
statement
{ root= new Statement_list($1); 
return 1;
};

statement:
create_statement 
{ $$= new Statement($1); }
| drop_statement
{ $$= new Statement($1); }
| select_statement 
{ $$= new Statement($1); }
| insert_statement
{ $$= new Statement($1); }
| delete_statement
{ $$= new Statement($1); }
;

create_statement:
CREATE TABLE table_name '(' attribute_type_list ')' 
{ $$= new Create_statement($3, $5); }
;

attribute_type_list:
attribute_name data_type
{ $$= new Attribute_type_list($1, $2); } 
|  attribute_name data_type ',' attribute_type_list
{ $$= new Attribute_type_list($1, $2, $4); }
;

drop_statement:
DROP TABLE table_name 
{ $$= new Drop_statement($3); }
;

select_statement:
SELECT DISTINCT select_statement_rest
{ $$= new Select_statement( $3, "DISTINCT" ); }
| SELECT select_statement_rest
{ $$= new Select_statement($2); }

;

select_statement_rest:
select_list FROM table_list 
{ $$= new Select_statement_rest($1, $3); 
}
| select_list FROM table_list WHERE search_condition 
{ $$= new Select_statement_rest($1, $3, $5); }
| select_list FROM table_list WHERE search_condition ORDER BY column_name
{ $$= new Select_statement_rest($1, $3, $5, $8); }
;

select_list:
'*'
{ $$= new Select_list('*'); }
| select_sublist
{ $$= new Select_list($1); }
;

select_sublist:
column_name 
{ $$= new Select_sublist($1); }
| column_name ',' select_sublist
{ $$= new Select_sublist($1, $3); }
;

table_list:
table_name
{ $$= new Table_list($1); }
| table_name ',' table_list
{ $$= new Table_list($1, $3); }
;


delete_statement:
DELETE FROM table_name 
{ $$= new Delete_statement($3); }
| DELETE FROM table_name WHERE search_condition
{ $$= new Delete_statement($3, $5); }
;

insert_statement:
INSERT INTO table_name '('attribute_list')' insert_touples
{ $$= new Insert_statement($3, $5, $7); }
;

attribute_list:
attribute_name 
{ $$= new Attribute_list($1); }
| attribute_name',' attribute_list
{ $$= new Attribute_list($1, $3); }
;

search_condition:
boolean_term 
{ $$= new Search_condition($1); }
| boolean_term OR search_condition
{ $$= new Search_condition($1, $3); }
;

boolean_term:
boolean_factor 
{ $$= new Boolean_term($1); }
| boolean_factor AND boolean_term
{ $$= new Boolean_term($1, $3); }
;

boolean_factor:
expression comp_op expression
{ $$= new Boolean_factor($1, $2, $3); }
;

expression:
term
{ $$= new Expression($1); }
| '('term '+' term')'
{ $$= new Expression($2, '+', $4); }
| '('term '-' term')'
{ $$= new Expression($2, '-', $4); }
| '('term '*' term')'
{ $$= new Expression($2, '*', $4); }
;

term:
INTEGER
{ cout<<"sam"<<endl;
$$= new Term($1); }
| NAME
{ cout<<"sam1111"<<endl;
$$= new Term($1); }
| LITERAL
{ cout<<"sam1111"<<endl;
$$= new Term($1);
};

insert_touples:
 VALUES '('value_list')' 
{ $$= new Insert_touples($3); }
| select_statement
{ $$= new Insert_touples($1); }
;


value:
INTEGER 
{ $$= new Value($1); }
| NUL
{ $$= new Value("NULL"); }
| LITERAL
{ $$= new Value($1); }
;

value_list:
value 
{ $$= new Value_list($1); }
| value',' value_list
{ $$= new Value_list($1, $3); }
;

comp_op:
'>' 
{ $$= new Comp_op('>'); }
| '<' 
{ $$= new Comp_op('<'); }
| '='
{ $$= new Comp_op('='); }
;

table_name:
NAME
{ $$= new Table_name($1); }
;

attribute_name:
NAME 
{ 
$$= new Attribute_name($1); }
;

column_name:
attribute_name 
{ $$= new Column_name($1); }
| table_name'.'attribute_name
{ $$= new Column_name($1, $3); }
;

data_type:
INT_L
{ $$= new Data_type("INT"); } 
| STR20_L
{ $$= new Data_type("STR20"); }
;


%%
int main()
{
 MainMemory mem;
 Disk disk;
 db_manager dbManager(&mem, &disk);
 cout<<"main here"<<endl;
 
 while(1)
 {
	yyparse();
	if(root != '\0')
	    root -> printFunc();
	else
	    cout<<"end of print root null"<<endl;
	
	string result = dbManager.process_statement(root) == 0? "FAILURE" : "SUCCESS";  
	cout<<result<<endl;
	
	yylex_destroy();
 }
 return 0;
}


void yyerror(const char * s)
{
  fprintf(stderr, "Line :%s\n",s);
  yylex_destroy();
  yyparse();
}


