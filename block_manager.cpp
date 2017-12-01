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
#include <utility>
#include<stack>

using namespace std;

struct less_than_key
{
    string field_name;
    string filed_type;

    less_than_key(string field_name, string filed_type) 
    {
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
  
  bool process_select_in_memory(vector<string>& table_names , vector<string>& select_lists, string order_by_att, vector<pair<string,string>>& postfixExpression)
  {
    if(table_names.size() == 1) //Only 1 table involved in the query
    {
        Relation* relation_ptr=schema_manager->getRelation(table_names[0]);
        if(relation_ptr == '\0') //no such table
        {
            return false; 
        }
        
        cout<<"SELECT-STATEMENT: Bring Blocks from Relation: BEGIN "<<endl;
        int relation_size = relation_ptr->getNumOfBlocks();
        int mem_size = mem->getMemorySize();
        bool res;
        
        if(mem_size >= relation_size)
        {
            res = one_pass_select_in_memory(relation_ptr, table_names, select_lists, order_by_att, postfixExpression);
        }
        else
        {
            res = two_pass_select_in_memory(relation_ptr, table_names, select_lists, order_by_att, postfixExpression);
        }
        
        if(res == false)
            return false;
            
        cout<<*relation_ptr<<endl;
    }
    
    return true;
  }
  
  bool one_pass_select_in_memory(Relation* relation_ptr, vector<string> table_names , vector<string> select_lists, string order_by_att,  vector<pair<string,string>>& postfixExpression)
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
          if(satisfies_condition(tp[i], postfixExpression) == true)
          {
              if(project( tp[i], table_names, select_lists) == false)
                return false;
          }
      }
      
      return true;
  }
  
  bool two_pass_select_in_memory(Relation* relation_ptr, vector<string> table_names , vector<string> select_lists, string order_by_att,  vector<pair<string,string>>& postfixExpression)
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
        
            cout<<*relation_ptr<<endl;
        
    
        cout<<"Tuple based select is here "<<relation_size<<"    "<<endl;
        
        for(int i = 0; i < relation_size; i++)
        {
            Block *block_ptr = mem->getBlock(0); //Grab 0th main memory block
            block_ptr->clear(); //clear the block
            relation_ptr->getBlock(i , 0);
            
            vector<Tuple> tp = mem->getTuples(0, 1);
            
            for(int i = 0; i< tp.size(); i++)
            {
                if(satisfies_condition(tp[i], postfixExpression) == true) //Check the tuple for where clause
                {
                    if(project(tp[i], table_names,select_lists) == false)
                        return false;
                }
            }
            
        }
      
      return true;
  }
}
  

static bool  processTupleOperator(Tuple tuple, string op, pair<string,string> p1, pair<string,string> p2, stack<pair<string,string>>& S )
{
    //p1's value is val2/strVal2; p2's value is val1/strVal1; since p1 is popped first, so it is on right of the operator 
    int val1, val2;
    string s,t;
    int truthVal;
    string truthValType; 
    
    size_t found;
    Schema tuple_schema = tuple.getSchema();
    string strVal1, strVal2;
    //p2<p1 return is a TRUTH VALUE
    if (p2.second == "LITERAL" || p1.second == "LITERAL")
    {//a LITERAL can be involved only in = operator, and the other being a literal or a string columns
        if(p2.second == "LITERAL")
        {
            strVal1 = p2.first;
            if(p1.second == "LITERAL" || p1.second == "COLUMN-NAME")
            {
                if(p1.second == "COLUMN-NAME")
                {
                    //extract the strig value of column
                    s = p1.first;
                    found = s.find('.');
                    //extract the column name 
                    t = s.substr(found+1);
                                            
                    if (tuple_schema.getFieldType(t) == INT) 
                        return false;
                    else
                    {
                        strVal2 = *(tuple.getField(t).str);
                    }
                        
                }else{//p1.second == "LITERAL"
                    strVal2 = p1.first;
                }
            }else return false;
            
        }else //p1.second == "LITERAL"
        {
            strVal2 = p1.first;
            if(p2.second == "LITERAL" || p2.second == "COLUMN-NAME")
            {
                if(p2.second == "COLUMN-NAME")
                {
                    //extract the strig value of column
                    s = p2.first;
                    found = s.find('.');
                    //extract the column name 
                    t = s.substr(found+1);
                                            
                    if (tuple_schema.getFieldType(t) == INT) 
                        return false;
                    else
                    {
                        strVal1 = *(tuple.getField(t).str);
                    }
                        
                }else{//p1.second == "LITERAL"
                    strVal1 = p2.first;
                }
            }else return false;
        }
        
        if(op != "=")  //Literals/ Columns with string type only work with equal operator
            return false;
        
        if(strVal1 == strVal2)
        {
            truthVal = 1;
        }
        else
        {
            truthVal = 0;
        }
        truthValType = "TRUTH-VALUE";

    }else{                           
        if(p1.second == "COLUMN-NAME")
        {
             s = p1.first;
             found = s.find('.');
            //extract the column name 
             t = s.substr(found+1);
                                    
            if (tuple_schema.getFieldType(t) == INT) 
                val2 = tuple.getField(t).integer;
            else
                return false;
                                        //cout << tuple.getField(t).str << "\t";
      
        }else val2 = stoi(p1.first);
                                
        if(p2.second == "COLUMN-NAME")
        {
            s = p2.first;
            found = s.find('.');
            //extract the column name 
            t = s.substr(found+1);
                                    
            if (tuple_schema.getFieldType(t) == INT) 
              val1 = tuple.getField(t).integer;
            else
                return false;
                                        //cout << tuple.getField(t).str << "\t";
      
        }else val1 = stoi(p2.first);
                                
        //we have v1 and v2
        //we will calculate the truth value and push it back to stack
        truthValType = "TRUTH-VALUE";
        if (op == "<")
            truthVal = (val1 < val2)?1:0;
        if (op == ">")
            truthVal = (val1 > val2)?1:0;
        if (op == "=")
            truthVal = (val1 == val2)?1:0;
        if (op == "+")
        {
            truthVal = (val1 + val2);
            truthValType = "OP-VALUE";
        }
        if (op == "-")
        {
            truthVal = (val1 - val2);
            truthValType = "OP-VALUE";
        }
        if (op == "*")
        {    
            truthVal = (val1 * val2);
            truthValType = "OP-VALUE";
        }
    }     
    S.push(make_pair(to_string(truthVal), truthValType ));
    return true;
  }
  
  static bool satisfies_condition(Tuple tp, vector<pair<string,string>>& postfixExpression)
  {
      if(postfixExpression.size() == 0) return true; //if there is no where clause, all tuples satisfy
      stack<pair<string,string>> S;
      /*
        1/0 TRUTH-VALUE
        Integer OP-VALUE
      
      */
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
                        if (postfixExpression[i].first ==  "<" || postfixExpression[i].first ==  ">" || postfixExpression[i].first ==  "+" ||postfixExpression[i].first ==  "-"||postfixExpression[i].first ==  "*" || postfixExpression[i].first ==  "=")
                        {
                            bool flag = processTupleOperator(tp,postfixExpression[i].first,p1, p2,S );
                            if (flag == false)
                                return false;
                        }
                        // else 
                        // if (postfixExpression[i].first ==  "=")
                        // {
                        //     if(p1.second == "LITERAL" || p2.second == "LITERAL" ){
                        //         processTupleLiteral(tp,postfixExpression[i].first,p1, p2,S);
                        //     }
                        // }
                    }
                    
      }
      
      pair<string,string> ret = S.top();
      S.pop();
      
      if(ret.first == "1")
        return 1;
       else return 0;
     
  }
  
  bool project(Tuple tp, vector<string>& table_names, vector<string>& select_lists)
  {
      cout<<"Projection/Printing: BEGIN:"<<endl;
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