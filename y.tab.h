/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

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
    EE = 260,
    GE = 261,
    LE = 262,
    NE = 263,
    AA = 264,
    OO = 265,
    NEWLINE = 266,
    CREATE = 267,
    TABLE = 268,
    WHERE = 269,
    DELETE = 270,
    FROM = 271,
    INTO = 272,
    INSERT = 273,
    SELECT = 274,
    VALUES = 275,
    DROP = 276,
    ORDER = 277,
    BY = 278,
    INT = 279,
    STR20 = 280,
    AND = 281,
    OR = 282,
    DISTINCT = 283,
    NUL = 284
  };
#endif
/* Tokens.  */
#define INTEGER 258
#define NAME 259
#define EE 260
#define GE 261
#define LE 262
#define NE 263
#define AA 264
#define OO 265
#define NEWLINE 266
#define CREATE 267
#define TABLE 268
#define WHERE 269
#define DELETE 270
#define FROM 271
#define INTO 272
#define INSERT 273
#define SELECT 274
#define VALUES 275
#define DROP 276
#define ORDER 277
#define BY 278
#define INT 279
#define STR20 280
#define AND 281
#define OR 282
#define DISTINCT 283
#define NUL 284

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 15 "typ.y" /* yacc.c:1909  */

    int d;
    char *sttr;
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
    
    

#line 145 "y.tab.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
