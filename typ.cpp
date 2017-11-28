#include "typ.h"

Data_type::Data_type(char *tp)
{
	//this-> tp= tp;
	this->tp= new char[40];
	strcpy(this->tp, tp);
	cout<<"type "<<this->tp<<"\n";
}


Value::Value(int int_num)
{
	this-> nm='\0';
	this->int_num= int_num;
	cout<<"value "<<this->int_num<<"\n";
}
Value::Value(char *nm)
{
	int_num= INT_MAX;
	this->nm= new char[40];
	strcpy(this->nm, nm);
	cout<<"val  "<<nm<<"\n";
}

Value_list::Value_list(Value *vl)
{
	this->vl= vl;
	vl_ls= '\0';
}
Value_list::Value_list(Value *vl, Value_list *vl_ls)
{
	this->vl= vl;
	this-> vl_ls= vl_ls;
}
/*Select_list::Select_list(Attribute_list *at_ls)
{
	this->at_ls= at_ls;
}*/

Comp_op::Comp_op(char op)
{
	this-> op= op;
	cout<<"op  |"<<op<<"|\n";
}

Table_name::Table_name(char *nm)
{
	cout<<"gete 1"<<nm<<endl;
	this->nm= new char[40];
	int i=0;
	for(i=0; nm[i]!= '\0'; i++)
	{
		if(!isalnum(nm[i]))
		{
			//this-> nm[i]= '\0';
			break;
			//this->nm[i]= nm[i];
		}
		this-> nm[i]=nm[i];
	}
	this-> nm[i] = 0;
	//strcpy(this->nm, nm);
	cout<<"TABLE  "<<this->nm<<"\n";
}

Attribute_name::Attribute_name(char *nm)
{
	/*this->nm= new char[40];
		
	strcpy(this->nm, nm);
	cout<<"ATT  "<<this->nm<<"\n";*/
	this->nm= new char[40];
	int i = 0;
	for(i=0; nm[i]!= '\0'; i++)
	{
		if(!isalnum(nm[i]))
		{
			//this-> nm[i]= '\0';
			break;
		}
		if(nm[i]==' ')
		{
			break;
		}

		if(nm[i]=='F')
		{
			if(nm[i+1]!= '\0' && nm[i+1]=='R' && nm[i+2]!= '\0' && nm[i+2]=='O' &&nm[i+3]!= '\0' && nm[i+3]=='M')
			{
				//this-> nm[i]= '\0';
				break;
			}
		}

		this->nm[i]= nm[i];
	}
	this->nm[i] = '\0';
	cout<<"ATT  NMMMMMMM    "<<nm<<"\n";
	cout<<"ATT  "<<this->nm<<"\n";


}



Column_name::Column_name(Attribute_name *at_nm)
{
	tb_nm= '\0';
	this->at_nm= at_nm;
}
Column_name::Column_name(Table_name *tb_nm, Attribute_name *at_nm )
{
	this->tb_nm= tb_nm;
	this->at_nm= at_nm;
	//cout<<tb_nm<<" aa "<<at_nm<<"  ";

}

/*Column_name::Column_name(Table_name *tb_nm, Column_name *cl_nm )
{
	this->tb_nm= tb_nm;
	this->cl_nm= cl_nm;
	//cout<<tb_nm<<" aa "<<at_nm<<"  ";

}*/


Attribute_list::Attribute_list(Attribute_name *at_nm)
{
	at_ls= '\0';
	this->at_nm= at_nm;
	//cout<<at_nm<<"  ";
}
Attribute_list::Attribute_list(Attribute_name *at_nm, Attribute_list *at_ls)
{
	this->at_ls= at_ls;
	this->at_nm= at_nm;
	//cout<<at_nm<<"  ";
}

Term::Term(int int_num)
{
	nm='\0';
	this->int_num= int_num;
	this-> cl_nm= '\0';
	cout<<"term  |"<<int_num<<"|\n";
}

Term::Term(char *nm)
{
	int_num=-INT_MAX;
	this->nm= new char[40];
	strcpy(this->nm, nm);
	this-> cl_nm= '\0';
	cout<<"term1  |"<<nm<<"|\n";
}

Term::Term(Column_name *cl_nm)
{
	int_num=-INT_MAX;
	this->nm= '\0';
	this-> cl_nm= cl_nm;
	//cout<<nm<<"  ";
}

Expression::Expression(Term *tr1)
{
	this->tr1= tr1;
	this->op= '\0';
	this->tr2= '\0';
}

Expression::Expression(Term *tr1, char op,Term *tr2)
{
	this->tr1= tr1;
	this->op= op;
	this->tr2= tr2;
}

Boolean_factor::Boolean_factor(Expression *ep1, Comp_op *cm_op, Expression *ep2)
{
	this-> ep1= ep1;
	this->cm_op= cm_op;
	this->ep2= ep2;
}

Boolean_term::Boolean_term(Boolean_factor *bl_fc, Boolean_term *bl_tr)
{
	this-> bl_fc= bl_fc;
	this-> bl_tr= bl_tr;
}
Boolean_term::Boolean_term(Boolean_factor *bl_fc)
{
	this-> bl_fc= bl_fc;
	this-> bl_tr= '\0';
}

Search_condition::Search_condition(Boolean_term *bl_tm)
{
	this->bl_tm= bl_tm;
	this-> sr_cn= '\0';
}

Search_condition::Search_condition(Boolean_term *bl_tm, Search_condition *sr_cn)
{
	this->bl_tm= bl_tm;
	this-> sr_cn= sr_cn;
}


Attribute_type_list::Attribute_type_list(Attribute_name *at_nm, Data_type *dt_tp)
{
	this-> at_nm= at_nm;
	this-> dt_tp= dt_tp;
	this->at_tp_ls= '\0';
}

Attribute_type_list::Attribute_type_list(Attribute_name *at_nm, Data_type *dt_tp, Attribute_type_list *at_tp_ls)
{
	this-> at_nm= at_nm;
	this-> dt_tp= dt_tp;
	this->at_tp_ls= at_tp_ls;
}

Create_statement::Create_statement(Table_name *tb_nm, Attribute_type_list *at_tp_ls)
{
	this->tb_nm= tb_nm;
	this->at_tp_ls= at_tp_ls;
}

Drop_statement::Drop_statement(Table_name *tb_nm)
{
	cout<<"here   "<<endl;
	this->tb_nm= tb_nm;
}

Table_list::Table_list(Table_name *tb_nm)
{
	this-> tb_nm= tb_nm;
	this-> tb_ls= '\0';
}

Table_list::Table_list(Table_name *tb_nm, Table_list *tb_ls)
{
	this-> tb_nm= tb_nm;
	this-> tb_ls= tb_ls;
}

Select_sublist::Select_sublist(Column_name *cl_nm)
{
	this->cl_nm= cl_nm;
	this->sl_sls= '\0';
}

Select_sublist::Select_sublist(Column_name *cl_nm, Select_sublist *sl_sls)
{
	this->cl_nm= cl_nm;
	this->sl_sls= sl_sls;
}


Select_list::Select_list(char c)
{
	this->	c= '*';
	this-> sl_sb_ls= '\0';
}

Select_list::Select_list(Select_sublist *sl_sb_ls)
{
	this->	c= '\0';
	this-> sl_sb_ls= sl_sb_ls;
}


Select_statement_rest::Select_statement_rest(Select_list *sl_ls, Table_list *tb_ls)
{
	this-> sl_ls= sl_ls;
	this-> tb_ls= tb_ls;
	this->sr_cn= '\0';
	this-> cl_nm= '\0';
}

Select_statement_rest::Select_statement_rest(Select_list *sl_ls, Table_list *tb_ls, Search_condition *sr_cn)
{
	this-> sl_ls= sl_ls;
	this-> tb_ls= tb_ls;
	this->sr_cn= sr_cn;
	this-> cl_nm= '\0';
}

Select_statement_rest::Select_statement_rest(Select_list *sl_ls, Table_list *tb_ls, Column_name *cl_nm)
{
	this-> sl_ls= sl_ls;
	this-> tb_ls= tb_ls;
	this->sr_cn= '\0';
	this-> cl_nm= cl_nm;
}

Select_statement_rest::Select_statement_rest(Select_list *sl_ls, Table_list *tb_ls, Search_condition *sr_cn, Column_name *cl_nm)
{
	this-> sl_ls= sl_ls;
	this-> tb_ls= tb_ls;
	this->sr_cn= sr_cn;
	this-> cl_nm= cl_nm;
}


Select_statement::Select_statement(Select_statement_rest *sl_st_rs)
{
	this-> sl_st_rs= sl_st_rs;
	this->op_ds ='\0';
}

Select_statement::Select_statement(Select_statement_rest *sl_st_rs, char *op_ds)
{
	this-> sl_st_rs= sl_st_rs;
	
	this->op_ds =new char[10];
	strcpy(this->op_ds, op_ds);
}


Insert_touples::Insert_touples(Value_list *vl_ls)
{
	this-> vl_ls= vl_ls;
	this-> sl_st= '\0';
}

Insert_touples::Insert_touples(Select_statement *sl_st)
{
	this-> vl_ls= '\0';
	this-> sl_st= sl_st;
}


Insert_statement::Insert_statement(Table_name *tb_nm, Attribute_list *at_ls, Insert_touples *is_tp)
{
	this-> tb_nm= tb_nm;
	this-> at_ls= at_ls;
	this-> is_tp= is_tp;
}

Delete_statement::Delete_statement(Table_name *tb_nm)
{
	this-> tb_nm= tb_nm;
	this-> sr_cn= '\0';
}
Delete_statement::Delete_statement(Table_name *tb_nm, Search_condition *sr_cn)
{
	this-> tb_nm = tb_nm;
	this-> sr_cn= sr_cn;
}

Statement::Statement(Create_statement *cr_st)
{
	this->cr_st= cr_st;
	this-> sl_st= '\0';
	this-> is_st= '\0';
	this-> dl_st= '\0';
	this-> dr_st= '\0';	
}
Statement::Statement(Drop_statement *dr_st)
{
	cout<<"here  222222222222"<<endl;
	this->cr_st= '\0';
	this-> sl_st= '\0';
	this-> is_st= '\0';
	this-> dl_st= '\0';
	this-> dr_st= dr_st;	
}

Statement::Statement(Select_statement *sl_st)
{
	this->cr_st= '\0';
	this-> sl_st= sl_st;
	this-> is_st= '\0';
	this-> dl_st= '\0';
	this-> dr_st= '\0';	
}

Statement::Statement(Insert_statement *is_st)
{
	this->cr_st= '\0';
	this-> sl_st= '\0';
	this-> is_st= is_st;
	this-> dl_st= '\0';
	this-> dr_st= '\0';	
}

Statement::Statement(Delete_statement *dl_st)
{
	this->cr_st= '\0';
	this-> sl_st= '\0';
	this-> is_st= '\0';
	this-> dl_st= dl_st;
	this-> dr_st= '\0';	
}

Statement_list::Statement_list(Statement *st, Statement_list *st_ls)
{
	this-> st= st;
	this-> st_ls= st_ls;
}

Statement_list::Statement_list(Statement *st)
{
	//cout<<"here  1111111111111111"<<endl;
	this-> st= st;
	this-> st_ls= '\0';
	//printFunc();
}

