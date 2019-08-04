#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/postlist.h"

// create a new node 
postNode* create_postNode(int id){
	postNode* node = (postNode*)malloc(sizeof(postNode));
	node->text_id = id;
	node->tf = 1;
	node->next = NULL;
	return node;
}

// print postlist
void print_postList(postNode* start){
	postNode* current = start;
	while(current != NULL){
		printf("[id:%d ", current->text_id);
		printf("tf:%d]\n", current->tf);
		current = current->next;
	}
}

// destroy a postlist
void destroy_postlist(postNode* head){
	if(head!=NULL){
		destroy_postlist(head->next);
		free(head);
	}
}

// get document frequency of word , using postlist
int get_df(postNode* start){
	int count = 0;
	postNode* current = start;
	while(current != NULL){
		count++; 
		current = current->next;
	}
	return count;
}


// insert a node into post list , if it doesn't exist
int insert_postNode(postNode** node,int id){
	postNode* current = *node;
	while( current != NULL){
		if( current->text_id == id ){
			current->tf = current->tf + 1;
			return 1;
		}
		current = current->next;
	}
	//insert to postList
	postNode* newNode = create_postNode(id);
	newNode->next = *node;
	*node = newNode;
	return 0;
}