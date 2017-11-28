#include "db_manager.h"

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
        return process_select_statement(root->st->sl_st->sl_st_rs, root->st->sl_st->op_ds);
    }
}

bool db_manager::process_select_statement(Select_statement_rest *sl_rs, char *d)
{
    vector<string> table_names;
    vector<string> select_lists;
    process_table_list(sl_rs->tb_ls, table_names);
    string order_by_att = "";
    
    if(sl_rs->cl_nm != '\0')
    {
        order_by_att = process_column_name(sl_rs->cl_nm, table_names);
    }
    
    if(sl_rs->sl_ls->c == '\0')
    {
        bool check = process_select_list(sl_rs->sl_ls->sl_sb_ls, select_lists, table_names);
        //std::cout << "ssssssssssssssssssssssssssssssssssssssss" << std::endl;
        if(check == false)
            return false;
    }
    else
    {
        select_lists.push_back(".*");
        cout<<"It is *"<<endl;
    }
    
    process_search_condition(sl_rs->sr_cn);
    
    if(d == '\0')
    {
        std::cout << "Not distinct" << std::endl;
    }
    return bl_mg->process_select_in_memory(table_names , select_lists, order_by_att);
    
    //return true;
}

string db_manager::process_column_name(Column_name *cl_nm, vector<string>& table_names)
{
    string att_name(cl_nm->at_nm->nm);
    
    
    if(cl_nm->tb_nm != '\0')
    {
        string table_name(cl_nm->tb_nm->nm);
        cout<<"table name is "<<table_name<<endl;
        
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

bool db_manager::process_select_list(Select_sublist *sl_sb_lst, vector<string>& select_lists, vector<string>& table_names)
{
    
    string att_name = process_column_name(sl_sb_lst->cl_nm, table_names);
    if(att_name == "")
    {
        return false;
    }
    select_lists.push_back(att_name);
    cout<<"Name of the relation 111111111111"<<att_name<<endl;
    
    if(sl_sb_lst -> sl_sls != '\0')
    {
        process_select_list(sl_sb_lst -> sl_sls, select_lists, table_names);
    }
    
    return true;
}

void db_manager::process_table_list(Table_list *tb_lst, vector<string>& table_names)
{
    string name(tb_lst->tb_nm->nm);
    cout<<"Name of the relation "<<name<<endl;
    table_names.push_back(name);
    
    if(tb_lst->tb_ls != '\0')
        process_table_list(tb_lst->tb_ls , table_names);
    
}

bool db_manager::process_search_condition(Search_condition *sr_cn)
{
    return 1;
}

bool db_manager::process_insert_statement(Insert_statement *is_st)
{
    string relation_name(is_st->tb_nm->nm);
    Relation* relation_ptr=schema_manager.getRelation(relation_name);
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
        //process_value_list(tuple, att_ls, is_st->vl_ls);
        Value_list *value_list= is_st-> is_tp->vl_ls;
        for(int i=0; i< att_ls.size(); i++)
        {
            string att_name= att_ls[i];
            if(value_list->vl->nm != '\0')
            {
                string name(value_list->vl->nm);
                tuple.setField(att_name, name); 
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
    insertTupleToRelation(relation_ptr,tp_vc);
    // Now we have created a tuple, let's insert it into relation
    return true;
}

void db_manager::insertTupleToRelation(Relation* relation_ptr, vector<Tuple> &tp_vc)
{
    for(int i=0; i< tp_vc.size(); i++)
    {
        bl_mg->appendTupleToRelation(relation_ptr, 0, tp_vc[i]);
    }
    
    cout << "The table currently have " << relation_ptr->getNumOfBlocks() << " blocks" << endl;
    cout << "The table currently have " << relation_ptr->getNumOfTuples() << " tuples" << endl << endl;

}

void db_manager::process_attribute_list(Attribute_list* att_ls, vector<string>& att_nm)
{
    string name(att_ls->at_nm->nm);
    att_nm.push_back(name);
    
    if(att_ls->at_ls != '\0')
        process_attribute_list(att_ls->at_ls, att_nm);
}

bool db_manager::process_drop_statement(Drop_statement *dr_st)
{
    cout<<"ffffffffff"<<endl;
    string relation_name(dr_st->tb_nm->nm);
    cout<<"|"<<relation_name<<"|"<<endl;
    std::cout << schema_manager << std::endl;
    return schema_manager.deleteRelation(relation_name);
}

bool db_manager::process_create_statement(Create_statement *cr_st)
{
    vector<string> field_names;
    vector<enum FIELD_TYPE> field_types;
    process_attribute_type_list(cr_st->at_tp_ls, field_names, field_types);
    Schema schema(field_names , field_types);
    
    cout<<"error here"<<cr_st->tb_nm->nm<<endl;
    string relation_name(cr_st->tb_nm->nm);
    cout << "Creating table " << relation_name << endl;  
    Relation* relation_ptr=schema_manager.createRelation(relation_name,schema);
    if(relation_ptr == '\0')
    {
        return false;
    }
    
    cout << "The table has name " << relation_ptr->getRelationName() << endl;
    cout << "The table has schema:" << endl;
    cout << relation_ptr->getSchema() << endl;
    cout << "The table currently have " << relation_ptr->getNumOfBlocks() << " blocks" << endl;
    cout << "The table currently have " << relation_ptr->getNumOfTuples() << " tuples" << endl << endl;
    std::cout << schema_manager << std::endl;
    return true;
}

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