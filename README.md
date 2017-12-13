# Introduction

Project 2 for CSCE 608 Database Systems, Fall 2017.
Github Repository located at: https://github.com/caffrey26/db_project_2

This course project was intended to design and implement a simple SQL (called Tiny-SQL) interpreter.
The interpreter accepts SQL queries that are valid in terms of Tiny-SQL grammar, execute queries, and output results. 
As a part of this project we wrote: 
1. Parser: accepts the input Tiny-SQL query and converts it into a parse tree. 
2. Expression tree: the logical query plan generator converts a parse tree into a logical query plan, or expression tree. This phase also includes any possible logical query plan optimization.  
3. A physical query plan generator, which converts optimized logical query plans into executable physical query plans. This phase also includes any possible physical query plan optimization. •
4. A set of subroutines that implement a variety of data query operations necessary for execution of queries in Tiny-SQL. The subroutines use the released library StorageManager, provided for this project, which simulates computer disks and memory. 

## Authors             
Sambartika Guha (sambartika.guha @ tamu . edu)

Rahul Saini (rssaini @ tamu . edu)


## Build Project
In a terminal window with the directory changed to project directory, run the following command:
> flex typ.l

> bison -dy typ.y

> g++ -std=c++11 lex.yy.c y.tab.c *.cpp -o a.exe

This will produce the executable file in a.out

### Run Project        

> ./a.out

> #input queries here ...


