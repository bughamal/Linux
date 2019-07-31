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
	/*如果不需要bind,那么sockaddr也就没有意义了*/
	/*客户端链接服务器时会自动将自己的信息传递给服务器*/
	struct sockaddr_in serveraddr;
	char buf[BUFSIZE];
	bzero(&serveraddr,sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	/*通过inet_pton将字符串ip转换为大端32位数*/
	inet_pton(AF_INET,SERVER_IP,&serveraddr.sin_addr.s_addr);
	serveraddr.sin_port = htons(SERVER_PORT);
	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	/*listen的参数,128: 刚刚三次握手成功队列,等待三次握手队列*/
	/*请求链接*/
	connect(sockfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
	while((fgets(buf,sizeof(buf),stdin))!=NULL)
	{
		//发送请求
		write(sockfd,buf,strlen(buf));
		//读取响应
		read(sockfd,buf,sizeof(buf));
		//输出响应结果
		write(STDOUT_FILENO,buf,strlen(buf));
		bzero(buf,sizeof(buf));
	}
	close(sockfd);
	return 0;
	
}
