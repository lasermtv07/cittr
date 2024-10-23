//TODO: fix a LOT of memory leaks!
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include "parse.c"
#include "bst.c"
#include "structs.c"
#include "response.c"

#define MAX_CONN 256
int main(){
	int sock=socket(AF_INET,SOCK_STREAM,0);
	if(sock==-1){
		printf("failed\n");
		return 1;
	}
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
	printf("listening\n");
	char* reqinfo=malloc(102400);
	struct requestData a;
	char* route;
	char* response;
	struct paramNode* get=newNode(".",".");
	struct response resp;
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
			response=malloc(5);
			strcpy(response,"404");
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
