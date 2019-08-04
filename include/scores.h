#include <stdio.h>

// contain document's id and its relevance score
typedef struct score{
	int id;
	double score;
}score;

// initialize data structure
void initialize_score(score** array,int k);
// insert score to our data structure
void insert_score(score** array,int k,int doc_id,double doc_score);
// print our data structure
void print_score(score** array,int k);
// sort our data structure
void sort_score(score** array,int k);
// destroy our data structure
void destroy_score(score** array,int k);