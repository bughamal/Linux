#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/select.h>
#include <sys/poll.h>
#include <sys/epoll.h>
#include <string.h>

#define	SERVERIP "192.168.11.128" 
#define	SERVERPORT	8000
#define	BUFSIZE	1500
#define	IPSIZE	16
#define	LISTEN	128
#define	EPOLLSIZE	60000


int main(void)
{
		struct sockaddr_in serveraddr,clientaddr;
		int ready=0;
		struct epoll_event eparr[EPOLLSIZE]; //就绪队列  传出
		struct epoll_event eptmp;
		char buf[BUFSIZE];
		int len;
		int clientfd;
		bzero(&serveraddr,sizeof(serveraddr));
		serveraddr.sin_family = AF_INET;
		serveraddr.sin_port = htons(SERVERPORT);
		inet_pton(AF_INET,SERVERIP,&serveraddr.sin_addr.s_addr);
		int serverfd = socket(AF_INET,SOCK_STREAM,0);
		bind(serverfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
		listen(serverfd,LISTEN);
		int epfd = epoll_create(EPOLLSIZE); //创建epoll监听的红黑树
		eptmp.events=EPOLLIN;
		eptmp.data.fd = serverfd;
		epoll_ctl(epfd,EPOLL_CTL_ADD,serverfd,&eptmp);
		printf("EPoll Server Runing....\n");
		while(1)
		{
				ready = epoll_wait(epfd,eparr,EPOLLSIZE,-1);
				if(ready == -1)
						perror("EPoll Call Failed..\n");
				else{
						while(ready){
								ready--;
								if(eparr[ready].data.fd == serverfd)	
								{
										int clientsize = sizeof(clientaddr);
										//建立新的链接
										clientfd = accept(serverfd,(struct sockaddr*)&clientaddr,&clientsize);
										eptmp.events = EPOLLIN;
										eptmp.data.fd = clientfd;
										epoll_ctl(epfd,EPOLL_CTL_ADD,clientfd,&eptmp);
								}else{
										if((len = read(eparr[ready].data.fd,buf,sizeof(buf)))>0)
										{
												int j=0;
												while(len > j)
												{
														buf[j]=toupper(buf[j]);
														j++;
												}
												write(eparr[ready].data.fd,buf,len);
												bzero(buf,sizeof(buf));
										}else if(len == 0){
												epoll_ctl(epfd,EPOLL_CTL_DEL,eparr[ready].data.fd,NULL);
												close(eparr[ready].data.fd);
										}
								}

						}
				}
		}
}
