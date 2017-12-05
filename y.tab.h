/* A Bison parser, made by GNU Bison 3.0.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    INTEGER = 258,
    NAME = 259,
    LITERAL = 260,
    EE = 261,
    GE = 262,
    LE = 263,
    NE = 264,
    AA = 265,
    OO = 266,
    NEWLINE = 267,
    CREATE = 268,
    TABLE = 269,
    WHERE = 270,
    DELETE = 271,
    FROM = 272,
    INTO = 273,
    INSERT = 274,
    SELECT = 275,
    VALUES = 276,
    DROP = 277,
    ORDER = 278,
    BY = 279,
    INT_L = 280,
    STR20_L = 281,
    AND = 282,
    OR = 283,
    DISTINCT = 284,
    NUL = 285
  };
#endif
/* Tokens.  */
#define INTEGER 258
#define NAME 259
#define LITERAL 260
#define EE 261
#define GE 262
#define LE 263
#define NE 264
#define AA 265
#define OO 266
#define NEWLINE 267
#define CREATE 268
#define TABLE 269
#define WHERE 270
#define DELETE 271
#define FROM 272
#define INTO 273
#define INSERT 274
#define SELECT 275
#define VALUES 276
#define DROP 277
#define ORDER 278
#define BY 279
#define INT_L 280
#define STR20_L 281
#define AND 282
#define OR 283
#define DISTINCT 284
#define NUL 285

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE YYSTYPE;
union YYSTYPE
{
#line 15 "typ.y" /* yacc.c:1909  */

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
    
    

#line 148 "y.tab.h" /* yacc.c:1909  */
};
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
