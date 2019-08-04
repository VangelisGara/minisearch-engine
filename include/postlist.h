#ifndef _POSTLISTH_
#define _POSTLISTH_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct postNode
{
	int text_id;
	int tf;
	struct postNode* next; 
}postNode;

// creates a node for our postlist
postNode* create_postNode(int id);
// prints postlist
void print_postList(postNode* start);
// inserts a postnode
int insert_postNode(postNode** node,int id);
// return the size of postlist
int get_df(postNode* start);
// destroy the postlist
void destroy_postlist(postNode* head);
#endif