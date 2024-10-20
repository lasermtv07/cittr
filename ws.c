#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include "parse.c"

#define MAX_CONN 256
int main(){
	char resp[] = "HTTP/1.0 200 OK\r\n"
	"Server: ws\r\n"
	"Content-type: text/html\r\n\r\n"
	"<html><center><h1>heya</h1>"
	"<iframe width=\"560\" height=\"315\" src=\"https://www.youtube.com/embed/19y8YTbvri8?si=FyGI9bQbXyFIa753\" title=\"YouTube video player\" frameborder=\"0\" allow=\"accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture; web-share\" referrerpolicy=\"strict-origin-when-cross-origin\" allowfullscreen></iframe>"
	"<form method=POST><input type=text name=uwu /><input type=submit /></form>"
	"</center></html>"
	;
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
	for(;;){
		int new=accept(sock,(struct sockaddr*)&ad,(socklen_t*)&al);
		if(new<0){
			printf("error accept\n");
			continue;
		}
		read(new,reqinfo,102400);
		parseRequest(&a,reqinfo);
		printf("Request: %d\nRoute: %s\nUseragent:%s\ncookies:%s\n",a.reqType,a.route,a.userAgent,a.cookies);
		//printf("%s\n",reqinfo);
		route=malloc(strlen(a.route));
		strcpy(route,getRoute(a.route));
		//printf("%s\n",route);
		if(strcmp(route,"/")==0){
			response=malloc(strlen(resp));
			strcpy(response,resp);
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
