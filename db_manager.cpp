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
        return process_select_statement(root->st->is_st);
    }
}

bool db_manager::process_select_statement(Select_statement *sl_st)
{
    vector<string> table_names;
    vector<enum FIELD_TYPE> field_types;
    process_table_list(sl_st->tb_ls, table_names);
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
        block_manager::appendTupleToRelation(relation_ptr, *(this->mem), 0, tp_vc[i]);
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