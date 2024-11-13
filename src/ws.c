#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include "list.c"
#include "parse.c"
#include "dumbdb.c"

//(maybe) mistake here..?
char* myRepl(char* string,char* search, char* repl){
	char* new=malloc(strlen(string)+strlen(repl)+1);
	strncpy(new,"",strlen(string)+strlen(repl)+1);
	for(int i=0;i<strlen(string)-strlen(search);i++){
		if(string[i]==search[0]){
			if(strncmp(string+i,search,strlen(search))==0 && i<strlen(string)-strlen(search)){
				strcat(new,repl);
				strcat(new,string+i+strlen(search));
				break;
			}
		}
		new[i]=string[i];
	}
	free(string);
	return new;
}
int checkExtension(char* str,char* ext){
	char string[1024];
	strncpy(string,"",1024);
	strncpy(string,str,1024);
	char* tok=strtok(string,".");
	tok=strtok(NULL,".");
	if(tok==NULL) return 0;
	return !strcmp(tok,ext);
}
int main(){
	int sock=socket(AF_INET,SOCK_STREAM,0);
	 if(sock==-1){
		printf("error creating socket\n");
		return 1;
	 }
	struct sockaddr_in ad;
	ad.sin_family=AF_INET;
	ad.sin_port=htons(6969);
	ad.sin_addr.s_addr=htonl(INADDR_ANY);
	int al=sizeof(ad);
	//allows reusing same socket, basically doesnt require wait after server restart
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &(int){1}, al) < 0)
		printf("error setsockopt\n");
	if(bind(sock,(struct sockaddr *)&ad,sizeof(ad)) != 0){
		printf("error binding socket\n");
		return 1;
	}
	printf("socket bound to 6969\n");
	
	if(listen(sock,SOMAXCONN)!=0){
		printf("error listening\n");
		return 1;
	}
	struct reqInfo info;
	char reqbuff[10240];
	strncpy(reqbuff,"",10240);
	char* resp=NULL;
	char buff[1024];
	FILE *f;
	char* head1="HTTP/1.1 200\n\n";
	char* error="HTTP/1.1 404\n\n<html><center><h1>404: not found</h1></center></html>";
	while(1){
		int newfd=accept(sock,(struct sockaddr *)&ad,&al);
		if(newfd<0){
			printf("connection rejected\n");
			continue;
		}
		read(newfd,&reqbuff,10239);
		//printf("%s\n",reqbuff);
		parsePost(&info,reqbuff);
		
		resp=NULL;

		/*resp=malloc(1);
		resp[0]=0x0;*/

		if(checkExtension(info.path,"html") || !strcmp(info.path,"/")){
			if(info.post==NULL || strcmp(info.path,"/register.html")==0 || strcmp(info.path,"/login.html")==0){
				resp=realloc(resp,1+strlen(head1));
				strcpy(resp,head1);
			}
			else {
				resp=malloc(1048);
				sprintf(resp,"HTTP/1.1 303\nLocation: %s\n",info.path);
			}
		}
		strncpy(buff,"",1024);
		if(strcmp(info.path,"/")==0)
			f=fopen("index.html","r");
		else
			f=fopen(info.path+1,"r");
		if(f!=NULL){
			if(resp==NULL) {
				resp=malloc(2);
				strncpy(resp,"",2);
			}
			while(fgets(buff,1024,f)!=NULL && resp!=NULL){
				resp=realloc(resp,strlen(resp)+strlen(buff)+1);
				strcat(resp,buff);
				strncpy(buff,"",1024);
			}
			fclose(f);
		}
		else {
			resp=realloc(resp,strlen(error)+1);
			strcpy(resp,error);
		}

		printf("%p\n",info.post);
		if(strcmp(info.path,"/register.html")==0){
			if(info.post!=NULL){
				if(readNode(info.post,"login")!=NULL && readNode(info.post,"pass")!=NULL){
					switch(registerDb(readNode(info.post,"login"),readNode(info.post,"pass"))){
						case 0:
							resp=myRepl(resp,"[[message]]","Invalid username!");
							break;
						case 1:
							resp=myRepl(resp,"[[message]]","Invalid password! (length atleast 8)");
							break;
						case 2:
							resp=myRepl(resp,"[[message]]","User already exists!");
							break;
						case 4:
							resp=myRepl(resp,"[[message]]","Registered succesfully!");
							break;
					}
				}
			}
		}
		else if(strcmp(info.path,"/login.html")==0){
			if(info.post!=NULL){
				if(readNode(info.post,"login")!=NULL && readNode(info.post,"pass")!=NULL){
					if(loginDb(readNode(info.post,"login"),readNode(info.post,"pass"))){
						resp=myRepl(resp,"[[message]]","Login correct!");
						size_t loginSize=strlen(readNode(info.post,"login"));
						size_t passSize=strlen(readNode(info.post,"pass"));
						char* loginCook=malloc(loginSize+passSize+2);
						loginCook[0]=0x00;
						strcat(loginCook,readNode(info.post,"login"));
						strcat(loginCook,"~");
						strcat(loginCook,readNode(info.post,"pass"));

						resp=addCookie(resp,loginCook);

						printf("%s\n",resp);
						free(loginCook);
					}
					else
						resp=myRepl(resp,"[[message]]","Invalid credentials!");
				}

			}
		}
		else if(strcmp(info.path,"/")==0){
			char* tmpadmin=malloc(strlen(info.cookie)+1);
			strcpy(tmpadmin,info.cookie);
			char*adName=strtok(tmpadmin,"~");
			char* posts=readToHtmlDb(isAdmin(adName),adName);
			resp=myRepl(resp,"[[posts]]",posts);
			free(posts);
			free(tmpadmin);
			if(info.post!=NULL){
				if(readNode(info.post,"tweet")!=NULL){
					if(verifyCookie(info.cookie)){

					printf("EEEJH %d\n",verifyCookie(info.cookie));
						char* namae=strtok(info.cookie,"~");
						if(namae!=NULL){
							char* nodePost=readNode(info.post,"tweet");
							if(nodePost!=NULL){

								printf("POSTING:%s\n",nodePost);
								char* parsed=url2txt(nodePost);
								char* escaped=escapeBrackets(parsed);
								char* nulled=myNl2br(escaped);
								writePostDb(namae,nulled);
								free(parsed);
								free(nulled);
								free(escaped);
							}
						}
					}
				}
			}
		}
		else if(strncmp(info.path,"/remove",strlen("/remove"))==0){
			printf("KEEEK\n");
			if(strlen(info.path)>7){
				char* tmpadmin=malloc(strlen(info.cookie)+1);
				strcpy(tmpadmin,info.cookie);
				char*adName=strtok(tmpadmin,"~");
				char* toAdd="HTTP/1.1 303\nLocation: /\n";
				if(isAdmin(tmpadmin) || isPostByNameDb(info.path+7,tmpadmin)!=0)
					dbRemovePost(info.path+7);
				free(resp);
				free(tmpadmin);
				resp=malloc(strlen(toAdd)+1);
				strcpy(resp,toAdd);
			}
		}
		else if (strcmp(info.path,"/logout")==0){
			free(resp);
			char* toWrite="HTTP/1.1 303\nSet-Cookie: _\nLocation: /\n";
			resp=malloc(strlen(toWrite));
			strcpy(resp,toWrite);
		}
		if(strcmp(info.path,"/")==0 && strcmp(info.path,"/login.html")==0 && strcmp(info.path,"/register.html")){
			if(verifyCookie(info.cookie)){
				char* tmp2=malloc(strlen(info.cookie)+1);
				strcpy(tmp2,info.cookie);
				char* tok=strtok(tmp2,"~");
				char* string=malloc(strlen(tok)+128);
				sprintf(string,"<a href=logout >%s</a>",tok);
				resp=myRepl(resp,"[[name]]",string);
				free(tmp2);
				free(string);
			}
			else
				resp=myRepl(resp,"[[name]]","anon");
		}
		//printf("%s\n",resp);
		printf("%s\n",info.cookie);
		printf("Stats:%d\n",checkCookieValid(info.cookie));
		write(newfd,resp,strlen(resp));
		if(info.post!=NULL)
			freeList(info.post);
		strncpy(reqbuff,"",10240);
		close(newfd);
		if(resp!=NULL)
			free(resp);
	}

}
