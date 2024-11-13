#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct listNode {
	char key[1024];
	char value[1024];
	struct listNode* next;
} listNode;
listNode* newNode(char*key,char* value){
	listNode* n=malloc(sizeof(listNode));
	if(n!=NULL){
		if(key!=NULL)
			strncpy(n->key,key,1024);
		else
			n->key[0]=0;
		if(value!=NULL)
			strncpy(n->value,value,1024);
		else
			n->value[0]=0;
		n->next=NULL;
	}
	return n;
}
void addNode(listNode* a,char* key,char* value){
	if(a->next!=NULL)
		addNode(a->next,key,value);
	else
		 a->next=newNode(key,value);
	
}

//NOTE: returns pointer to the original string, ie. change modifies original string, too
char* readNode(listNode* a,char* string){
	listNode* i=a;
	 while(1) {
		if(strcmp(i->key,string)==0) return i->value;
		if(i->next==NULL) break;
		i=i->next;
	}
	return NULL;
}
char* readIndex(listNode* a,int index){
	listNode* j=a;
	if(index==0)
		return a->value;
	for(int i=0;i<index;i++){
		j=j->next;
		if(j->next==NULL) return NULL;
	}
	return j->value;
}
listNode* deleteIndex(listNode* a,int index){
	listNode* n;
	listNode* i;
	if(index==0){
		n=a->next;
		free(a);
		return n;
	}
	else {
		i=a;
		for(int c=0;;c++){
			if(c==index-1){
				n=i->next;
				i->next=(i->next)->next;
				free(n);
			}
			else
				i=i->next;
			if(i->next==NULL) break;
		}
		return a;
	}
}
void freeList(listNode* a){
	if(a->next!=NULL)
		freeList(a->next);

	free(a);
}
