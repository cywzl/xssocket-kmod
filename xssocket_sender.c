#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include "xssocket_test.h"
#define MAX_MSG_LEN 2048
int
main (int argc, char **argv)
{
	int rc=0;	
	unsigned char rcv_buf[MAX_MSG_LEN];
  	unsigned char send_buf[MAX_MSG_LEN];
	int sendgref=0;
  	int recvgref=0;
	int sendsock;
	int recvsock;
	struct sockaddr_xe sendaddr;
	struct sockaddr_xe recvaddr;
	FILE * stream;
	char	buff[MAX_MSG_LEN];
	char    xenstoreBuff[MAX_MSG_LEN];
	int 	sendid=0;
	int	recvid=0;	
	int 	ptr1,ptr2;
/*
	while(1)
	{
		memset(xenstoreBuff,0,MAX_MSG_LEN);
		sprintf(xenstoreBuff,"xenstore-ls /local/domain/%d",sendid);
		stream = popen(xenstoreBuff,"r");
        	memset(buff,0,MAX_MSG_LEN);
        	fread(buff,sizeof(char),MAX_MSG_LEN,stream);
		pclose(stream);
		if(buff[0]!='\0')
		{
			break;
		}else{
			sendid++;
		}
		
	}
*/
       
       memset(buff,0,MAX_MSG_LEN);
       stream = popen("xenstore-read domid","r");
       fread(buff,sizeof(char),MAX_MSG_LEN,stream);
       sendid = atoi(buff);
       pclose(stream);
       printf("===============sendid = %d=============\n",sendid);

	memset(xenstoreBuff,0,MAX_MSG_LEN);
	sprintf(xenstoreBuff,"xenstore-write /local/domain/0/sendid %d",sendid);
	stream = popen(xenstoreBuff,"r");
	
	while(1){
		memset(xenstoreBuff,0,MAX_MSG_LEN);
        	sprintf(xenstoreBuff,"xenstore-read /local/domain/0/recvid");
        	stream = popen(xenstoreBuff,"r");
        	memset(buff,0,MAX_MSG_LEN);
        	fread(buff,sizeof(char),MAX_MSG_LEN,stream);
        	pclose(stream);
		recvid = atoi(buff);
		if(recvid !=0){
			break;
		}else{
			printf("Waiting recvid\n");
			sleep(5);
		}
	}
	printf("Get recvid\n");
	memset(xenstoreBuff,0,MAX_MSG_LEN);
        sprintf(xenstoreBuff,"xenstore-write /local/domain/0/recvid 0");
        stream = popen(xenstoreBuff,"r");
	pclose(stream);
        sendaddr.sxe_family = AF_XEN;
        sendaddr.remote_domid = recvid;
        printf("sendaddr.remote_domid = %d\n", sendaddr.remote_domid);
        sendsock = socket(21, SOCK_STREAM, -1);
        if (sendsock < 0)
        {
        printf("socket error!\n");
                return -1;
        }
        recvaddr.sxe_family = AF_XEN;
        recvaddr.remote_domid = recvid;
        printf("recvaddr.remote_domid = %d\n", recvaddr.remote_domid);
        recvsock = socket(21, SOCK_STREAM, -1);
        if (recvsock < 0)
        {
        printf("socket error!\n");
                return -1;
        }
	
	while(1){
		memset(xenstoreBuff,0,MAX_MSG_LEN);
        	sprintf(xenstoreBuff,"xenstore-read /local/domain/0/sendgref");
        	stream = popen(xenstoreBuff,"r");
        	memset(buff,0,MAX_MSG_LEN);
        	fread(buff,sizeof(char),MAX_MSG_LEN,stream);
        	pclose(stream);
		sendgref = atoi(buff);
		printf("%d\n",sendgref);
		if(sendgref!=0){
			sendaddr.shared_page_gref = sendgref;
			break;
		}else{
			printf("Waiting sendgref\n");
			sleep(5);
		}
	}
	printf("Get sendgref\n");
	memset(xenstoreBuff,0,MAX_MSG_LEN);
        sprintf(xenstoreBuff,"xenstore-write /local/domain/0/sendgref 0");
        stream = popen(xenstoreBuff,"r");
	pclose(stream);
	while(1){
		memset(xenstoreBuff,0,MAX_MSG_LEN);
	        sprintf(xenstoreBuff,"xenstore-read /local/domain/0/recvgref");
        	stream = popen(xenstoreBuff,"r");
        	memset(buff,0,MAX_MSG_LEN);
        	fread(buff,sizeof(char),MAX_MSG_LEN,stream);
        	pclose(stream);
		recvgref = atoi(buff);
		if(recvgref != 0){
			recvaddr.shared_page_gref = recvgref;
			break;
		}else{
			printf("Waiting recfgref\n");
			sleep(5);
		}
	}
	printf("Get recvgref\n");
	memset(xenstoreBuff,0,MAX_MSG_LEN);
        sprintf(xenstoreBuff,"xenstore-write /local/domain/0/recvgref 0");
        stream = popen(xenstoreBuff,"r");
	pclose(stream);


	rc = connect (sendsock, (struct sockaddr *)&recvaddr, sizeof(recvaddr));
	if (rc < 0) {
	  printf ("connect failed\n");
	  return -1;
	}else{
	printf("sendsock ok\n");
	}

	rc = connect (recvsock, (struct sockaddr *)&sendaddr, sizeof(sendaddr));
	if (rc < 0) {
	  printf ("connect failed\n");
	  return -1;
	}else{
		printf("recvsock ok\n");
	}

	while(1){
		memset( send_buf, 0, MAX_MSG_LEN );
		fgets(send_buf,MAX_MSG_LEN,stdin);
		rc = send(sendsock, send_buf, MAX_MSG_LEN, 0);
                memset(rcv_buf,0,MAX_MSG_LEN);
                rc = recv(recvsock, rcv_buf, MAX_MSG_LEN, 0);
                printf("%s", rcv_buf);
		printf(">");	
	}
	
	  return 0;
}

