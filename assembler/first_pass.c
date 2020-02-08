
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "first_pass.h"
#include "utills.h"
#include "error_handling.h"

#define MAX_LABEL_LENGTH 31
static unsigned int line_number; // the line number in the source file.
static const char* file_name;

bool is_legal_label(const char*); // check if a lable declaration is legal, if not record the error as well.
int calculate_L(int n_arg, ...); // we can't really encode all the instructions in the first pass but we need to know the length of the instructions in bytes.
bool return_data(char*,int[]); // take .data xx,xx,x,xx and return an array populated with numbers
char* return_string_vals(char*); // take .string "xxx" and return and return just the sting without the qoutes.
int get_instruction_length(const char*); // recieve an instruction parse it and then get the L calculated.

bool readSource(const char* path)
{
    line_number = 0;
    file_name = path;
    FILE* fp = open_file(path);
    char* line = malloc(MAX_LINE_LENGTH);
    char* head = line;
    while ( read_next_line(fp, line) )
    {
        ++line_number;
        // is this a comment line or empty line?
        char* skip = line;
        while (isspace(*skip)) ++skip;
        if ( (getLine_type(skip) == comment) || (getLine_type(skip) == empty) ) continue;
       
        char result[MAX_LINE_LENGTH]; // will hold the label name + ':'
        if (isLabel(line,result))
        {
           if(is_legal_label(result))
           {
               const char* stripped = stripLabel(line, strlen(result));
               line_type dType = getLine_type(stripped);
               result[strlen(result) - 1] = '\0'; // get rid of the : in label's name
               switch (dType)
               {
                  case comment:case empty://comment or empty line after a label is error.
                  add_error(line_number, SYNTAX_ERROR, file_name);
                  break;
                  case command:case error:// will give us more detailed error message
                  // create and store Label with IC value, get L, increment IC + L
                   insert(result, IC, relocatable, false, false);
                   IC += get_instruction_length(stripped);
                   break;
                   case dotEntry: case dotExtern: // has no meaning
                     printf("\n Warning: .entry or .extern after Label disregarded.\n");
                    break;
                  case dotData:
                   // create label with DC value, load data in memory.
                      insert(result, DC, relocatable, true, false);
                      int numbers[MAX_LINE_LENGTH];
                      if (return_data((char*)stripped, numbers))
                      load_data(numbers); // load data advances the DC.
                    break;
                   case dotString:
                   // create label with DC value, load data in memory.
                      insert(result, DC, relocatable, true, false);
                      char* str;
                       if  ( (str = return_string_vals((char*)stripped) ) )
                       {
                           load_string(str); // load string advances the DC.
                            free(str);
                            str = NULL;
			}
                     break;
                    default:
                       printf("\n Debug message: no specific line type passed label found\n");
                       break;
                 }//switch
             }//if legal
        }else // this line is not a label declaration.
        {
          while (isspace(*line)) ++line;
          line_type dType = getLine_type(line);
          switch (dType)
          {
                case comment: case empty: case dotEntry:
                    break;
		case command:case error:// L will be 0 in case of error but we'll get more details
                //claculate L and advance the IC accordingly.
		{
                  int n = get_instruction_length(line);
                  IC += n;
                   break;
                }

		case dotData:
                 {
                   int numbers[MAX_LINE_LENGTH];
                   if (return_data((char*)line, numbers))
                   load_data(numbers); // load data advances the DC.
                   break;
		  }

		case dotString:
                {
                    char* str;
                    if  ( (str = return_string_vals((char*)line) ) )
                    {
                     load_string(str); // load string advances the DC.
                     free(str);
                     str = NULL;
                    }
		    break;
                }

	        case dotExtern:
                {
                   // create the label with value 0 and type extern, rest of the fields don't matter, 			   // check that label is not actually defined in current file.
                    line += 7;
                    while (isspace(*line)) ++line; // trim leading white spaces.
                    if (is_valid_label_name((char*)line) == NONE)
                    {
                       if (lookup(line) != INVALID_LABEL) // label already exists.
                           // record the error
                           add_error(line_number, LABEL_ALLREADY_DECLARED, file_name);
                        else // create the label.....
                            insert(line, 0, external, false, false);
                    }else
                        // record the error.
                        add_error(line_number, INVALID_LABEL_NAME, file_name);
                     break;
                  }
                 default:
                      printf("\n Debug message: no specific line type found\n");
                    break;    
                   
	   }//switch
         }//else
    }//while
    
    free(head);
    head = NULL;
    fclose(fp);
    return true;
}

// check if a label declaration is legal, if not record the error as well.
// the parameter passed is known for a fact to be a label declaration and has the format "xxxxxx:"
bool is_legal_label(const char* label)
{
    size_t len = strlen(label);
    char* pure = malloc(len); // make a copy of the label but without the ':'
    strncpy(pure,label,len - 1); // copy up to the char before the ':'
    pure[len - 1] = '\0'; // the null terminator needs to be added manutaly.

    err_codes code = is_valid_label_name(pure);
    if ( code != NONE )
    {
        add_error(line_number, code, file_name);
	free(pure);
    	pure = NULL;		
        return false;
    }
    
    // make sure label doesn't already exists
    if (lookup(pure) != INVALID_LABEL)
    {
        // record the error
        add_error(line_number, LABEL_ALLREADY_DECLARED, file_name);
        free(pure);
        pure = NULL;
        return false;
    }
    free(pure);
    pure = NULL;
    
    return true;
}


// we can't really encode all the instructions in the first pass but we need to know the length of the instructions in words.
int calculate_L(int n_arg, ...)
{
    int L = 1; // will be at least one word long.
    
    char* arg1 = NULL;
    char* arg2 = NULL; // could be a max of two arguments.
    va_list ap;
    va_start(ap, n_arg);
    if (n_arg > 0)
    arg1 = va_arg(ap, char*);
    if (n_arg > 1)
        arg2 = va_arg(ap, char*);
    va_end(ap);
    // determine the addressing mode of the first argument if it exists.
    int addrMode1 = 0;
    if (arg1)
        addrMode1 = get_addressing_mode(arg1);
    int addrMode2 = 0;
    if (arg2)
        addrMode2 = get_addressing_mode(arg2);
    
    // how many parameters are there?
    // if its two registers we get one more word and then we're done.
    if ((addrMode2 == 5) && (addrMode1 == 5))
       return 2;
    
    if (addrMode1) // if not zero means we have a source operand
       L = 2;
    
    if (addrMode2) // if not zero means we have a destination operand
       L = 3;
        
    return L;
}

// take .data xx,xx,x,xx and return an array populated with numbers, the first element will store a number with the size of the array.
bool return_data(char* line,int results[])
{
    char* begin = line; // a pointer to the first char of line.
    int counter = 0;
    line += 6; // skip to the part after the .data
    unsigned long len = strlen(line);
    char* copy = malloc(len + 1);
    strcpy(copy, line);
    char *pt;
    pt = strtok (copy,",");
    while (pt != NULL)
    {
        ++counter;
        if ( isNumber(pt) )
         results[counter] = atoi(pt);
        else
        {
            // record the error
            add_error(line_number, SYNTAX_ERROR, file_name);
            free(copy);
            copy = NULL;
            return false;
        }
        
        pt = strtok (NULL, ",");
    }
    // last check for exessive commas, since the way strtok works, it will let two successive commas pass.
    int comma_ctr = 0;
    while (*begin != '\0')
    {
        if (*begin == ',')
            comma_ctr++;
        ++begin;
    }
    if ( comma_ctr > counter - 1)
    {
        // record the error.
        add_error(line_number, EXCESSIVE_COMMAS, file_name);
        return false;
    }
    
    results[0] = counter;
    free(copy);
    copy = NULL;
    return true;
}

//take .string "xxx" and return just the string without the qoutes.
// caller needs to free the memory, after using this function
char* return_string_vals(char* line)
{
    int results[MAX_LINE_LENGTH];
    int counter = 0;
     line += 8; // skip to the part after .string
     while(isspace(*line)) ++line; // move to the first spot after the white spaces.
     // first char needs to be '\"' or else it is a syntax error.
    if ( (*line) != '\"' )
    {
        // register the error.
        add_error(line_number, SYNTAX_ERROR, file_name);
        return NULL;
    }
    ++line; // on to the next char.
    bool end = false;
    while ( !( end = (*line == '\0') || (*line == '\"') ) )
    {
         ++counter;
        results[counter] = *line;
        ++line;
       
    }
    results[0] = counter;
   
   if ( *line == '\0' ) // end of string without closing qoutes.
    {
        // register the error.
        add_error(line_number, UNBALANCED_QOUTES, file_name);
        return NULL;
    }else // we found the closing qoutes, make sure there is nothing but white spaces behind it.
    {
        ++line;
        while ( (*line) != '\0' )
        {
            
            if (!isspace(*line)) // something other than white space after the closing qoutes.
                {
                    
                  // record the error.
                    add_error(line_number, SYNTAX_ERROR, file_name);
                    return NULL;
                }
            ++line;
        }
    }
    char* string = malloc(results[0] + 1);
    for ( int i = 0; i <= results[0]; ++i)
        string[i] = results[i+1];
    
    return string;
}


// recieve an instruction parse it and then get the L calculated.
int get_instruction_length(const char* line)
{
    char* results[3] = {NULL,NULL,NULL};
    int num = parse_inst(line, results,file_name,line_number);
    int L = 0;
    // num includes the instruction name as well, it is not needed to calculate the L
    switch (num)
    {
        case 1:
            L = 1; // this instruction has no parameters, so L is one word long
            break;
            
        case 2:
            L = calculate_L(1,results[1]);
            break;
        case 3:
	     L = calculate_L(2,results[1],results[2]);
            break;
            
    }
    
    // free the memory in results
       for (int i = 0; i < num; ++i)
       {
	 if (results[i])
          {
	    free(results[i]);
            results[i] = NULL;
          }
       }
       return L;
}
// at the end of the first pass, the labels marked as data need to have their value adjusted according to the final IC value.
void adjust_data_labels()
{
    struct label* itr = Label;
    while ( itr )
    {
        if (itr->isData)
            itr->address += IC;
        itr = itr->next;
    }
}

// append the data image to the end of inst_memory
bool append_data_image()
{
    int* newMem = (int*)realloc(inst_memory, (IC + DC) * sizeof(int));
    if (newMem)
        inst_memory = newMem;
    else
        return false;
    // now transfer data_image to the end of inst_mem
    for ( int i = 0; i < DC; ++i )
     *(inst_memory + IC + i) = data_image[i];

     // now it's possible to free the memory used by data_image.
     if (data_image)
     {
      free(data_image);
      data_image = NULL;
     }
     return true;
}





