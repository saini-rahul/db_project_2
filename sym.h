#include<iostream>
#include<conio.h>
#include<vector>
#include<string.h>
using namespace std;

class sym_Node{
public:
      virtual void type_check()
      {
      }
      virtual void display()
      {
      }

};


class sym_Verdec:public sym_Node{

public:

char ty[10];
struct ver_List{

char typ[10];
char name[20];
struct ver_List *next;

}*p;

sym_Verdec()
{
    p=NULL;
}

void create_table(char *aa,char *bb)
{
    ver_List *temp,*q;
    if(p==NULL)
    {
	p=new ver_List;
	p->next=NULL;
	strcpy(p->typ,aa);
	strcpy(p->name,bb);
    }
    else
    {
      q=p;
      while(q!=NULL)
      {
	if(strcmp(q->name,bb)==0)
        {
	    cout<<"\nType Error5";
	    exit(0);
	}
        else
        q=q->next;
      }
      q = p;
      while( q->next != NULL )
      q = q->next;

      temp= new ver_List;
      temp->next = NULL;
      strcpy(temp->typ,aa);
      strcpy(temp->name,bb);
      q->next = temp;
   }
}



void display()
{
    ver_List *q=p;
    while(q!=NULL)
    {
	cout<<" "<<q->typ<<" ";
        cout<<" "<<q->name<<" ";
	cout<<" \n";
	q=q->next;
    }
}

void copy(sym_Verdec *v2)
{
    v2->p=p;
}

int search(char *ss4,char **tt)
{
    ver_List *q=p;
    int flag=0;
    while(q!=NULL)
    {
    	if(strcmp(q->name,ss4)==0)
	{
	    flag=1;
	    (*tt)=(char *)malloc(10);
    	    strcpy((*tt),q->typ);
	    break;
	}
	q=q->next;
    }
    return flag;
}

};


class sym_Arg:public sym_Node{
public:

struct arg_List{

char typ1[10];
char name1[20];
struct arg_List *next;

}*p;

sym_Arg()
{
    p=NULL;
}

void populate_arglist(char *aa,char *bb)
{
    arg_List *temp,*q;

    if(p==NULL)
    {
	p=new arg_List;
	p->next=NULL;
	strcpy(p->typ1,aa);
	strcpy(p->name1,bb);
    }
    else
    {
      q=p;
      while(q!=NULL)
      {
	if(strcmp(q->name1,bb)==0)
        {
	    cout<<"\nType Error4";
	    exit(0);
	}
        else
        q=q->next;
      }
      q = p;
      while( q->next != NULL )
      q = q->next;

      temp= new arg_List;
      temp->next = NULL;

      strcpy(temp->name1,bb);

      strcpy(temp->typ1,aa);
      
      q->next = temp;
   }
   
}



void display()
{
    arg_List *q=p;
    while(q!=NULL)
    {
	cout<<" "<<q->typ1<<" ";
        cout<<" "<<q->name1<<" ";
	cout<<" \n";
	q=q->next;
    }
}

void copy(sym_Arg *v2)
{
    v2->p=p;
}

void check(sym_Arg *ss)
{
	int flag=0;
	arg_List *v=p;
	arg_List *a=ss->p;
	while(v!=NULL&&a!=NULL)
	{
	    if(strcmp(v->typ1,a->typ1)!=0)
	    {
		flag=1;
		break;
	    }
	    v=v->next;
            a=a->next;
	}
	if(v==NULL&&a!=NULL)
	flag=1;
	if(v!=NULL&&a==NULL)
	flag=1;
		
	if(flag==0)
	{
	    cout<<"\nType Error3\n";
	    exit(0);
	}

}

int search1(char *ss4,char **tt)
{
    arg_List *q=p;
    int flag=0;
    (*tt)=(char *)malloc(10);
    while(q!=NULL)
    {
    	if(strcmp(q->name1,ss4)==0)
	{
	    strcpy((*tt),q->typ1);
            flag=1;
	}
	q=q->next;
    }
    return flag;
}

int methodpresent2(char *aa,char *bb)
{
    arg_List *q=p;
    if(q->next==NULL&&bb[0]=='\0')
    {
        if(strcmp(q->typ1,aa)==0)
	return 1;
	else
	{
	   return 0; 
	}
    }
    else if(q->next!=NULL&&aa[0]!='\0'&&bb[0]!=0)
    {
	if(strcmp(q->typ1,aa)==0&&strcmp(q->next->typ1,bb)==0)
	return 1;
	else
	return 0;
    }
    return 0;

}



};


class sym_Method:public sym_Node{
public:

struct sym_metnode{
char name3[10];
char typ3[10];
sym_Verdec *symv;
sym_Arg *syma;
sym_metnode *next;
}*p;

sym_Method()
{
    p=NULL;
}

void populate_method(char *ss,char *ss1,sym_Verdec *vv,sym_Arg *aa)
{
    sym_metnode *temp,*q;
    if(p==NULL)
    {
        p=new sym_metnode;
	p->next=NULL;
	strcpy(p->typ3,ss);
	strcpy(p->name3,ss1);
	p->symv=new sym_Verdec();
	p->symv=vv;
	p->syma=new sym_Arg();
	p->syma=aa;
    }
    else
    {
        q=p;
	int flag=0;
	while(q!=NULL)
	{
	    if(strcmp(p->name3,ss1)==0)
	    {
		sym_Arg *v=q->syma;
		sym_Arg *a=aa;
		v->check(a);		
	    }
            q=q->next;
	}

	q=p;
	while(q->next!=NULL)
	q=q->next;

	temp=new sym_metnode;
	temp->next=NULL;
	strcpy(temp->typ3,ss);
	strcpy(temp->name3,ss1);
	temp->symv=new sym_Verdec();
	temp->symv=vv;
	temp->syma=new sym_Arg();
	temp->syma=aa;
	q->next=temp;
    }    
}

void copy(sym_Method *m)
{
	m->p=p;
}

void display()
{
    sym_metnode *q=p;

    while(q!=NULL)
    {
	cout<<" "<<q->typ3<<" ";
        cout<<" "<<q->name3<<" ";
	cout<<" \n";
	if(q->symv!='\0')
	q->symv->display();
	cout<<" \n";
	if(q->syma!='\0')
	q->syma->display();
	cout<<"\n";
	q=q->next;
    }
}

int check_method(char *ss3,char *ss4,char **tt)
{
    sym_metnode *q=p;
    int flag=0,flag1=0;
    char *ff='\0',*ff1='\0';
    while(q!=NULL)
    {
	if(strcmp(q->name3,ss3)==0)
	{
	    flag=q->symv->search(ss4,&ff);
	    flag1=q->syma->search1(ss4,&ff1);
        }
	q=q->next;
    }
    if(flag==1)
    {
    	(*tt)=(char *)malloc(10);
    	strcpy((*tt),ff);
    }
    if(flag1==1)
    {
    	(*tt)=(char *)malloc(10);
    	strcpy((*tt),ff1);
    }
    if(flag==0&&flag1==0)
    return 0;
    else
    return 1;
}

void methodpresent1(char *mn,char *aa,char *bb,char **rt)
{
    sym_metnode *q=p;
    (*rt)=(char *)malloc(20);
    int flag=0;
    cout<<"mm";
    while(q!=NULL)
    {
	if(strcmp(q->name3,mn)==0)
	{
	    if(q->syma!='\0')
	    {
		if(aa[0]=='\0'&&bb=='\0')
		{
		    cout<<"\nType Error16\n";
		    exit(0);
		}
		else
		{
	            flag=q->syma->methodpresent2(aa,bb);
		    if(flag==0)
		    {
			cout<<"\nType Error17\n";
			exit(0);
		    }
		    else
		    {
			strcpy((*rt),q->typ3);
			return;
		    }
		}
	    }
	    else
	    {
		if(aa[0]=='\0'&&bb[0]=='\0')
		{
		    strcpy((*rt),q->typ3);
		    return;
		}
		else
		{
		    cout<<"\nType Error18\n";
		    exit(0);
		}
	    }
        }
    	q=q->next;
    }
}

};

class sym_Class:public sym_Node{
public:

struct class_List{
char name4[10];
char name5[10];
sym_Verdec *symv1;
sym_Method *syma1;
class_List *next;
}*p;

sym_Class()
{
    p=NULL;
}

void populate_class(char *ss,char *ss1,sym_Verdec *vv,sym_Method *aa)
{
    int flag=0;
    class_List *temp,*q;
    if(p==NULL)
    {
        p=new class_List;
	p->next=NULL;
	strcpy(p->name4,ss);
	if(ss1[0]=='\0')
	p->name5[0]='\0';
	else
	strcpy(p->name5,ss1);
	p->symv1=new sym_Verdec();
	p->symv1=vv;
	p->syma1=new sym_Method();
	p->syma1=aa;
    }
    else
    {
	q=p;
	while(q!=NULL)
        {
	    if(strcmp(q->name4,ss)==0)
	    {
		cout<<"\nType Error\n";
		exit(0);
	    }
	    q=q->next;
	}
	if(ss1[0]!='\0')
	{
	    q=p;
	    while(q!=NULL)
	    {
	    	if(strcmp(q->name4,ss1)==0)
	    	{
		    flag=1;
		    break;
	    	}
	    	q=q->next;
	    }
	    if(flag==0)
	    {
	        cout<<"\nType Error2\n";
	        exit(0);
	    }
	}
	q=p;
	while(q->next!=NULL)
	q=q->next;

	temp=new class_List;
	temp->next=NULL;
	strcpy(temp->name4,ss);
	if(ss1[0]!='\0')
	strcpy(temp->name5,ss1);
	else
	temp->name5[0]='\0';
	temp->symv1=new sym_Verdec();
	temp->symv1=vv;
	temp->syma1=new sym_Method();
	temp->syma1=aa;
	q->next=temp;
    }    
}

void copy(sym_Class *c)
{
	c->p=p;
}

void display()
{
    class_List *q=p;
    while(q!=NULL)
    {
	cout<<" "<<q->name4<<" ";
        if(q->name5[0]!='\0')
	cout<<" "<<q->name5<<" \n";
	if(q->symv1!='\0')
	q->symv1->display();
	cout<<" \n";
	if(q->syma1!='\0')
	q->syma1->display();
	cout<<"\n";
	q=q->next;
    }
}

void ifexists(char *ss)
{
    if(strcmp(p->name4,ss)==0)
    {
	cout<<"\nType Error1\n";
	exit(0);
    }
}

int check_class(char *ss1,char *ss4,char **tt)
{
    class_List *q=p;
    int flag=0;
    while(q!=NULL)
    {
        
	if(strcmp(q->name4,ss1)==0)
	{
	    if(q->symv1)
	    flag=q->symv1->search(ss4,tt);
        }
	q=q->next;
    }
    return flag;
}

int find_parentclass(char *ss1,char *ss4,char **tt)
{
    class_List *q=p;
    char aa[10];
    int flag=0;
    aa[0]='\0';
    while(q!=NULL)
    {
	if(strcmp(q->name4,ss1)==0)
	{
	    if(q->name5[0]!='\0')
	    {
		strcpy(aa,q->name5);
		flag=check_class(aa,ss4,tt);
		if(flag==1)
		return 1;
		else
		flag=find_parentclass(aa,ss4,tt);
		break;
            }
        }
	q=q->next;
    }
    return flag;
}

int check_class1(char *ss1,char *ss3,char *ss4,char **tt)
{
    int flag=0;
    cout<<" ";
    class_List *q=p;
    while(q!=NULL)
    {
	if(strcmp(q->name4,ss1)==0)
        {
    	    flag=q->syma1->check_method(ss3,ss4,tt);
	    break;
	}
        q=q->next;
    }
    return flag;
}

void classpresent(char *ss)
{
    	class_List *q=p;
	int flag=0;
	while(q!=NULL)
	{
	    	if(strcmp(q->name4,ss)==0)
	    	{
		    flag=1;
		    break;
	    	}
	    	q=q->next;
	}
	if(flag==0)
	{
	        cout<<"\nType Error7\n";
	        exit(0);
	}
}

void methodpresent(char *cn,char *mn,char *aa,char *bb,char **rt)
{
    class_List *q=p;
    while(q!=NULL)
    {
	if(strcmp(q->name4,cn)==0)
	{
	    q->syma1->methodpresent1(mn,aa,bb,rt);
    	}
	q=q->next;
   }
}

    

}; 



class type_check:public sym_Node{
public:

type_check()
{
}

void check_if_present(sym_Class *ss,char *ss1,sym_Method *ss2,char *ss3,char *ss4,char **tt)
{
    int flag=0;
    char *aa;
    flag=ss->check_class(ss1,ss4,tt);
    if(flag==1)
    return;

    flag=ss->find_parentclass(ss1,ss4,tt);
    cout<<" ";

    if(flag==1)
    return;    
    cout<<" ";
    
    if(ss2!='\0')
    {
    	flag=ss->check_class1(ss1,ss3,ss4,tt);
	if(flag==1)
	return;
	
    }    
    cout<<"\nType Error6\n";
    exit(0);
}
    
};
