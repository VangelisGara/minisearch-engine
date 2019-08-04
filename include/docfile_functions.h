#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// prints contents of a file
void printfile(FILE* df);

// count the number of lines in a file
int countlines(FILE* fp);

// returns a array with the size of each line of the docfile
int* get_linesizes(int doclines, FILE* fp);

// fill a dynamically allocated array with documents
char** docfile_mapping(FILE* fp);
