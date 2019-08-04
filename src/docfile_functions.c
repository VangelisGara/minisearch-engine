#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/docfile_functions.h"

// prints contents of a file
void printfile(FILE* df)
{
	// Read contents from file
    int c = fgetc(df);
    while (c != EOF){
        printf ("%c", c);
        c = fgetc(df);
    }
    printf("\n");
    rewind(df);
}

// count the number of lines in a file
int countlines(FILE* fp)
{
	int c,count = 0;
	int emptyLine = 0;
 	// Extract characters from file and store in character c
    for (c = getc(fp); c != EOF; c = getc(fp)){
    	emptyLine++;
    	if (c == '\n' && emptyLine != 1){ // Empty lines aren't count
        	count = count + 1;
        	emptyLine = 0;
        }
	}
	rewind(fp);
    return count;
}

// returns a array with the size of each line of the docfile
int* get_linesizes(int doclines, FILE* fp){
	int* array = malloc(doclines*sizeof(int));
	int c = fgetc(fp);
	int i = 0;
	while( c != EOF){
		int linesize = 0;
		while( c!= '\n'){ // count the length of this line
			linesize++ ;
			c = fgetc(fp);
		}
		array[i] = linesize;
		i++;
		c = fgetc(fp); // eat new l and continue
	}
	rewind(fp);
	return array;
}

// fill a dynamically allocated array with documents
char** docfile_mapping(FILE* fp)
{
	char** mapdocs;
	int doclines = countlines(fp);
	mapdocs = (char**)calloc(doclines,sizeof(char*)); // create a string array able to store the number of docs needed
	size_t len = 0;
	ssize_t length ;
	int l = 0;
	while((length  = getline(&mapdocs[l],&len,fp)) != -1){ // store in the line
		l++;
	}
  	rewind(fp);
  	return mapdocs;
}