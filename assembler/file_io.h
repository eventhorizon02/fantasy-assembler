#ifndef __file_io__
#define __file_io__

#include <stdio.h>
#include "globals.h"

bool create_object_file(const char*,int*,unsigned int,unsigned int);
void create_externals_file(const char*);
void create_entries_file(const char*);
FILE* open_file(const char*); // open a file and retrun a pointer to it
bool read_next_line(FILE*,char*); // get the next line from an open file

#endif
