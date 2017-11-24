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
  
    
};