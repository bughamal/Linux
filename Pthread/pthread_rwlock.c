#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int counter;

pthread_rwlock_t rwlock;
void * thread_write(void * arg)
{
		while(1){
				pthread_rwlock_wrlock(&rwlock);
				printf("WRITE THREAD:0x%x  ++COUNTER:%d\n",(unsigned int)pthread_self(),++counter);
				pthread_rwlock_unlock(&rwlock);
				usleep(500000);
		}
}
void * thread_read(void * arg)
{
		while(1){
				pthread_rwlock_rdlock(&rwlock);
				printf("READ THREAD:0x%x  COUNTER:%d\n",(unsigned int)pthread_self(),counter);
				pthread_rwlock_unlock(&rwlock);
				usleep(500000);
		}
}



int main(void)
{
		pthread_rwlock_init(&rwlock,NULL);
		pthread_t tid[8];
		for(int i=0;i<3;i++)
		{
				pthread_create(&tid[i],NULL,thread_write,NULL);
		}
		for(int i=3;i<8;i++)
		{
				pthread_create(&tid[i],NULL,thread_read,NULL);
		}
		for(int i=0;i<8;i++){
				pthread_join(tid[i],NULL);
		}
		pthread_rwlock_destroy(&rwlock);
		return 0;
}
