#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

char arr1[50],arr3[100],post[100],temp[100]={'\0'};
int nno=1,start=1,end=1;

typedef struct NFA{
               int nfarr[25];
               int no;
               char ch;
               struct NFA *next;

}NFA;

NFA *nfaarr[100]={NULL};

typedef struct stack
{
    char data[100];
    int top;
}stack;

typedef struct stack1
{
    NFA *data1[100];
    int top1;
}stack1;


int isempty(stack *s)
{
    return (s->top == -1) ? 1 : 0;
}

void emptystack(stack* s)
{
    s->top=-1;
}

void push(struct stack *s,char item)
{
    if(s->top == 99)
    {
        printf("\nSTACK FULL");
    }
    else
    {
        ++s->top;
        s->data[s->top]=item;
    }
}

char pop(struct stack* s)
{
    int a=-1;
    char pp=(char)a,q='n';
    if(!isempty(s))
    {
        pp= s->data[s->top];
        --s->top;
        return pp;    
    }
    
    else
    return q;
}



int isoperator(char e)
{
    if(e == '|' || e == '.' || e == '*')
        return 1;
    else
        return 0;
}

int priority(char e)
{
    int pri = 0;
    if(e == '|')
        pri = 1;
    else if(e=='.')
        pri=1;
    else if(e=='*')
        pri=1;
    return pri;
}

void build_nfa(char result)
{
     NFA *t,*q,*t1;
     int l=0,k=0,i=0;
     t=(NFA *)malloc(sizeof(NFA));
     t->no=nno;
     t->ch='\0';
     t->next=NULL;
     nfaarr[nno]=t;
     nno++;

     t1=(NFA *)malloc(sizeof(NFA));
     t1->no=nno;
     t1->ch='\0';
     t1->next=NULL;
     nfaarr[nno]=t1;
     end=nno;

     q=(NFA *)malloc(sizeof(NFA));
     q->ch=result;
     q->nfarr[i++]=nno;
     q->no=i;
     q->next=NULL;
     nfaarr[nno-1]->next=q;
     nno++;
}
	

void display()
{
    NFA *q=NULL,*p=NULL;
    int i=0,j=0;
    for(i=1;i<=nno;i++)
    {
	if(nfaarr[i]!=NULL)
	{
		printf("%d->",i);
                q=nfaarr[i]->next;
                while(q!=NULL)
                {
			printf("%c/",q->ch);
                        for(j=0;j<q->no;j++)
			{
				printf("%d ",q->nfarr[j]);
                        }
                        q=q->next;
			printf("\t");
		}
	}
        printf("\n");
    }
}
			         


int search(int a,NFA *q)
{
     NFA *p,*r,*t;
     int i=0,j=0,k=0,flag=0,flag1=0,l=0;
     p=nfaarr[a]->next;
     while(p!=NULL)
     {
         if(p->ch==q->ch)
         {
                flag1=1;
                j=p->no;
                k=q->no;
                flag=0;
		for(i=0;i<k;i++)
                {
		    for(l=0;l<j;l++)
                    {
			if(p->nfarr[l]==q->nfarr[i])
                        {
                            flag=1;
			    break;
                        }
                    }
                    if(flag==0)
                    p->nfarr[p->no++]=q->nfarr[i];
                    flag=0;
               }
               break;
          }
          p=p->next;
    }
    return flag1;
}

void addextra()
{
    NFA *t1=NULL,*t2=NULL,*p,*q,*t3=NULL;
    int i=0,j=0,k=0,flag=0,flag1=0,l=0,flag2=0,flag3=0,flag4=0;
    for(i=1;i<=end;i++)
    {
        if(nfaarr[i]!=NULL)
	{
	    p=nfaarr[i]->next;
	    while(p!=NULL)
	    {
			flag1=p->no;
			for(j=0;j<flag1;j++)
			{
			    flag=p->nfarr[j];
    			    q=nfaarr[flag]->next;
			    while(q!=NULL)
			    {
				if(q->ch=='#')
				{
				    for(k=0;k<q->no;k++)
				    {
					for(l=0;l<p->no;l++)
                                        {
					    if(p->nfarr[l]==q->nfarr[k])
                                            {
       						flag2=1;
						break;
                                            }
                                        }
                                        if(flag2==0)
					p->nfarr[p->no++]=q->nfarr[k];
					flag2=0;
				    }
				    break;
			        }
				else
				q=q->next;
			    }
			}
			p=p->next;
	    }
	}
    }
   
    for(i=1;i<=end;i++)
    {
        if(nfaarr[i]!=NULL)
	{
	    p=nfaarr[i]->next;
	    while(p!=NULL)
	    {
		if(p->ch=='#')
                {
			flag1=p->no;
			for(j=0;j<flag1;j++)
			{
			    flag=p->nfarr[j];
			    q=nfaarr[flag]->next;
                            t1=NULL;
                            t2=NULL;
			    t3=NULL;
			
			    while(q!=NULL)
			    {
                                if(q->ch!='#')
                                {
                                    flag4=0;
                                    flag4=search(i,q);
                                    if(flag4==0)
                                    {
			              t1=(NFA *)malloc(sizeof(NFA));
				        t1->ch=q->ch;
			                t1->no=q->no;
				        for(l=0;l<q->no;l++)
				        {
				            t1->nfarr[l]=q->nfarr[l];
				        }
				    	t1->next=NULL;
					
				    	if(t3==NULL)
				    	{
				        	t3=t1;
						t2=t1;
				    	}
				    	else
				    	{
						t2->next=t1;
						t2=t1;
				    	}
                                    }
                          	    q=q->next;
				}
				else
	                        {
				    for(l=0;l<q->no;l++)
                                    {
                                        for(k=0;k<p->no;k++)
                                        {
					    if(p->nfarr[k]==q->nfarr[l])
                                            {
						flag3=1;
						break;
                                            }
					}
                                        if(flag3==0)
                            	        p->nfarr[p->no++]=q->nfarr[l];
                                    }
                                    q=q->next;
                            	}
                            }
                          if(t3!=NULL&&flag4!=1)
                          {
                              while(p->next!=NULL)
			      p=p->next;
                              p->next=t3;
                          }
                     
			}
			break;
		}
		else
		{
		    p=p->next;
		}
	    }
	}
    }
}
	
void makeor()
{
     NFA *t1,*t,*s,*r,*q,*p;
     int i=0,flag=0;

     t=(NFA *)malloc(sizeof(NFA));
     t->no=nno;
     t->ch='\0';
     t->next=NULL;
     nfaarr[nno]=t;
     nno++;

     t1=(NFA *)malloc(sizeof(NFA));
     t1->no=nno;
     t1->ch='\0';
     t1->next=NULL;
     nfaarr[nno]=t1;

     s=(NFA *)malloc(sizeof(NFA));
     s->ch='#';
     s->nfarr[i++]=start;
     s->no=i;
     s->nfarr[s->no++]=end-1;
     s->next=NULL;
     nfaarr[nno-1]->next=s;

     i=0;
     s=(NFA *)malloc(sizeof(NFA));
     s->ch='#';
     s->nfarr[i++]=end+2;
     s->no=i;
     s->next=NULL;    
     nfaarr[end-2]->next=s;

     i=0;
     s=(NFA *)malloc(sizeof(NFA));
     s->ch='#';
     s->nfarr[i++]=end+2;
     s->no=i;
     s->next=NULL;    
     nfaarr[end]->next=s;

     nno++;
     end=end+2;
     start=end-1;
}




void makestar()
{
    NFA *p,*q,*r,*s,*t,*t1,*t2,*t3,*t4=NULL,*t5=NULL;
    char c='#';
    int flag=0,i=0,j=0,flag1=0,k=0;
    nno--;
                    
    r=nfaarr[end];
    p=nfaarr[end]->next;
    while(p!=NULL)
    {
        
        if(p->ch=='#')
        {
            p->nfarr[p->no++]=start;
            p->nfarr[p->no++]=end+2;
            flag=1;
	    break;
        }
        else
        {
            p=p->next;
	    r=r->next;
        }
    }
    if(flag==0)
    {
    	q=(NFA *)malloc(sizeof(NFA));
    	q->ch='#';
        q->no=0;
        q->nfarr[q->no++]=start;
        q->nfarr[q->no++]=end+2;
        q->next=NULL;
        r->next=q;
    }
     nno++;

     t=(NFA *)malloc(sizeof(NFA));
     t->no=nno;
     t->ch='\0';
     t->next=NULL;
     nfaarr[nno]=t;
     nno++;

     t1=(NFA *)malloc(sizeof(NFA));
     t1->no=nno;
     t1->ch='\0';
     t1->next=NULL;
     nfaarr[nno]=t1;

     s=(NFA *)malloc(sizeof(NFA));
     s->ch='#';
     s->nfarr[i++]=nno;
     s->no=i;
     s->nfarr[s->no++]=start;
     s->next=NULL;
     nfaarr[nno-1]->next=s;

    start=nno-1;
    end=nno;
    nno++;

}
     
void concat(char c)
{
	NFA *p,*q,*t,*r;
        int flag=0;
	t=(NFA *)malloc(sizeof(NFA));
     	t->no=nno;
     	t->ch='\0';
     	t->next=NULL;
     	nfaarr[nno]=t;
        end=end-2;
        

	r=nfaarr[end];
        p=nfaarr[end]->next;
    	while(p!=NULL)
    	{
        
        	if(p->ch==c)
        	{
            		p->nfarr[p->no++]=nno;
            		flag=1;
	    		break;
        	}
        	else
        	{
            		p=p->next;
	    		r=r->next;
        	}
    	}
    	if(flag==0)
    	{
    		q=(NFA *)malloc(sizeof(NFA));
    		q->ch=c;
        	q->no=0;
        	q->nfarr[q->no++]=nno;
        	q->next=NULL;
        	r->next=q;
    	}
        nno++;
        end=end+1;
        nfaarr[nno]=NULL;
}
    

char *infixtopostfix(char *in)
{
    char *i=&in[0];
    int loop=0;
    struct stack X;
    char n1;
    emptystack(&X);
    while(*i)
    {
        if( isdigit(*i) || isalpha(*i))
        {
            while( isdigit(*i) || isalpha(*i))
            {
                 temp[loop]=*i;
                 loop++;
                 i++;
            }
        }
        if( *i == '(' )
        {
            push(&X,*i);
            i++;
        }
        if( *i == ')')
        {
            n1 = pop(&X);
            while( n1 != '(' )
            {
                temp[loop] = n1;
                loop++;
                n1 = pop(&X);
            }
            i++;
        }
        if( isoperator(*i) )
        {
            if(isempty(&X))
                push(&X,*i);
            else
            {
                n1 = pop(&X);
                while(priority(n1) >= priority(*i))
                {
                    temp[loop] = n1;
                    loop++;
                    n1 = pop(&X);
                }
                if(n1!='n')
                push(&X,n1);
                push(&X,*i);
            }
            i++;
        }
    }
    while(!isempty(&X))
    {
        n1 = pop(&X);
        temp[loop] = n1;
        loop++;        
    }
    temp[loop] = '\0';
    printf("%s\n",temp);
    return (&temp);

}

void evaluate(char *p)
{
    stack stk;
    char opc1,opc2,resultc;
    float op2=0.0,op1=0.0;
    int j=0;
    char str[100],*str1;
    emptystack(&stk);
    NFA *fa=NULL,*fa1=NULL,*fa2=NULL,*fa3=NULL,*result=NULL;
    
    while(*p != '\0')
    {
        if(isdigit(*p)||isalpha(*p))
        {
            resultc=(*p);
            build_nfa(resultc);
            push(&stk,*p);
        }
        else
        {
            if((*p)=='*')
            {
                makestar();
                addextra();
	    }
            else
            {
		opc1=pop(&stk);
                switch(*p)
            	{
                	case '.':
                                nno=nno-2;
				concat(opc1);
        			addextra();
	               		break;
                	case '|':
				makeor();
				addextra();
		    		break;
                }
            }
            
        }  
        p++;
    }
    addextra();
 
}


int main()
{
    printf("Enter the expression\n");
    scanf("%s",arr1);
    char *inf=infixtopostfix(arr1);
    evaluate(inf);
    display();
    return 0;
}
