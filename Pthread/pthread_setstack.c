#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

void * jobs(void * arg)
{
		//printf("thread tid:0x%x\n",(unsigned int)pthread_self());
		while(1)
				sleep(1);
}

int main(void)
{
		int flags=0;
		int DFL_state,err;
		void * stackaddr;
		size_t stacksize;
		pthread_attr_t attr;
		pthread_t tid;
		//init
		pthread_attr_init(&attr);
		//set detach state
		pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
		//get stacksize and addr
		pthread_attr_getstack(&attr,&stackaddr,&stacksize);
		printf("DFL stackaddr:%p\tstacksize:%d\n",stackaddr,(unsigned int)stacksize);
		//如果用户向自定义线程栈的大小与地址,那就必须人为申请空间,改变一个元素是不成立的
		while(1){
				//malloc and set stackstate
				stacksize = 0x100000;
				if((stackaddr = (void*)malloc(stacksize))==NULL)
				{
						perror("malloc error:");
						exit(0);
				}
				pthread_attr_setstack(&attr,stackaddr,stacksize);
				//create thread
				if((err = pthread_create(&tid,&attr,jobs,NULL))>0)
				{
					printf("pthread_create error:%s\n",strerror(err));
					exit(0);
				}
				printf("FLAGS:%d\n",++flags);
		}
		//destroy
		pthread_attr_destroy(&attr);
		return 0;
}
