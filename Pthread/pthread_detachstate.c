#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

void * jobs(void * arg)
{
	printf("thread tid:0x%x\n",(unsigned int)pthread_self());
	while(1)
		sleep(1);
}

int main(void)
{
	int DFL_state,err;
	pthread_attr_t attr;
	pthread_t tid;
	//init
	pthread_attr_init(&attr);
	//get DFL detachstate
	pthread_attr_getdetachstate(&attr,&DFL_state);
	if(DFL_state == PTHREAD_CREATE_JOINABLE)
		printf("DFL state is JOIN.\n");
	else
		printf("DFL state is DETACH.\n");
	//set detach state
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
	printf("set state:\n");
	pthread_attr_getdetachstate(&attr,&DFL_state);
	if(DFL_state == PTHREAD_CREATE_JOINABLE)
		printf("DFL state is JOIN.\n");
	else
		printf("DFL state is DETACH.\n");

	//create thread
	pthread_create(&tid,&attr,jobs,NULL);
	//main thread join 
	if((err = pthread_join(tid,NULL))>0)
	{
		printf("pthread_join call failed:%s\n",strerror(err));
	}
	//destroy
	pthread_attr_destroy(&attr);
	return 0;
}
