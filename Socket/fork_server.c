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
/*捕捉函数回收,一次尽可能回收多个子进程,因信号不支持排队*/
void sig_wait(int n)
{
		pid_t wpid;
		int status;
		while((wpid = waitpid(-1,&status,WNOHANG))>0)
		{
				printf("thread tid:0x%x\t,wait child pid:%d\n",(unsigned int)pthread_self(),wpid);
		}
}
/*错误处理*/
void err_str(const char * str,int err)
{
		perror(str);
		exit(err);
}
/*线程工作,设置sigchld的信号捕捉,并解除对sigchld信号的屏蔽*/
void * pwait(void * arg)
{
		struct sigaction act,oact;
		act.sa_handler =sig_wait; 
		act.sa_flags = 0;
		sigemptyset(&act.sa_mask);
		sigaction(SIGCHLD,&act,&oact);
		sigset_t set;
		sigemptyset(&set);
		sigprocmask(SIG_SETMASK,&set,NULL);

		while(1)
				sleep(5);
}
/*父进程负责accpet,为每一个客户端创建子进程,父进程中对sigchld信号屏蔽,子进程负责对客户端进行响应,子进程要检测出客户单异常终止,并结束自身*/
int main(void)
{
		struct sockaddr_in serveraddr, clientaddr;
		int serverfd,clientfd,clientsize,err;
		pid_t pid;
		pthread_t tid;
		sigset_t set,oset;
		sigemptyset(&set);
		sigaddset(&set,SIGCHLD);
		sigprocmask(SIG_BLOCK,&set,&oset);
		if((err = pthread_create(&tid,NULL,pwait,NULL))>0)
		{
				printf("Create Pthread Error:%s\n",strerror(err));
				exit(0);
		}
		bzero(&serveraddr,sizeof(serveraddr));
		serveraddr.sin_family = AF_INET;
		serveraddr.sin_port = htons(SERVER_PORT);
		inet_pton(AF_INET,SERVER_IP,&serveraddr.sin_addr.s_addr);
		if((serverfd = socket(AF_INET,SOCK_STREAM,0))==-1)
				err_str("Create Socket ERROR",-1);
		if((bind(serverfd,(struct sockaddr *)&serveraddr,sizeof(serveraddr)))==-1)
				err_str("Bind Socket ERROR",-2);
		listen(serverfd,LISTEN);
		printf("Fork SERVER RUNING......\n");
		while(1)
		{
				clientsize = sizeof(clientaddr)	;
				if((clientfd = accept(serverfd,(struct sockaddr *)&clientaddr,&clientsize))==-1)
						err_str("Accept CALL FAILED",-3);
				if(clientfd > 0){
						pid = fork();
						if(pid == 0){
								/*要让子进程检测出客户端的异常,从而终止子进程*/
								char buf[BUFSIZE];
								char ipstr[IPSIZE];
								bzero(ipstr,IPSIZE);
								int len,i;
								/*对该客户端进行请求与响应*/
								printf("Server pid:%d\tClient ip:%s\tport:%d\n",getpid(),inet_ntop(AF_INET,&clientaddr.sin_addr.s_addr,ipstr,16),ntohs(clientaddr.sin_port));
								/*读取clientfd 拿到请求*/
								while((len = read(clientfd,buf,sizeof(buf)))>=0)
								{   
										if(len == 0){
												close(clientfd);
												exit(0);
										}
										i=0;
										while(len > i){ 
												buf[i]=toupper(buf[i]);
												i++;
										}   
										write(clientfd,buf,len);
										bzero(buf,sizeof(buf));
								}
						}else if(pid < 0){
								err_str("Fork CALL FAILED",-4);
						}
				}
		}
		close(serverfd);
		return 0;
}
