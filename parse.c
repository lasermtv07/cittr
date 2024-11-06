#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.c"

struct reqInfo {
	char path[1024];
	char cookie[1024];
	listNode* post;

};
//TODO: finish parsing
void parsePost(struct reqInfo* req,char* string){
	char* t=malloc(10240);
	strncpy(t,string,10240);
	char* tok=strtok(t,"\n");
	char* prev;
	strncpy(req->path,"",1024);
	strncpy(req->cookie,"",1024);
	while(1){
		prev=tok;
		if(strncmp(tok,"Cookie:",strlen("Cookie:"))==0)
			strncpy(req->cookie,tok+strlen("Cookie: "),1024);
		if(strncmp(tok,"GET",3)==0){
			for(int i=4;i<strlen(tok);i++){
				if(tok[i]==' ' || tok[i]=='?'){
					strncpy(req->path,tok+4,i-4);
					printf("PATH:%s\n",req->path);
					break;
				}
			}
		}
		if(strncmp(tok,"POST",4)==0){
			for(int i=5;i<strlen(tok);i++){
				if(tok[i]==' ' || tok[i]=='?'){
					strncpy(req->path,tok+5,i-5);
					printf("PATH:%s\n",req->path);
					break;
				}
			}
		}
		tok=strtok(NULL,"\n");
		if(tok==NULL) break;
	}
	//printf("%s\n",prev);
	req->post=NULL;
	char* tk=strtok(prev,"&");
	char key[1024];
	char value[1024];
	size_t pad=0;
	while(1){
		if(tk[0]==13){
			free(t);
			return;
		}
		strncpy(key,"",1024);
		strncpy(value,"",1024);
		pad=0;
		for(;tk[pad]!=0 && tk[pad]!='=';pad++);
		strncpy(key,tk,pad);
		sprintf(value,"%s",tk+pad+1);
		if(req->post==NULL)
			req->post=newNode(key,value);
		else
			addNode(req->post,key,value);
		printf("k:%s\nv:%s\n",key,value);
		tk=strtok(NULL,"&");
		if(tk==NULL) break;
	}

	free(t);
}

char* addCookie(char* header,char* cookie){
	size_t shift=0;
	for(int i=0;i<strlen(header);i++){
		shift++;
		if(header[i]!=0){
			if(header[i]=='\n' && header[i-1]=='\n')
				break;
		}
	}
	if(shift==strlen(header))
		return header;

	char* cpy=malloc(strlen(header)+1);
	strncpy(cpy,header,strlen(header));
	char* tok=strtok(cpy,"\n");
	char* first=malloc(strlen(tok)+1);
	strcpy(first,tok);

	char* resp=malloc(strlen(header)+1);
	strncpy(resp,"",strlen(header)+1);

	strcpy(resp,header+shift-1);
	char* fin=malloc(strlen(header)+strlen(cookie)+256);
	strncpy(fin,"",strlen(header)+strlen(cookie)+255);
	sprintf(fin,"%s\nSet-Cookie: %s\n%s",first,cookie,resp);

	free(resp);
	free(cpy);
	free(first);
	free(header);
	return fin;

}
int checkCookieValid(char* cookie){
	char cok[1024];
	strncpy(cok,"",1024);
	strncpy(cok,cookie,1024);
	char* log=strtok(cok,"~");
	char* pass=malloc(strlen(cookie));
	strncpy(pass,"",strlen(cookie));
	char* tmp=strtok(NULL,"~");
	if(tmp!=NULL)
		strcpy(pass,tmp);
	else {
		free(pass);
		return 0;
	}
	printf("%s\n",pass);
	FILE *f=fopen("acc.txt","r");
	char  buff[1024];
	strncpy(buff,"",1024);

	while(fgets(buff,1024,f)!=NULL){
		if(strlen(buff)==strlen(log)+strlen(pass)+3){
			if(strncmp(buff+2,log,strlen(log))==0 && strncmp(buff+3+strlen(log),pass,strlen(cookie)-strlen(log)-2)==0){
				free(pass);
				fclose(f);
				return 1;
			}
		}
		strncpy(buff,"",1024);
	}

	free(pass);
	fclose(f);
	return 0;
}
