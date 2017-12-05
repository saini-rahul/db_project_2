#include "db_manager.h"

//Process statement list and call functions accordingly
bool db_manager::process_statement(Statement_list *root)
{
    if(root->st->cr_st != '\0')
    {
        return process_create_statement(root->st->cr_st);
    }
    else if(root->st->dr_st != '\0')
    {
        return process_drop_statement(root->st->dr_st);
    }
    else if(root->st->is_st != '\0')
    {
        return process_insert_statement(root->st->is_st);
    }
    else if(root->st->sl_st != '\0')
    {
        vector<vector<string>> result;
        if(process_select_statement(root->st->sl_st->sl_st_rs, root->st->sl_st->op_ds, result) == false)
            return false;
        printSelect(result);
        return true;
    }
    else if(root->st->dl_st != '\0')
    {
        return process_delete_statement(root->st->dl_st);
    }
}

//Delete from table accoring to given condition
bool db_manager::process_delete_statement(Delete_statement *dl_st)
{
    vector< pair<string,string> > postfixExpression;    
    /* Convert the search condition to a postfix vector of strings, if it exists */
    if(dl_st->sr_cn != '\0'){
        /* OR OPERATOR 
           AND OPERATOR 
           < OPERATOR
           > OPERATOR
           = OPERATOR
           + OPERATOR
           - OPERATOR
           * OPERATOR
           
           OPERANDS
           C COLUMN-NAME
           L LITERAL
           I INTEGER
           
            1/0 TRUTH-VALUE
            Integer OP-VALUE
           
        */
        vector<string> table_name;
        table_name.push_back(dl_st->tb_nm->nm);
        if (!process_search_condition(dl_st->sr_cn, postfixExpression, table_name)){ 
            return false;
        }
        else{
                for(int i =0; i<postfixExpression.size(); i++){
                }
            }
    }
    return bl_mg->processDeleteStatement(dl_st->tb_nm->nm, postfixExpression);
}

//Print the result of select query
void db_manager::printSelect(vector<vector<string>> result)
{
      for(int j = 0; j < result.size(); j++)
      {
            vector<string> values = result[j];
            if(values.size() == 0)
                continue;
            for(int i = 0; i < values.size(); i++)
            {
                if(values[i] == "2147483647")
                    cout<<"NULL"<<"\t";
                else
                    cout<<values[i]<<"\t";
            }
            cout<<endl;
      }
}

//Process select statements
bool db_manager::process_select_statement(Select_statement_rest *sl_rs, char *d, vector<vector<string>>& result)
{
    vector<string> table_names;
    vector<string> select_lists;

    process_table_list(sl_rs->tb_ls, table_names);
    string order_by_att = "";
    
    if(sl_rs->cl_nm != '\0')
    {
        order_by_att = process_column_name(sl_rs->cl_nm, table_names);
    }

    vector< pair<string,string> > postfixExpression; 

    if(sl_rs->sl_ls->c == '\0') // If the select list contains a select sublist
    {
        bool check = process_select_list(sl_rs->sl_ls->sl_sb_ls, select_lists, table_names);
        if(check == false)
            return false;
    }
    else
    {
        select_lists.push_back(".*");
    }
    
    /* Convert the search condition to a postfix vector of strings, if it exists */
    if(sl_rs->sr_cn != '\0'){
        /* OR OPERATOR 
           AND OPERATOR 
           < OPERATOR
           > OPERATOR
           = OPERATOR
           + OPERATOR
           - OPERATOR
           * OPERATOR
           
           OPERANDS
           C COLUMN-NAME
           L LITERAL
           I INTEGER
           
            1/0 TRUTH-VALUE
            Integer OP-VALUE
           
        */
        
        if (!process_search_condition(sl_rs->sr_cn, postfixExpression, table_names)){ 
            cout<<"Process Search Condition Failed."<<endl;
            return false;
        }
        else{
                for(int i =0; i<postfixExpression.size(); i++){
                }
            }
    }
    
    return bl_mg->process_select_in_memory(table_names , select_lists, order_by_att, postfixExpression, d, result);
}

/* returns empty string in error condition */
string db_manager::process_column_name(Column_name *cl_nm, vector<string>& table_names)
{
    string att_name(cl_nm->at_nm->nm);
    
    
    if(cl_nm->tb_nm != '\0')
    {
        string table_name(cl_nm->tb_nm->nm);
        
        /* If the table name used in column names do not match the table names in the table-list, return false */
        if(std::find(table_names.begin(), table_names.end(), table_name) != table_names.end()== false) 
            return "";
            
        att_name = table_name + "." + att_name ;
    }
    else
    {
        att_name = "." + att_name ;
    }
    
    return att_name;
}

/* Recursively evaluate select list, and push the required columns into a vector, select_lists 
    Returns false if the table name used to descope column names do not match the table names in the table-list
*/
bool db_manager::process_select_list(Select_sublist *sl_sb_lst, vector<string>& select_lists, vector<string>& table_names)
{
    
    string att_name = process_column_name(sl_sb_lst->cl_nm, table_names);
    if(att_name == "")
    {
        return false;
    }
    select_lists.push_back(att_name);
    
    if(sl_sb_lst -> sl_sls != '\0')
    {
        process_select_list(sl_sb_lst -> sl_sls, select_lists, table_names);
    }
    
    return true;
}

/* Recursively evaluate the table names, and keep pushing them in the vector table_names */

void db_manager::process_table_list(Table_list *tb_lst, vector<string>& table_names)
{
    string name(tb_lst->tb_nm->nm);
    table_names.push_back(name);
    
    if(tb_lst->tb_ls != '\0')
        process_table_list(tb_lst->tb_ls , table_names);
    
}

/* This will store the entire where clause in a post fix notation */
bool db_manager::process_search_condition(Search_condition *sr, vector<pair<string,string>>& postfixExpression,  vector<string>& table_names)
{
    if( !process_boolean_term(sr->bl_tm, postfixExpression, table_names)) //read the boolean term recursively
        return false;
    
    if(sr->sr_cn != '\0') //OR exists 
    {
        //sr_cn->sr_cn; 
        if ( !process_search_condition(sr->sr_cn, postfixExpression, table_names))
            return false;
        
        postfixExpression.push_back(make_pair(string("OR"), string("OPERATOR")));
    }
    return true;
    
}

//Process boolean term of the select query
bool db_manager::process_boolean_term(Boolean_term *bl_tm, vector<pair<string,string>>& postfixExpression, vector<string>& table_names)
{
    //boolean_factor  | boolean-factor AND boolean-term
    if (!process_boolean_factor(bl_tm->bl_fc, postfixExpression, table_names))
        return false;
        
    if(bl_tm->bl_tr != '\0')
    {
        if (!process_boolean_term(bl_tm->bl_tr, postfixExpression, table_names))
            return false;
            
        postfixExpression.push_back(make_pair(string("AND"), string("OPERATOR")));
    }
    return true;
}

//Process boolean expression of select query
bool db_manager::process_boolean_factor(Boolean_factor *bl_fc, vector<pair<string,string>>& postfixExpression, vector<string>& table_names)
{
    //expression comp-op expression
    if (!process_expression(bl_fc->ep1, postfixExpression, table_names))
        return false;
    if (!process_expression(bl_fc->ep2, postfixExpression, table_names))
        return false;
    
    if (!process_comp_op(bl_fc->cm_op, postfixExpression))
        return false;
        
    return true;

}

//Process expression of select query
bool db_manager::process_expression(Expression *ep, vector<pair<string,string>>& postfixExpression, vector<string>& table_names)
{
    if(ep-> op == '\0') //single term
    {
        if(!process_term(ep->tr1, postfixExpression,table_names))
            return false;
    }else
    {
            //operand is present in expression
            if(!process_term(ep->tr1,postfixExpression,table_names ))
                return false;
                
            if (!process_term(ep->tr2,postfixExpression,table_names))
                return false;
            
            char c = ep->op;
            if (c!= '+' && c!= '-' && c!= '*') 
                return false;
            string v_operator (1,c);
            postfixExpression.push_back(make_pair(v_operator, string("OPERATOR")));

            
    }
    return true;
}

//Process term of select query
bool db_manager::process_term(Term *t, vector<pair<string,string>>& postfixExpression, vector<string>& table_names)
{
    if(t->cl_nm != '\0')
    {
        string column_name = process_column_name(t->cl_nm, table_names);
        if (column_name == "") return false;
        postfixExpression.push_back(make_pair(column_name , string("COLUMN-NAME")));

    }else if (t->nm != '\0')
    {
        string literal_name(t->nm);
        postfixExpression.push_back(make_pair(literal_name , string("LITERAL")));
        
    }else
    {
        int term_integer = t->int_num;
        string v_int = to_string(term_integer);
        postfixExpression.push_back(make_pair(v_int, string("INTEGER")));

    }
    return true;
}

//Process comparison operator of select query
bool db_manager::process_comp_op(Comp_op *cm_op, vector<pair<string,string>>& postfixExpression)
{
    char c =  cm_op->op;
    if (c!= '>' && c!= '<' && c!= '=') 
        return false;
    postfixExpression.push_back(make_pair(string(1, c), string("OPERATOR")));
    return true;
}


//Process insert statement
bool db_manager::process_insert_statement(Insert_statement *is_st)
{
    string relation_name(is_st->tb_nm->nm);
    Relation* relation_ptr=schema_manager.getRelation(relation_name);
    Schema schema = schema_manager.getRelation(relation_name)->getSchema();
    
    if(relation_ptr == '\0')
    {
        return false;
    }
    
    vector<Tuple> tp_vc;
    vector<string> att_ls;
    Tuple tuple = relation_ptr->createTuple();
    
    process_attribute_list(is_st->at_ls, att_ls);
    
    if(is_st-> is_tp->vl_ls != '\0')
    {
        Value_list *value_list= is_st-> is_tp->vl_ls;
        for(int i=0; i< att_ls.size(); i++)
        {
            string att_name= att_ls[i];
            if(value_list->vl->nm != '\0')
            {
                if(strcmp(value_list->vl->nm, "NULL") == 0)
                {
                    enum FIELD_TYPE typp = schema.getFieldType(att_name);
                    if(typp == INT)
                    {
                        tuple.setField(att_name, INT_MAX); 
                    }
                    else
                    {
                        string name(value_list->vl->nm);
                        tuple.setField(att_name, name);
                    }
                    
                }
                else
                {
                    string name(value_list->vl->nm);
                    tuple.setField(att_name, name); 
                }
            }
            else
            {
                tuple.setField(att_name, value_list->vl->int_num); 
            }
            
            if(value_list->vl_ls == '\0' && i != att_ls.size()-1)
                return false;
                
            if(value_list->vl_ls != '\0')    
                value_list = value_list->vl_ls;
        }
        
        if(tuple.isNull())
            return false;
            
        tp_vc.push_back(tuple); 
    }
    else
    {
        vector<vector<string>> result;
        if(process_select_statement(is_st-> is_tp->sl_st->sl_st_rs, is_st-> is_tp->sl_st->op_ds, result) == false)
            return false;
        if(result.size() == 0)
        {
            return true;
        }
            
        Schema schema = schema_manager.getRelation(is_st->tb_nm->nm)->getSchema();
        if(att_ls.size() != result[0].size())
        {
            return false;
        }
        
        for(int j = 0; j < result.size(); j++)
        {
            vector<string> values = result[j];
            if(values.size() == 0)
                continue;
            
            Tuple tpl = relation_ptr->createTuple();
            if(j == 0)
                continue;
            for(int i = 0; i < values.size(); i++)
            {
                //else
                //{
                    enum FIELD_TYPE typ = schema.getFieldType(att_ls[i]);
                    if(typ == INT)
                    {
                        string aa= values[i];
                        for(int k =0; k < aa.size(); k++)
                        {
                            if(isdigit(aa[k]) == false)
                            {
                                return false;
                            }
                        }
                        tpl.setField(i, stoi(values[i]));
                    }
                    else
                    {
                        tpl.setField(i, values[i]);
                    }
                //}
            }
            if(tpl.isNull())
                return false;
            
            tp_vc.push_back(tpl);
            
        }
        
        
    }
    insertTupleToRelation(relation_ptr,tp_vc);
    // Now we have created a tuple, let's insert it into relation
    return true;
}


//Insert tuples to relation
void db_manager::insertTupleToRelation(Relation* relation_ptr, vector<Tuple> &tp_vc)
{
    for(int i=0; i< tp_vc.size(); i++)
    {
        bl_mg->appendTupleToRelation(relation_ptr, 0, tp_vc[i]);
    }
    
    cout << "The table currently have " << relation_ptr->getNumOfBlocks() << " blocks" << endl;
    cout << "The table currently have " << relation_ptr->getNumOfTuples() << " tuples" << endl << endl;

}

//Process attribute list 
void db_manager::process_attribute_list(Attribute_list* att_ls, vector<string>& att_nm)
{
    string name(att_ls->at_nm->nm);
    att_nm.push_back(name);
    
    if(att_ls->at_ls != '\0')
        process_attribute_list(att_ls->at_ls, att_nm);
}

//Process drop statement 
bool db_manager::process_drop_statement(Drop_statement *dr_st)
{
    string relation_name(dr_st->tb_nm->nm);
    return schema_manager.deleteRelation(relation_name);
}


//Process create statement and create a new relation
bool db_manager::process_create_statement(Create_statement *cr_st)
{
    vector<string> field_names;
    vector<enum FIELD_TYPE> field_types;
    process_attribute_type_list(cr_st->at_tp_ls, field_names, field_types);
    Schema schema(field_names , field_types);
    
    string relation_name(cr_st->tb_nm->nm);
    cout << "Creating table " << relation_name << endl;  
    Relation* relation_ptr=schema_manager.createRelation(relation_name,schema);
    if(relation_ptr == '\0')
    {
        return false;
    }
    
    return true;
}

//Process attribute type list
void db_manager::process_attribute_type_list(Attribute_type_list* att_ls, vector<string>& field_names, vector<enum FIELD_TYPE>& field_types)
{
    string name(att_ls->at_nm->nm);
    field_names.push_back(name);
    
    string data_type(att_ls->dt_tp->tp);
    
    if(data_type == "INT")
        field_types.push_back(INT);
    else
        field_types.push_back(STR20);
        
    if(att_ls->at_tp_ls != '\0')
        process_attribute_type_list(att_ls->at_tp_ls, field_names, field_types);
}