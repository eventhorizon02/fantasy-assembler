/* this file holds all the data stuctures that holds the labes, instructions and data.
This files also holds all the associated utility functions to work with these data structures */

# ifndef __hash__
# define __hash__
#define INVALID_LABEL -99

#include "globals.h"

extern int* inst_memory; // the instruction memory array.
extern int* data_image; // the data meory array.

void init_hash_memory(); // intialize the data structes for the labels, instruction and data memory and .entry array
// *********************** labels storage, lookup and functions ************************************
typedef enum { noLabel,external, relocatable } labelType;
 struct label
{
  const char* label;
  int address;
  labelType type;
  bool isData; // is this label in the .data or .string section?
  bool isEntry; // is this label marked as an entry label?
  struct label* next;
};

extern struct label* Label;
extern struct label** extrn_arr; // will holds each reference to an external label and the address where it was referenced.

// get the address of a label
int lookup(const char*);

// looks for a given label and returns if it's external or rlocatable
labelType getLabelType(const char*);

// store a new label
void insert(const char*, int,labelType, bool, bool);

// destroy the label list to free up memory
void free_label_hash();

// print the whole label table, just for debugging purposes.
void print_label_hash();

// return the label at an address, it exists
const char* label_at_address(const int);

// return a pointer to a label for a given label name
struct label* getLabel(const char*);

// return the number of extern refrences.
unsigned int get_extrn_ref_num();




//************************************* instructions storage, loading, etc ****************************************************
// load decoded instruction into instruction memory.
bool load_instruction(int*);

//*************************************** data storage, loading, etc ***********************************************************
bool load_string(const char*); // load a string into the data image from a .string directive
bool load_data(int*); // load ints into the data image

//**************************************** memory address references to externals ******************************************************
bool add_extrn_ref(const char*, unsigned int); // store new reference to an external

// free the memory used by the extrn_ref array.
void free_extrn_ref();



# endif











