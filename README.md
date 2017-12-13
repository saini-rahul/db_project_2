#Introduction

Project 2 for CSCE 608 Database Systems, Fall 2017.
Github Repository located at: https://github.com/caffrey26/db_project_2

*************************
** Authors             **
*************************
Sambartika Guha 
Rahul Saini


*************************
** Build Project       **
*************************
In a terminal window with the directory changed to project directory, run the following command:
> flex typ.l
> bison -dy typ.y
> g++ -std=c++11 lex.yy.c y.tab.c *.cpp -o a.exe

This will produce the executable file in a.out


*************************
** Run Project         **
*************************

> ./a.out
> #input queries here ...
> exit
