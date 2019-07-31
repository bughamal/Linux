#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>


#define	SERVER_IP	"192.168.11.128"
#define	SERVER_PORT	8000
#define	LISTEN		128
#define	BUFSIZE		1500
#define IPSIZE		16
/*错误处理*/
typedef struct
{
		int clientfd;
		struct sockaddr_in clientaddr;
}thread_arg;
void err_str(const char * str,int err)
{
		perror(str);
		exit(err);
}
void * jobs(void * parg)
{
		pthread_detach(pthread_self());
		thread_arg  arg = *(thread_arg*)parg;
		char buf[BUFSIZE];
		char ipstr[IPSIZE];
		bzero(ipstr,IPSIZE);
		int len,i;
		/*对该客户端进行请求与响应*/
		printf("Server tid:0x%x\tClient ip:%s\tport:%d\n",(unsigned int)pthread_self(),inet_ntop(AF_INET,&arg->clientaddr.sin_addr.s_addr,ipstr,16),ntohs(arg->clientaddr.sin_port));
		/*读取clientfd 拿到请求*/
		while((len = read(arg->clientfd,buf,sizeof(buf)))>=0)
		{   
				if(len == 0){
						close(arg->clientfd);
						pthread_exit(NULL);
				}
				i=0;
				while(len > i){ 
						buf[i]=toupper(buf[i]);
						i++;
				}   
				write(arg->clientfd,buf,len);
				bzero(buf,sizeof(buf));
		}

}
int main(void)
{
		struct sockaddr_in serveraddr;
		int serverfd,clientfd,clientsize,err;
		thread_arg tmp;
		pthread_t tid;
		bzero(&serveraddr,sizeof(serveraddr));
		serveraddr.sin_family = AF_INET;
		serveraddr.sin_port = htons(SERVER_PORT);
		inet_pton(AF_INET,SERVER_IP,&serveraddr.sin_addr.s_addr);
		if((serverfd = socket(AF_INET,SOCK_STREAM,0))==-1)
				err_str("Create Socket ERROR",-1);
		if((bind(serverfd,(struct sockaddr *)&serveraddr,sizeof(serveraddr)))==-1)
				err_str("Bind Socket ERROR",-2);
		listen(serverfd,LISTEN);
		printf("THREAD SERVER RUNING......\n");
		while(1)
		{
				clientsize = sizeof(tmp.clientaddr)	;
				if((clientfd = accept(serverfd,(struct sockaddr *)&tmp.clientaddr,&clientsize))==-1)
						err_str("Accept CALL FAILED",-3);
				if(clientfd > 0){
						tmp.clientfd = clientfd;
						if((err = pthread_create(&tid,NULL,jobs,(void*)&tmp))>0)
						{
								printf("Create Pthread Error:%s\n",strerror(err));
								exit(0);
						}

				}
		}
		close(serverfd);
		return 0;
}
