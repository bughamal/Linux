#include <stdio.h>
#include <unistd.h>
#include <pthread.h>


/*
   thread_a 与 thread_b 通过线程同步交叉报数,每人各+5000次
   用户互斥锁保证资源操作原子,条件变量实现两个线程挂起与唤醒交替执行
   a工作条件,奇数工作,偶数挂起
   b工作条件,偶数工作,奇数挂起

   通过条件变量实现挂起唤醒
	pthread_cond_wait(pthread_cond_t * ,pthread_mutex_t *); 挂起当前线程
			wait工作:
					1:第一次执行挂起当前线程并解锁
					2:第二次被唤醒时执行,上锁
	pthread_cond_signal(pthread_cond_t *); 唤醒一个阻塞于某条件变量上的线程
	pthread_cond_broadcast(pthread_cond_t*)唤醒所有阻塞与某条件变量上的线程
 */
int COUNTER=1;
pthread_mutex_t lock=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t a=PTHREAD_COND_INITIALIZER;
pthread_cond_t b=PTHREAD_COND_INITIALIZER;

void * thread_a(void * arg)
{
		while(1){
				pthread_mutex_lock(&lock);
				if(COUNTER % 2 != 0){
						usleep(50000);
						printf("pthread_a tid:0x%x COUNTER:%d\n",(unsigned int)pthread_self(),COUNTER++);

				}
				pthread_mutex_unlock(&lock);
		}
}

void * thread_b(void * arg)
{
		while(1){
				pthread_mutex_lock(&lock);
				if(COUNTER % 2 == 0){
						usleep(500000);
						printf("pthread_b tid:0x%x COUNTER:%d\n",(unsigned int)pthread_self(),COUNTER++);

				}
				pthread_mutex_unlock(&lock);
		}

}

int main(void)
{
		int i=2;
		pthread_t tid[2];
		pthread_create(&tid[0],NULL,thread_a,NULL);
		pthread_create(&tid[1],NULL,thread_b,NULL);
		while(i--)
		{
				pthread_join(tid[i],NULL);
		}
		pthread_mutex_destroy(&lock);
		pthread_cond_destroy(&a);
		pthread_cond_destroy(&b);
		return 0;

}
