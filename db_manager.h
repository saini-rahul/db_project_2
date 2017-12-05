#ifndef _db_manager
#define _db_manager

#include "typ.h"
#include "block_manager.cpp"
#include <algorithm>    // std::find


using namespace std;

//Process all queries. Process SELECT, CREATE, DELETE, DROP, INSERT queries
class db_manager{
    private:
        MainMemory *mem;
        Disk *disk;
        SchemaManager schema_manager;
        block_manager *bl_mg;
    public:
        db_manager(MainMemory *mm, Disk *d): schema_manager(mm, d)
        {
            this->mem = mm;
            this->disk= d;
            this->bl_mg = new block_manager(mm,d, &schema_manager);
        }
        
        bool process_statement(Statement_list *root);
        
        bool process_create_statement(Create_statement *cr_st);
        void process_attribute_type_list(Attribute_type_list* , vector<string>&, vector<enum FIELD_TYPE>&);
        
        bool process_insert_statement(Insert_statement *cr_st);
        void process_attribute_list(Attribute_list*, vector<string>&);
        void insertTupleToRelation(Relation* , vector<Tuple>&);
        void printSelect(vector<vector<string>> result);
        
        bool process_drop_statement(Drop_statement *dr_st);

        bool process_select_statement(Select_statement_rest *, char *, vector<vector<string>>&);
        void process_table_list(Table_list *tb_ls, vector<string>& table_names);
        
        bool process_search_condition(Search_condition *sr_cn,  vector<pair<string,string>>&, vector<string>& ); 
        bool process_boolean_term(Boolean_term*,vector<pair<string,string>>&, vector<string>& ); 
        bool process_boolean_factor(Boolean_factor*,vector<pair<string,string>>&, vector<string>& ); 
        bool process_expression(Expression*,vector<pair<string,string>>&, vector<string>& ); 
        bool process_comp_op(Comp_op*, vector<pair<string,string>>& );
        bool process_term(Term*,vector<pair<string,string>>&, vector<string>& ); 

        bool process_select_list(Select_sublist *, vector<string>& , vector<string>& );
        string process_column_name(Column_name *, vector<string>& );
        
        bool process_delete_statement(Delete_statement *dl_st);

};

#endif