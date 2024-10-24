#pragma once
#include "structs.c"
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

//checks if first argument is first in alphabetical order
bool alphOrderFirst(char first[], char second[]){
	int minLen;
	if(strlen(first)>strlen(second)) minLen=strlen(second);
	else minLen=strlen(first);
	for(int i=0;i<minLen;i++){
		if(first[i]>second[i]) return false;
		else if(first[i]<second[i]) return true;
	}
	if(strlen(first)>strlen(second)) return false;
	else return true;
}
struct paramNode* newNode(char key[],char value[]){
	struct paramNode* n=malloc(sizeof(struct paramNode));
	char* str1=malloc(strlen(key));
	char* str2=malloc(strlen(value));
	strcpy(str1,key);
	strcpy(str2,value);
	if(n!=NULL){
		n->key=str1;
		n->value=str2;
		n->left=NULL;
		n->right=NULL;
	}
	return n;
}
void insertNode(struct paramNode* root,char k[],char v[]){
	char* key=malloc(strlen(k));
	strcpy(key,k);
	char* value=malloc(strlen(v));
	strcpy(value,v);
	struct paramNode* new=newNode(key,value);
	if(strcmp(root->key,key)==0){
		root->value=value;
	}
	else {
		if(alphOrderFirst(key,root->key)){
			if(root->left==NULL) root->left=new;
			else insertNode(root->left,key,value);
		}
		else {
			if(root->right==NULL) root->right=new;
			else insertNode(root->right,key,value);
		}
	}
	free(key);
	free(value);
}
char* searchNode(struct paramNode* root,char key[]){
	if(strcmp(root->key,key)==0){
		return root->value;
	}
	else {
		if(alphOrderFirst(key,root->key)){
			if(root->left==NULL) return NULL;
			else return searchNode(root->left,key);
		}
		else {
			if(root->right==NULL) return NULL;
			else return searchNode(root->right,key);
		}
	}
	return NULL;
}
void freeTree(struct paramNode* a){
	if(a->left!=NULL) freeTree(a->left);
	if(a->right!=NULL) freeTree(a->right);
	free(a);
	return;
}
bool existNode(struct paramNode* root,char* value){
	if(root!=NULL){
		if(strcmp(value,root->key)==0) return true;
		else {
			if(root->left!=NULL && root->right!=NULL){
				return existNode(root->left,value) || existNode(root->right,value);
			}
			else if(root->left!=NULL && root->right==NULL){
				return existNode(root->left,value);
			}
			else if(root->left==NULL && root->right!=NULL){
				return existNode(root->right,value);
			}
		if(strcmp(value,root->key)!=0) return false;
		}
	}
}
