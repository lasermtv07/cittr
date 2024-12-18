//TODO: fix login
#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "likes.c"


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
	fprintf(f,"0;%s;%s;\n",login,pass);
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
	if(content[0]==0 || content[0]=='\n')
		return 0;
	char toWrite[256];
	strncpy(toWrite,"",256);
	strncpy(toWrite,content,256);
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
	char* file=malloc(1);
	file[0]=0;
	size_t fsize=1;
	while(fgets(buff,1024,f)!=NULL){
		fsize+=strlen(buff)+1;
		file=realloc(file,fsize);
		strcat(file,buff);
		char* tok=strtok(buff,";");
		if(tok!=NULL)
			if(max<atoi(tok))
				max=atoi(tok);
		strncpy(buff,"",1024);
	}
	fclose(f);
	max++;
	f=fopen("posts.txt","w");
	if(f==NULL){
		free(file);
		return 1;
	}
	
	fprintf(f,"%d;%s;%s;%s\n%s",max,time,name,toWrite,file);
	free(file);
	fclose(f);

}
char* myNl2br(char* string){
	//printf("\n%d - %s\n",strlen(string),string);
	char* o=malloc(1024*4+1);
	strncpy(o,"",1024*4+1);
	char* tmp=malloc(1024+1);
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
char* readToHtmlDb(int ad,char* isName){
	char buff[1025];
	char buffCpy[1025];
	strncpy(buff,"",1024);
	strncpy(buffCpy,"",1024);
	FILE *f=fopen("posts.txt","r");
	char* e=malloc(1);
	e[0]=0;
	if(f==NULL)
		return e;
	free(e);
	char* posts=malloc(1);
	posts[0]=0;
	char name[1024];
	char date[1024];
	char msg[1024];
	char number[1024];

	strncpy(name,"",1023);
	strncpy(date,"",1023);
	strncpy(msg,"",1023);
	strncpy(number,"",1023);
	while(fgets(buff,1024,f)!=NULL){
		strcpy(buffCpy,buff);
		strncpy(name,"",1023);
		strncpy(date,"",1023);
		strncpy(msg,"",1023);
		strncpy(number,"",1023);
		char* tok=strtok(buff,";");
		if(tok!=NULL){
			strcpy(number,tok);
			tok=strtok(NULL,";");
			if(tok!=NULL){
				strcpy(date,tok);
				tok=strtok(NULL,";");
				if(tok!=NULL){
					strcpy(name,tok);
					tok=strtok(NULL,";");
					if(tok!=NULL)
						strcpy(msg,tok);
				}
			}
		}
		if(name[0]!=0){
			posts=realloc(posts,strlen(posts)+strlen(buff)+512);
			strcat(posts,"<br><b>");
			strcat(posts,name);
			strcat(posts,"</b> - <i>");
			strcat(posts,date);
			strcat(posts,"</i>");
			if(ad || (isName!=NULL && strcmp(name,isName)==0)){
				strcat(posts,"<a style=float:right href=remove");
				strcat(posts,number);
				strcat(posts," >Delete</a>");
			}
			strcat(posts,"<br>");
			strcat(posts,msg);
			strcat(posts,"<br>");
			char* tmp=getLikesPerComment(number);
			strcat(posts,"<a href=like");
			strcat(posts,number);
			if(checkLiked(isName,number))
				strcat(posts,">Dislike (");
			else
				strcat(posts,">Like (");
			strcat(posts,tmp);
			strcat(posts,")</a>");
			free(tmp);
			strcat(posts,"<hr>");

			strncpy(buff,"",1024);
			strncpy(buffCpy,"",1024);
		}
	}
	fclose(f);
	return posts;
}
int verifyCookie(char* cookie){
	char* name=strtok(cookie,"~");
	if(name==NULL)
		return 0;
	char* pass=strtok(NULL,"~");
	if(pass==NULL)
		return 0;
	FILE*f=fopen("acc.txt","r");

	if(f==NULL)
		return 0;
	char buff[1025];
	strncpy(buff,"",1024);
	while(fgets(buff,1024,f)!=NULL){
		char* tok=strtok(buff,";");
		if(tok!=NULL){
			tok=strtok(NULL,";");
			if(tok!=NULL && strcmp(tok,name)==0){
				tok=strtok(NULL,";");
				if(tok!=NULL && strcmp(tok,pass))
					return 1;
			}
		}

		strncpy(buff,"",1023);
	}
	printf("ehh yaa got here\n");
	return 0;
}
int isAdmin(char* name){
	if(name==NULL)
		return 0;
	FILE* f=fopen("acc.txt","r");
	if(f==NULL)
		return 0;
	char buff[1024];
	while(fgets(buff,1024,f)){
		char* tok=strtok(buff,";");
		if(tok!=NULL && tok[0]=='1'){
			tok=strtok(NULL,";");
			if(tok!=NULL){
				if(strcmp(tok,name)==0){
					fclose(f);
					return 1;
				}
			}
		}
		strncpy(buff,"",1024);
	}
	return 0;
}
int dbRemovePost(char* number){
	FILE *f=fopen("posts.txt","r");
	if(f==NULL)
		return 0;
	char buff[1024];
	strncpy(buff,"",1024);
	char buff2[1024];
	strncpy(buff2,"",1024);
	char* newFile=malloc(1);
	size_t size=1;
	newFile[0]=0;
	while(fgets(buff,1024,f)!=NULL){
		strcpy(buff2,buff);
		char* tok=strtok(buff,";");
		if(tok!=NULL && strcmp(number,tok)){
			size+=strlen(buff2)+1;
			newFile=realloc(newFile,size);
			strcat(newFile,buff2);
		}
		strncpy(buff2,"",1024);
		strncpy(buff,"",1024);
	}
	fclose(f);
	f=fopen("posts.txt","w");
	fprintf(f,"%s",newFile);
	fclose(f);
	free(newFile);
	return 1;
}
int isPostByNameDb(char* num, char* name){
	FILE* f=fopen("posts.txt","r");
	if(f==NULL)
		return 0;
	char buff[1024];
	strncpy(buff,"",1024);
	char n[1024];
	strncpy(n,"",1024);
	char namae[1024];
	strncpy(namae,"",1024);
	while(fgets(buff,1024,f)!=NULL){
		strncpy(n,"",1024);
		strncpy(namae,"",1024);
		char* tok=strtok(buff,";");
		if(tok!=NULL){
			strcpy(n,tok);
			tok=strtok(NULL,";");
			if(tok!=NULL){
				tok=strtok(NULL,";");
				if(tok!=NULL){
					//TODO: fix issue here (doesnt check password)
					if(strcmp(n,num)==0 && strncmp(name,namae,strlen(namae))==0){
						printf("%s=>%s\n%s=>%s\n",n,num,namae,name);
						fclose(f);
						return 1;
					}
				}
			}
		}
		strncpy(buff,"",1024);
	}
	return 0;
}
