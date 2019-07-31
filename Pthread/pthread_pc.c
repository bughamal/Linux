#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>


#define	TRUE	1
#define	FALSE	0
#define	TIMEOUT	10
#define	SIZE	100

/*任务元素*/
typedef struct{
		void *(*job)(void *);
		void * arg;
}task_t;

typedef struct{
		int size;	//当前任务数
		int queue_max;  //最大任务数
		int front;//任务队列头索引
		int rear; //任务队列尾索引
		task_t * tasks;	//任务队列数组首地址
		pthread_t * tids;//所有线程id数组
		pthread_cond_t not_full; //生产者条件变量
		pthread_cond_t not_empty;//消费者条件变量
		pthread_mutex_t lock;	//锁资源,仓库使用互斥
}queue_t;

/*生产者工作*/
int Producer(queue_t * q,void*(*job)(void *),void * arg)
{
		/*
		   1.判断仓库为满,则挂起自己
		   2.生产任务,向任务队列中投递
		   3.每生产完一个任务重新计算索引front
		   4.将当前任务数累加
		   5.唤醒一个或多个消费者,消费任务
		 */
		pthread_mutex_lock(&q->lock);
		if(q->size == q->queue_max)
		{

				printf("Producer Thread Tid:0x%x Waiting.....\n",(unsigned int)pthread_self());
				pthread_cond_wait(&q->not_full,&q->lock);
		}
		q->tasks[q->front].job=job;
		q->tasks[q->front].arg=arg;
		q->size++;
		q->front = (q->size + 1)%q->queue_max;
		printf("Producer Thread Tid:0x%x Add Task.....\n",(unsigned int)pthread_self());
		pthread_cond_signal(&q->not_empty);
		pthread_mutex_unlock(&q->lock);
		return 0;
}
/*消费者工作,是线程的默认工作,所以遵循线程工作接口*/
void * Custom(void * arg)
{
		/*
		   1.判断仓库为空,则挂起自己
		   2.从任务队列尾部拿取一个任务执行
		   3.重新计算索引rear
		   4.将当前任务数-1
		   5.唤醒一个生产者生产任务
		 */
		queue_t * q = (queue_t *)arg;
		task_t task;
		while(1){
				pthread_mutex_lock(&q->lock);
				if(q->size == 0)
				{
						printf("Custom Thread Tid:0x%x Waiting.....\n",(unsigned int)pthread_self());
						pthread_cond_wait(&q->not_empty,&q->lock);
				}
				task.job = q->tasks[q->rear].job;
				task.arg = q->tasks[q->rear].arg;
				q->size--;
				q->rear = (q->size + 1)%q->queue_max;
				pthread_mutex_unlock(&q->lock);
				printf("Custom Thread Tid:0x%x Working.....\n",(unsigned int)pthread_self());
				(*task.job)(task.arg);
				//pthread_cond_signal(&q->not_empty);
		}

}

/*用户自定义任务,功能*/
void * Work(void * arg)
{
		//用户自定义工作.....
		printf("working....\n");
}

/*任务队列初始化功能*,接收任务队列最大值,用户创建的消费者数*/
queue_t * queue_init(int quemax,int threadnum)
{
		queue_t * q = NULL;
		if((q = (queue_t *)malloc(sizeof(queue_t)))==NULL)
		{
				perror("Queue_t Malloc Error");
				return NULL;
		}
		q->size=0;
		q->queue_max=quemax;
		q->front=0;
		q->rear=0;
		if((q->tasks = (task_t *)malloc(sizeof(task_t) * quemax))==NULL)
		{
				perror("Tasks Malloc Error");
				return NULL;
		}
		if((q->tids = (pthread_t *)malloc(sizeof(pthread_t) * threadnum))==NULL)
		{
				perror("Tids Malloc Error");
				return NULL;
		}
		memset(q->tids,0,sizeof(pthread_t)*threadnum);
		if(pthread_cond_init(&(q->not_full),NULL)!=0 || pthread_cond_init(&(q->not_empty),NULL)!=0||pthread_mutex_init(&(q->lock),NULL)!=0)
		{
				printf("Init Cond or Mutex Error\n");
				return NULL;
		}
		for(int i=0;i<threadnum;i++){
				pthread_create(&q->tids[i],NULL,Custom,(void*)q);
		}
		printf("Init Task_Queue Success...\n");
		return q;
}
void queue_destroy(queue_t * q)
{
		/*
		   释放所有申请的资源
		   结束所有的消费者
		 */
}
/*主控制流程包含_START启动信息*/
int main(void)
{
		//创建初始化任务队列
		queue_t * q;
		q = queue_init(50,10);
		//生产者投递任务
		while(1){
				Producer(q,Work,NULL);
				sleep(5);
		}
		//多线程为消费者去消费任务
		//销毁任务队列,结束所有线程
		return 0;
}
