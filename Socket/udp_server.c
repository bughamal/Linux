#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>

#define BUFSIZE 1500


int main(void)
{
		struct sockaddr_in serveraddr,clientaddr;
		int sockfd,len,i;
		char ipstr[16];
		char buf[BUFSIZE];
		bzero(ipstr,16);
		bzero(&serveraddr,sizeof(serveraddr));
		serveraddr.sin_family = AF_INET; //协议族指定ipv4
		serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
		serveraddr.sin_port = htons(8000);
		sockfd = socket(AF_INET,SOCK_DGRAM,0);
		if((bind(sockfd,(struct sockaddr *)&serveraddr,sizeof(serveraddr)))==-1)
				perror("Bind Call Failed");
		printf("UDP Server Accepting....\n");
		while(1){
				int clientsize = sizeof(clientaddr);
				if((len = recvfrom(sockfd,buf,sizeof(buf),0,(struct sockaddr*)&clientaddr,&clientsize))>0){
						printf("Client ip:%s\tport:%d\n",inet_ntop(AF_INET,&clientaddr.sin_addr.s_addr,ipstr,16),ntohs(clientaddr.sin_port));
						printf("客户端请求:%s\n",buf);
						i=0;
						while(len > i){
								buf[i]=toupper(buf[i]);
								i++;
						}
						printf("服务器响应:%s\n",buf);
						sendto(sockfd,buf,len,0,(struct sockaddr *)&clientaddr,sizeof(clientaddr));
						bzero(buf,sizeof(buf));
				}
		}
		close(sockfd);
		return 0;
}
