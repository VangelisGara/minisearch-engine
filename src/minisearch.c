#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <sys/ioctl.h>
#include "../include/scores.h"
#include "../include/docfile_functions.h"
#include "../include/trie.h"
#include "../include/postlist.h"

/* NOTES */
/* ========================================================================== */
/*    1.gdb --args ./minisearch -i ./inputs/doc1.txt     	                  */
/*    2.valgrind --leak-check=full -v ./minisearch -i ./inputs/bigDataset.txt */
/* ========================================================================== */

extern int trieSize; // global variable that stores trie size

// checks if id is valid
int valid_id(const char *s){
    while (*s) {
        if (isdigit(*s++) == 0) return 0;
    }
    return 1;
}

// return the average words
double avrg(int* array,int n){ 
	int sum = 0;
	for(int i=0;i<n;i++)
		sum += array[i];
	return ((sum)/n);
}

// return position of word in string
int substring(const char* str,const char* pat){
	char *copied = strdup(pat);
	char* ptr;
	ptr = strstr(str,pat);
	if (ptr == NULL)
		return -1;
	int pos = ptr - str;
	if(str[pos+strlen(pat)] != ' ')
		return -1;
	if(str[pos-1] != ' ')
		return -1;
	//printf("%d\n", pos);
	return pos;
}

// print a hintful menu
int print_menu(){
	printf("=============================================================================\n");
	printf("~ Welcome to Minisearch , please type one of the following type of commands~:\n");
	printf("=============================================================================\n");
	printf("- \"/search q1 q2 ... q10\" , please give at least one query and max 10.     \n");
	printf("- \"/df\" or \"/df word\" , please consider that you can use max 1 word.     \n");
	printf("- \"/tf id word\" , please consider you must use one word and id.            \n");
	printf("- \"/exit\" , for exiting the program.                                       \n");
	printf("!  Please keep command format inact.                                         \n");
	printf("=============================================================================\n");
	printf("Type you command:                                                            \n");
	return 1;
}

int main(int argc,char* argv[]){
	int i;
	
	/********* Initialization of program ***************/
	int input = 0; // docfile
	int K = 10; // top-k docs
	FILE *df; // our docfile
	/* get arguments */
	for (i = 0; i < argc; i++){
		if(!strcmp(argv[i],"-k"))
			K = atoi(argv[++i]);
		if(!strcmp(argv[i],"-i")){
			df = fopen(argv[++i], "r"); // open our file
			if ( df == NULL ){
				printf("Error opening docfile\n"); // error handling
				return -1;
			}
			input = 1 ; // we got a proper file ready
		}
	}
	if(input != 1){
		printf("Plase give input\n"); // error handling
		return -1;
	}

	printf("Initializating data base...\n\n\n");
 	/********* Create data structures ***************/
   	int doclines = countlines(df); // get number of lines
 	char** map = docfile_mapping(df); // map our docfile to a dynamically allocated array
   	
   	trieNode* root = NULL; // trie resides here
   	int WordCount[doclines]; // store the number of words each document has
 	int maxSize = 0; // max word size

   	/*         Tokenize text         */
   	int id = 0;
   	for (int l = 0; l < doclines; ++l){  // tokenize each document
  		char *copied = strdup(map[l]); // work on a temp string
  		char* pch = strtok (copied," \t\n"); // get id
  		int oldId = id;
  		id = atoi(pch);
  		if (id - oldId > 1){ // id's should start from 0 and have difference 1
  			printf("error on document id's\n");
  			exit(-1);
  		}
  		WordCount[l] = 0;
  		// start tokenizing document
  		pch = strtok (NULL, " \t\n"); // skip id
		while (pch != NULL){ // create trie word by word
    		if( strlen(pch) > maxSize) // find the biggest word in trie
    			maxSize = strlen(pch);
    		trie_insert(&root,pch,id);
    		WordCount[l] = WordCount[l] + 1;
    		pch = strtok (NULL, " \t\n");
  		}
  		free(copied); // free temp string
	}
	//print_trie(root);

 	/********** Interface starts here ***************/
 	size_t len = 0;
	ssize_t length ;
	char* command; // initialize minisearch to get user commands
	
	double avgdl = avrg(WordCount,doclines); // average words in index
	const double kl = 1.2; // parameters
	const double b = 0.75;

	while(print_menu() && ((length  = getline(&command,&len,stdin)) != -1)) { // start getting commands
		printf("\n");
		char* pch = strtok(command," /"); // command's error handling
		if(command[0] != '/'){
			printf("Commands start with / \n");
			continue;
		}

		//case search
		if(!strcmp(pch,"search")){
			score* topk[K]; // store top-k documents here
			initialize_score(topk,K);

			// resolve the Query
			char* q[10]; // store each word to be searched here
			int n = 0;
			pch = strtok (NULL, " \n"); // start getting queries
			while (pch != NULL && n != 10){
    			q[n] = pch; // store all words into an array
    			n++;
    			pch = strtok (NULL, " \n");
  			}

  			// find all the fifferent document ids D [use a postlist for this]
  			postNode* distinct_docs = NULL;
  			for(int i =0; i<n ; i++){ // for each word 
  				postNode* postlist = search_word(root,q[i]); // find its postlist
  				postNode* current = postlist;
  				while(current!=NULL){
  					insert_postNode(&(distinct_docs),current->text_id);
  					current = current->next;
  				}
			}

			//for each document calculate its relevance score
			postNode* current = distinct_docs;
			while(current != NULL){
				int D = current->text_id; // Compute score for this document
				//printf("Will Compute the relevance score of document : %d\n", D);
				double score = 0; // will hold BM25 score
				for(int i=0; i<n ; i++){ // Q = q1 q2 ... qn
					// get ready for the score calculation
					int fqD = term_frequency(root,D,q[i]);
					int DW = WordCount[D-1];
					int N = doclines;
					// calulate IDF
					int nqi = document_frequency(root,q[i]);
					double idf_score = log10((N - nqi + 0.5)/(nqi + 0.5));
					// calculate BM25
					score += idf_score*((fqD*(kl+1))/(fqD+kl*(1-b+((b*DW)/avgdl))));
				}
				insert_score(topk,K,D,score);
				current = current->next;
			}

			// find the top-k documents
			sort_score(topk,K);
			//print_score(topk,K);
			
			// print results with the format needed
			struct winsize w;
    		ioctl(0, TIOCGWINSZ, &w);
    		int terminal_size = w.ws_col;

			for(int i=0 ; i<K ; i++){ // Underline every text
				if(topk[i]->id == -1)
					continue;
				
				// print results with format
   				printf("(%d)[%lf] \n",topk[i]->id,topk[i]->score); // print document and its score
					
				char filline[terminal_size]; // the line to be filled with text
				memcpy(filline, map[topk[i]->id]+4, terminal_size); // move to the beggining of the document
			
				int full = strlen(map[topk[i]->id]);
				int part = terminal_size; // we got that part of the text

				char underline[terminal_size]; // the words-marking will be set here
				for(int j=0; j<terminal_size; j++)
					underline[j] = ' ';					
				
				int done = 0; // be sure to mark each line
				int done_fromStart = 0;
				while(done != 2 && done_fromStart != 2){ // for every part of the document
					if(part >= full)
						done_fromStart = 2;
					for(int t=0; t<n ; t++){ // mark every word in this part of the document
						int here = 4;
						int offset = 4;
						while(offset != -1){     
							offset = substring(filline+offset,q[t]); // find the position of word in the string
							here += offset;
							if( offset != -1){
	   							memset(underline+here,'^',strlen(q[t])); // mark bellow it
	   							offset = here + strlen(q[t]);
	   							here = offset;
							}
	   					}
	   				}
	   				printf("%s\n",filline); // print document
					printf("%s\n", underline); // print the markers

					memcpy(filline, map[topk[i]->id]+part, terminal_size); // get the next part till we print whole text
					for(int j=0; j<terminal_size; j++)
						underline[j] = ' ';	
						
					part += strlen(filline); // move to the next part
					if(part >= full) // if last part , mark it and process end of search
						done++;
				}
			}

			// free structs for search
			destroy_postlist(distinct_docs);
			destroy_score(topk,K);
		}	

		// case df
		else if(!strcmp(pch,"df") || !strcmp(pch,"df\n")){
			pch = strtok (NULL, " \n"); // start getting words
			if(pch == NULL) // document frequency of all words
				df_All(root,maxSize);
			else
				printf("%s %d\n",pch,document_frequency(root,pch)); // document frequency of a single word
		}

		// case tf
		else if(!strcmp(pch,"tf")){		
			pch = strtok (NULL, " \n"); // start getting id
			if(pch == NULL){
				printf("Please give id first !\n");
				continue;
			}
			if(!valid_id(pch)){
				printf("Please give valid id !\n");
				continue;
			}
			int id = atoi(pch);

			pch = strtok (NULL, " \n"); // start getting word		
			if(pch == NULL){
				printf("Please give a word !\n");
				continue;
			}
			int tf = term_frequency(root,id,pch); // find the word's term frequency
			if ( tf == 0 )
				printf("Word not found on document %d\n", id);
			else
				printf("Answer:\n%d %s %d\n\n",id , pch , tf);
		}

		// case exit
		else if(!strcmp(pch,"exit\n")){
			printf("Bye.\n");
			break;
		}
		// case unkown command
		else{
			printf("Unknown command , please mind reading info menu !\n");
		}
	}

   	/********* Finilize program ****************/
	/* free dynamically created data structures */
	for(int l=0;l<doclines;l++){
		free(map[l]);	
	}
	free(map);
	trie_destroy(root);
	
    fclose(df);
    return 0;
}
