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
#include <string.h>
#include <algorithm>
#include <utility>
#include<stack>
#include <cmath>
#include<climits>

using namespace std;

//Performs operations involving memory operations

//Comparator class for sorting
struct less_than_key_distinct
{
    string filed_type;

    inline bool operator() (const pair<vector<string>,string>& tp1, const pair<vector<string>,string>&  tp2)
    {
        string str1 = tp1.second;
        string str2 = tp2.second;
        int len1 = str1.size();
        int len2 = str2.size();
        int i =0;
        int j =0;
        
        while(i < len1 && j < len2)
        {
            string ss1 = "";
            string ss2 = "";
            int flag1 = 1;
            int flag2 = 1;
            
            
            for(; i < len1; i++)
            {
                if(str1[i] == '\"')
                {
                    break;
                }
                ss1 = ss1 + str1[i];
                if(isdigit(str1[i]) == false)
                {
                    flag1 = 0;
                }
            }
            i++;
            
            for(; j < len2; j++)
            {
                if(str2[j] == '\"')
                {
                    break;
                }
                ss2 = ss2 + str2[j];
                if(isdigit(str2[j]) == false)
                {
                    flag2 = 0;
                }
            }
            j++;
            
            if(flag1 == 1)
            {
                if(ss1 != ss2)
                    return (stoi(ss1) < stoi(ss2));
            }
            
            if( ss1 != ss2)
            {
                return (ss1 < ss2);
            }
        }
        
        return (str1 < str2);
    }
};

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

//class to performs operations involving memory operations
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
        
  //Appends tuple to a given relation
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
        
        int relation_size = relation_ptr->getNumOfBlocks();
        int mem_size = mem->getMemorySize();
        bool res;
        
        if(mem_size >= relation_size)
        {
            res = one_pass_select_in_memory(relation_ptr, final_rel, select_lists, order_by_att, postfixExpression, result, d);
        }
        else
        {
            res = two_pass_select_in_memory(relation_ptr, final_rel, select_lists, order_by_att, postfixExpression, result, d);
        }
        
        if(final_rel[0] != table_names[0])
        {
            schema_manager->deleteRelation(final_rel[0]);
        }
        
        if(res == false)
            return false;
        
        
        //printSelect(result);
    
        return true;
  }
  
  bool one_pass_sort(Schema schema, vector<Tuple>& tp, string order_by_att)
  {
         string fields = order_by_att;
        
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
  
  
  bool one_pass_distinct(Schema schema, vector<Tuple>& tp, vector<vector<string>>& result, string fields)
  {
         if(tp.size() == 0)
            return true;
         
         vector<pair<vector<string>,string>> temp_res;
         for(int i = 0; i < result.size(); i++)
         {
            string ss = "";
            for(int j = 0; j < result[i].size(); j++)
            {
                ss = ss + result[i][j];
                if(j != result[i].size()-1)
                {
                    ss = ss + +"\"";
                }
            }
            temp_res.push_back(pair<vector<string>,string> (result[i], ss));
         }
         
         sort(temp_res.begin(), temp_res.end(), less_than_key_distinct());
         result.clear();
         
         for(int i = 0; i < temp_res.size(); i++)
         {
             if(i == 0)
                result.push_back(temp_res[i].first);
             else
             {
                 if(temp_res[i].second != temp_res[i - 1].second)
                 {
                     result.push_back(temp_res[i].first);
                 }
             }
         }
        
         
         mem->setTuples(0, tp);
         return true;
  }
  
  bool one_pass_select_in_memory(Relation* relation_ptr, vector<string> table_names , vector<string> select_lists, string order_by_att,  vector<pair<string,string>>& postfixExpression, vector<vector<string>>& result, char *d)
  {
      int relation_size = relation_ptr->getNumOfBlocks();
      relation_ptr->getBlocks(0,0,relation_size);
      vector<Tuple> tp = mem->getTuples(0,relation_size);
      
      Schema schema = schema_manager->getRelation(table_names[0])->getSchema();
      
      if(order_by_att.size()>0)
      {
          string fields = "";
          if(table_names[0].find("temp") != 0)
                fields = order_by_att.substr(order_by_att.find(".")+1, order_by_att.size()-1);
            else
                fields = order_by_att;
          
          if(schema.fieldNameExists(fields) == false)
                return false;
          
          if(one_pass_sort(schema, tp, fields) == false) 
              return false;
      }
      
      vector<string> att_nm = add_attribute_name(table_names, select_lists, result);
      result.push_back(att_nm);
      
      vector<Tuple> tp_list;
      for(int i = 0; i< tp.size(); i++)
      {
          vector<string> values;
          if(satisfies_condition(tp[i], table_names[0], postfixExpression) == true)
          {
              
              if(project( tp[i], table_names, select_lists, values) == false)
                    return false;
              tp_list.push_back(tp[i]);
              result.push_back(values);
          }
      }
      
      if(d != '\0')
      {
          if(one_pass_distinct(schema, tp_list, result, select_lists[0]) == false) 
              return false;
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
  
  Relation *two_pass_distinct(Relation *relation_ptr, string& table_names, string fields, vector<vector<string>>& result)
  {
         Schema schema = schema_manager->getRelation(table_names)->getSchema();
         Relation *temp_ptr = schema_manager->createRelation("sublist_rel", schema);
         Relation *curr_ptr = schema_manager->createRelation(table_names+"_final_rel", schema);
         
         int mem_size = mem->getMemorySize();
         int relation_size = relation_ptr->getNumOfBlocks();
         
         enum FIELD_TYPE typ;
         if(fields[0] == '*' || (fields.size() > 1 && (fields[0] == '*' || fields[1] == '*')))
            fields = schema.getFieldName(0);
         if(fields[0] == '.')
            fields = fields.substr(fields.find(".")+1, fields.size()-1);
            
         typ = schema.getFieldType(fields);
         
         
         string cc = "";
          
         if(typ == INT)
         {
            cc = "INT";
         }
         else
            cc= "STR20";
            
         int read_blocks_no = mem_size - 1;
         int no_part = 0;
         int rest_tp = 0;
         for(int i = 0; i < relation_size;)
         {
            if((i+ mem_size - 1) < relation_size)
            {
                read_blocks_no = mem_size - 1;
                relation_ptr->getBlocks(i, 0, mem_size - 1);
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
            
            i = i + mem_size - 1;
            no_part++;
            
         }
         
         read_blocks_no = mem_size - 1;
         int tp_bl = schema.getTuplesPerBlock();
         int count_arr[no_part];
         int count_bl[no_part];
         int exh[no_part];
         result = getTypess(result);
         clearMM();
         /*int j = 0;
         for(int i = 0; i < no_part; i++)
         {
             count_arr[i] = 0;
             count_bl[i] = 1;
             exh[i] = 0;
             temp_ptr->getBlock(j, i);
             j = j + mem_size - 1;
         }
         
         vector<Tuple> tp = mem->getTuples(0, no_part);
         int sorted_count = 0;
         string str1 = "";
         int int_val = INT_MIN;
         
         int no_tp = schema.getTuplesPerBlock();
         Block *bl = mem->getBlock(mem_size - 1);
         int bl_count = 0;
         
         
         while(sorted_count < relation_size)
         {
            int i=0;
            int min = INT_MAX;
            string minStr = "";
            int min_pos = -1;
            int tpp_sz = tp.size();
            
            if(cc == "INT")
            {
                for(i = 0; i < no_part; i++)
                {
                    if(exh[i] == 1)
                        continue;
                    int curr_pos = i*no_tp + count_arr[i];
                    int temp = INT_MAX;
                    
                    if(curr_pos < 0 || curr_pos >= tpp_sz)
                    {
                        
                    }
                    else
                    {
                        temp = tp[i*no_tp + count_arr[i]].getField(fields).integer;
                    }
                    
                    if(temp < min)
                    {
                        min = temp;
                        min_pos = i;
                    }
                }
                
            }
            else
            {
                minStr = "zzzzzzzzzzzzzzzzzzzz";
                min_pos = -1;
                for(i = 0; i < no_part; i++)
                {
                    if(exh[i] == 1)
                        continue;
                    int curr_pos = i*no_tp + count_arr[i];
                    string temp = "zzzzzzzzzzzzzzzzzzzz";
                    
                    if(curr_pos < 0 || curr_pos >= tpp_sz)
                    {
                        
                    }
                    else
                    {
                        temp = *(tp[i*no_tp + count_arr[0]].getField(fields).str);
                    }
                    
                    if(temp < minStr)
                    {
                        minStr = temp;
                        min_pos = i;
                    }
                }
            }
            
            if(!bl->isFull() && bl_count < no_tp)
            {
                if(min_pos != -1)
                {
                    bl->appendTuple(tp[min_pos*no_tp + count_arr[min_pos]]);
                }
                else
                {
                    curr_ptr->setBlock(sorted_count, mem_size - 1);
                }
                bl_count ++;
            }
            else
            {
                bl_count = 0;
                curr_ptr->setBlock(sorted_count, mem_size - 1);
                sorted_count ++;
                bl->clear();
                int pos = min_pos*no_tp + count_arr[min_pos];
                
                if(min_pos != -1 && pos >= 0)
                    bl->appendTuple(tp[min_pos*no_tp + count_arr[min_pos]]);
                
                bl_count ++;
            }
            
            count_arr[min_pos]++;
            if(count_arr[min_pos] == no_tp )
            {
                count_bl[min_pos] ++;
                int aa = min_pos*(mem_size - 1) + count_bl[min_pos] - 1;
                
                if(count_bl[min_pos] > mem_size - 1)
                {
                    exh[min_pos] = 1;
                }
                else if(min_pos == (no_part - 1) && count_bl[min_pos] > rest_tp)
                {
                    if(count_bl[min_pos] > rest_tp)
                    {
                        exh[min_pos] = 1;
                    }
                }
                else
                {
                    temp_ptr->getBlock(aa, min_pos);
                    
                    tp.clear();
                    tp = mem->getTuples(0, no_part);
                }
                count_arr[min_pos] = 0;
            }
            
         }*/
         int sss = curr_ptr->getNumOfBlocks();
         schema_manager->deleteRelation("sublist_rel");
         table_names = table_names+"_final_rel";
         return curr_ptr;
  }

  
  Relation *two_pass_sort(Relation *relation_ptr, string& table_names, string fields)
  {
         Schema schema = schema_manager->getRelation(table_names)->getSchema();
         Relation *temp_ptr = schema_manager->createRelation("sublist_rel", schema);
         Relation *curr_ptr = schema_manager->createRelation(table_names+"_final_rel", schema);
         
         int mem_size = mem->getMemorySize();
         int relation_size = relation_ptr->getNumOfBlocks();
         
         if(fields[0] == '*' || (fields.size() > 1 && (fields[0] == '*' || fields[1] == '*')))
            fields = schema.getFieldName(0);
         if(fields[0] == '.')
            fields = fields.substr(fields.find(".")+1, fields.size()-1);
         
         enum FIELD_TYPE typ;
         
         typ = schema.getFieldType(fields);
         
         
         string cc = "";
          
         if(typ == INT)
         {
            cc = "INT";
         }
         else
            cc= "STR20";
            
         int read_blocks_no = mem_size - 1;
         int no_part = 0;
         int rest_tp = 0;
         if(relation_size < mem_size)
         {
             relation_ptr->getBlocks(0, 0, relation_size);
             vector<Tuple> tp = mem->getTuples(0, relation_size);
             sort(tp.begin(), tp.end(), less_than_key(fields , cc));
             mem->setTuples(0, tp);
             curr_ptr->setBlocks(0,0 , relation_size);
             schema_manager->deleteRelation("sublist_rel");
             table_names = table_names+"_final_rel";
             return curr_ptr;
         }
         
         for(int i = 0; i < relation_size;)
         {
            if((i+ mem_size - 1) < relation_size)
            {
                read_blocks_no = mem_size - 1;
                relation_ptr->getBlocks(i, 0, mem_size - 1);
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
            
            i = i + mem_size - 1;
            no_part++;
            
         }
         read_blocks_no = mem_size - 1;
         int tp_bl = schema.getTuplesPerBlock();
         int count_arr[no_part];
         int count_bl[no_part];
         int exh[no_part];
         clearMM();
         int j = 0;
         for(int i = 0; i < no_part; i++)
         {
             count_arr[i] = 0;
             count_bl[i] = 1;
             exh[i] = 0;
             temp_ptr->getBlock(j, i);
             j = j + mem_size - 1;
         }
         
         vector<Tuple> tp = mem->getTuples(0, no_part);
         int sorted_count = 0;
         string str1 = "";
         int int_val = INT_MIN;
         
         int no_tp = schema.getTuplesPerBlock();
         Block *bl = mem->getBlock(mem_size - 1);
         int bl_count = 0;
         
         
         while(sorted_count < relation_size)
         {
            int i=0;
            int min = INT_MAX;
            string minStr = "";
            int min_pos = -1;
            int tpp_sz= tp.size();
            
            if(cc == "INT")
            {
                for(i = 0; i < no_part; i++)
                {
                    if(exh[i] == 1)
                        continue;
                    int poss = i*no_tp + count_arr[i];
                    int temp = INT_MAX;
                    
                    if(poss >= tpp_sz || poss < 0)
                    {
                    }
                    else
                    {
                        temp = tp[poss].getField(fields).integer;
                    }
                    
                    if(temp < min)
                    {
                        min = temp;
                        min_pos = i;
                    }
                }
                
            }
            else
            {
                minStr = "zzzzzzzzzzzzzzzzzzzz";
                min_pos = -1;
                for(i = 0; i < no_part; i++)
                {
                    if(exh[i] == 1)
                        continue;
                        
                    int poss = i*no_tp + count_arr[i];
                    string temp = "zzzzzzzzzzzzzzzzzzzz";
                    
                    if(poss >= tpp_sz || poss < 0)
                    {
                    }
                    else
                    {
                        temp = *(tp[i*no_tp + count_arr[0]].getField(fields).str);
                    }
                    
                    //string temp = *(tp[i*no_tp + count_arr[0]].getField(fields).str);
                    if(temp < minStr)
                    {
                        minStr = temp;
                        min_pos = i;
                    }
                }
            }
            
            if(!bl->isFull() && bl_count < no_tp)
            {
                if(min_pos != -1)
                {
                    bl->appendTuple(tp[min_pos*no_tp + count_arr[min_pos]]);
                }
                else
                {
                    curr_ptr->setBlock(sorted_count, mem_size - 1);
                }
                bl_count ++;
            }
            else
            {
                bl_count = 0;
                curr_ptr->setBlock(sorted_count, mem_size - 1);
                sorted_count ++;
                bl->clear();
                int pos = min_pos*no_tp + count_arr[min_pos];
                
                if(min_pos != -1 && pos >= 0)
                    bl->appendTuple(tp[min_pos*no_tp + count_arr[min_pos]]);
                
                bl_count ++;
            }
            
            count_arr[min_pos]++;
            if(count_arr[min_pos] == no_tp )
            {
                count_bl[min_pos] ++;
                int aa = min_pos*(mem_size - 1) + count_bl[min_pos] - 1;
                
                if(count_bl[min_pos] > mem_size - 1)
                {
                    exh[min_pos] = 1;
                }
                else if(min_pos == (no_part - 1) && count_bl[min_pos] > rest_tp)
                {
                    if(count_bl[min_pos] > rest_tp)
                    {
                        exh[min_pos] = 1;
                    }
                }
                else
                {
                    temp_ptr->getBlock(aa, min_pos);
                    
                    tp.clear();
                    tp = mem->getTuples(0, no_part);
                }
                count_arr[min_pos] = 0;
            }
            
         }
         
         schema_manager->deleteRelation("sublist_rel");
         table_names = table_names+"_final_rel";
         return curr_ptr;
  }

  
  bool two_pass_select_in_memory(Relation* relation_ptr, vector<string>& table_names , vector<string> select_lists, string order_by_att,  vector<pair<string,string>>& postfixExpression, vector<vector<string>>& result, char *d)
  {
        int mem_size = mem->getMemorySize();
        int relation_size = relation_ptr->getNumOfBlocks();
        //int sorted = 0;
        
        Schema schema = schema_manager->getRelation(table_names[0])->getSchema();
        
        if(order_by_att.size()>0)
        {
            string fields = "";
            if(table_names[0].find("temp") != 0)
                fields = order_by_att.substr(order_by_att.find(".")+1, order_by_att.size()-1);
            else
                fields = order_by_att;
         
            if(schema.fieldNameExists(fields) == false)
                return false;
        
            relation_ptr = two_pass_sort(relation_ptr, table_names[0], fields);
        }
        relation_size = relation_ptr->getNumOfBlocks();
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
            if(i == 0)
            {
                vector<string> att_nm = add_attribute_name(table_names, select_lists, result);
                result.push_back(att_nm);
            }
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
            
            i = i + mem_size;
        }
        
        
        if(order_by_att.size() > 0 )    
            schema_manager->deleteRelation(table_names[0]);
        
        if(d != '\0')
        {
            two_pass_distinct(relation_ptr, table_names[0], select_lists[0], result);
        }
        return true;
}
  
vector<string> add_attribute_name(vector<string>& table_names , vector<string> select_lists, vector<vector<string>>& result)
{
      Schema schema = schema_manager->getRelation(table_names[0])->getSchema();
      
      vector<string> all_at;
      for(int i = 0; i < select_lists.size(); i++)
      {
          string fields = select_lists[i];
          
          if(fields[0] == '*' || (fields.size() > 1 && fields[1] == '*'))
          {
              
              all_at = schema.getFieldNames();
              break;
          }
          if(fields[0] == '.')
          {
             fields = fields.substr(fields.find(".")+1, fields.size()-1); 
          }
          
          all_at.push_back(fields);
          
      }
    
      
      //result.push_back(all_at);
      return all_at;
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
                        t = s.substr(found+1);
                    }
                    else
                    {
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
                        t = s.substr(found+1);
                    }
                    else
                    {
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
                t = s.substr(found+1);
             }
             else
             {
                t = s;
             }
                                    
            if (tuple_schema.getFieldType(t) == INT) 
                val2 = tuple.getField(t).integer;
            else
                return false;
      
        }else val2 = stoi(p1.first);
                                
        if(p2.second == "COLUMN-NAME")
        {
            s = p2.first;
            found = s.find('.');
            //extract the column name 
            //t = s.substr(found+1);
            if(table_names.find("temp") != 0)
            {
                t = s.substr(found+1);
            }
            else
            {
                t = s;
            }
                                    
            if (tuple_schema.getFieldType(t) == INT) 
              val1 = tuple.getField(t).integer;
            else
                return false;
      
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
  
  /* returns the index (min, s_min) of smallest, and second smallest relations based on relation size*/
  void getMinTwo(vector<int> relation_size, int &min, int &s_min)
  {
      int tmin = INT_MAX;
      for(int i = 0; i < relation_size.size(); i++)
      {
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
  
    struct expressionTree
    {
      string value;
      string type;
      expressionTree* leftChild;
      expressionTree* rightChild;
      expressionTree(){ leftChild = NULL; rightChild = NULL;}
      
    };
    
    static void formAClause (expressionTree* root, vector<pair<string,string>>& c)
    {
        if(root->leftChild)
        {
            formAClause(root->leftChild, c);
        }
        if(root->rightChild)
        {
            formAClause(root->rightChild, c);
        }
        c.push_back(make_pair(root->value, root->type));
        delete root;
    }
    
    static void postOrderTraversalExpressionTree(expressionTree* root, vector< vector<pair<string,string> > >& clauses)
    {
        if(root->leftChild)
        {
            if(root->leftChild->value == "AND")
            {
                postOrderTraversalExpressionTree(root->leftChild, clauses);
                
            }else
            {
                //we have a cluase starting this node
                vector<pair<string,string>> clause;
                formAClause(root->leftChild, clause);
                clauses.push_back(clause);
            }
        }
        if(root->rightChild)
        {
            if(root->rightChild->value == "AND")
            {
                postOrderTraversalExpressionTree(root->rightChild, clauses);
                
            }else
            {
                //we have a cluase starting this node
                vector<pair<string,string>> clause;
                formAClause(root->rightChild, clause);
                clauses.push_back(clause);
            }
            
        }
    }
  
  static bool returnNaturalJoinAttribute (vector<string> field_name, vector<enum FIELD_TYPE> field_types, vector<pair<string,string>>& postfixExpression, string& joinCol)
  {
      joinCol="";
    //   joinCol2="";
      int n = postfixExpression.size();

        if(n == 0 ) return true; //no where clause
        
        if(postfixExpression[n-1].first == "OR" || postfixExpression[n-1].first != "AND")//root of the search condition is an OR, we need to run the where clause only when all the columns used in where are available 
        {
            return false;
        }
        else if (postfixExpression[n-1].first == "AND")
        {
            stack<expressionTree*> S;
            //prepare a ANDTree to get all the cluases involved in AND
            for(int i = 0; i < postfixExpression.size(); i++)
            {
                if (postfixExpression[i].second != "OPERATOR")
                {
                    expressionTree* e = new expressionTree;
                    e->value = postfixExpression[i].first;
                    e->type = postfixExpression[i].second;
                    S.push(e);
                }else 
                {
                    expressionTree* e = new expressionTree;
                    e->value = postfixExpression[i].first;
                    e->type = postfixExpression[i].second;
                    expressionTree* right = S.top();
                    S.pop();
                    expressionTree* left = S.top();
                    S.pop();
                    e->leftChild = left;
                    e->rightChild = right;
                    S.push(e);
                }
            }
            expressionTree* root = S.top();
            //print expression tree
            //work on expression tree
            vector< vector< pair<string,string> > > clauses;
            postOrderTraversalExpressionTree(root, clauses);
            
            //check if the current tuple has all the fields to satisfy any clause
            for(int i = 0; i<clauses.size() ;i++)
            {
                    //clause i
                    if(clauses[i].size() == 3)
                    {
                        if(clauses[i][2].first == "=")
                        {
                            if(clauses[i][0].second == "COLUMN-NAME" && clauses[i][1].second == "COLUMN-NAME")
                            {
                                if(find (field_name.begin(), field_name.end(), clauses[i][0].first) != field_name.end())
                                {
                                    //column present
                                    if(find (field_name.begin(), field_name.end(), clauses[i][1].first) != field_name.end())
                                    {
                                        //column present
                                        size_t dotPos = clauses[i][0].first.rfind('.');
                                        string col1 = clauses[i][0].first.substr(dotPos+1);
                                        
                                        dotPos = clauses[i][1].first.rfind('.');
                                        string col2 = clauses[i][1].first.substr(dotPos+1);
                                        
                                        if(col1 == col2)
                                        {
                                            //natural join on attribute col1
                                            joinCol = col1;
                                          
                                            return true;
                                        }
                                    }
                                }
                            }
                        }
                    }
            }
            return false;
      
    }
  }
  static bool satisfies_condition1(Tuple tp, string table_names, vector<string> field_name, vector<enum FIELD_TYPE> field_types, vector<pair<string,string>>& postfixExpression)
  {

    //extract all the column attributes from postfixExpression, so that we can try to push the selections down
    // vector<string> columnNamesInWhere;
    
        int n = postfixExpression.size();

        if(n == 0 ) return true; //no where clause
        
        if(postfixExpression[n-1].first == "OR" || postfixExpression[n-1].first != "AND")//root of the search condition is an OR, we need to run the where clause only when all the columns used in where are available 
        {
            
            for(int i = 0; i < postfixExpression.size(); i++)
            {
                if(postfixExpression[i].second == "COLUMN-NAME")
                {
                    //do we have this column presently in this tuple?
                    if (find (field_name.begin(), field_name.end(), postfixExpression[i].first) == field_name.end())
                    {
                        //column is not present 
                        return true;
                    }
                }
            }
            //all columns in where clause are present in the tuple. Evaluate the tuple now
            return satisfies_condition(tp, table_names, postfixExpression);
        }
        else if (postfixExpression[n-1].first == "AND")
        {
            stack<expressionTree*> S;
            //prepare a ANDTree to get all the cluases involved in AND
            for(int i = 0; i < postfixExpression.size(); i++)
            {
                if (postfixExpression[i].second != "OPERATOR")
                {
                    expressionTree* e = new expressionTree;
                    e->value = postfixExpression[i].first;
                    e->type = postfixExpression[i].second;
                    S.push(e);
                }else 
                {
                    expressionTree* e = new expressionTree;
                    e->value = postfixExpression[i].first;
                    e->type = postfixExpression[i].second;
                    expressionTree* right = S.top();
                    S.pop();
                    expressionTree* left = S.top();
                    S.pop();
                    e->leftChild = left;
                    e->rightChild = right;
                    S.push(e);
                }
            }
            expressionTree* root = S.top();
            //print expression tree
            //work on expression tree
            vector< vector< pair<string,string> > > clauses;
            postOrderTraversalExpressionTree(root, clauses);
            
            // for(int i = 0; i<clauses.size() ;i++)
            // {
            //     for(int j = 0; j < clauses[i].size(); j++)
            //     {
            //     }
            // }
            
            
            //check if the current tuple has all the fields to satisfy any clause
            for(int i = 0; i<clauses.size() ;i++)
            {
                    for(int j = 0; j < clauses[i].size(); j++)
                    {
                        if(clauses[i][j].second == "COLUMN-NAME")
                        {
                            //do we have this column presently in this tuple?
                            if (find (field_name.begin(), field_name.end(), clauses[i][j].first) == field_name.end())
                            {
                                //column is not present 
                                return true;
                            }
                        }
                    }
                    //all columns in where clause are present in the tuple. Evaluate the tuple now
                    return satisfies_condition(tp, table_names, clauses[i]);
            }
        }
    
      return true;
  }
  
  //
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
      
      /* Push relation pointers for all the tables in vector relation_ptr, and their corresponding sizes in vector relation_size */
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
      
      int sz =  table_names.size(); //number of tables in join
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
  
  void getJoinAttributes(vector<string>&& ss)
  {
      ss.push_back("sid");
      ss.push_back("sid");
  }
  

  //Joins two current minimum tables and create a temporary relation
  //Performs both one pass and two pass join

  Relation* join(vector<string>& table_names, vector<Relation*>& relation_ptr, vector<int>& relation_size, int min, int s_min, int itr, vector<pair<string,string>>& postfixExpression)
  {
      vector<string> values;
      string table_name1 = table_names[min];
      string table_name2 = table_names[s_min];
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
      int max_lar = INT_MIN;
      int min_lar = INT_MAX;
      
      for(int i =0; i < no_fields1; i++)
      {
          if(table_names[min].find("temp") != 0)
          {
            field_name[i] = table_name1+"."+field_name[i];
          }
          
          field_name1.push_back(field_name[i]);
          field_types1.push_back(field_types[i]);
      }
      
      for(int i =0; i < no_fields2; i++)
      {
          if(table_names[s_min].find("temp") != 0)
          {
              field_name2[i] = table_name2 + "."+field_name2[i];
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
    
        string joinCol = "";
        
        string joinCol1 = "";
        string joinCol2 = "";
        if(postfixExpression.size() == 0)
        {
        }
        else
        {
        for(int i=0; i< postfixExpression.size() - 2; i++)
        {
            int flag1 =0;
            int pos = 0;
            int pos1=-1;
            if(postfixExpression[i].second == "COLUMN-NAME" && postfixExpression[i+1].second == "COLUMN-NAME" && postfixExpression[i+2].second == "OPERATOR")
            {
                int j;
                if(postfixExpression[i+2].first == "=")
                {
                    
                    for(j = 0; j < field_name1.size(); j++)
                    {
                        if(field_name1[j] == postfixExpression[i].first || field_name1[j] == postfixExpression[i+1].first)
                        {
                            if(flag1 == 0)
                                pos = j;
                            else
                                pos1 = j;
                            flag1++;
                        }
                    }
                }
                if(flag1 == 2)
                {
                    if(pos < field_name.size())
                    {
                        joinCol1 = field_name1[pos];
                        //joinCol2 = postfixExpression[i+1].first;
                    }
                    else
                    {
                        joinCol2 = field_name1[pos];
                    }
                    
                    if(pos1 < field_name.size())
                    {
                        joinCol1 = field_name1[pos1];
                    }
                    else
                    {
                        joinCol2 = field_name1[pos1];
                    }
                    if(table_names[min].find("temp") != 0)
                    {
                        
                    }
                    if(table_names[min].find("temp") != 0)
                    {
                        
                        string ss= table_names[min];
                        joinCol1 = joinCol1.substr(joinCol1.find(".")+1, joinCol1.size()-1);
                        relation_ptr[min] = two_pass_sort(relation_ptr[min], ss, joinCol1);
            
                    }
            
                    if(table_names[s_min].find("temp") != 0)
                    {
                        string ss= table_names[s_min];
                        joinCol2 = joinCol2.substr(joinCol2.find(".")+1, joinCol2.size()-1);
                        relation_ptr[s_min] = two_pass_sort(relation_ptr[s_min], ss, joinCol2);
                    }
                    
                    
                    break;
                }
            }
            
        }
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
                int flag = 0;
                for(int j = 0; j < tp2.size(); j++)
                {
                    flag = 0;
                    if(joinCol1 != "")
                    {
                            flag = 1;
                            int sid1 = tp1[i].getField(joinCol1).integer;
                            int sid2 = tp2[j].getField(joinCol2).integer;
                            // cout<<"sid1 "<<sid1<<"sid2 "<<sid2<<endl;
                            if(sid1 == sid2)
                            {
                                flag =2;
                            }
                            if(i == 0)
                            {
                                if(sid2 > max_lar)
                                {
                                    max_lar = sid2;
                                }
                                
                                if(sid2 < min_lar)
                                {
                                    min_lar = sid2;
                                }
                                
                                if(sid1 == sid2)
                                {
                                    flag = 2;
                                }
                            }
                            else
                            {
                                if(sid1 > max_lar)
                                    break;
                                if(sid1 < min_lar)
                                    break;
        
                                if(satisfies_condition1(tp2[j], table_names[s_min], field_name2, field_types2,postfixExpression) == false)
                                {
                                    break;
                                }
                            
                                if(tp1[i].getField(joinCol1).integer < tp2[j].getField(joinCol2).integer)
                                {
                                    break;
                                }
                                
                                if(sid1 == sid2)
                                {
                                    flag = 2;
                                }
                            }
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
                    if(flag == 0 || flag == 2)
                    {
                        
                        appendTupleToRelation(temp_ptr, relation_size[min]+1, tuple);
                    }
                }
              }
          }
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
                int flag = 0;
                for(int j = 0; j < tp2.size(); j++)
                {
                    
                    if(joinCol1 != "")
                    {
                        int sid1 = tp1[m].getField(joinCol1).integer;
                        int sid2 = tp2[j].getField(joinCol2).integer;
                        

                        if(i == 0 && m == 0)
                        {
                            if(sid2 > max_lar)
                            {
                                max_lar = sid2;
                            }
                            
                            if(sid2 < min_lar)
                            {
                                min_lar = sid2;
                            }
                        }
                        else
                        {
                            flag = 1;
                            break;

                            if(sid1 > max_lar)
                                break;
                            if(sid1 < min_lar)
                                break;
                        
                            if(satisfies_condition1(tp2[j], table_names[s_min], field_name2, field_types2,postfixExpression) == false)
                            {
                                break;
                            }
                        
                            if(sid1 < sid2)
                            {
                                flag = 1;
                                break;
                            }
                        }
                    }   
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
                    
                    // cout<<tuple<<endl;
                    appendTupleToRelation(temp_ptr, read_blocks_no+1, tuple);
                }
                if(flag == 1)
                    break;
                
              }
            }
            
            i = i + mem_size - 2;
        }
      }
    //   cout<<*temp_ptr<<endl;
      
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
    
      
      return true;
  }
  
  vector<vector<string>> getTypess(vector<vector<string>> result)
  {
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
  
  
  bool processDeleteStatement(string table_name, vector<pair<string,string>>& postfixExpression) 
  {
    //std::string tableName = root->children[2]->value;
    Relation* relation_ptr = schema_manager->getRelation(table_name);

    if(relation_ptr == '\0')
        return false;
        
    if(postfixExpression.size() == 0) 
    {
        relation_ptr->deleteBlocks(0);
        return true;
    }

    Block* out_block = mem->getBlock(1);
    out_block->clear();

    int writeBlockIndex = 0;
    int size = relation_ptr->getNumOfBlocks();
    
    for (int i = 0; i < size; ++i) 
    {
        relation_ptr->getBlock(i, 0);
        Block* block = mem->getBlock(0);
        vector<Tuple> tuples = block->getTuples();
        for (int j = 0; j < tuples.size(); ++j) 
        {
            if (satisfies_condition(tuples[j], table_name, postfixExpression) == false) 
            {
                if(out_block-> isFull()) 
                {
                    relation_ptr->setBlock(writeBlockIndex, 1);
                    out_block->clear();
                    out_block->appendTuple(tuples[j]);
                    writeBlockIndex++;
                }
                else
                {
                    out_block->appendTuple(tuples[j]);
                }
            }
        }
    }

    if (!out_block->isEmpty()) 
    {
      relation_ptr->setBlock(writeBlockIndex, 1);
      out_block->clear();
      writeBlockIndex++;
    }

    relation_ptr->deleteBlocks(writeBlockIndex);
    return true;
  }
  
  
};