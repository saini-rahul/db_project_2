#include<iostream>
#include<string.h>


using namespace std;

class Node
{
	public:	
	/*virtual void typecheck1()
	{
	}*/

};

class Data_type:public Node{
public:
	char *tp;
	Data_type(char *tp);
};


class Value:public Node{
public:
	int int_num;
	char *nm;

	Value(int int_num);
	Value(char *nm);

};

class Value_list:public Node{
public:
	Value *vl;
	Value_list *vl_ls;

	Value_list(Value *vl);
	Value_list(Value *vl, Value_list *vl_ls);
};

class Comp_op:public Node{
public:
	char op;
	Comp_op(char op);
};

class Table_name:public Node{
public:
	char *nm;
	Table_name(char *nm);
};

class Attribute_name:public Node{
public:
	char *nm;
	Attribute_name(char *nm);
};

class Column_name:public Node{
public:
	Table_name *tb_nm;
	Attribute_name *at_nm;
	//Column_name *cl_nm;

	Column_name(Attribute_name *at_nm);
	Column_name(Table_name *tb_nm, Attribute_name *at_nm );
	//Column_name(Table_name *tb_nm, Column_name *cl_nm );
};

class Attribute_list:public Node{
public:
	Attribute_name *at_nm;
	Attribute_list *at_ls;

	Attribute_list(Attribute_name *at_nm);
	Attribute_list(Attribute_name *at_nm, Attribute_list *at_ls);
};

class Term:public Node{
public:
	int int_num;
	char *nm;
	Column_name *cl_nm;

	Term(int int_num);
	Term(char *nm);
	Term(Column_name *cl_nm);
};

class Expression:public Node{
public:
	Term *tr1;
	char op;
	Term *tr2;

	Expression(Term *tr1);
	Expression(Term *tr1, char op,Term *tr2);
};

class Boolean_factor:public Node{
public:
	Expression *ep1;
	Comp_op *cm_op;
	Expression *ep2;

	Boolean_factor(Expression *ep1, Comp_op *cm_op, Expression *ep2);
};

class Boolean_term:public Node{
public:
	Boolean_factor *bl_fc;
	Boolean_term *bl_tr;

	Boolean_term(Boolean_factor *bl_fc, Boolean_term *bl_tr);
	Boolean_term(Boolean_factor *bl_fc);
};

class Search_condition:public Node{
public:
	Boolean_term *bl_tm;
	Search_condition *sr_cn;

	Search_condition(Boolean_term *bl_tm);
	Search_condition(Boolean_term *bl_tm, Search_condition *sr_cn);

};

class Attribute_type_list:public Node{
public:
	Attribute_name *at_nm;
	Data_type *dt_tp;
	Attribute_type_list *at_tp_ls;

	Attribute_type_list(Attribute_name *at_nm, Data_type *dt_tp);
	Attribute_type_list(Attribute_name *at_nm, Data_type *dt_tp, Attribute_type_list *at_tp_ls);

};

class Create_statement:public Node{
public:
	Table_name *tb_nm;
	Attribute_type_list *at_tp_ls;

	Create_statement(Table_name *tb_nm, Attribute_type_list *at_tp_ls);

};


class Drop_statement:public Node{
public:
	Table_name *tb_nm;
	Drop_statement(Table_name *tb_nm);
};

class Table_list:public Node{
public:
	Table_name *tb_nm;
	Table_list *tb_ls;

	Table_list(Table_name *tb_nm);
	Table_list(Table_name *tb_nm, Table_list *tb_ls);	
};

class Select_sublist:public Node{
public:
	Column_name *cl_nm;
	Select_sublist *sl_sls;

	Select_sublist(Column_name *cl_nm);
	Select_sublist(Column_name *cl_nm, Select_sublist *sl_sls);	
};

class Select_list:public Node{
public:
	char c;
	Select_sublist *sl_sb_ls;
	//Attribute_list *at_ls;

	Select_list(char c);
	Select_list(Select_sublist *sl_sb_ls);
	//Select_list(Attribute_list *at_ls);
};

class Select_statement_rest:public Node{
public:
	Select_list *sl_ls;
	Table_list *tb_ls;
	Search_condition *sr_cn;
	Column_name *cl_nm;

	Select_statement_rest(Select_list *sl_ls, Table_list *tb_ls);
	Select_statement_rest(Select_list *sl_ls, Table_list *tb_ls, Search_condition *sr_cn);
	Select_statement_rest(Select_list *sl_ls, Table_list *tb_ls, Search_condition *sr_cn, Column_name *cl_nm);
};


class Select_statement:public Node{
public:
	Select_statement_rest *sl_st_rs;
	Select_statement(Select_statement_rest *sl_st_rs);
};

class Insert_touples:public Node{
public:
	Value_list *vl_ls;
	Select_statement *sl_st;

	Insert_touples(Value_list *vl_ls);
	Insert_touples(Select_statement *sl_st);	
};

class Insert_statement:public Node{
public:
	Table_name *tb_nm;
	Attribute_list *at_ls;
	Insert_touples *is_tp;

	Insert_statement(Table_name *tb_nm, Attribute_list *at_ls, Insert_touples *is_tp);
};

class Delete_statement:public Node{
public:
	Table_name *tb_nm;
	Search_condition *sr_cn;

	Delete_statement(Table_name *tb_nm);
	Delete_statement(Table_name *tb_nm, Search_condition *sr_cn);
};

class Statement:public Node{
public:
	Create_statement *cr_st;
	Drop_statement *dr_st;
	Select_statement *sl_st;
	Insert_statement *is_st;
	Delete_statement *dl_st;

	Statement(Create_statement *cr_st);
	Statement(Drop_statement *dr_st);
	Statement(Select_statement *sl_st);
	Statement(Insert_statement *is_st);
	Statement(Delete_statement *dl_st);
	void printFunc()
	{
		cout<<"printtttttSSSSS"<<'\n';
	}

};

class Statement_list:public Node{
public:
	Statement_list *st_ls;
	Statement *st;

	Statement_list(Statement *st, Statement_list *st_ls);
	Statement_list(Statement *st);
	void printFunc()
	{
		cout<<"printttttt"<<'\n';
	}
};

