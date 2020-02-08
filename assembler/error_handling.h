#ifndef __error_handling__
#define __error_handling__

#include "globals.h"

typedef enum Err_codes
{
   NONE,
   LABEL_TOO_LONG,
   INVALID_LABEL_NAME,
   LABEL_ALLREADY_DECLARED,
   SYNTAX_ERROR,
   UNBALANCED_QOUTES,
   UNKNOWN_INSTRUCTION,
   EXCESSIVE_PARAMETERS,
   EXCESSIVE_COMMAS,
   MISSING_PARAMETERS,
   INVALID_ADDR_MODE_SRC,
   INVALID_ADDR_MODE_DEST,
   MISSING_LABEL_DECLARATION,
   ENTRY_LABEL_MISSING
} err_codes;

unsigned int get_errors_number(); // how many errors are there total in all files
unsigned int get_new_errors_number();// how many errors in current file.
bool add_error(int,err_codes,const char*); // add an error occurence to the array of errors.
void init_err_arr(); // initialize the errors array.
void print_error_report(); // report all errors, if any exists.
bool isErrorLine(const char*, int); // check if a line was recoreded as an error.
void reset_current_error_counter(); // reset the current file error counter.
void free_error_arr();

#endif
