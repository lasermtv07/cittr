#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

int registerDb(char* login,char* pass){
	for(int i=0;i<strlen(login);i++){
		if(!((login[i]>='A' && login[i]<='Z') || 
		     (login[i]>='a' && login[i]<='z') ||
		     (login[i]>='0' && login[i]<='9'))){
			return 0;
		}
	}
	if(strlen(login)>256)
		return 0;
	for(int i=0;i<strlen(pass);i++){
		if(!((pass[i]>='A' && pass[i]<='Z') ||
		     (pass[i]>='a' && pass[i]<='z') ||
		     (pass[i]>='0' && pass[i]<='9') ||
		     (pass[i]=='.' || pass[i]=='#' || pass[i]=='?'))){
			return 1;
		}
	}
	if(strlen(pass)<6 || strlen(pass)>256)
		return 1;
	char buff[1024];
	strncpy(buff,"",1024);
	char* login2=malloc(strlen(login)+2);
	strcpy(login2,login);
	strcat(login2,";");
	FILE *f=fopen("acc.txt","r");
	if(f==NULL)
		return 3;
	while(fgets(buff,1024,f)!=NULL){
		if(strlen(buff)>8){
			if(strncmp(buff+2,login2,strlen(login2))==0){
				fclose(f);
				free(login2);
				return 2;
			}
		}
		strncpy(buff,"",1024);
	}
	fclose(f);
	free(login2);
	f=fopen("acc.txt","a");
	fprintf(f,"0;%s;%s\n",login,pass);
	fclose(f);
	return 4;
}

int loginDb(char* login,char* pass){
	FILE* f=fopen("acc.txt","r");
	char buff[1024];
	strncpy(buff,"",1024);
	while(fgets(buff,1024,f)){
		if(strlen(buff)>strlen(login)+strlen(pass)+2){
			if(strncmp(buff+2,login,strlen(login))==0 &&
		   	strncmp(buff+3+strlen(login),pass,strlen(pass))==0){
				fclose(f);
				return 1;
			}
		}
		strncpy(buff,"",1024);
	}
	fclose(f);
	return 0;
}
int writePostDb(char* name,char* content){
	time_t now=time(NULL);
	char time[100];
	struct tm tm_now;
	localtime_r(&now,&tm_now);
	strftime(time,100,"%d/%m/%Y %H:%M",&tm_now);

	int max=0;
	FILE* f=fopen("posts.txt","r");
	if(f==NULL)
		return 1;
	char buff[1024];
	strncpy(buff,"",1024);
	while(fgets(buff,1024,f)!=NULL){
		char* tok=strtok(buff,";");
		if(tok!=NULL)
			if(max<atoi(tok))
				max=atoi(tok);
		strncpy(buff,"",1024);
	}
	fclose(f);
	max++;
	f=fopen("posts.txt","a");
	if(f==NULL)
		return 1;
	fprintf(f,"%d;%s;%s;%s\n",max,time,name,content);
	fclose(f);

}
char* myNl2Br(char* string){
	char* o=malloc(strlen(string)*4+1);
	strncpy(o,"",strlen(string)*4+1);
	char* tmp=malloc(strlen(string)+1);
	strcpy(tmp,string);
	char* tok=strtok(tmp,"\n");

	while(tok!=NULL){
		strcat(o,tok);
		strcat(o,"<br>");
		tok=strtok(NULL,"\n");
	}
	free(tmp);
	return o;
}
