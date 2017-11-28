#include<iostream>
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
#include<climits>
#include <string>
#include <algorithm>
using namespace std;

struct less_than_key
{
    string field_name;
    string filed_type;
    /*MainMemory *mem;
    Disk *disk;
    SchemaManager *schema_manager;*/
    
    less_than_key(string field_name, string filed_type) 
    {
        //this->mem = mem;
        //this->disk = disk;
        //this->schema_manager = schema_manager;
        this->field_name = field_name; 
        this->filed_type = filed_type;
    }
    
    inline bool operator() (const Tuple& tp1, const Tuple& tp2)
    {
        if(filed_type == "INT")
        {
            int val1 = tp1.getField(field_name).integer;
            int val2 = tp2.getField(field_name).integer;
            return (val1 < val2);
        }
        else
        {
            string str1 = *(tp1.getField(field_name).str);
            string str2 = *(tp2.getField(field_name).str);
            return (str1 < str2);
        }
    }
};


class block_manager
{
  private:
        MainMemory *mem;
        Disk *disk;
        SchemaManager *schema_manager;
    
  public:
        block_manager(MainMemory *mm, Disk *d, SchemaManager *schema_manager)
        {
            this->mem = mm;
            this->disk= d;
            this->schema_manager = schema_manager;
        }
  
  void appendTupleToRelation(Relation* relation_ptr, int memory_block_index, Tuple& tuple) 
  {
    Block* block_ptr;
    if (relation_ptr->getNumOfBlocks()==0) {
        cout << "The relation is empty" << endl;
        cout << "Get the handle to the memory block " << memory_block_index << " and clear it" << endl;
        block_ptr=mem->getBlock(memory_block_index);
        block_ptr->clear(); //clear the block
        block_ptr->appendTuple(tuple); // append the tuple
        cout << "Write to the first block of the relation" << endl;
        relation_ptr->setBlock(relation_ptr->getNumOfBlocks(),memory_block_index);
    } 
    else 
    {
        cout << "Read the last block of the relation into memory block 5:" << endl;
        relation_ptr->getBlock(relation_ptr->getNumOfBlocks()-1,memory_block_index);
        block_ptr=mem->getBlock(memory_block_index);

        if (block_ptr->isFull()) {
            cout << "(The block is full: Clear the memory block and append the tuple)" << endl;
            block_ptr->clear(); //clear the block
            block_ptr->appendTuple(tuple); // append the tuple
            cout << "Write to a new block at the end of the relation" << endl;
            relation_ptr->setBlock(relation_ptr->getNumOfBlocks(),memory_block_index); //write back to the relation
        } 
        else 
        {
            cout << "(The block is not full: Append it directly)" << endl;
            block_ptr->appendTuple(tuple); // append the tuple
            cout << "Write to the last block of the relation" << endl;
            relation_ptr->setBlock(relation_ptr->getNumOfBlocks()-1,memory_block_index); //write back to the relation
        }
    }  
  }
  
  
  bool process_select_in_memory(vector<string>& table_names , vector<string>& select_lists, string order_by_att)
  {
    if(table_names.size() == 1)
    {
        Relation* relation_ptr=schema_manager->getRelation(table_names[0]);
        if(relation_ptr == '\0')
        {
            return false;
        }
        
        cout<<"Tuple based select is here "<<endl;
        int relation_size = relation_ptr->getNumOfBlocks();
        int mem_size = mem->getMemorySize();
        bool res;
        
        if(mem_size >= relation_size)
        {
            res = one_pass_select_in_memory(relation_ptr, table_names, select_lists, order_by_att);
        }
        else
        {
            res = two_pass_select_in_memory(relation_ptr, table_names, select_lists, order_by_att);
        }
        
        if(res == false)
            return false;
            
        cout<<*relation_ptr<<endl;
    }
    
    return true;
  }
  
  bool one_pass_select_in_memory(Relation* relation_ptr, vector<string> table_names , vector<string> select_lists, string order_by_att)
  {
      cout<<"One pass here "<<endl;
      int relation_size = relation_ptr->getNumOfBlocks();
      relation_ptr->getBlocks(0,0,relation_size);
      vector<Tuple> tp = mem->getTuples(0,relation_size);
      
      if(order_by_att.size()>0)
      {
         Schema schema = schema_manager->getRelation(table_names[0])->getSchema();
         
         string fields = order_by_att.substr(order_by_att.find(".")+1, order_by_att.size()-1);
          
         if(schema.fieldNameExists(fields) == false)
            return false;
          
         enum FIELD_TYPE typ = schema.getFieldType(fields);
         string cc = "";
          
         if(typ == INT)
         {
            cc = "INT";
         }
         else
            cc= "STR20";
            
         sort(tp.begin(), tp.end(), less_than_key(fields , cc));
         
      }
      
      cout<<"sorted starts here aaaaaaaaaaaaaaaaaaaaaaaaaa"<<endl;
      cout<<mem<<endl;
      cout<<"sorted ends here aaaaaaaaaaaaaaaaaaaaaaaaaa"<<endl;
      
      for(int i = 0; i< tp.size(); i++)
      {
          if(satisfies_condition(tp[i]) == true)
          {
              if(project( tp[i], table_names, select_lists) == false)
                return false;
          }
      }
      
      return true;
  }
  
  bool two_pass_select_in_memory(Relation* relation_ptr, vector<string> table_names , vector<string> select_lists, string order_by_att)
  {
      cout<<"Two pass here "<<endl;
      int mem_size = mem->getMemorySize();
      int relation_size = relation_ptr->getNumOfBlocks();
      for(int i = 0; i < relation_size; i++)
        {
            //Block *block_ptr=mem->getBlock(0);
            //block_ptr->clear(); //clear the block
            relation_ptr->getBlock(i , 0);
            vector<Tuple> tp = mem->getTuples(0, 1);
            
            for(int i = 0; i< tp.size(); i++)
            {
                if(satisfies_condition(tp[i]) == true)
                {
                    if(project(tp[i], table_names,select_lists) == false)
                        return false;
                }
            }
            
        }
      
      return true;
  }
  
  bool satisfies_condition(Tuple tp)
  {
      
      return true;
  }
  
  bool project(Tuple tp, vector<string>& table_names, vector<string>& select_lists)
  {
      vector<string> values;
      Schema schema = schema_manager->getRelation(table_names[0])->getSchema();
      for(int i = 0; i < select_lists.size(); i++)
      {
          string fields = select_lists[i].substr(select_lists[i].find(".")+1, select_lists[i].size()-1);
          
          if(fields[0] == '*')
          {
              tp.printTuple();
              return true;
          }
          if(schema.fieldNameExists(fields) == false)
            return false;
          
          enum FIELD_TYPE typ = schema.getFieldType(fields);
          string cc= "";
          
          if(typ == INT)
          {
              cc = to_string(tp.getField(fields).integer);
          }
          else
              cc= *(tp.getField(fields).str);
              
          values.push_back(cc);
      }

      for(int i = 0; i < values.size(); i++)
      {
          cout<<values[i]<<"\t";
      }

      return true;
  }
  
  
};