#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include "xssocket_test.h"
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include <time.h>
#define MAX_MSG_LEN 2048 

int main (int argc, char **argv) {
	
	int rc;
	unsigned char rcv_buf[MAX_MSG_LEN];
  	unsigned char send_buf[MAX_MSG_LEN];
	int sendgref;
  	int recvgref;
	int sendsock;
	int recvsock;
	struct sockaddr_xe sendaddr;
	struct sockaddr_xe recvaddr;
	FILE *stream;
	char    buff[MAX_MSG_LEN];
	int  n=0,j=0,i=0,flag=0;	
	char cd[100];
	char	xenstoreBuff[MAX_MSG_LEN];
	int sendid=0;
	int recvid=0;
/*
	while(1)
        {
                memset(xenstoreBuff,0,MAX_MSG_LEN);
                sprintf(xenstoreBuff,"xenstore-ls /local/domain/%d",recvid);
                stream = popen(xenstoreBuff,"r");
                memset(buff,0,MAX_MSG_LEN);
                fread(buff,sizeof(char),MAX_MSG_LEN,stream);
                pclose(stream);
		if(buff[0]!='\0')
                {
                        break;
                }else{
                        recvid++;
                }

        }
    */
        
        memset(buff,0,MAX_MSG_LEN);   
        stream=popen("xenstore-read domid","r");
	fread(buff,sizeof(char),MAX_MSG_LEN,stream);
	recvid = atoi(buff);
	pclose(stream);
        printf("==============recvid=%d===============\n",recvid);

        memset(xenstoreBuff,0,MAX_MSG_LEN);
        sprintf(xenstoreBuff,"xenstore-write /local/domain/0/recvid %d",recvid);
        stream = popen(xenstoreBuff,"r");

	while(1){
                memset(xenstoreBuff,0,MAX_MSG_LEN);
                sprintf(xenstoreBuff,"xenstore-read /local/domain/0/sendid");
                stream = popen(xenstoreBuff,"r");
                memset(buff,0,MAX_MSG_LEN);
                fread(buff,sizeof(char),MAX_MSG_LEN,stream);
                pclose(stream);
		sendid = atoi(buff);
                if(sendid !=0){
                        break;
                }else{
			printf("Waiting sendid\n");
                        sleep(5);
                }
        }
        memset(xenstoreBuff,0,MAX_MSG_LEN);
        sprintf(xenstoreBuff,"xenstore-write /local/domain/0/sendid 0");
        stream = popen(xenstoreBuff,"r");

	




	sendaddr.sxe_family = AF_XEN;
	sendaddr.remote_domid = sendid;
	printf("sendaddr.remote_domid = %d\n", sendaddr.remote_domid);
	sendsock = socket(21, SOCK_STREAM, -1);
	if (sendsock < 0)
    	{
    	printf("socket error!\n");
		return -1;
	}
	sendgref = bind(sendsock, (struct sockaddr * )&sendaddr, sizeof(sendaddr));
	printf("sendgref = %d\n",sendgref);
	
	
	recvaddr.sxe_family = AF_XEN;
	recvaddr.remote_domid = sendid;
	printf("recvaddr.remote_domid = %d\n", recvaddr.remote_domid);
	recvsock = socket(21, SOCK_STREAM, -1);
	if (recvsock < 0)
	{
    	printf("socket error!\n");
		return -1;
	}
	recvgref = bind(recvsock, (struct sockaddr * )&recvaddr, sizeof(recvaddr));
	printf("recvgref = %d\n",recvgref);
	

	sprintf(xenstoreBuff,"xenstore-write /local/domain/0/sendgref %d",sendgref);
	stream = popen(xenstoreBuff,"r");
	sprintf(xenstoreBuff,"xenstore-write /local/domain/0/recvgref %d",recvgref);
	stream = popen(xenstoreBuff,"r");

	while(1){
		memset(rcv_buf,0,MAX_MSG_LEN);
		rc = recv(recvsock, rcv_buf, MAX_MSG_LEN, 0);
		printf("%s", rcv_buf);
		if(rcv_buf[0]=='c'&&rcv_buf[1]=='d'){
                  for(i=2;i<strlen(rcv_buf)-1;i++){
                      while(flag==0&&rcv_buf[i]==' '){
                         i++;
                       }
                      flag=1;
                      cd[j++]=rcv_buf[i];
                  }
                   chdir(cd);
                   send(sendsock,">\n",strlen(">\n"),0);
                   memset(cd,0,100);
		   i=0;
		   j=0;
		   flag=0;
                   continue;
               }
		stream = popen(rcv_buf,"r");
                memset(buff,0,MAX_MSG_LEN);
		fread(buff,sizeof(char),MAX_MSG_LEN,stream);
                if(!strlen(buff)){
                    send(sendsock,"GoOn\n",strlen("GoOn\n"),0);
                    pclose(stream);
                    continue;
                }
                send(sendsock,buff,strlen(buff),0);
               pclose(stream);		
	}
  return 0;
 }

