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
#include <utility>

using namespace std;

class block_manager
{
  public:
  static void appendTupleToRelation(Relation* relation_ptr, MainMemory& mem, int memory_block_index, Tuple& tuple) 
  {
    Block* block_ptr;
    if (relation_ptr->getNumOfBlocks()==0) {
        cout << "The relation is empty" << endl;
        cout << "Get the handle to the memory block " << memory_block_index << " and clear it" << endl;
        block_ptr=mem.getBlock(memory_block_index);
        block_ptr->clear(); //clear the block
        block_ptr->appendTuple(tuple); // append the tuple
        cout << "Write to the first block of the relation" << endl;
        relation_ptr->setBlock(relation_ptr->getNumOfBlocks(),memory_block_index);
    } 
    else 
    {
        cout << "Read the last block of the relation into memory block 5:" << endl;
        relation_ptr->getBlock(relation_ptr->getNumOfBlocks()-1,memory_block_index);
        block_ptr=mem.getBlock(memory_block_index);

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
  
  static bool process_select_in_memory(SchemaManager schema_manager, Disk disk, MainMemory& mem, vector<string> table_names , vector<string> select_lists)
  {
    if(table_names.size() == 1) //Only 1 table involved in the query
    {
        
        Relation* relation_ptr=schema_manager.getRelation(table_names[0]);
        if(relation_ptr == '\0') //no such table
        {
            return false; 
        }
        
        cout<<"SELECT-STATEMENT: Bring Blocks from Relation: BEGIN "<<endl;
        int relation_size = relation_ptr->getNumOfBlocks();
        
        
        cout<<*relation_ptr<<endl;
        
    
        cout<<"Tuple based select is here "<<relation_size<<"    "<<endl;
        
        for(int i = 0; i < relation_size; i++)
        {
            Block *block_ptr = mem.getBlock(0); //Grab 0th main memory block
            block_ptr->clear(); //clear the block
            relation_ptr->getBlock(i , 0);
            
            vector<Tuple> tp = mem.getTuples(0, 1);
            
            for(int i = 0; i< tp.size(); i++)
            {
                if(satisfies_condition(tp[i]) == true) //Check the tuple for where clause
                {
                    if(project(schema_manager.getRelation(table_names[0])->getSchema(), tp[i], select_lists) == false)
                        return false;
                }
            }
            
        }
        
    }
    
    return true;
  }
  
  static bool satisfies_condition(Tuple tp)
  {
      return true;
  }
  
  static bool project(Schema schema, Tuple tp, vector<string>& select_lists)
  {
      vector<string> values;
      
      for(int i = 0; i < select_lists.size(); i++)
      {
          string fields = select_lists[i].substr(select_lists[i].find(".")+1, select_lists[i].size()-1);
          //cout<< fields <<" is the fl nm"<<endl;
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
              //std::cout << "INTTTTTTTTTTTTTTTTTTTTTTT     " << tp.getField(fields).integer<<std::endl;
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
  
  static bool tuple_based_select(SchemaManager schema_manager, Relation* relation_ptr, MainMemory mem, int relation_size, vector<string> table_names , vector<string> select_lists)
  {
      cout<<"One pass here "<<endl;
      int rl_sz = relation_ptr->getNumOfBlocks();
      //
      for(int i = 0; i < rl_sz; i++)
      {
          relation_ptr->getBlock(i, 0);
      }
      
      return true;
  }
  
  static bool two_pass_process(SchemaManager schema_manager, Relation* relation_ptr, MainMemory mem, int relation_size, vector<string> table_names , vector<string> select_lists)
  {
      cout<<"Two pass here "<<endl;
      return true;
  }
};