#include <thread_pool.h>
void err_str(const char * str,int err)
{
		perror(str);
		exit(err);
}

pool_t * Pool_create(int max,int min,int que_max)
{
		int err;
		pool_t * p;
		if((p = (pool_t *)malloc(sizeof(pool_t)))==NULL)
				err_str("malloc pool error:",-1);
		p->thread_max = max;
		p->thread_min = min;
		p->thread_alive = 0;
		p->thread_busy = 0;
		p->thread_shutdown = TRUE;
		p->thread_wait = 0;
		p->queue_max = que_max;
		p->queue_cur = 0;
		p->queue_front = 0;
		p->queue_rear = 0;
		if(pthread_cond_init(&p->not_full,NULL)!=0 || pthread_cond_init(&p->not_empty,NULL)!=0 || pthread_mutex_init(&p->lock,NULL)!=0){
				err_str("init cond or mutex error:",-1);
		}
		if((p->tids = (pthread_t *)malloc(sizeof(pthread_t) * max))==NULL)
				err_str("malloc tids error:",-1);
		bzero(p->tids,sizeof(pthread_t) * max);
		if((p->queue_task = (task_t *)malloc(sizeof(task_t) * que_max))==NULL)
				err_str("malloc task queue error:",-1);
		for(int i=0;i<min;i++)
		{
				if((err = pthread_create(&p->tids[i],NULL,Custom,(void *)p))>0)
				{
						printf("create custom error:%s\n",strerror(err));
						return NULL;
				}
				++(p->thread_alive);
		}
		if((err = pthread_create(&p->manager_tid,NULL,Manager,(void*)p))>0)
		{
				printf("create mamager error:%s\n",strerror(err));
				return NULL;
		}
		return p;


}
int Init_socket(void)
{
		int serverfd;
		struct sockaddr_in serveraddr;
		bzero(&serveraddr,sizeof(serveraddr));
		serveraddr.sin_family = AF_INET;
		serveraddr.sin_port = htons(PORT);
		if((inet_pton(AF_INET,IP,&serveraddr.sin_addr.s_addr))==-1)
				err_str("init ip error:",-1);
		if((serverfd = socket(AF_INET,SOCK_STREAM,0))==-1)
				err_str("create socketfd error:",-1);
		if((bind(serverfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr)))==-1)
				err_str("bind error:",-1);
		if((listen(serverfd,LISTEN))==-1)
				err_str("listen error:",-1);
		return serverfd;


}
void * Jobs(void * arg)
{
		struct sockaddr_in clientaddr;
		char ipstr[IPSIZE];
		char buf[BUFSIZE];
		int clientsize = sizeof(clientaddr);
		long long serverfd = (long long)arg;
		int clientfd;
		/*建立链接*/
		pthread_mutex_lock(&alock);	
		if((clientfd = accept(serverfd,(struct sockaddr *)&clientaddr,&clientsize))==-1)
				perror("Custom Thread Accept Error:");	
		pthread_mutex_unlock(&alock);	
		printf("Custom Thread Tid:0x%x\tClient ip:%s\tClient Port:%d\n",(unsigned int)pthread_self(),inet_ntop(AF_INET,&clientaddr.sin_addr.s_addr,ipstr,sizeof(ipstr)),ntohs(clientaddr.sin_port));
		/*对客户端进行处理和响应*/
		while(1)
			sleep(1);
}
int Producer_add(pool_t * p,void *(task)(void*arg),void *arg)
{
		pthread_mutex_lock(&p->lock);
		while(p->queue_cur == p->queue_max && p->thread_shutdown == TRUE){
				pthread_cond_wait(&p->not_full,&p->lock);
		}
		if(p->thread_shutdown == FALSE)
		{
				pthread_mutex_unlock(&p->lock);
				return -1;
		}
		p->queue_task[p->queue_front].task = task;
		p->queue_task[p->queue_front].arg = arg;
		p->queue_front = (p->queue_front + 1) % p->queue_max;
		++(p->queue_cur);
		pthread_cond_signal(&p->not_empty);
		pthread_mutex_unlock(&p->lock);
		return 0;
}

void * Custom(void * arg)
{
		pool_t * p = (pool_t *)arg;
		task_t task;
		while(p->thread_shutdown){
				pthread_mutex_lock(&p->lock);
				while(p->queue_cur == 0 && p->thread_shutdown == TRUE)
				{
						pthread_cond_wait(&p->not_empty,&p->lock);
				}
				if(p->thread_shutdown == FALSE)
				{
						pthread_mutex_unlock(&p->lock);
						pthread_exit(NULL);
				}
				if(p->thread_wait > 0 && p->thread_alive > p->thread_min)
				{
						--(p->thread_wait);
						--(p->thread_alive);
						pthread_mutex_unlock(&p->lock);
						pthread_exit(NULL);
				}
				task.task = p->queue_task[p->queue_rear].task;
				task.arg = p->queue_task[p->queue_rear].arg;
				p->queue_rear= (p->queue_rear + 1) % p->queue_max;
				--(p->queue_cur);
				pthread_cond_signal(&p->not_full);
				++(p->thread_busy);
				pthread_mutex_unlock(&p->lock);
				(*task.task)(task.arg);
				pthread_mutex_lock(&p->lock);
				--(p->thread_busy);
				pthread_mutex_unlock(&p->lock);
		}
}
int if_thread_alive(pthread_t tid)
{
		if((pthread_kill(tid,0))==-1){
				if(errno == ESRCH)
						return FALSE;
		}else
				return TRUE;

}
void * Manager(void * arg)
{
		pool_t * p = (pool_t *)arg;
		int i=0;
		int add=0;
		int alive;
		int cur;
		int busy;
		while(p->thread_shutdown == TRUE){
				pthread_mutex_lock(&p->lock);	
				alive = p->thread_alive;
				busy = p->thread_busy;
				cur = p->queue_cur;
				pthread_mutex_unlock(&p->lock);	
				printf("Manager Thread output Info:\n\n");
				printf("Thread tid:0x%x\tALIVE:%d\tBUSY:%d\tFREE:%d\tB/A:%.2f%%\tA/M:%.2f%%\n\n",(unsigned int)pthread_self(),alive,busy,alive-busy,(float)busy/alive*(float)100,(float)alive/p->thread_max*(float)100);
				/*动态扩容*/
				if((cur > alive - busy || (float)busy / alive * 100 >= (float)70 ) && p->thread_max > alive){
						//创建线程,复用tids数组,>0的要判断出是否存活,为0的直接使用
						for(i;i<p->thread_max,add<DEFCOUNT;i++,add++)
						{
								pthread_mutex_lock(&p->lock);	
								if(p->tids[i]==0 || !if_thread_alive(p->tids[i]))
								{
										pthread_create(&p->tids[i],NULL,Custom,(void*)p);
										++(p->thread_alive);
								}
								pthread_mutex_unlock(&p->lock);	
						}

				}
				/*缩减*/	
				if(busy * 2 < alive - busy && alive > p->thread_min)
				{
						pthread_mutex_lock(&p->lock);
						p->thread_wait = DEFCOUNT;
						pthread_mutex_unlock(&p->lock);
						for(int i = 0;i<DEFCOUNT;i++)
								pthread_cond_signal(&p->not_empty);
				}
				sleep(TIME_OUT);
		}
}
int main(void)
{
		long long serverfd;
		int epfd;
		int ready;
		struct epoll_event eparr[EPOLL];
		struct epoll_event eptmp;
		pool_t * pool;
		serverfd = Init_socket();
		epfd = epoll_create(EPOLL);
		eptmp.events = EPOLLIN;
		eptmp.data.fd = serverfd;
		epoll_ctl(epfd,EPOLL_CTL_ADD,serverfd,&eptmp);
		pool = Pool_create(300,10,100);
		//向任务队列投递任务 ***生产者
		while(1)
		{
				ready = epoll_wait(epfd,eparr,EPOLL,-1);
				while(ready)
				{
						if(eparr[0].data.fd == serverfd)
						{
								Producer_add(pool,Jobs,(void*)serverfd);
						}
						ready--;
				}

		}
		return 0;
}
