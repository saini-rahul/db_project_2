#include <iostream>
#include<conio.h>
#include "calsi.h"
using namespace std;
#define YES 1
#define NO 0

number::number(int a)
{
     this->data=a;
}

ope::ope(char c)
{
     this->ch=c;
}
 
ope::ope()
{
    ch='\0';
}
            
expression::expression()
{
}

expression::expression(expression *a,char c,expression *b)
{
     ope op(c);
     mid=new ope();
     left=new expression();
     right=new expression();
     (*mid)=op;
     left=a;
     right=b;
     return;
}



statement::statement(expression *e)
{
      next=new expression();
      (*next)=(*e);
      return;
}

int statement::eval()
{
             int f=next->eval1();
	     return f;
             
}      
