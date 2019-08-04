#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/postlist.h"
#include "../include/trie.h"

int trieSize = 0;

// creates and initializes an empty trie node
trieNode* create_trieNode(char letter){
	trieNode* node  = NULL;
	node = (trieNode*)malloc(sizeof(trieNode));
	node->thisLevel = NULL;
	node->nextLevel = NULL;
	node->data = letter;
	node->postList = NULL;
	return node;
}

// prints contents of a trie node
void print_trieNode(trieNode* node){
	printf("letter :%c \n", node->data);
	printf("this level %p: \n",node->thisLevel);	
	printf("next level: %p \n",node->nextLevel);	
	printf("post level: %p\n" , node->postList);
}

// print a word
void print_list(trieNode* root){
	trieNode* cur = root;
	while(cur != NULL){
		print_trieNode(cur);
		cur = cur->nextLevel;
	}
}

// print each level of a trie ( debug-like function)
void print_trie(trieNode* root){
	trieNode* depth[100];
	int i = 0;
	trieNode* cur = root;
	while(cur != NULL){
		printf("%c", cur->data);
		if(cur->nextLevel != NULL){
			depth[i] = cur->nextLevel;
			i++;
		}
		cur = cur->thisLevel;
		printf("->");
	}
	printf("\n");
	for(int j = 0 ; j < i ; j++)
		print_trie(depth[j]);
}

// insert letter to current level
trieNode* insert_char(trieNode** root,char letter){
	trieNode* current = *root;
	while( current != NULL){
		if(current->data == letter){
			return current;
		}
		current = current->thisLevel;
	}
	// insert to start of the list
	trieNode* newNode = create_trieNode(letter);
	newNode->thisLevel = *root;
	*root = newNode;
	trieSize++;
	return newNode;
}

// insert a word to trie
void trie_insert(trieNode** root,const char* word,int id){
	//printf("Inserting word: %s\n", word);
	int length = strlen(word);
	trieNode* current = *root;
	for(int i=0; i<length ; i++){ // split word to chars and insert one by one
		char thisChar = word[i]; 
		if ( i == 0 ){ // root shows to this word
			int oldSize = trieSize;
			current = insert_char(&(current),thisChar);
			if( i == length - 1)// if last character
				insert_postNode(&(current->postList),id);
			if( trieSize > oldSize) // if we didn't insert new character , root doesn't need to change
				*root = current;
		}
		else{
			current = insert_char(&(current->nextLevel),thisChar); // insert each letter one level down
			if( i == length - 1)// if last character
				insert_postNode(&(current->postList),id);
		}
	}
}

/********* Command functions on trie ************/

// return a postList of a word
postNode* search_word(trieNode* root,const char* word){
	int length = strlen(word);
	trieNode* current = root;
	for(int i=0; i<length ; i++){ // traverse the trie
		char thisChar = word[i];
		int found = 0;
		while(current != NULL){ // search level
			if(current->data == thisChar){
				if(i == length -1) // when you hit rock bottom and found the word , then return its posting list
					return current->postList;
				current = current->nextLevel; // go to next level
				found = 1;
				break;
			}
			current = current->thisLevel;
		}
		if(!found)
			return NULL;
	}
	return NULL;
}

// search the term frequency of a word in a document
int term_frequency(trieNode* root,int id ,const char* word){
	postNode* postlist = search_word(root,word); // find word's postlist
	//print_postList(postlist);
	postNode* current = postlist;
	int found = 0;
	while(current != NULL){
		if(current->text_id == id){
			found = 1;
			break;
		}
		current=current->next;
	}
	if(found)
		return current->tf;
	else
		return 0;
}

// search document frequency of a word
int document_frequency(trieNode* root,const char* word){
	postNode* postlist = search_word(root,word); // find word's postlist
	return get_df(postlist);
}

// print word
void printArray(char word[], int len) {
  for (int i=0; i<len; i++) 
    printf("%c", word[i]);
}

// df function for whole trie
void df_All(trieNode* root,int max) {
  char path[max];
  printTrie(root, path, 0);
}
 
// travels the trie
void printTrie(trieNode* node,char path[], int pathLen) 
{
  if (node==NULL) 
    return;
 
  path[pathLen] = node->data;
  pathLen++;
 
  if(node->postList != NULL){ // store path
  	printArray(path, pathLen);
  	printf(" %d\n",get_df(node->postList));
  }

  printTrie(node->nextLevel, path, pathLen);
  printTrie(node->thisLevel, path, pathLen-1); 
}

// destroy a trie
void trie_destroy(trieNode* node){
	if (node==NULL) 
    	return;
 
  	if(node->postList != NULL){ // destroy its postlist
  		destroy_postlist(node->postList);
  	}

  	trie_destroy(node->nextLevel); // destroy its level
  	trie_destroy(node->thisLevel); // destroy next levels

  	free(node);
}