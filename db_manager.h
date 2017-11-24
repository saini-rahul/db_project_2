#ifndef _db_manager
#define _db_manager

#include "typ.h"
#include "block_manager.cpp"

using namespace std;

class db_manager{
    private:
        MainMemory *mem;
        Disk *disk;
        SchemaManager schema_manager;
    
    public:
        db_manager(MainMemory *mm, Disk *d): schema_manager(mm, d)
        {
            this->mem = mm;
            this->disk= d;
        }
        
        bool process_statement(Statement_list *root);
        
        bool process_create_statement(Create_statement *cr_st);
        void process_attribute_type_list(Attribute_type_list* , vector<string>&, vector<enum FIELD_TYPE>&);
        
        bool process_insert_statement(Insert_statement *cr_st);
        void process_attribute_list(Attribute_list*, vector<string>&);
        void insertTupleToRelation(Relation* , vector<Tuple>&);
        
        bool process_drop_statement(Drop_statement *dr_st);
        
        bool process_select_statement(Select_statement *sl_st);
};

#endif