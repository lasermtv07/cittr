//TODO: fix a LOT of memory leaks! #include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <sqlite3.h>
#include "parse.c"
#include "bst.c"
#include "structs.c"
#include "response.c"

#define MAX_CONN 256
size_t fileSize(char* file){
	FILE* f=fopen(file,"r");
	if(f==NULL) return -1;
	fseek(f,0,SEEK_END);
	size_t size=ftell(f);
	fclose(f);
	return size;
}
bool checkHTML(char string[]){
	int l=0;
	while(string[l]!='.' && l<strlen(string)){
		l++;
	}
	if(strcmp(string+l,".html")==0 || strcmp(string+l,".htm")==0) return true;
	else return false;
}
int regCallback(void* NotUsed,int argc,char**argv,char** azColName){
	printf("%d\n",argv[0]);
	return 0;
}
int main(){
	sqlite3 *db;
	char stmt[10240];
	if(sqlite3_open("data.db",&db)!=SQLITE_OK){
		printf("error with sqlite\n");
		return 1;
	}
	printf("SQL:%s\n",sqlite3_libversion());
	int sock=socket(AF_INET,SOCK_STREAM,0);
	if(sock==-1){
		printf("failed\n");
		return 1;
	}
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
    	perror("setsockopt(SO_REUSEADDR) failed");

	printf("socket created\n");
	struct sockaddr_in ad;
	ad.sin_family=AF_INET;
	ad.sin_port=htons(8080);
	ad.sin_addr.s_addr=htonl(INADDR_ANY);
	
	if(bind(sock,(struct sockaddr*)&ad,sizeof(ad))!=0){
		printf("failed bind\n");
		return 1;
	}
	if(listen(sock,MAX_CONN)!=0){
		printf("failed listen\n");
	}
	size_t al=sizeof(ad);
	char* reqinfo=malloc(102400);
	struct requestData a;
	char* route;
	char* response;
	struct paramNode* get=newNode(".",".");
	struct paramNode* post=newNode(".",".");
	struct response resp;
	FILE* f;
	char currentChar;
	int currentCtr;
	size_t l=0;
	char buff[10240];
	char* tmp;
	char* tmp2;
	char* login;
	char* pas;
	char* newpost;
	for(;;){
		int new=accept(sock,(struct sockaddr*)&ad,(socklen_t*)&al);
		if(new<0){
			printf("error accept\n");
			continue;
		}
		get=newNode(".",".");
		post=newNode(".",".");

		read(new,reqinfo,102400);
		printf("%d\n",strlen(reqinfo));
		//printf("%s\n~~~~~~~~~~~~~~~~~~~~~~",reqinfo);
		//problem HERE
		getPost(reqinfo,post);
		//insertNode(post,"login","asddddddddddddddddddd");
		//insertNode(post,"pas","asdasdasdasdas");

		//printf("%s",reqinfo);
		parseRequest(&a,reqinfo);

		//printf("Request: %d\nRoute: %s\nUseragent:%s\ncookies:%s\n",a.reqType,a.route,a.userAgent,a.cookies);

		//getGet(a.route,get);

		if(searchNode(get,"tst")!=NULL){
			//printf("tst get value:%s\n",searchNode(get,"tst"));
		}
		else {
			//printf("get: tst isn't set!\n");
		}
		//printf("%s\n",reqinfo);
		route=getRoute(a.route);
		if(strcmp(route,"/")==0){
			resp.code=200;
			resp.cookie="tst1";
			resp.content="<html><body><center><h1>response</h1></center></body></html>";
			response=genResponse(resp);
		}
		else if(strcmp(route,"/test")==0){
			response=malloc(5);
			strcpy(response,"test");
		}
		else {
			response=malloc(150);
			strcpy(response,route+1);
		if(access(route+1,F_OK)==0){
			free(response);
			resp.code=200;
			resp.cookie="";
			f=fopen(route+1,"r");
			fseek(f,0,SEEK_END);
			l=ftell(f)+1;
			rewind(f);
			response=malloc(l);
			response[0]=0;
			while(fgets(buff,10240,f)){
				strcat(response,buff);
			}
			//TODO: potential memory leak here
			resp.content=response;
			fclose(f);
			if(checkHTML(route)){
				response=genResponse(resp);
			}
			else {
				response=resp.content;
			}
		}

		}

		//TODO: fix register bugs
		if(strcmp(route,"/register.html")==0){
			tmp=replStr(response,"[[user]]","uwu");
			free(response);
			response=tmp;
			if(existNode(post,"s")){
				if(existNode(post,"login") && existNode(post,"pas")){
					login=searchNode(post,"login");
					pas=searchNode(post,"pas");
					if(strlen(pas)>8){
						tmp2=malloc(strlen(login)+strlen(pas)+1024);
						sprintf(tmp2,"select login from acc where exists (select login from acc where login='%s')",login);
						printf("%s",tmp2);
						sqlite3_exec(db,tmp2,regCallback,0,NULL);
						tmp=malloc(strlen(login)+strlen(pas)+1024);
						sprintf(tmp,"insert into acc (login,pas) values '%s','%s'",login,pas);
						free(tmp);

						free(tmp2);
					}
					else {
						tmp2=replStr(response,"[[message]]","Password must be atleast 8 characters long!");
						free(response);
						response=tmp2;
					}
				}			

			}
			else {
			tmp=replStr(response,"[[message]]"," ");
			free(response);
			response=tmp;
			}
			
		}
		size_t responseLen=strlen(response);
		printf("%d\nEND\n%s\n",responseLen,response);
		response[responseLen-1]=0;
		if(write(new,response,479)<0){
			printf("write error\n");
			return 1;
		}
		printf("WRITE SUCCESS!");
		free(route);
		free(response);
		freeTree(get);
		freeTree(post);
		free(resp.content);
		close(new);
		printf("\n-----------------------------------------------\n");
	}
	return 0;
}
