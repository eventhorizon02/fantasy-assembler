// utilty and helper functions that are used by multiple files.

#ifndef __utills__
#define __utills__
#include <stdarg.h>
#include "hash.h"
#include "globals.h"
#include "error_handling.h"
#define INVALID_CODE -99

typedef enum { SOURCE,DESTINATION } parameterType;

const char encode64(char); // convert a six bit binary to base64
unsigned int get_opcode(const char*); // the opcode for an assembly instruction.
void encode_instruction(int*,int,...); // this function gets an instruction and a variable number of argumetnts and return an array with the decoded instruction.
int get_addressing_mode(char*); // a helper function to determine the addressing mode of an operand.
bool isNumber(const char*); // check if passed string is a number, positive or negative.
bool is_valid_register(char*); // check if a string represents a valid register.
err_codes is_valid_label_name(char*); // check if a passed parameter could make a valid label name, if not return the error code.
bool isLabel(const char*, char*); // is a given line a label decleration.
const char* stripLabel(const char*, unsigned long); // send over a line of code that start with a label and remove the label definition from it.
line_type getLine_type(const char*); // what type of line is this.
line_type getDir_type(const char*); // if it starts with a dot, what type of pre-processor dirctive is it.
unsigned int get_instruction(const char*); // which instruction is this line.
int parse_inst(const char*, char*[],const char*, unsigned int); // take an instruction line and return and array populated with the inst name, parameters, return the number of params or error.

#endif

