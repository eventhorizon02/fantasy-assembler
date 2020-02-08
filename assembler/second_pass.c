
#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include "utills.h"
#include "file_io.h"
#include "second_pass.h"
#include "hash.h"
#include "globals.h"
#include "error_handling.h"


unsigned int line_number; // the line number in the source file.
const char* file_name;

bool get_addr_error(int n_arg, ...); // check if the addressing modes are applied correctly.
void move_to_memory(const char*); // prepare the instruction, then load into memory


bool second_pass(const char* path)
{
    
    IC = 0;
    line_number = 0;
    file_name = path;
    FILE* fp = open_file(path);
    char* line = malloc(MAX_LINE_LENGTH);
    char* head = line; // will use head to free memory of pointed at by line later
    while ( read_next_line(fp, line) )
    {
       ++line_number;
        
        // if this line was found to have an error in the first pass, skip it.
        if (isErrorLine(path,line_number)) continue;
        
        // is this a comment line or empty line?
        char* skip = line;
        while (isspace(*skip)) ++skip;
        if ( (getLine_type(skip) == comment) || (getLine_type(skip) == empty) ) continue;

        char result[MAX_LINE_LENGTH]; // will hold the label name + ':'
        if (isLabel(line,result))
        {
            const char* stripped = stripLabel(line, strlen(result));
            line_type dType = getLine_type(stripped);
            if (dType == command)
              move_to_memory(stripped);// load the instruction into memory.
            
        }else // it's not a Label.
          {
            while (isspace(*line)) ++line;
            line_type dType = getLine_type(line);
            if (dType == command)
                move_to_memory(line);// load the instruction into memory.
            else if (dType == dotEntry)
            {
                // check that label had been loaded in first pass
                line += 6; // move to the position after .entry.
                while (isspace(*line)) ++line; // clear white space before label name.
                char* end = (char*)(line + strlen(line) - 1);
                while (isspace(*end)) --end;
                *(end + 1) = '\0';
                
                if ( lookup(line) == INVALID_LABEL )
                    add_error(line_number,ENTRY_LABEL_MISSING , file_name);
                else
                {
                    // change it's type to .entry, so it can be printed out later.
                    struct label* mod = getLabel(line);
                    mod->isEntry = true;
                }
                
            }
        }
    }
    fclose(fp);
    free((char*)head);
    head = NULL;
    return true;
}



 // check if the addressing modes are applied correctly and if using a label that it has actually been declared.
bool get_addr_error(int n_arg, ...)
{
    // in each element of add_mode, are encoded the addressing modes available, for source parameter, destination parametre and number of parameters allowed.
    // the information is stored in the bits of each char in the following system.
    // bits 0 and 1 hold the number of parameters available for the instruction.
    // bit 2 says if addressing mode 1 is available for the source parameter.
    // bit 3 says if addressing mode 3 is available for the source parameter.
    // bit 4 says if addressing mode 5 is available for the source parameter.
    // bits 5, 6 and 7 the same but for the destination parameters.
    // the indices in the array match the indices of the inst array.
    static const unsigned char addr_modes_arr[16] = { 222, 254, 222, 222, 193, 193, 202, 193, 193, 193, 193, 193, 225, 193, 0, 0 };
    static const char* inst_array[16] = { "mov","cmp","add","sub","not","clr","lea","inc","dec","jmp","bne","red","prn","jsr","rts","stop" };
    
    char* instruction;
    char* arg1 = NULL;
    char* arg2 = NULL;
    int addr_mode_src = 0;
    int addr_mode_dest = 0;
    va_list ap;
    
    va_start(ap, n_arg);
    instruction =  va_arg(ap, char*);
    if (n_arg == 2) // 1 parameter only.
    {
        arg2 = va_arg(ap, char*);
        addr_mode_dest = get_addressing_mode(arg2); // for only 1 parameter, it is the destination paramenter.
    }
    if (n_arg == 3) // 2 parameters
    {
        arg1 = va_arg(ap, char*);
        addr_mode_src = get_addressing_mode(arg1);
        
        arg2 = va_arg(ap, char*);
        addr_mode_dest = get_addressing_mode(arg2);
    }
    va_end(ap);

    int index = 0; // the index of the instruction.
    while ( strcmp(instruction, inst_array[index]) ) ++ index;
    unsigned char encoded = addr_modes_arr[index];
    
    n_arg--; // discount the instruction from the number of arguments.
    int diff = (encoded & 3) - n_arg;
     if ( diff > 0 ) // not enough parameters.
     {
         add_error(line_number, MISSING_PARAMETERS, file_name);
         return false;
     }
    else if (diff < 0) // too many parameters
    {
        add_error(line_number, EXCESSIVE_PARAMETERS, file_name);
        return false;
    }
    
    if (addr_mode_src)
    {
        switch (addr_mode_src)
        {
            case immediate:
               if (!(encoded & (1 << 2)))
               {
                   add_error(line_number, INVALID_ADDR_MODE_SRC, file_name);
                   return false;
               }
                
                break;
                
            case direct:
                if (!(encoded & (1 << 3)))
                {
                    add_error(line_number, INVALID_ADDR_MODE_SRC, file_name);
                    return false;
                }
                if (lookup(arg1) == INVALID_LABEL)
                {
                    add_error(line_number, MISSING_LABEL_DECLARATION, file_name);
                    return false;
                }
                break;
                
            case dir_reg:
                if (!(encoded & (1 << 4)))
                {
                     add_error(line_number, INVALID_ADDR_MODE_SRC, file_name);
                    return false;
                }
                break;
        }
    }
    
    if (addr_mode_dest)
    {
       switch (addr_mode_dest)
        {
            case immediate:
                if ( !(encoded & (1 << 5)))
                {
                    add_error(line_number, INVALID_ADDR_MODE_DEST, file_name);
                    return false;
                }
                
                break;
                
            case direct:
                if (!(encoded & (1 << 6)))
                {
                    add_error(line_number, INVALID_ADDR_MODE_DEST, file_name);
                    return false;
                }
                if (lookup(arg2) == INVALID_LABEL)
                {
                    add_error(line_number, MISSING_LABEL_DECLARATION, file_name);
                    return false;
                }
                break;
                
            case dir_reg:
                if (!(encoded & (1 << 7)))
                {
                    add_error(line_number, INVALID_ADDR_MODE_DEST, file_name);
                    return false;
                }
                break;
        }
    }

     return true;
}

// prepare the instruction, then load into memory
void move_to_memory(const char* stripped)
{
    char* strings[3];
    int results[4];
    int num = parse_inst(stripped, strings,file_name,line_number);
    bool success = false;
    switch (num)
    {
        case 1:
            encode_instruction(results, 1,strings[0]);
            success = get_addr_error(1, strings[0]);
            break;
            
        case 2:
            encode_instruction(results, 2,strings[0],strings[1]);
            success = get_addr_error(2, strings[0],strings[1]);
            break;
            
        case 3:
            encode_instruction(results, 3,strings[0],strings[1],strings[2]);
            success = get_addr_error(3, strings[0],strings[1],strings[2]);
            break;
    }
    
    if (success)
    load_instruction(results);
 // free memory in strings.   
    for (int i = 0; i < num; ++i)
    {
	if (strings[i])
        {
           free(strings[i]);
           strings[i] = NULL;
        }
    }
}





























