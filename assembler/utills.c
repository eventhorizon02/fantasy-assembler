#include "utills.h" 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

int encodeTwoRegs(const char*, const char*); // a helper function to endcode the second word in case of two register.
int encodeFirstWord(int,int,int); // a helper function to encode the first word of an instruction.
int encodeSingleReg(const char* ,parameterType); // a helper function to encode a single register
int encodeLabel(const char*); // encode a parameter from a label.
int encodeImm(const char*); // encode an immidate value
bool check_param_syntax(char*); // recive a parameter and check if it's valid syntax or garbage.


// convert a six bit binary to base64
const char encode64(char source)
{
  char mask = 63; // mask is now 00111111
  source &= mask; // the source now has the right six bits with a number and the two msb's are zero.
  if ( (source >= 0) && (source <= 25) )
	return source + 'A';
  else if ( (source >= 26) && (source <= 51) )
	return source - 26 + 'a';
  else if ( (source >= 52) && (source <= 61) )
	return source - 52 + '0';
  else if (source == 62)
	return '+';
  else if (source == 63)
	return '/';
  else
  {
     fprintf(stderr,"\ninvalid base64 code\n");
	exit(0);
  }
   return INVALID_CODE;
}

// get the opcode for an instruction in decimal.
unsigned int get_opcode(const char* inst)
{
   static const char* instructions[16] = {"mov","cmp","add","sub","not","clr","lea","inc","dec","jmp","bne","red","prn","jsr","rts","stop"};
  
  int opCode = INVALID_OPCODE; // syntax error.
   for (int i = 0; i < 16; ++i)
    {
     if (!strcmp(instructions[i],inst)) // we found the instruction.
      {
        opCode = i;
	break;
      }
    }
  return opCode;
}
// this function encodes an instruction, it is assumed that the instruction and parameters are all in a legal format.
// the results are stored in the provided array which is of size 4, the first element stores the number of entries to look for.

void encode_instruction(int* results,int n_arg, ...)
{
    results[0] = 1; // there is going to be at least one word.
    char* arg1 = NULL;
    char* arg2 = NULL; // could be a max of two arguments.
    va_list ap;
    va_start(ap, n_arg);
    unsigned int first = get_opcode(va_arg(ap, char*));
    if (n_arg > 1)
        arg1 = va_arg(ap, char*);
    if (n_arg > 2)
        arg2 = va_arg(ap, char*);
    va_end(ap);
    // determine the addressing mode of the first argument if it exists.
    int addrMode1 = 0;
    if (arg1)
        addrMode1 = get_addressing_mode(arg1);
    int addrMode2 = 0;
    if (arg2)
        addrMode2 = get_addressing_mode(arg2);
    results[1] = encodeFirstWord(first, addrMode1, addrMode2);
    // how many parameters are there?
    // if its two registers we get one more word and then we're done.
    if ((addrMode2 == dir_reg) && (addrMode1 == dir_reg))
    {
        results[0] = 2;
        results[2] = encodeTwoRegs(arg1, arg2);
        return;
    }
    if (addrMode1) // if not zero means we have a source operand
    {
        switch (addrMode1)
        {
                case immediate:
                results[2] = encodeImm(arg1);
                break;
                
                case direct:
                results[2] = encodeLabel(arg1);
                break;
                
                case dir_reg:
                results[2] = encodeSingleReg(arg1, SOURCE);
                break;
        }
        results[0] = 2;
    }
    
    if (addrMode2) // if not zero means we have a destination operand
    {
        switch (addrMode2)
        {
            case immediate:
                results[3] = encodeImm(arg2);
                break;
                
            case direct:
                results[3] = encodeLabel(arg2);
                break;
            case dir_reg:
                results[3] = encodeSingleReg(arg2, DESTINATION);
                break;
        }
        results[0] = 3;
        return;
    }
}

// a helper function to determine the addressing mode of an operand.
int get_addressing_mode(char* operand)
{
 // this function will return either 1,3 or 5 depending on the addresing mode determined
     if ( *operand == '@' ) // direct register addressing
        return dir_reg;
    else if (isdigit(*operand) || (*operand == '+') || (*operand == '-') ) // this is a number, so we are looking at an immediate addressing mode
        return immediate;
    else // must be a label
        return direct; // it's a variable
}

// a helper function to encode the first word of an instruction.
int encodeFirstWord(int op_code,int srcAddMode,int destAddMode)
{
    // if there is only one parameter, then it has to be the destination parameter.
    // therefore if destAddMode is 0, swap it srcAddMode.
    if (!destAddMode) // swap needed!
    {
        destAddMode = srcAddMode;
        srcAddMode = 0;
    }
    // the ARE of the first word is not depended on loading location and therefore the bits number 0 & 1 are allways 00.
    int word = 0;
    word |= (srcAddMode << 9); // source operand addressing mode in place now.
    word |= (op_code << 5); // op_code in place now.
    word |= (destAddMode << 2); // destination operand addressing mode in place now.
    return word;
}

// a helper function to endcode the second word in case of two register.
int encodeTwoRegs(const char* source, const char* dest)
{
    // ARE stays 00 in this case
    int word = 0;
    // get the first register number.
    word |= (encodeSingleReg(source, SOURCE));
    word |= (encodeSingleReg(dest, DESTINATION));
    return word;
}

// a helper function to encode a single register
int encodeSingleReg(const char* reg, parameterType type)
{
    // ARE stays 00 in this case
    int word = 0;
    char regChar = reg[2]; // the char right after the '@'
    int regNum = regChar - '0';
    int shiftAmout = (type == SOURCE) ? 7 : 2;
    regNum = (regNum << shiftAmout);
    word |= regNum;
    return word;
}
// encode a parameter from a label.
int encodeLabel(const char* label)
{
    // this assumes the label has already been recorded.
    int word = 0;
    labelType myType = getLabelType(label); // is this a relocatable or external label, needed to determine ARE value.
    int value = lookup(label);
    value = (value << 2); // shift left twice since the ARE is sitting on the 2 LSB bits
    // ARE can either be 01 for external or 10 for relocatable
    int ARE = (myType == external) ? 1 : 2; // the decimal equivalents of 01 and 10 respectivley.
    // add the ARE the word
    word |= ARE;
    word |= value;
    // also for an external label, add it to the externals array with the current IC
    if (myType == external)
    add_extrn_ref(label, IC + 101); // +100, since it is assumed that the code will be loaded staring at addr 100

    return word;
}

// encode an immidate value
int encodeImm(const char* val)
{
    // ARE will be 00 in this case.
    int num = atoi(val);
    // in case of a negative value and since negatives are represented in two's complement, I will only keep the right most 10 bits.
    int mask = 1023;
    num = num & mask;
    num = (num << 2); // add the ARE 00.
    return num;
}

// check if passed string is a number, positive or negative.
bool isNumber(const char* num)
{
    int i = 0;
    while (isspace(num[i])) ++i; // get to the first position after the leading spaces.
    // Trim trailing space
    char* end;
    end = (char*)num + strlen(num) - 1;
    while(end > num && isspace((unsigned char)*end)) end--;
    
    // Write new null terminator character
    end[1] = '\0';

    char c = num[i];
    if ( (!isdigit(c)) && ( c != '+' ) && ( c != '-') )
        return false;
    ++i;
    while ( num[i] != '\0' )
    {
        if ( !isdigit(num[i]))
            return false;
        ++i;
    }
   return true;
}

// recive a parameter and check if it's valid syntax or garbage.
bool check_param_syntax(char* param)
{
    if (isNumber(param))
        return true;
    else if (is_valid_register(param))
        return true;
    else if (is_valid_label_name(param) == NONE)
        return true;
    else
        return false;
}

// check if a string represents a valid register. will need to be @rx format.
bool is_valid_register(char* param)
{
    if (strlen(param) != 3)
        return false;
    if (! (param[0] == '@'))
        return false;
    if (! (param[1] == 'r'))
        return false;
    int r = param[2] - '0';
    if ( r < 0 || r > 7)
        return false;
    
    return true;
}

// check if a passed parameter could make a valid label name, if not return the error code.
err_codes is_valid_label_name(char* param)
{
    unsigned long len = strlen(param);
    // check if label is not too long
    if (len > 31 ) // 31 is the max label length
        return LABEL_TOO_LONG;
    
    // check if the first char is a letter
    if (!isalpha(param[0]) )
        return INVALID_LABEL_NAME;
    // check if all elements of labels are alpha numeric.
    for ( int i = 1; i < len; ++i) // the first char was already checked
    {
        if (param[i] == '\n')
        {
            param[i] = '\0';
            break; // igonore the enter at endl of line.
        }
        if (!isalnum(param[i]))
          return INVALID_LABEL_NAME;
    }
    //check that the param is not a reserved word
    // first copy a modified version of the param
   
    if (get_opcode(param) != INVALID_OPCODE) // this is a reserved word.
       return INVALID_LABEL_NAME;
    
    return NONE;
}
// is a given line a label decleration.
// parse the string representing the line to determine wether or not it declares a label.
// if it does declare a label we will check somewhere else if it complies with legality of label definitions.
bool isLabel(const char* line, char* result)
{
    
    int counter = 0;
    char c;
    while ( ((c = line[counter]) != '\n') && (!isspace(line[counter])) && (counter < MAX_LINE_LENGTH) )
    {
        result[counter] = c;
        ++counter;
        if ( c == ':') break; // end of label name.
    }
    // we could potentialy have a label here but it could be too long to be legal (max is 31 chars)
    if ( c == ':')
    {
        result[counter] = '\0';
        return true; //we are returning someting that could be a label but did not check yet if it's legal label.
    }
    
    return false;
}

// send over a line of code that start with a label and remove the label declaration from it and remove the white spaces after it.
const char* stripLabel(const char* line, unsigned long label_length)
{
    // label_length is the length of the label name + ':' excluding the null terminator.
    const char* trimed = line + label_length;
    //trimed = trim_leading_spaces(trimed);
    while (isspace(*trimed)) ++trimed;
    return trimed;
}

// sent parameter will have no leading white spaces.
line_type getLine_type(const char* line)
{
    switch (line[0])
    {
        case '!':
            return comment;
            break;
            
        case '.':
            return getDir_type(line);
            break;
            
        case '\n':
            return empty;
            break;
            
        default:
        {
            
            if (get_instruction(line) != INVALID_OPCODE)
                return command;
            else
                return error;
        }
            break;
    }
}

// if it starts with a dot, what type of pre-processor dirctive is it.
line_type getDir_type(const char* line)
{
    // there has to be a space between the directive name and the first parameter.
    char dir[MAX_LINE_LENGTH] = {'\0'};
    int counter = 0;
    
    while ( !isspace(line[counter]) )
    {
        dir[counter] = line[counter];
        ++counter;
    }
    
    if (!strcmp(".data",dir))
        return dotData;
    else if (!strcmp(".string", dir))
        return dotString;
    else if (!strcmp(".entry", dir))
        return dotEntry;
    else if (!strcmp(".extern", dir))
        return dotExtern;
    else
        return error;
}

// is this line an instruction.
unsigned int get_instruction(const char* line)
{
    // there has to be a space between the instruction name and the first parameter.
    char inst[MAX_LINE_LENGTH];
    int counter = 0;
    
    while (  (!isspace(line[counter])) && ((line[counter]) != '\0' ) )
    {
        inst[counter] = line[counter];
        ++counter;
    }
    inst[counter] = '\0';
    return get_opcode(inst);
}

/* take an instruction line and return and array populated with the inst name, parameters, return   	the  number of params or error.
    the caller of this function will have to release the memory allocated.*/
int parse_inst(const char* line, char* results[],const char* file_name, unsigned int line_number)
{
    char* begin = (char*)line; // a pointer to the begining of the line string.
    int counter = 0;
    while(isspace(*line)) ++line; // move pointer to first char afrer spaces
    int i = 0;
    char name[MAX_LINE_LENGTH];
    while ( (!isspace(line[i])) && (line[i]) != '\0'  )//should be a space after instruction name.
    {
        name[i] = line[i];
        ++i;
    }
    name[i] = '\0';
    line += i;
 // check if there is a legitimate instruction name inside of 'name' or garbage.
    if (get_opcode(name) != INVALID_OPCODE)
    {
        // copy the instruction name into results[0]
        ++counter;
        char* command = malloc(strlen(name) + 1);// freed by caller.
        strcpy(command, name);
        results[0] = command;
    }else
    {
        // record the error
        add_error(line_number, UNKNOWN_INSTRUCTION, file_name);
        return 0;
    }
    // move pointer to the next non whitespace char.
    while(isspace(*line)) ++line;
    char* params = malloc(strlen(line) + 1); // using strtok to parse the parameters so a copy of the 						  original string in needed.mem freed at end of function.
    strcpy(params, line);
    char *pt;
    int param_num = 0; // number of parameters found.
    pt = strtok(params,",");
    while (pt != NULL)
    {
        param_num++;
        
        if (param_num > 2) // too many parameters, record the error
        {
            add_error(line_number, EXCESSIVE_PARAMETERS, file_name);
            return 0;
        }
        // stick the parameter in the results array, then later check if it looks like a real 		//   parameter.
        // trim white spaces from parameter first.
        while (isspace(*pt)) ++pt;
        char* end = pt + strlen(pt) - 1; // a pointer to the end of the parameter string.
        while (end > pt && isspace(*end)) --end;
        end[1] = '\0';
        char* copyPt = malloc(strlen(pt) + 1);
        strcpy(copyPt,pt);// copy pt so it can be freed later by the caller.
        results[counter] = copyPt;
        if (!check_param_syntax(results[counter]))
        {
            // record the error.
            add_error(line_number, SYNTAX_ERROR, file_name);
            return 0;
        }
        counter++;
        pt = strtok (NULL, ",");
    }
    
    // with the way strtok works, it is necessary to check the number of commas, since successive    		commas with no space between them will pass.
    int comma_ctr = 0;
    while (*begin != '\0')
    {
        if (*begin == ',')
            comma_ctr++;
        ++begin;
    }
    
    if ((comma_ctr >= 1) && (param_num < comma_ctr + 1) ) // too many commas.
    {
        // record the error.
        add_error(line_number, EXCESSIVE_COMMAS, file_name);
        return 0;
    }

    free(params);
    params = NULL;
    return counter;
}


