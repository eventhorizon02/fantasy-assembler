// this file holds all the global variables, definitions, constants and error codes.
#ifndef __globals__
#define __globals__
#define MAX_LINE_LENGTH 81  // 80 plus 1 for the newline char
#define INVALID_OPCODE 99

typedef enum { directive, command, empty,comment,dotData, dotString, dotEntry, dotExtern ,error } line_type;
typedef enum { false, true } bool;
enum addressingModes { immediate = 1, direct = 3, dir_reg = 5 };
extern unsigned int IC; // instruction counter
extern unsigned int DC; // data counter
void init_globals(); 
#endif // __globals__
