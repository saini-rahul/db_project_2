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
#include<stack>

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
  
  static bool process_select_in_memory(SchemaManager schema_manager, Disk disk, MainMemory& mem, vector<string> table_names , vector<string> select_lists, vector<pair<string,string>>& postfixExpression)
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
                if(satisfies_condition(tp[i], postfixExpression) == true) //Check the tuple for where clause
                {
                    if(project(schema_manager.getRelation(table_names[0])->getSchema(), tp[i], select_lists) == false)
                        return false;
                }
            }
            
        }
        
    }
    
    return true;
  }
  
  
static bool  processTupleOperator(Tuple tuple, string op, pair<string,string> p1, pair<string,string> p2, stack<pair<string,string>>& S )
{
    int val1, val2;
    string s,t;
    size_t found;
    Schema tuple_schema = tuple.getSchema();

    //p2<p1 return is a TRUTH VALUE
    if (p2.second == "LITERAL" || p1.second == "LITERAL")
        return false;
                                
    if(p1.second == "COLUMN-NAME")
    {
         s = p1.first;
         found = s.find('.');
        //extract the column name 
         t = s.substr(found+1);
                                
        if (tuple_schema.getFieldType(t) == INT) 
            val1 = tuple.getField(t).integer;
        else
            return false;
                                    //cout << tuple.getField(t).str << "\t";
  
    }else val1 = stoi(p1.first);
                            
    if(p2.second == "COLUMN-NAME")
    {
        s = p2.first;
        found = s.find('.');
        //extract the column name 
        t = s.substr(found+1);
                                
        if (tuple_schema.getFieldType(t) == INT) 
          val2 = tuple.getField(t).integer;
        else
            return false;
                                    //cout << tuple.getField(t).str << "\t";
  
    }else val2 = stoi(p2.first);
                            
    //we have v1 and v2
    //we will calculate the truth value and push it back to stack
    int truthVal;
    if (op == "<")
        truthVal = (val2 < val1)?1:0;
    if (op == ">")
        truthVal = (val2 > val1)?1:0;
    if (op == "=")
        truthVal = (val2 == val1)?1:0;
    if (op == "+")
        truthVal = (val2 + val1);
    if (op == "-")
        truthVal = (val2 - val1);
    if (op == "*")
        truthVal = (val2 * val1);
    
        
    S.push(make_pair(to_string(truthVal), "OP-VALUE"));
    return true;
  }
  
  static bool satisfies_condition(Tuple tp, vector<pair<string,string>>& postfixExpression)
  {
      if(postfixExpression.size() == 0) return true; //if there is no where clause, all tuples satisfy
      stack<pair<string,string>> S;
    //   int fail_flag = 0;
      //evaluate postfix expression 
      for(int i =0; i<postfixExpression.size(); i++)
      {
                    if (postfixExpression[i].second != "OPERATOR") //postfixExpression[i].first
                    {
                        //this is an operand just push it in stack.
                        S.push(postfixExpression[i]);
                    }else
                    {
                        //this is a operator. Pop two guys out of stack, and push back the result onto stack
                        pair<string,string> p1 = S.top();
                        S.pop();
                        pair<string,string> p2 = S.top();
                        S.pop();
                        
                        // if ( postfixExpression[i].first ==  "OR")
                        // {
                        //     //p1 and p2 have to be truth values and if either is 1 we return a truth value of 1
                        // }else if (postfixExpression[i].first ==  "AND") 
                        // {
                            
                        // }else 
                        if (postfixExpression[i].first ==  "<" || postfixExpression[i].first ==  ">" || postfixExpression[i].first ==  "=" || postfixExpression[i].first ==  "+" ||postfixExpression[i].first ==  "-"||postfixExpression[i].first ==  "*" )
                        {
                            bool flag = processTupleOperator(tp,postfixExpression[i].first,p1, p2,S );
                            if (flag == false)
                                return false;
                        } 
                    }
                    
      }
      
      pair<string,string> ret = S.top();
      S.pop();
      
      if(ret.first == "1")
        return 1;
       else return 0;
     
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