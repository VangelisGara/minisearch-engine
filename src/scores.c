#include <stdio.h>
#include <stdlib.h>
#include "../include/scores.h"

// print k biggest scores , if we have k
void print_score(score** array,int k){
	printf("----------------\n");
	for (int i = 0; i < k; ++i)
		if(array[i]->id != -1)
			printf("ID:%d SCORE:%lf\n",array[i]->id,array[i]->score);
	printf("----------------\n");
}

// initialize our data structure
void initialize_score(score** array,int k){
	for(int i=0;i<k;i++){
		array[i] = (score*)malloc(sizeof(score)); // create the space to hold top-K documents
		array[i]->id = -1;
		array[i]->score = -10000000.0;
	}
}

// insert a score to our data structure
void insert_score(score** array,int k,int doc_id,double doc_score){
	//printf("doc_score : %lf\n",doc_score);
	double min_score = 10000000000.0;
	int index;
	for (int i = 0; i < k; i++) // find the minimum score in trie
	{
		if(array[i]->score < min_score){
			min_score = array[i]->score;
			index = i;
		}
	}
	if(doc_score > array[index]->score){ // if we have a greater score put it in the elites
		array[index]->score = doc_score;
		array[index]->id = doc_id;
	}
}

// sort our scores in descending order
void sort_score(score** array,int k){
	for(int i=0;i<k-1;i++){
		for(int j=0;j<k-i-1;j++){
			if(array[j]->score < array[j+1]->score){
				score* temp = array[j];
				array[j] = array[j+1];
				array[j+1] = temp; 
			}
		}
	}
}

// destroy our data structure
void destroy_score(score** array,int k){
	for (int i = 0; i < k; ++i)
		free(array[i]);
}