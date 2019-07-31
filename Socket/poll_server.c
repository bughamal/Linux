#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/select.h>
#include <sys/poll.h>
#include <string.h>

#define	SERVERIP "192.168.11.128" 
#define	SERVERPORT	8000
#define	BUFSIZE	1500
#define	IPSIZE	16
#define	LISTEN	128


int main(void)
{
		struct sockaddr_in serveraddr,clientaddr;
		int ready=0;
		struct pollfd clientarr[4096];
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
		for(int i=0;i<4096;i++)
				clientarr[i].fd=-1;
		clientarr[0].fd=serverfd;
		clientarr[0].events = POLLIN;
		printf("Poll Server Runing....\n");
		while(1)
		{
				ready = poll(clientarr,4096,-1);
				if(ready == -1)
						perror("Poll Call Failed..\n");
				else{
						while(ready--){
								if(clientarr[0].revents & POLLIN)	
								{
										int clientsize = sizeof(clientaddr);
										//建立新的链接
										clientfd = accept(serverfd,(struct sockaddr*)&clientaddr,&clientsize);
										//将新的描述符加入到socket数组中
										for(int i=1;i<4096;i++)
										{
												if(clientarr[i].fd==-1)
												{
														clientarr[i].fd=clientfd;
														clientarr[i].events=POLLIN;
														break;
												}
										}
								}else{
										for(int i=1;i<4096;i++){
												if(clientarr[i].fd!=-1)
												{
														if(clientarr[i].revents & POLLIN)
														{
																if((len = read(clientarr[i].fd,buf,sizeof(buf)))>0)
																{
																		int j=0;
																		while(len > j)
																		{
																				buf[j]=toupper(buf[j]);
																				j++;
																		}
																		write(clientarr[i].fd,buf,len);
																		bzero(buf,sizeof(buf));
																}else if(len == 0){
																		//要关闭文件描述符
																		close(clientarr[i].fd);
																		//将该socket从监听集合摘除
																		clientarr[i].fd=-1;
																}
																break;
														}
												}
										}
								}

						}
				}
		}
}
