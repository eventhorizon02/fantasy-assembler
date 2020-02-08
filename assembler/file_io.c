#include <stdlib.h>
#include "utills.h"
#include "hash.h"
#include "file_io.h"


// produce the requried object file
bool create_object_file(const char* file_name,int* mem,unsigned int inst_size, unsigned int data_size)
{
  /*at this point the inst_memory has been expanded and the data image has been appended to the end  	of it, therefore inst_memory is actually both instructions + data now*/
  unsigned int mask = 63; // 6 bits of 1's equals 63
  unsigned int twoWord;
  char word1,word2;
  FILE *fp;
  if (!(fp=fopen(file_name, "w")))
    {
	fprintf(stderr,"\nfailed to produce source file:%s\n",file_name);
	exit(0);
    }
   // print the first line
   fprintf(fp,"%d %d\n",inst_size,data_size);
  // loop through the array
   for (int i = 0; i < inst_size + data_size; ++i)
   {
     twoWord = *(mem + i);
     word1 = (char)(twoWord & mask);
     mask = mask << 6;
     word2 = (char)( (twoWord & mask) >> 6 ); // move result to LSB side before casting to char.
     word2 = encode64(word2);
     word1 = encode64(word1);
     mask = mask >> 6; // reset the mask for the next iteration.
     fputc(word2,fp);
     fputc(word1,fp);
     fputc('\n',fp);
   }
  fclose(fp);
  return true;
}
// create the .ext file
void create_externals_file(const char* file_name)
{
	unsigned int num = get_extrn_ref_num();
	if (!num) return; // do not print externals file if there are no externals recorded.
    FILE *fp;   
    if (!(fp=fopen(file_name, "w")))
    {
	 fprintf(stderr,"\nfailed to produce externals file:%s\n",file_name);
	 exit(0);
    }
   // iterate through the extrn_arr and print out the label names and addresses
    for ( int i = 0; i < num; ++i)
    fprintf(fp,"%-32s%d\n",extrn_arr[i]->label,extrn_arr[i]->address);
}


void create_entries_file(const char* file_name)
{
	// first check that there are any entries to be printed.
	struct label* itr = Label;
	bool found = false;
    while (itr)
    {
        if (itr->isEntry)
        {
        	found = true;
        	 break;
        }
            
        itr = itr->next;
    }
	
    if (!found) return;
    
    FILE *fp;
    if (!(fp=fopen(file_name, "w")))
    {
        fprintf(stderr,"\nfailed to produce entries file:%s\n",file_name);
        exit(0);
    }
    // iterate through the labels list and print out the ones that are marked as entry
    itr = Label;
    while (itr)
    {
        if (itr->isEntry)
            fprintf(fp,"%-32s%d\n",itr->label,itr->address);
        itr = itr->next;
    }
}

// open a file and retrun a pointer to it
FILE* open_file(const char* path)
{
	FILE *fp;
    if (!(fp=fopen(path, "r")))
    {
        fprintf(stderr,"\ncannot open source file:%s\n",path);
        exit(0);
    }
    return fp;
}

// get the next line from an open file
bool read_next_line(FILE* fp, char* line)
{
   return ( fgets (line, MAX_LINE_LENGTH, fp) != NULL );
}















