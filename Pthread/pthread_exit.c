#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>

void * thread_1(void * arg)
{
		printf("thread_1 tid:0x%x\n",(unsigned int)pthread_self());
		return (void*)1;
}
void * thread_2(void * arg)
{
		printf("thread_2 tid:0x%x\n",(unsigned int)pthread_self());
		pthread_exit((void*)2);
}
void * thread_3(void * arg)
{
		while(1){
				//printf("thread_3 tid:0x%x Runing...\n",(unsigned int)pthread_self());
				//sleep(1);
				pthread_testcancel();
		}
}




int main(void)
{
		pthread_t tid;
		void * reval;
		pthread_create(&tid,NULL,thread_1,NULL);
		pthread_join(tid,&reval);
		printf("thread_1 return number:%d\n",(int)reval);
		pthread_create(&tid,NULL,thread_2,NULL);
		pthread_join(tid,&reval);
		printf("thread_2 exit code:%d\n",(int)reval);
		pthread_create(&tid,NULL,thread_3,NULL);
		sleep(3);
		pthread_cancel(tid);
		pthread_join(tid,&reval);
		printf("thread_3 cancel:%d\n",(int)reval);
		return 0;
}
