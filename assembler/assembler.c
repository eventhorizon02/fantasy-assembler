#include <stdio.h>
#include "globals.h"
#include "file_io.h"
#include "utills.h"
#include <string.h>
#include "hash.h"
#include <stdlib.h>
#include "first_pass.h"
#include "second_pass.h"
#include "error_handling.h"


int main(int argc, const char * argv[])
{
    init_err_arr(); // initialize the errors table.

    for (int i = 1; i < argc; ++i)
    {
        init_globals();
        init_hash_memory(); // initialize the hash tables for instruction and data memory
        reset_current_error_counter();
        const char* path = argv[i];
        char* sourceFile = malloc(strlen(path) + 5);
        strcpy(sourceFile,path);
        strcat(sourceFile,".as");
        readSource(sourceFile); // first pass.
        adjust_data_labels(); // add IC value to the labels makrked as data.
        second_pass(sourceFile);
        if (!get_new_errors_number())
        {
            append_data_image(); // append the data to the end of the instructions
            strcpy(sourceFile,path);
            strcat(sourceFile,".ob");
            create_object_file(sourceFile,inst_memory,IC ,DC);
            strcpy(sourceFile,path);
            strcat(sourceFile,".ext");
            create_externals_file(sourceFile);
            strcpy(sourceFile,path);
            strcat(sourceFile,".ent");
            create_entries_file(sourceFile);
        }
        // free all memories used.
        free_label_hash();
        free(inst_memory); // at this point inst_memory also includes data_image.
        free_extrn_ref();
        inst_memory = NULL;
        free(sourceFile);
        sourceFile = NULL;
      }
    
    print_error_report();
    free_error_arr();
    return 0;
}

