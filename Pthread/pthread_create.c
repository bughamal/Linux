#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>

void * thread_jobs(void * arg)
{
		//printf("thread tid:0x%x\n",(unsigned int)pthread_self());
		pthread_detach(pthread_self());
		while(1)
				sleep(1);
}


int main(void)
{
		/*创建线程*/
		pthread_t tid;
		int flags=0;
		int err;
		printf("main tid:0x%x\n",(unsigned int)pthread_self());
		/*线程工作接口  : void *(*thread_job)(void *)  */
		if((err = pthread_create(&tid,NULL,thread_jobs,NULL))>0)
		{
				printf("pthread_create error:%s\n",strerror(err));
				exit(-1);
		}
		sleep(1);
		if((err = pthread_join(tid,NULL))>0){
				printf("pthread_join error:%s\n",strerror(err));
				exit(-1);
		}
		return 0;
}
