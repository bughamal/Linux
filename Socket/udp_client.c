#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>

#define	SERVER_IP "192.168.11.128"
#define	SERVER_PORT	8000
#define	BUFSIZE	1500
#define	LISTEN	128
#define	IP		16

int main(void)
{
	struct sockaddr_in serveraddr;
	char buf[BUFSIZE];
	bzero(&serveraddr,sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	inet_pton(AF_INET,SERVER_IP,&serveraddr.sin_addr.s_addr);
	serveraddr.sin_port = htons(SERVER_PORT);
	int sockfd = socket(AF_INET,SOCK_DGRAM,0);
	while((fgets(buf,sizeof(buf),stdin))!=NULL)
	{
		//发送请求
		sendto(sockfd,buf,strlen(buf),0,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
		//读取响应
		recvfrom(sockfd,buf,sizeof(buf),0,NULL,NULL);
		//输出响应结果
		write(STDOUT_FILENO,buf,strlen(buf));
		bzero(buf,sizeof(buf));
	}
	close(sockfd);
	return 0;
	
}
