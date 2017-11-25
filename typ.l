%{
#include <stdio.h>
#include "typ.h"
#include "y.tab.h"
//extern "C" int yylex();
%}

%%

"{" { return yytext[0]; }
"}" { return yytext[0]; }
"(" { return yytext[0]; }
")" { return yytext[0]; }
"[" { return yytext[0]; }
"]" { return yytext[0]; }
"+" { return yytext[0]; }
"-" { return yytext[0]; }
"*" { return yytext[0]; }
"/" { return yytext[0]; }
"=" { return yytext[0]; }
"!" { return yytext[0]; }
"." { return yytext[0]; }
">" { return yytext[0]; }
"<" { return yytext[0]; }
";" { return yytext[0]; }
"," { return yytext[0]; }
"\"" { return yytext[0]; }
"\'" { return yytext[0]; }

"==" { return EE; }
">=" { return GE; }
"<=" { return LE; }
"!=" { return NE; }
"&&" { return AA; }
"||" { return OO; }

"CREATE" { return CREATE; }
"TABLE" { return TABLE; }
"WHERE" { return WHERE; }
"DELETE" { return DELETE; }
"DISTINCT" { return DISTINCT; }
"FROM" { return FROM; }
"SELECT" { return SELECT; }
"INTO" { return INTO; }
"VALUES" { return VALUES; }
"DROP" { return DROP; }
"ORDER" { return ORDER; }
"BY" { return BY; }
"INSERT" { return INSERT; }
"NULL" { return NUL; }
"OR" { return OR; }
"AND" { return AND; }
"INT" { return INT_L; }
"STR20" { return STR20_L; }
"EOL" { return 0; }

[\"][a-zA-Z0-9!#\$%&'\(\)\*\+,-\./:;<=>\?@\[\]\^_`\{\|\}]+[\"] { 
yylval.sttr1=(char *)yytext;
int i=0;
for(i=0; yylval.sttr1[i+2]!= '\0'; i++)
{
    yylval.sttr1[i] = yylval.sttr1[i+1];   
}
yylval.sttr1[i]= '\0';
return LITERAL; 
}

[a-zA-Z][a-zA-Z0-9]* { 
yylval.sttr = (char *)yytext;
cout<<"here is "<<yylval.sttr<<endl;
return NAME; }

[0-9]+ { yylval.d=atoi(yytext);
         return(INTEGER);
        }
[\t]+ ;
[\n]+ { 
return NEWLINE; };  



%%


int yywrap()
{
    return 1;
}

