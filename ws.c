//TODO: fix a LOT of memory leaks!
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include "parse.c"
#include "bst.c"
#include "structs.c"
#include "response.c"

#define MAX_CONN 256

bool checkHTML(char string[]){
	int l=0;
	while(string[l]!='.' && l<strlen(string)){
		l++;
	}
	if(strcmp(string+l,".html")==0 || strcmp(string+l,".htm")==0) return true;
	else return false;
}

int main(){
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
	struct response resp;
	FILE* f;
	char currentChar;
	int currentCtr;
	size_t l=0;
	char buff[10240];
	for(;;){
		int new=accept(sock,(struct sockaddr*)&ad,(socklen_t*)&al);
		if(new<0){
			printf("error accept\n");
			continue;
		}
		read(new,reqinfo,102400);
		parseRequest(&a,reqinfo);
		printf("Request: %d\nRoute: %s\nUseragent:%s\ncookies:%s\n",a.reqType,a.route,a.userAgent,a.cookies);

		getGet(a.route,get);
		if(searchNode(get,"tst")!=NULL){
			printf("tst get value:%s\n",searchNode(get,"tst"));
		}
		else {
			printf("get: tst isn't set!\n");
		}
		printf("%s\n",reqinfo);
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
		if(write(new,response,strlen(response))<0){
			printf("write error\n");
			return 1;
		}
		free(route);
		free(response);
		close(new);
	}
	return 0;
}
