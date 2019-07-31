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
		/*1.初始化构造socket结构*/
		struct sockaddr_in serveraddr,clientaddr;
		int sockfd,clientfd,len,i;
		char ipstr[16];
		char buf[BUFSIZE];
		bzero(ipstr,16);
		bzero(&serveraddr,sizeof(serveraddr));
		serveraddr.sin_family = AF_INET; //协议族指定ipv4
		/*通过大小端转换赋值IP和PORT,大小端转换函数
		  1.htonl()小端转大端ip
		  2.htons()小端转大端端口
		  3.ntohl()
		  4.ntohs()
		  5.inet_pton()ip字符串转换为大端ip
		  6.inet_ntop()大端ip转ip字符串
		  7.inet_addr()
		 */
		serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
		serveraddr.sin_port = htons(8000);
		/*2.创建socket*/
		/*tcp 选择SOCK_STREAM流, udp选择SOCK_DGRAM 报*/
		sockfd = socket(AF_INET,SOCK_STREAM,0);
		if((bind(sockfd,(struct sockaddr *)&serveraddr,sizeof(serveraddr)))==-1)
				perror("Bind Call Failed");
		listen(sockfd,128);
		/*3.等待建立链接*/
		printf("Server Accepting....\n");
		while(1){
				int clientsize = sizeof(clientaddr);
				if((clientfd = accept(sockfd,(struct sockaddr*)&clientaddr,&clientsize))>0)
				{
						printf("Client ip:%s\tport:%d\n",inet_ntop(AF_INET,&clientaddr.sin_addr.s_addr,ipstr,16),ntohs(clientaddr.sin_port));
						/*读取clientfd 拿到请求*/
						while((len = read(clientfd,buf,sizeof(buf)))>0)
						{
								printf("客户端请求:%s\n",buf);
								i=0;
								while(len > i){
										buf[i]=toupper(buf[i]);
										i++;
								}
								/*写clientfd,反馈响应*/
								printf("服务器响应:%s\n",buf);
								write(clientfd,buf,len);
								bzero(buf,sizeof(buf));
						}
				}
		}
		close(clientfd);
		close(sockfd);
		return 0;
}
