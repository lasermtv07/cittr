#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "list.c"
//TODO: when message delete, delete like
listNode* determineLine(char* line){
    //printf("LINE:%s\n",line);
    if(strlen(line)<2)
        return 0;
    char* str=malloc(strlen(line)+1);
    strcpy(str,line);
    char* tok=strtok(str,";");
    if(tok==NULL){
        free(str);
        return NULL;
    }
    for(int i=0;i<2;i++){
        tok=strtok(NULL,";");
        if(tok==NULL){
            free(str);
            return NULL;
        }
    }
    tok=strtok(NULL,",");
    listNode* a=newNode(tok,tok);

    while(tok!=NULL){
            char* toStr=malloc(strlen(tok)+1);
            strncpy(toStr,"",strlen(tok)+1);
            if(tok[strlen(tok)-1]=='\n')
                strncpy(toStr,tok,strlen(tok)-1);
            else
                strncpy(toStr,tok,strlen(tok));

            addNode(a,toStr,toStr);
            free(toStr);

        tok=strtok(NULL,",");

    }
    strcpy(a->key,".");
    strcpy(a->value,".");
    listNode* b=a;
    free(str);
    return a;
}
char* listToString(listNode* a){
    char* out=malloc(2);
    size_t size=2;
    out[0]=0;
    out[1]=0;
    while(a!=NULL){
        if(a->key[0]!='.'){
            // hehe.. watashi loves myself some quality fix ^^ :3
            size+=strlen(a->value)+1024;
            out=realloc(out,size);
            if(a->key[strlen(a->key)-2]!='\n'){
                strcat(out,a->value);
                strcat(out,",");
            }
            else {
                char* tmp=malloc(strlen(a->key));
                strncpy(tmp,"",strlen(a->key));
                strncpy(tmp,a->key,strlen(a->key)-1);
                strcat(out,tmp);
                strcat(out,",");
                free(tmp);
            }
        }
        a=a->next;
    }
    if(strlen(out)>0)
        out[strlen(out)-1]=0;
    return out;
}

int liked(char* name,char* number){
    FILE* f=fopen("acc.txt","r");
    if(f==NULL)
        return 0;
    char buff[102400];
    strncpy(buff,"",102400);
    char* out=malloc(1);
    size_t sizOut=1;
    out[0]=0;
    while(fgets(buff,102400,f)!=NULL){
        if(strlen(buff)<3)
            continue;
        listNode* line=determineLine(buff);
        if(line!=NULL){
            
            if(strncmp(buff+2,name,strlen(name))==0){
                sizOut+=196+strlen(buff);
                out=realloc(out,sizOut);
                if(readNode(line,number)==NULL)
                    addNode(line,number,number);
                else
                    removeKey(line,number);
                //printf("%s\n",buff);
                int semCount=0;
                for(int i=0;i<strlen(buff);i++){
                    if(buff[i]==';') semCount++;
                    if(semCount==3){
                        strncat(out,buff,i);
                        break;
                    }
                }
                    strcat(out,";");
                    char* tmp=listToString(line);
                    strcat(out,tmp);
                    strcat(out,"\n");
                    free(tmp);
                    freeList(line);
            }
            else {
                sizOut+=strlen(buff);
                out=realloc(out,sizOut);
                strcat(out,buff);
                freeList(line);
            }
        }
    }
    fclose(f);
    f=fopen("acc.txt","w");
    fprintf(f,"%s",out);
    free(out);
    fclose(f);
    return 1;
}
char* getLikesPerComment(char* id){
    FILE* f=fopen("acc.txt","r");
    if(f==NULL)
        return NULL;
    char buff[102400];
    strncpy(buff,"",10240);
    int count=0;
    while(fgets(buff,10240,f)!=NULL){
        listNode* a=determineLine(buff);
        if(a==NULL)
            continue;
        if(!(readNode(a,id)==NULL))
            count++;
        freeList(a);
        strncpy(buff,"",10240);       
    }
    fclose(f);
    char* out=malloc(64);
    strncpy(out,"",64);
    sprintf(out,"%d",count);
    return out;
}
//TODO: fix, acting wierd
int checkLiked(char* name,char* post){
    FILE* f=fopen("acc.txt","r");
    if(f==NULL)
        return 0;
    char buff[102400];
    strncpy(buff,"",102400);
    while(fgets(buff,102400,f)!=NULL){
        if(strlen(buff)>3 && strncmp(name,buff+2,strlen(name)-1)==0){
            listNode* a=determineLine(buff);
            if(readNode(a,post)!=NULL){
                if(readNode(a,post)[0]>='0' && readNode(a,post)[0]<='9'){
                    freeList(a);
                    fclose(f);
                    return 1;
                }
            }
            freeList(a);
        }
    strncpy(buff,"",102400);
    }
    fclose(f);
    return 0;
}
void deleteAllLikesOf(char* post){
    FILE* f=fopen("acc.txt","r");
    if(f==NULL)
        return;
    char buff[102400];
    strncpy(buff,"",102400);
    while(fgets(buff,102400,f)!=NULL){
        listNode* a=determineLine(buff);
        if(readNode(a,post)!=NULL){
            char* cpy=malloc(102400);
            strncpy(cpy,"",102400);
            strcpy(cpy,buff);
            char* tok=strtok(cpy,";");
            if(tok!=NULL){
                tok=strtok(NULL,";");
                if(tok!=NULL){
                    liked(tok,post);
                }
            }
        free(cpy);
        }
        freeList(a);
    }
}