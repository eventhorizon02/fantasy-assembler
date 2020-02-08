
#include "globals.h"
#include <stdlib.h>

unsigned int IC;
unsigned int DC;

void init_globals()
{
    IC = 100; // program is assumed to be loaded starting at address 100
    DC = 0;
}
