#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/select.h>
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
		int maxfd;
		int clientarr[1024];
		char buf[BUFSIZE];
		int len;
		int clientfd;
		fd_set set,oset;
		bzero(&serveraddr,sizeof(serveraddr));
		serveraddr.sin_family = AF_INET;
		serveraddr.sin_port = htons(SERVERPORT);
		inet_pton(AF_INET,SERVERIP,&serveraddr.sin_addr.s_addr);
		int serverfd = socket(AF_INET,SOCK_STREAM,0);
		bind(serverfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
		listen(serverfd,LISTEN);
		maxfd = serverfd;
		FD_ZERO(&oset);
		FD_SET(serverfd,&oset);
		for(int i=0;i<1024;i++)
				clientarr[i]=-1;
		printf("Select Server Runing....\n");
		while(1)
		{
				set = oset;
				ready = select(maxfd+1,&set,NULL,NULL,NULL);
				if(ready == -1)
						perror("Select Call Failed..\n");
				else{
						while(ready--){
								if(FD_ISSET(serverfd,&set))	
								{
										int clientsize = sizeof(clientaddr);
										//建立新的链接
										clientfd = accept(serverfd,(struct sockaddr*)&clientaddr,&clientsize);
										//判断最大的文件描述符
										if(clientfd > maxfd)
												maxfd = clientfd;
										//将新的描述符加入到监听集合当中
										FD_SET(clientfd,&oset);
										//将新的描述符加入到socket数组中
										for(int i=0;i<1024;i++)
										{
												if(clientarr[i]==-1)
												{
														clientarr[i]=clientfd;
														break;
												}
										}
								}else{
										for(int i=0;i<1024;i++){
												if(clientarr[i]!=-1)
												{
														if(FD_ISSET(clientarr[i],&set))
														{
																if((len = read(clientarr[i],buf,sizeof(buf)))>0)
																{
																		int j=0;
																		while(len > j)
																		{
																				buf[j]=toupper(buf[j]);
																				j++;
																		}
																		write(clientarr[i],buf,len);
																		bzero(buf,sizeof(buf));
																}else if(len == 0){
																		//将该socket从监听集合摘除
																		FD_CLR(clientarr[i],&oset);
																		//要关闭文件描述符
																		close(clientarr[i]);
																		clientarr[i]=-1;
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
