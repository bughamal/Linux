#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#define	NUMBER	5000
int COUNTER;
sem_t sem;
void * jobs(void * arg)
{
		int tmp;
		for(int i=0;i<NUMBER;i++)
		{
				sem_wait(&sem);
				tmp = COUNTER;
				printf("++COUNTER:%d\n",++tmp);
				COUNTER=tmp;
				sem_post(&sem);
		}
}


int main(void)
{
		int i=2;
		sem_init(&sem,0,1);
		pthread_t tid[2];
		pthread_create(&tid[0],NULL,jobs,NULL);
		pthread_create(&tid[1],NULL,jobs,NULL);
		while(i--){
				pthread_join(tid[i],NULL);
		}
		sem_destroy(&sem);
		return 0;
}
