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
#include <cmath>
#include<climits>

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
        block_ptr=mem->getBlock(memory_block_index);
        block_ptr->clear(); //clear the block
        block_ptr->appendTuple(tuple); // append the tuple
        relation_ptr->setBlock(relation_ptr->getNumOfBlocks(),memory_block_index);
    } 
    else 
    {
        relation_ptr->getBlock(relation_ptr->getNumOfBlocks()-1,memory_block_index);
        block_ptr=mem->getBlock(memory_block_index);

        if (block_ptr->isFull()) {
            block_ptr->clear(); //clear the block
            block_ptr->appendTuple(tuple); // append the tuple
            relation_ptr->setBlock(relation_ptr->getNumOfBlocks(),memory_block_index); //write back to the relation
        } 
        else 
        {
            block_ptr->appendTuple(tuple); // append the tuple
            relation_ptr->setBlock(relation_ptr->getNumOfBlocks()-1,memory_block_index); //write back to the relation
        }
    }  
  }
  
  bool process_select_in_memory(vector<string>& table_names , vector<string>& select_lists, string order_by_att, vector<pair<string,string>>& postfixExpression, char *d, vector<vector<string>>& result)
  {
        vector<string> final_rel;
        Relation *relation_ptr = prepare_join(table_names, final_rel, select_lists, postfixExpression);
    
        //Relation* relation_ptr=schema_manager->getRelation(table_names[0]);
        if(relation_ptr == '\0') //no such table
        {
            return false; 
        }
        
        cout<<"SELECT-STATEMENT: Bring Blocks from Relation: BEGIN "<<endl;
        int relation_size = relation_ptr->getNumOfBlocks();
        int mem_size = mem->getMemorySize();
        bool res;
        //vector<vector<string>> result;
        
        if(mem_size >= relation_size)
        {
            res = one_pass_select_in_memory(relation_ptr, final_rel, select_lists, order_by_att, postfixExpression, result);
        }
        else
        {
            res = two_pass_select_in_memory(relation_ptr, final_rel, select_lists, order_by_att, postfixExpression, result);
        }
        
        if(final_rel[0] != table_names[0])
        {
            schema_manager->deleteRelation(final_rel[0]);
        }
        
        if(res == false)
            return false;
        
        if(d != '\0')    
            result = getDistinct(result);
        
        //printSelect(result);
    
        return true;
  }
  
  bool one_pass_sort(Schema schema, vector<Tuple>& tp, string order_by_att)
  {
         string fields = order_by_att.substr(order_by_att.find(".")+1, order_by_att.size()-1);
         std::cout << "field name is " << fields<<" are  "<<order_by_att<<std::endl;
         
        
         if(schema.fieldNameExists(fields) == false)
            return false;
         
         
         enum FIELD_TYPE typ;
        
         typ = schema.getFieldType(fields);
         
         string cc = "";
          
         if(typ == INT)
         {
            cc = "INT";
         }
         else
            cc= "STR20";
            
         sort(tp.begin(), tp.end(), less_than_key(fields , cc));
         mem->setTuples(0, tp);
         return true;
  }
  
  bool one_pass_select_in_memory(Relation* relation_ptr, vector<string> table_names , vector<string> select_lists, string order_by_att,  vector<pair<string,string>>& postfixExpression, vector<vector<string>>& result)
  {
      cout<<"One pass here "<<endl;
      int relation_size = relation_ptr->getNumOfBlocks();
      relation_ptr->getBlocks(0,0,relation_size);
      vector<Tuple> tp = mem->getTuples(0,relation_size);
      
      Schema schema = schema_manager->getRelation(table_names[0])->getSchema();
      
      if(order_by_att.size()>0)
      {
          string fields = order_by_att.substr(order_by_att.find(".")+1, order_by_att.size()-1);
          
          if(schema.fieldNameExists(fields) == false)
                return false;
          
          if(one_pass_sort(schema, tp, fields) == false) 
              return false;
      }
      
      cout<<*mem<<endl;
      //vector<vector<string>> result;
      for(int i = 0; i< tp.size(); i++)
      {
          vector<string> values;
          if(satisfies_condition(tp[i], table_names[0], postfixExpression) == true)
          {
              
              if(project( tp[i], table_names, select_lists, values) == false)
                    return false;
              
              result.push_back(values);
          }
      }
      
      return true;
  }
  
  void clearMM()
  {
      int mem_size = mem-> getMemorySize();
      Block* block_ptr;
      for( int i=0; i < mem_size; i++)
      {
          block_ptr=mem->getBlock(i);
          block_ptr->clear(); //clear the block   
      }
  }
  
  Relation *two_pass_sort(Relation *relation_ptr, vector<string>& table_names, string fields)
  {
         //string fields = order_by_att.substr(order_by_att.find(".")+1, order_by_att.size()-1);
         //std::cout << "field name is " << fields<<" are  "<<order_by_att<<std::endl;
         Schema schema = schema_manager->getRelation(table_names[0])->getSchema();
         Relation *temp_ptr = schema_manager->createRelation("sublist_rel", schema);
         Relation *curr_ptr = schema_manager->createRelation("final_rel", schema);
         
         int mem_size = mem->getMemorySize();
         int relation_size = relation_ptr->getNumOfBlocks();
         
         enum FIELD_TYPE typ;
         
         typ = schema.getFieldType(fields);
         
         
         string cc = "";
          
         if(typ == INT)
         {
            cc = "INT";
         }
         else
            cc= "STR20";
            
         cout<<*relation_ptr<<endl;
         int read_blocks_no = mem_size;
         int no_part = 0;
         int rest_tp = 0;
         for(int i = 0; i < relation_size;)
         {
            if((i+ mem_size) < relation_size)
            {
                read_blocks_no = mem_size;
                relation_ptr->getBlocks(i, 0, mem_size);
            }
            else
            {
                read_blocks_no = relation_size - i;
                rest_tp = read_blocks_no;
                relation_ptr->getBlocks(i,0 , read_blocks_no);
            }

            vector<Tuple> tp = mem->getTuples(0, read_blocks_no);

            sort(tp.begin(), tp.end(), less_than_key(fields , cc));
            mem->setTuples(0, tp);
            temp_ptr->setBlocks(i,0 , read_blocks_no);
            tp.clear();
            i = i + mem_size;
            no_part++;
         }
         
         int tp_bl = schema.getTuplesPerBlock();
         int count_arr[no_part];
         int count_bl[no_part];
         int exh[no_part];
         
         clearMM();
         int j = 0;
         for(int i = 0; i < no_part; i++)
         {
             count_arr[i] = 0;
             count_bl[i] = 0;
             exh[i] = 0;
             temp_ptr->getBlock(j, i);
             j = j + mem_size;
         }
         
         vector<Tuple> tp = mem->getTuples(0, no_part);
         int sorted_count = 0;
         string str1 = "";
         int int_val = INT_MIN;
         
         //cout<<"I want this "<<endl;
         //cout<< *mem <<endl;
         
         int no_tp = schema.getTuplesPerBlock();
         Block *bl = mem->getBlock(mem_size - 1);
         int bl_count = 0;
         
         while(sorted_count != relation_size)
         {
            int i=0;
            int min = INT_MAX;
            string minStr = "";
            int min_pos = -1;
            
            if(cc == "INT")
            {
                for(i = 0; i < no_part; i++)
                {
                    //cout<<"aaa"<<endl;
                    int temp = tp[i*no_tp + count_arr[i]].getField(fields).integer;
                    //cout<<"bb"<<endl;
                    if(temp < min && exh[i] != 1)
                    {
                        min = temp;
                        min_pos = i;
                    }
                    //cout<<"ccc"<<endl;
                }    
            }
            else
            {
                minStr = *(tp[i*no_tp + count_arr[0]].getField(fields).str);
                min_pos = 0;
                for(i = 0; i < no_part && count_bl[i] < mem_size; i++)
                {
                    string temp = *(tp[i*no_tp + count_arr[0]].getField(fields).str);
                    if(temp < minStr)
                    {
                        minStr = temp;
                        min_pos = i;
                    }
                }
            }
            
            //cout<<"minnnnnnnnnnnnn issssssssssssssss  "<<min<<endl;
            
            if(!bl->isFull() && bl_count < no_tp)
            {
                //cout<<"ddd"<<endl;
                bl->appendTuple(tp[min_pos*no_tp + count_arr[0]]);
                bl_count ++;
                //cout<<"eeee"<<endl;
                //cout<<"cccccccccccccccccccccc"<<*bl<<endl;
            }
            else
            {
                bl_count = 0;
                //cout<<"fff"<<endl;
                //cout<<"fffffffffffffffffffffff ss cnt  "<<*bl<<endl;
                curr_ptr->setBlock(sorted_count, mem_size - 1);
                //cout<<"ggg"<<endl;
                sorted_count ++;
                bl->clear();
                bl->appendTuple(tp[min_pos*no_tp + count_arr[0]]);
                //cout<<"hhh"<<endl;
                bl_count ++;
            }
            
            count_arr[min_pos]++;
            if(count_arr[min_pos] == no_tp )
            {
                
                count_bl[min_pos] ++;
                //cout<<"iii"<<count_bl[min_pos]<<endl;
                if(count_bl[min_pos] >= mem_size || (min_pos == (no_part - 1) && count_arr[min_pos] >= rest_tp))
                {
                    exh[min_pos] = 1;
                        
                    //cout<<"kkkk"<<endl;
                    
                }
                else
                {
                    //cout<<"lll"<<endl;
                    temp_ptr->getBlock(min_pos*mem_size + count_bl[min_pos], min_pos);
                    tp.clear();
                    tp = mem->getTuples(0, no_part);
                    //cout<<"mmm"<<endl;
                }
                count_arr[min_pos] = 0;
                
                
                //cout<<"nnnn"<<endl;
            }
            
            cout<<sorted_count<<endl;
            //cout<<"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"<<sorted_count<<endl;
            
         }
         cout<<*curr_ptr<<endl;
         schema_manager->deleteRelation("sublist_rel");
         return curr_ptr;
  }

  
  bool two_pass_select_in_memory(Relation* relation_ptr, vector<string> table_names , vector<string> select_lists, string order_by_att,  vector<pair<string,string>>& postfixExpression, vector<vector<string>>& result)
  {
        cout<<"Two pass here "<<endl;
        int mem_size = mem->getMemorySize();
        int relation_size = relation_ptr->getNumOfBlocks();
        //int sorted = 0;
        
        //cout<<"Tuple based select is here "<<relation_size<<"    "<<endl;
        Schema schema = schema_manager->getRelation(table_names[0])->getSchema();
        std::cout << "sssssssssssssssschhhemeler" <<schema<< std::endl;
        /*if(d != '\0')
        {
            string fields = "";
            if(order_by_att.size() > 0 && select_lists[0][1] == '*')
            {
                sorted = 1;
                fields = order_by_att.substr(order_by_att.find(".")+1, order_by_att.size()-1);
            }
            else if(order_by_att.size() == 0 && select_lists[0][1] == '*')
                fields = schema.getFieldName(0);
            else
            {
                if(find(select_lists.begin(), select_lists.end(), order_by_att) != select_lists.end())
                {
                    sorted = 1;
                    fields = order_by_att.substr(order_by_att.find(".")+1, order_by_att.size()-1);
                }
                else
                    fields = select_lists[0].substr(select_lists[0].find(".")+1, select_lists[0].size()-1);
            }
            relation_ptr = two_pass_sort(relation_ptr, table_names, fields);
        }*/
        
        if(order_by_att.size()>0)
        {
            string fields = order_by_att.substr(order_by_att.find(".")+1, order_by_att.size()-1);
         
            if(schema.fieldNameExists(fields) == false)
                return false;
        
            relation_ptr = two_pass_sort(relation_ptr, table_names, fields);
        }
        
        //cout<<"fffffffffffffffffffff"<<endl;
        int read_blocks_no = mem_size;
        for(int i = 0; i < relation_size;)
        {
            if((i+ mem_size) < relation_size)
            {
                read_blocks_no = mem_size;
                relation_ptr->getBlocks(i, 0, mem_size);
            }
            else
            {
                read_blocks_no = relation_size - i;
                relation_ptr->getBlocks(i,0 , read_blocks_no);
            }

            vector<Tuple> tp = mem->getTuples(0, read_blocks_no);
            //vector<vector<string>> result;
            for(int j = 0; j< tp.size(); j++)
            {
                vector<string> values;
                if(satisfies_condition(tp[j], table_names[0], postfixExpression) == true) //Check the tuple for where clause
                {
                    if(project( tp[j], table_names, select_lists, values) == false)
                        return false;
                    
                    result.push_back(values);
                }
            }
            
            i = i + mem_size - 1;
        }
        
        if(order_by_att.size() > 0 )    
            schema_manager->deleteRelation("final_rel");
        
        return true;
}
  

static bool  processTupleOperator(Tuple tuple, string table_names, string op, pair<string,string> p1, pair<string,string> p2, stack<pair<string,string>>& S )
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
                    if(table_names.find("temp") != 0)
                    {
                        //cout<<"iffffffffffffffffffffffff "<<table_names<<endl;
                        t = s.substr(found+1);
                    }
                    else
                    {
                        //cout<<"iffffffffffffffffffffffff "<<table_names<<endl;
                        t = s;
                    }
                        
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
                    //t = s.substr(found+1);
                    
                    if(table_names.find("temp") != 0)
                    {
                        //cout<<"iffffffffffffffffffffffff "<<table_names<<endl;
                        t = s.substr(found+1);
                    }
                    else
                    {
                        //cout<<"iffffffffffffffffffffffff "<<table_names<<endl;
                        t = s;
                    }
                                            
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

    }else{       //(p2.second <> "LITERAL" and p1.second <> "LITERAL")                    
        if(p1.second == "COLUMN-NAME")
        {
             s = p1.first;
             found = s.find('.');
            //extract the column name 
             //t = s.substr(found+1);
             if(table_names.find("temp") != 0)
             {
                //cout<<"iffffffffffffffffffffffff "<<table_names<<endl;
                t = s.substr(found+1);
             }
             else
             {
                //cout<<"iffffffffffffffffffffffff "<<table_names<<endl;
                t = s;
             }
                                    
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
            //t = s.substr(found+1);
            if(table_names.find("temp") != 0)
            {
                //cout<<"iffffffffffffffffffffffff "<<table_names<<endl;
                t = s.substr(found+1);
            }
            else
            {
                //cout<<"iffffffffffffffffffffffff "<<table_names<<endl;
                t = s;
            }
                                    
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
  
  static bool satisfies_condition(Tuple tp,string table_names, vector<pair<string,string>>& postfixExpression)
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
                        
                        if ( postfixExpression[i].first ==  "OR")
                        {
                            if(p1.second != "TRUTH-VALUE" || p2.second != "TRUTH-VALUE")
                            {
                                return false;
                            }else //both operands of OR are truth values
                            {
                                //if either is 1 push 1, else push 0
                                if(p1.first == "1" || p2.first == "1")
                                    S.push(make_pair("1", "TRUTH-VALUE" ));
                                else
                                    S.push(make_pair("0", "TRUTH-VALUE" ));
                            }
                        }else if (postfixExpression[i].first ==  "AND") 
                        {
                            if(p1.second != "TRUTH-VALUE" || p2.second != "TRUTH-VALUE")
                            {
                                return false;
                            }else //both operands of AND are truth values
                            {
                                //if both are 1 push 1, else push 0
                                if(p1.first == "1" && p2.first == "1")
                                    S.push(make_pair("1", "TRUTH-VALUE" ));
                                else
                                    S.push(make_pair("0", "TRUTH-VALUE" ));
                            }
                            
                        }else 
                        if (postfixExpression[i].first ==  "<" || postfixExpression[i].first ==  ">" || postfixExpression[i].first ==  "+" ||postfixExpression[i].first ==  "-"||postfixExpression[i].first ==  "*" || postfixExpression[i].first ==  "=")
                        {
                            bool flag = processTupleOperator(tp, table_names, postfixExpression[i].first,p1, p2,S );
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
  
  void getMinTwo(vector<int> relation_size, int &min, int &s_min)
  {
      int tmin = INT_MAX;
      for(int i = 0; i < relation_size.size(); i++)
      {
          //cout<<"pos is "<<i<<"  aaa   "<<relation_size[i];
          if(relation_size[i] < tmin)
          {
              tmin = relation_size[i];
              min = i;
          }
      }
      
      tmin = INT_MAX;
      for(int i = 0; i < relation_size.size(); i++)
      {
          if(relation_size[i] < tmin && i != min)
          {
              tmin = relation_size[i];
              s_min = i;
          }
      }
  }
  
  static bool satisfies_condition1(Tuple tp, string table_names, vector<string> field_name, vector<enum FIELD_TYPE> field_types, vector<pair<string,string>>& postfixExpression)
  {
      return true;
  }
  
  //Performs join of multiple tables
  Relation* prepare_join(vector<string> table_namess, vector<string>& final_rel, vector<string>& select_lists, vector<pair<string,string>>& postfixExpression)
  {
      vector<string> table_names(table_namess);
      if(table_names.size() == 1)
      {
          Relation *ptr = schema_manager->getRelation(table_names[0]);
          final_rel.push_back(table_names[0]);
          return ptr;
      }
      
      vector<Relation*> relation_ptr;
      vector<int> relation_size;
      int min = -1;
      int s_min = -1;
      cout<<"prepare_join comes"<<endl;
      
      for(int i =0; i < table_names.size(); i++)
      {
            Relation* rl = schema_manager->getRelation(table_names[i]);
            if(rl == '\0') //no such table
            {
                return rl; 
            }
            relation_ptr.push_back(rl);
            relation_size.push_back(relation_ptr[i]->getNumOfBlocks());
      }
      
      int sz =  table_names.size();
      Relation *final_rel_ptr;
      for(int i =0; i < sz-1; i++)
      {
          getMinTwo(relation_size, min, s_min);
          final_rel_ptr = join(table_names, relation_ptr, relation_size, min, s_min, i, postfixExpression);
      }
      string ss = "temp"+to_string(sz-2);
      final_rel.push_back(ss);
      
      for(int i =0; i < sz-2; i++)
      {
          schema_manager->deleteRelation("temp"+to_string(i));
      }
      
      for(int i =0; i < select_lists.size(); i++)
      {
          if(select_lists[i][1] != '*')
            select_lists[i] = "."+ select_lists[i];
      }
      
      return final_rel_ptr;
  }
  
  //Joins two current minimum tables and create a temporary relation
  //Performs both one pass and two pass join
  Relation* join(vector<string>& table_names, vector<Relation*>& relation_ptr, vector<int>& relation_size, int min, int s_min, int itr, vector<pair<string,string>>& postfixExpression)
  {
      vector<string> values;
      Schema schema1 = schema_manager->getRelation(table_names[min])->getSchema();
      Schema schema2 = schema_manager->getRelation(table_names[s_min])->getSchema();
      vector<string> field_name = schema1.getFieldNames();
      vector<enum FIELD_TYPE> field_types = schema1.getFieldTypes();
      
      vector<string> field_name1;
      vector<enum FIELD_TYPE> field_types1;
      
      vector<string> field_name2 = schema2.getFieldNames();
      vector<enum FIELD_TYPE> field_types2 = schema2.getFieldTypes();
      int no_fields1 = schema1.getNumOfFields();
      int no_fields2 = schema2.getNumOfFields();
      cout<<"join comes "<<endl;
      
      for(int i =0; i < no_fields1; i++)
      {
          
          if(table_names[min].find("temp") != 0)
          {
            field_name[i] = table_names[min]+"."+field_name[i];
          }
          
          field_name1.push_back(field_name[i]);
          field_types1.push_back(field_types[i]);
      }
      
      for(int i =0; i < no_fields2; i++)
      {
          if(table_names[s_min].find("temp") != 0)
          {
              field_name2[i] = table_names[s_min]+"."+field_name2[i];
          }
          
          field_name1.push_back(field_name2[i]);
          field_types1.push_back(field_types2[i]);
      }
      
      Schema schema(field_name1 , field_types1);      
      string relation_name = "temp"+to_string(itr);
      Relation* temp_ptr=schema_manager->createRelation(relation_name, schema);
      
      if(temp_ptr == '\0')
      {
          return temp_ptr;
      }
      
      int mem_size = mem->getMemorySize();
      if(mem_size - 2 > relation_size[min])
      {
          //One pass join starts
          relation_ptr[min]->getBlocks(0,0,relation_size[min]);
          vector<Tuple> tp1 = mem->getTuples(0,relation_size[min]);
          
          for(int i = 0; i < tp1.size(); i++)
          {
              if(satisfies_condition1(tp1[i], table_names[min], field_name, field_types, postfixExpression) == false)
              {
                  continue;
              }
              for(int rl = 0; rl < relation_size[s_min]; rl++)
              {
                relation_ptr[s_min]->getBlocks(rl, relation_size[min], 1);
                vector<Tuple> tp2 = mem->getTuples(relation_size[min], 1);
                for(int j = 0; j < tp2.size(); j++)
                {
                    if(satisfies_condition1(tp2[j], table_names[s_min], field_name2, field_types2,postfixExpression) == false)
                    {
                        continue;
                    }
                    
                    Tuple tuple = temp_ptr->createTuple();
                    int k =0;
                    for(int l =0; l < no_fields1; l++)
                    {
                        if(field_types1[k] == INT)
                        {
                            tuple.setField(k, tp1[i].getField(l).integer);
                        }
                        else
                        {
                            tuple.setField(k, *(tp1[i].getField(l).str));
                        }
                        k++;
                    }
                    
                    for(int l =0; l < no_fields2; l++)
                    {
                        
                        if(field_types1[k] == INT)
                        {
                            tuple.setField(k, tp2[j].getField(l).integer);
                        }
                        else
                        {
                            tuple.setField(k, *(tp2[j].getField(l).str));
                        }
                        k++;
                    }
                    //cout<<tuple<<endl;
                    if(satisfies_condition1(tuple, relation_name, field_name1, field_types1,postfixExpression) == true)
                    {
                        appendTupleToRelation(temp_ptr, relation_size[min]+1, tuple);
                    }
                }
              }
          }
          
          cout<<*temp_ptr<<endl;
      }
      else
      {
        //Two pass join starts
        int read_blocks_no = -1;
        for(int i = 0; i < relation_size[min];)
        {
            if((i+ mem_size-2) < relation_size[min])
            {
                read_blocks_no = mem_size-2;
                relation_ptr[min]->getBlocks(i, 0, mem_size-2);
            }
            else
            {
                read_blocks_no = relation_size[min] - i;
                relation_ptr[min]->getBlocks(i, 0 , read_blocks_no);
            }

            vector<Tuple> tp1 = mem->getTuples(0, read_blocks_no);
            for(int m = 0; m < tp1.size(); m++)
            {
              for(int rl = 0; rl < relation_size[s_min]; rl++)
              {
                relation_ptr[s_min]->getBlocks(rl, read_blocks_no, 1);
                vector<Tuple> tp2 = mem->getTuples(read_blocks_no, 1);
                for(int j = 0; j < tp2.size(); j++)
                {
                    Tuple tuple = temp_ptr->createTuple();
                    int k =0;
                    for(int l =0; l < no_fields1; l++)
                    {
                        if(field_types1[k] == INT)
                        {
                            tuple.setField(k, tp1[m].getField(l).integer);
                        }
                        else
                        {
                            tuple.setField(k, *(tp1[m].getField(l).str));
                        }
                        k++;
                    }
                    
                    for(int l =0; l < no_fields2; l++)
                    {
                        
                        if(field_types1[k] == INT)
                        {
                            tuple.setField(k, tp2[j].getField(l).integer);
                        }
                        else
                        {
                            tuple.setField(k, *(tp2[j].getField(l).str));
                        }
                        k++;
                    }
                    //cout<<tuple<<endl;
                    appendTupleToRelation(temp_ptr, read_blocks_no+1, tuple);
                }
              }
            }
            
            i = i + mem_size - 2;
        }
        cout<<*temp_ptr<<endl;
      }
      
      swap(relation_ptr[min], relation_ptr.back());
      relation_ptr.pop_back();
      swap(relation_size[min], relation_size.back());
      relation_size.pop_back();
      swap(table_names[min], table_names.back());
      table_names.pop_back();
      
      swap(relation_ptr[s_min], relation_ptr.back());
      relation_ptr.pop_back();
      swap(relation_size[s_min], relation_size.back());
      relation_size.pop_back();
      swap(table_names[s_min], table_names.back());
      table_names.pop_back();
      
      relation_ptr.push_back(temp_ptr);
      relation_size.push_back(temp_ptr->getNumOfBlocks());
      table_names.push_back(relation_name);
      
      return temp_ptr;
  }
  
  bool project(Tuple tp, vector<string>& table_names, vector<string>& select_lists, vector<string>& values)
  {
      //cout<<"Projection/Printing: BEGIN:"<<endl;
      Schema schema = schema_manager->getRelation(table_names[0])->getSchema();
      //int pr_flag = 0;
      //int nd_flag = 0;
        
      for(int i = 0; i < select_lists.size(); i++)
      {
          string fields = select_lists[i].substr(select_lists[i].find(".")+1, select_lists[i].size()-1);
          
          if(fields[0] == '*')
          {
              int no = tp.getNumOfFields();
              
              for(int j = 0; j < no; j++)
              {
                  enum FIELD_TYPE typ = schema.getFieldType(j);
                  string cc= "";
          
                  if(typ == INT)
                  {
                      cc = to_string(tp.getField(j).integer);
                  }
                  else
                  {
                      cc= *(tp.getField(j).str);
                  }
              
                  values.push_back(cc);
              }
                
              break;
          }
          else{
          if(schema.fieldNameExists(fields) == false)
            return false;
          
          enum FIELD_TYPE typ = schema.getFieldType(fields);
          string cc= "";
          
          if(typ == INT)
          {
              cc = to_string(tp.getField(fields).integer);
          }
          else
          {
              cc= *(tp.getField(fields).str);
          }
          
          values.push_back(cc);
          }
      }
    
      /*for(int i = 0; i < values.size(); i++)
            {
                cout<<values[i]<<"\t";
            }
            cout<<endl;*/
      return true;
  }
  
  vector<vector<string>> getDistinct(vector<vector<string>> result)
  {
      cout<<"res is "<<result.size()<<endl;
      int sz = result.size();
      int printedd[sz];
      vector<vector<string>> unique;
      
      for(int j = 0; j < sz; j++)
      {
            vector<string> values = result[j];
            if(values.size() == 0)
                continue;
            if(printedd[j] == 1)
                continue;
                
            unique.push_back(values);  
            /*for(int i = 0; i < values.size(); i++)
            {
                cout<<values[i]<<"\t";
            }
            cout<<endl;*/
            for(int i = j + 1; i < sz; i++)
            {
                if(result[i] == values)
                {
                    printedd[i] = 1;
                }
            }
      }
       return unique;
  }
  
  void printSelect(vector<vector<string>> result)
  {
      cout<<"res is "<<result.size()<<endl;
      for(int j = 0; j < result.size(); j++)
      {
            vector<string> values = result[j];
            if(values.size() == 0)
                continue;
            for(int i = 0; i < values.size(); i++)
            {
                cout<<values[i]<<"\t";
            }
            cout<<endl;
      }
      
  }
  
  
};