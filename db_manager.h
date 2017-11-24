#ifndef _db_manager
#define _db_manager

#include<iterator>
#include<cstdlib>
#include<ctime>
#include<string>
#include "Block.h"
#include "Config.h"
#include "Disk.h"
#include "Field.h"
#include "MainMemory.h"
#include "Relation.h"
#include "Schema.h"
#include "SchemaManager.h"
#include "Tuple.h"
#include "typ.h"

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
        bool process_drop_statement(Drop_statement *dr_st);
};

#endif