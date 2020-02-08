#include "error_handling.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct error
{
    int line_number;
    int error_code;
    const char* file_name;
};

typedef struct error Error;


Error* error_arr; // the array to hold all the found errors.
static unsigned error_counter; // how many errors have we logged so far.
unsigned new_error_counter; // how many errors in current file.

const char* get_str_for_code(unsigned int); // get a string describing the error to be printed for an error code

// initialize the errors array.
void init_err_arr()
{
    error_arr = calloc(0,sizeof(Error));
    error_counter = 0;
    new_error_counter = 0;
}

// reset the current file error counter.
void reset_current_error_counter()
{
    new_error_counter = 0;
}

// is this file error free? if not, how many are there
unsigned get_errors_number()
{
    return error_counter;
}

// how many errors in current file.
unsigned int get_new_errors_number()
{
    return new_error_counter;
}

// add an error occurence to the array of errors.
bool add_error(int line_num, err_codes e_code, const char* file_name)
{
    error_counter++;
    new_error_counter++;
    Error* temp = realloc(error_arr, sizeof(Error) * error_counter);
    char* newName = malloc(strlen(file_name) + 1);
    strcpy(newName,file_name);
    if (temp)
    {
        error_arr = temp;
        error_arr[error_counter - 1 ].line_number = line_num;
        error_arr[error_counter - 1 ].error_code = e_code;
        error_arr[error_counter - 1 ].file_name = newName;
    }
    else
        return false;
    return true;
}

// get a string describing the error to be printed for an error code
const char* get_str_for_code(unsigned int code)
{
    
    static const char* msg[] = {
                                 "",
                                 "Label declaration too long.",
                                 "Invalid label name.",
                                 "Label already declared.",
                                 "Syntax Error.",
                                 "Unbalanced quotes.",
                                 "Unknown instruction.",
                                 "Too many parameters.",
                                 "Too many commas.",
                                 "Missing parameters.",
                                 "Invalid addressing mode for source parameter.",
                                 "Invalid addressing mode for destination parameter.",
                                 "Missing Label declaration.",
                                 ".entry Label declared, not found in source file."
                                };
   
    return msg[code];
}

// report all errors, if any exists.
void print_error_report()
{
    printf("\n%d errors found.\n",error_counter);
    for (int i = 0; i < error_counter; ++i)
    {
        Error temp = *(error_arr + i);
        printf("in file: '%s', line number %d, %s\n",temp.file_name,temp.line_number,get_str_for_code(temp.error_code));
    }
}

// check if a line was recoreded as an error.
bool isErrorLine(const char* fileName, int lineNum)
{
	for (int i = 0; i < error_counter; ++i)
	{
		Error temp = *(error_arr + i);
		if ( (!strcmp(fileName,temp.file_name)) && (temp.line_number == lineNum) )
			return true;
	}
	return false;
}


void free_error_arr()
{
    for (int i = 0; i < error_counter; ++i)
    {
       free((char*)(error_arr[i].file_name));
       error_arr[i].file_name = NULL;
    }
    free(error_arr);
    error_arr = NULL;
}














































