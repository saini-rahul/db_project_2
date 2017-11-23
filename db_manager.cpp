#include "db_manager.h"

void db_manager::process_statement(Statement_list *root)
{
    if(root->st->cr_st != '\0')
    {
        process_create_statement(root->st->cr_st);
    }
}

void db_manager::process_create_statement(Create_statement *cr_st)
{
    vector<string> field_names;
    vector<enum FIELD_TYPE> field_types;
    process_attribute_type_list(cr_st->at_tp_ls, field_names, field_types);
    Schema schema(field_names , field_types);
    
    string relation_name(cr_st->tb_nm->nm);
    cout << "Creating table " << relation_name << endl;  
    Relation* relation_ptr=schema_manager.createRelation(relation_name,schema);
    
    cout << "The table has name " << relation_ptr->getRelationName() << endl;
    cout << "The table has schema:" << endl;
    cout << relation_ptr->getSchema() << endl;
    cout << "The table currently have " << relation_ptr->getNumOfBlocks() << " blocks" << endl;
    cout << "The table currently have " << relation_ptr->getNumOfTuples() << " tuples" << endl << endl;
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