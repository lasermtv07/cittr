#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "structs.c"
char* genResponse(struct response resp){
	char* out=malloc(2048+strlen(resp.content));
	if(resp.code==200){
		strcpy(out,"HTTP/1.0 200 OK\r\n");
	}
	else {
		strcpy(out,"HTTP/1.0 ");
		out[9]=(resp.code-(resp.code%100))/100+0x30;
		out[10]=((resp.code-((out[9]-0x30)*100))-((resp.code-((out[9]-0x30)*100))%10))/10+0x30;
		out[11]=resp.code%10+0x30;
		out[12]='\r';
		out[13]='\n';
	}
	strcat(out,"Server: ws\r\n");
	strcat(out,"Content-type: text/html\r\n");
	sprintf(out,"%sSet-Cookie: %s\r\n\r\n",out,resp.cookie);
	strcat(out,resp.content);
	return out;
}

