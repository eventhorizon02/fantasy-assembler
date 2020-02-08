// all functions associated with the first stage of the assembling proccess.

#ifndef __first_pass__
#define __first_pass__

#include "file_io.h"


bool readSource(const char*);
void adjust_data_labels(); // at the end of the first pass, the labels marked as data need to have thier value adjusted according to the final IC value.
bool append_data_image(); // append the data image to the end of inst_memory

#endif /* __first_pass__ */
