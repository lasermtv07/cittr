#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.c"
#include "bst.c"
void parseRequest(struct requestData *a,char str[]){
	char* string=malloc(strlen(str));
	strcpy(string,str);
	char* tok=strtok(string,"\n");
	char* temp=malloc(1024);
	int count=0;
	while(tok!=NULL){
		if(strlen(tok)>10){
			if(strncmp(tok,"GET",3)==0 || strncmp(tok,"POST",4)==0){
				sprintf(temp,"%s",tok+4);
				sscanf(temp,"%s",temp);
				strncpy(a->route,temp,1024);
				if(strncmp(tok,"GET",3)==0) a->reqType=M_GET;
				if(strncmp(tok,"POST",4)==0) a->reqType=M_POST;
			}
			if(strncmp(tok,"Cookie",6)==0){
				sprintf(temp,"%s",tok+8);
				strncpy(a->cookies,temp,1024);
			}
			if(strncmp(tok,"User-Agent",10)==0){
				sprintf(temp,"%s",tok+12);
				strncpy(a->userAgent,temp,1024);
			}
		}
		tok=strtok(NULL,"\n");
	}
	free(string);
	free(tok);
	free(temp);
}
//adds key value pair
void addKVP(struct paramNode* a,char* string){
	char*string2=malloc(strlen(string));
	strcpy(string2,string);
	char* tok=strtok(string2,"=");
	char* key=malloc(strlen(string));
	strcpy(key,tok);
	tok=strtok(NULL,"=");
	char* value=malloc(strlen(string));
	strcpy(value,tok);
	insertNode(a,key,value);
	free(string2);
}

void getGet(char string[], struct paramNode* a){
	char* string2=malloc(strlen(string));
	strcpy(string2,string);
	char* tok=strtok(string2,"?");
	tok=strtok(NULL,"&");
	while(tok!=NULL){
		addKVP(a,tok);
		tok=strtok(NULL,"&");
	}
	free(string2);
}

//for extracting route from GET params
char* getRoute(char string[]){
	char* string2=malloc(strlen(string));
	strcpy(string2,string);
	char*tok=strtok(string2,"?");
	return tok;
}
//extracting POSTS

void getPost(char string[],struct paramNode* a){
	char* tmp=malloc(strlen(string));
	strcpy(tmp,string);
	char* tok=strtok(tmp,"\n");
	char* prev=malloc(10241);
	while(1){
		strcpy(prev,"");
		strncpy(prev,tok,10240);
		tok=strtok(NULL,"\n");
		if(tok==NULL) break;
	}
	if(prev[0]==13) return;
	char* key=malloc(strlen(prev));
	char* tk=strtok(prev,"&");
	while(tk!=NULL){
		for(int i=0;i<strlen(prev);i++) key[i]=0;
		for(int i=0;i<strlen(tk);i++){
			if(tk[i]=='='){
				strncpy(key,tk,i);
				insertNode(a,key,tk+i+1);
			}
		}
		tk=strtok(NULL,"&");
	}
	free(tmp);
	free(prev);
}

