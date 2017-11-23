#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define MAXQUEUE 100

char arr1[50],arr3[100],post[100],temp[100]={'\0'};
int final[26]={0};
int nno=1,start=1,end=1;

typedef struct NFA{
               int nfarr[25];
               int no;
               char ch;
               struct NFA *next;

}NFA;

NFA *nfaarr[100]={NULL};

typedef struct DFA{
		int dfarr[100];
		int dno;
		char ch1,ch2;
		struct DFA *link;
}DFA;

int dfano=0;

DFA *dfaarr[100]={NULL};
char charr[26]; 

typedef struct stack
{
    char data[100];
    int top;
}stack;

typedef struct stack1
{
    int ele[100];
    int top1;
}stack1;

stack1 stk1;

struct queue
{
    char items[MAXQUEUE];
    int r,f;
};

struct queue que;

void insert(struct queue *q,char d)
{
    if(q->r==(MAXQUEUE-1))
    {
        printf("Queue is full");
    }
    else
    {
        q->r+=1;
        q->items[q->r]=d;
    }
}
void deleteq(struct queue *q,char *data)
{
    if(q->f!=q->r)
    {
        (*data)=q->items[++(q->f)];
    }
    else
    printf("Empty queue");
}

int isemptyq(struct queue *q)
{
     if(q->f==q->r)
     return 1;
     else
     return 0;
}

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


int isempty1(stack1 *s)
{
    return (s->top1 == -1) ? 1 : 0;
}

void emptystack1(stack1* s)
{
    s->top1=-1;
}

void push1(struct stack1 *s,int item)
{
    if(s->top1 == 99)
    {
        printf("\nSTACK FULL");
    }
    else
    {
        ++s->top1;
        s->ele[s->top1]=item;
    }
}

int pop1(struct stack1* s)
{
    int a=-1;
    int pp=0;
    if(!isempty1(s))
    {
        pp= s->ele[s->top1];
        --s->top1;
        return pp;    
    }
    
    else
    return -1;
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

char movetostate(char state,char c)
{
	DFA *p;
	char ch='#';
	int flag=0;
	flag=state-65;
	p=dfaarr[flag]->link;
	while(p!=NULL)
	{
		if(p->ch2==c)
		{
			return p->ch1;
		}
		else
		p=p->link;
	}
	return ch;
}
void simulate(char *str)
{
	char state;
	int i=0,j=0,k=0;
	state='A';
	for(i=0;str[i]!='\0';i++)
	{
		state=movetostate(state,str[i]);
		if(state=='#')
		{
			printf("The string is not recognised\n");
			return;
		}
	}
	k=state-65;
	if(final[k]==1)
	printf("The string is recognised\n");
	else
	printf("The string is not recognised\n");
} 

void display1()
{
    DFA *t,*t1;
    int i=0,j=0;
    for(i=0;i<dfano;i++)
    {
	t=dfaarr[i];
	printf("%c  contains->\n",t->ch1);
        for(j=0;j<t->dno;j++)
	{
		printf("%d   ",t->dfarr[j]);
	}
	printf("\n");
    }
    for(i=0;i<dfano;i++)
    {
	t=dfaarr[i];
	printf("%c ->\n",t->ch1);
	t1=t->link;
	while(t1!=NULL)
	{
	    printf("%c/%c\t",t1->ch1,t1->ch2);
	    t1=t1->link;
        }
        printf("\n");
    }
}

int ispresent(DFA *t,int n)
{
    int i=0,j=0;
    for(i=0;i<t->dno;i++)
    {
        if(t->dfarr[i]==n)
        return 1;
    }
    return 0;
}

void finalstate()
{
	int i=0,j=0;
	DFA *t,*p;
	printf("Final states are\n");
	for(i=0;i<dfano;i++)
	{
		t=dfaarr[i];
		for(j=0;j<t->dno;j++)
		{
			if(t->dfarr[j]==end)
			{
				final[i]=1;
				printf("%c\n",i+65);
				break;
			}
		}
	}
}

void fsilanclosure(DFA **t)
{
     int i=0,j=0,k=0;
     stack1 s;
     emptystack1(&s);
     DFA *t1,*t2,*t3;
     NFA *p,*q,*r;
     t1=(*t);
     for(i=0;i<t1->dno;i++)
     {
         push1(&s,t1->dfarr[i]);
     }
     while(isempty1(&s)!=1)
     {
         j=pop1(&s);
         p=nfaarr[j]->next;
         while(p!=NULL)
         {
             if(p->ch=='#')
             {
                 for(i=0;i<p->no;i++)
                 {
                     if(ispresent(t1,p->nfarr[i])!=1)
                     {
                         (*t)->dfarr[(*t)->dno++]=p->nfarr[i];
                         push1(&s,p->nfarr[i]);
                     }
                 }
                 break;
             }
             else
             p=p->next;
         }
     }
     (*t)->link=NULL;
}
         
DFA * move(int in,char cc)
{
     int i=0,j=0,k=0;
     stack1 s;
     emptystack1(&s);
     DFA *t1,*t2,*t3;
     NFA *p,*q,*r;
     t2=(DFA *)malloc(sizeof(DFA));
     t2->dno=0;
         
     t1=dfaarr[in];
     for(i=0;i<t1->dno;i++)
     {
         push1(&s,t1->dfarr[i]);
     }
     while(isempty1(&s)!=1)
     {
         j=pop1(&s);
         p=NULL;
         p=nfaarr[j]->next;
         while(p!=NULL)
         {
             if(p->ch==cc)
             {
                 
                 for(i=0;i<p->no;i++)
                 {
                     if(ispresent(t2,p->nfarr[i])!=1)
                     {
                         t2->dfarr[t2->dno++]=p->nfarr[i];
                     }
                 }
                 break;
             }
             else
             p=p->next;
         }
     }
     t2->link=NULL;
     return t2;
}

int exists(DFA *t,char *cc)
{
    int i=0,j=0,flag=0;
    DFA *t1,*t2;
    NFA *p,*q;
    for(i=0;i<dfano;i++)
    {
        t1=NULL;
        t1=dfaarr[i];
        if(t->dno==t1->dno)
        {
            flag=0;
	    for(j=0;j<t->dno;j++)
            {
                if(ispresent(t1,t->dfarr[j])!=1)
                {
                    flag=1;
		    break;
                }
            }
            if(flag==0)
            {
                (*cc)=t1->ch1;
                return 1;
            }
        }
    }
    return 0;
}


void consubset()
{
     DFA *t,*t2,*t3,*t4,*t5;
     NFA *n,*p,*q;
     char c='A',c1='\0',c2,c3='\0';
     int i=0,j=0,k=0,flag=0,ele=0;
     t=(DFA *)malloc(sizeof(DFA));
     t->ch1=c;
     insert(&que,c);
     t->ch2='\0';
     t->dno=0;
     t->dfarr[t->dno++]=start;
     fsilanclosure(&t);
     t->link=NULL;
     dfaarr[dfano++]=t;
     while(isemptyq(&que)!=1)
     {
        deleteq(&que,&c1);
        i=c1-65;
         t2=NULL;
         for(j=0;charr[j]!='\0';j++)
         {
           t2=NULL;
           t2=move(i,charr[j]);
           fsilanclosure(&t2);
           if(t2->dno!=0)
           {
             flag=exists(t2,&c3);
	     if(flag!=1)
             {
		 
                 c2=dfano+65;
	         insert(&que,c2);
                 t2->link=NULL;
                 t5=(DFA *)malloc(sizeof(DFA));
                 t5->ch1=c2;
                 t5->ch2='\0';
                 t5->dno=t2->dno;
                 for(k=0;k<t5->dno;k++)
                 t5->dfarr[k]=t2->dfarr[k];
                 t5->link=NULL;
                 dfaarr[dfano++]=t5;
          	 t2->ch1=c2;
		 t2->ch2=charr[j];
		 t2->link=NULL;
             }
             else
             {
		 t2->ch1=c3;
                 t2->ch2=charr[j];
                 t2->link=NULL;
                 
             }
             t3=dfaarr[i];
             t4=dfaarr[i]->link;
	     while(t4!=NULL)
	     {
			t3=t3->link;
			t4=t4->link;
	     }
             t3->link=t2;
             t3=dfaarr[i];
	   }
         }
     }
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
     push1(&stk1,nno);
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
     push1(&stk1,nno);
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
     int i=0,flag=0,start1,start2,end1,end2;

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

     end2=pop1(&stk1);
     start2=pop1(&stk1);
     end1=pop1(&stk1);
     start1=pop1(&stk1);
     s=(NFA *)malloc(sizeof(NFA));
     s->ch='#';
     s->nfarr[i++]=start1;
     s->no=i;
     s->nfarr[s->no++]=start2;
     s->next=NULL;
     nfaarr[nno-1]->next=s;

     i=0;
     s=(NFA *)malloc(sizeof(NFA));
     s->ch='#';
     s->nfarr[i++]=nno;
     s->no=i;
     s->next=NULL;    
     nfaarr[end2]->next=s;

     i=0;
     s=(NFA *)malloc(sizeof(NFA));
     s->ch='#';
     s->nfarr[i++]=nno;
     s->no=i;
     s->next=NULL;    
     nfaarr[end1]->next=s;

     
     nno++;
     end=end+2;
     start=end-1;
     push1(&stk1,start);
     push1(&stk1,end);
}




void makestar()
{
    NFA *p,*q,*r,*s,*t,*t1,*t2,*t3,*t4=NULL,*t5=NULL;
    char c='#';
    int flag=0,i=0,j=0,flag1=0,k=0,start1,end1;
    nno--;
    
    end1=pop1(&stk1);
    start1=pop1(&stk1);
    r=nfaarr[end1];
    p=nfaarr[end1]->next;
    while(p!=NULL)
    {
        
        if(p->ch=='#')
        {
            p->nfarr[p->no++]=start1;
            p->nfarr[p->no++]=end1+2;
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
        q->nfarr[q->no++]=start1;
        q->nfarr[q->no++]=end1+2;
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
     s->nfarr[s->no++]=start1;
     s->next=NULL;
     nfaarr[nno-1]->next=s;

    start=nno-1;
    end=nno;
    push1(&stk1,start);
    push1(&stk1,end);
    nno++;

}
     
void concat()
{
	NFA *p,*q,*t,*r;
    int flag=0,start1,start2,end1,end2;
	end2=pop1(&stk1);
    start2=pop1(&stk1);
    end1=pop1(&stk1);
    start1=pop1(&stk1);
        

    q=(NFA *)malloc(sizeof(NFA));
    q->ch='#';
   	q->no=0;
   	q->nfarr[q->no++]=start2;
   	q->next=NULL;
   	nfaarr[end1]->next=q;
    end=end2;
    start=start1;
    push1(&stk1,start1);
    push1(&stk1,end2);
}
    

void infixtopostfix(char *in)
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
    //return (&temp);
}
int Present(char resultc)
{
  int i=0;
  for(i=0;charr[i]!='\0';i++)
  {
    if(charr[i]==resultc)
    {
      return 1;
    }
  }
  return 0;
}
void evaluate(char *p)
{
    char resultc;
    float op2=0.0,op1=0.0;
    int j=0,length=0;
    char str[100],*str1;
    emptystack1(&stk1);
    NFA *fa=NULL,*fa1=NULL,*fa2=NULL,*fa3=NULL,*result=NULL;
    
    while(*p != '\0')
    {
        if(isdigit(*p)||isalpha(*p))
        {
            resultc=(*p);
            build_nfa(resultc);
	    if(Present(resultc)==0)
	    {
	      length=strlen(charr);
	      charr[length++]=resultc;
	      charr[length++]='\0';
	    }
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
                switch(*p)
            	{
                	case '.':
				        concat();
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
    char str[100];
    printf("Enter the expression\n");
    scanf("%s",arr1);
    infixtopostfix(arr1);
    evaluate(temp);
    display();
    consubset();
    display1();
    finalstate();
    printf("Enter an expression\n");
    scanf("%s",str);
    simulate(str);
 //   getch();
    return 0;
}
