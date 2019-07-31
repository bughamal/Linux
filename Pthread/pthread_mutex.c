#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define	NUMBER	5000
int COUNTER;
/*初始化锁方式
	1.静态初始化:PTHREAD_MUTEX_INITIALIZER  
	2.动态初始化:pthread_mutex_init()函数 动态初始化可以改变锁属性: 例如更改线程锁或进程锁
	3.释放锁资源:pthread_mutex_destroy()函数
	4.上锁:pthread_mutex_lock()
	5.解锁:pthread_mutex_unlock()
*/
pthread_mutex_t lock=PTHREAD_MUTEX_INITIALIZER;
void * jobs(void * arg)
{
		int tmp;
		for(int i=0;i<NUMBER;i++)
		{
				pthread_mutex_lock(&lock);
				tmp = COUNTER;
				printf("++COUNTER:%d\n",++tmp);
				COUNTER=tmp;
				pthread_mutex_unlock(&lock);
		}
}


int main(void)
{
		int i=2;
		pthread_t tid[2];
		pthread_create(&tid[0],NULL,jobs,NULL);
		pthread_create(&tid[1],NULL,jobs,NULL);
		while(i--){
				pthread_join(tid[i],NULL);
		}
		pthread_mutex_destroy(&lock);
		return 0;
}
