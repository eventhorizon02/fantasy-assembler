# include "hash.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// intialize the data structes for the labels, instruction and data memory and .entry array
int* inst_memory; // the instruction memory array.
int* data_image; // the data memory array.
struct label** extrn_arr;
unsigned int extrn_counter;
struct label* Label;

// a helper function to allocate a new label node with the left and right pointers set to NULL
struct label* newLabel(const char*, int,labelType, bool, bool);


void init_hash_memory()
{
    inst_memory = calloc(0,sizeof(int));// 0 length array place holder
    data_image = calloc(0,sizeof(int)); // same here as above.
    extrn_arr= (calloc(0,sizeof(struct label*))); // holds refrences to externals.
    extrn_counter = 0;
    Label = NULL;
}
// *********************** labels functions and data stuctures****************


// return the label at an address, if it exists
const char* label_at_address(int address)
{
  const char* symbol = NULL;
  struct label* itr = Label;
  while ( itr )
  {
    if ( address == itr->address )
    {
     symbol = itr->label;
     break; // we have reached the desired address
    }
    itr = itr->next;
  }
  return symbol;
}

// get the address of a label if it exists
int lookup(const char* name)
{
  int result = INVALID_LABEL;
  struct label* itr = Label;
  while ( itr )
  {
    if (!strcmp(name,itr->label))
    {
     result = itr->address;
     break; // we found the label.
    }
      itr = itr->next;
  }
   return result;
}

// insert a new label
void insert(const char* name,int address,labelType type, bool isData, bool isEntry)
{
  // the new label will be inserted at the end of the list.
  struct label* nLabel = newLabel(name,address,type, isData, isEntry); // new label
  // check if list is empty
  if (!Label)
  Label = nLabel;
  else // list already exists, iterate through it and add new label at end.
  {
    struct label* itr = Label;
    while (1)
    {
      if (itr->next == NULL)
      {
       itr->next = nLabel;
        break;
      }
       itr = itr->next;
    } 
  }
}

// destroy the label tree to free up memory
void free_label_hash()
{
   struct label* itr = Label;
    while (itr)
    {
       free((char*)(itr->label));
       itr->label = NULL;
       struct label* nextL = itr->next;
       free(itr);
       itr = nextL;
    }
}

// a helper function to allocate a new label node.
struct label* newLabel(const char* name, int address,labelType type, bool isData, bool isEntry)
{
    char* symbol = malloc( strlen(name) + 1 );
    strcpy(symbol, name);
    struct label* node = malloc(sizeof(struct label));
    node->label = symbol;
    node->address = address;
    node->type = type;
    node->next = NULL;
    node->isData = isData;
    node->isEntry = isEntry;
    return node;
}

// print the whole label table, just for debugging purposes.
void print_label_hash()
{
  struct label* itr = Label;
   while ( itr )
  {
    printf("\n %s---%d---%d\n",itr->label,itr->address,itr->type);
    itr = itr->next;
  }
}

// looks for a given label and returns if it's external or rlocatable
labelType getLabelType(const char* name)
{
    labelType myType = noLabel;
    struct label* itr = Label;
    while ( itr )
    {
        if (!strcmp(name,itr->label))
        {
            myType = itr->type;
            break; // we found the label.
        }
        itr = itr->next;
    }
    return myType;
}

// load decoded instruction into instruction memory.
bool load_instruction(int* insts)
{
    //insts[0] holds the number of elements to be added to mem array
    // we need to expand inst_memory by the number of new elements, stored in insts[0]
    
    int* newMem = (int*)realloc(inst_memory, (IC + insts[0]) * sizeof(int));
    if (newMem)
        inst_memory = newMem;
    else
        return false;
    // now we can acually load the instruction.
    for ( int i = 1; i <= insts[0]; ++i)
        *(inst_memory + IC + i - 1 ) = insts[i];
    
    IC += insts[0];
    return true;
}

// load a string into the data image from a .string directive
bool load_string(const char* str)
{
    size_t l = strlen(str) + 1; // +1 for the null terminator
    int* newMem = realloc(data_image, (DC + l) * sizeof(int)); // expand the data image by the sring 									length plus 1.
    if (newMem)
        data_image = newMem;
    else
        return false;
    // now lets actually load the sting into the data memory.
    for ( int i = 0; i < l; ++i )
        *(data_image + DC + i) = str[i];
    
    DC += l;
    return true;
}

// load ints into the data image
bool load_data(int* nums)
{
    //nums[0] holds the number of elements to be added to data image array
    // we need to expand data image by the number of new elements, stored in num[0].
    
    int* newMem = realloc(data_image, (DC + nums[0]) * sizeof(int));
    if (newMem)
        data_image = newMem;
    else
        return false;
    // now we can acually load the data
    for ( int i = 1; i <= nums[0]; ++i)
        *(data_image + DC + i - 1 ) = nums[i];
    
    DC += nums[0];

    return true;
}

// return a pointer to a label for a given label name
struct label* getLabel(const char* name)
{
    struct label* itr = Label;
    while ( itr )
    {
        if (!strcmp(name,itr->label))
         return itr;
        itr = itr->next;
    }

    return NULL;
  }

  // return the number of extern refrences.
unsigned int get_extrn_ref_num()
{
  return extrn_counter;
}

 // store new reference to an external
 bool add_extrn_ref(const char* name, unsigned int address)
 {
   struct label* new_ref = newLabel(name, address, external, false, false); // the only fields that  	matter are the name and address
   struct label** newMem = (struct label**)realloc(extrn_arr, (extrn_counter + 1) * sizeof(struct label*));
    if (newMem)
        extrn_arr = newMem;
    else
        return false;
    extrn_arr[extrn_counter] = new_ref;

    extrn_counter++;
    return true;
 }

 void free_extrn_ref()
 {
   for (int i = 0; i < extrn_counter; ++i)
   {
     free((char*)extrn_arr[i]->label);
     extrn_arr[i]->label = NULL;
     free(extrn_arr[i]);
     extrn_arr[i] = NULL;
   }
    free (extrn_arr);
    extrn_arr = NULL;
 }




























