#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>

typedef struct{
		pthread_mutex_t lock;
		int counter;
}date;

/*
   通过更改互斥锁属性,将线程互斥锁改为进程互斥锁,保证资源操作的原子性
 */


int main(void)
{
		date * p;
		pid_t pid;
		pthread_mutexattr_t attr;
		pid = fork();
		if(pid > 0){
				/*创建文件,将文件拓展成指定大小便于映射,对该文件进行共享映射,向内存中写入数据,同步到文件*/
				int fd;
				fd = open("process_mutex",O_RDWR|O_CREAT,0664);
				ftruncate(fd,sizeof(date));
				p = mmap(NULL,sizeof(date),PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
				close(fd);
				/*改变锁属性,通过该属性初始化互斥锁*/
				pthread_mutexattr_init(&attr);
				pthread_mutexattr_setpshared(&attr,PTHREAD_PROCESS_SHARED);
				/*首先初始化锁属性,变为进程锁属性,通过该锁属性初始化互斥锁*/
				pthread_mutex_init(&(p->lock),&attr);
				p->counter=0;
				for(int i=0;i<30;i++){
						pthread_mutex_lock(&(p->lock));
						printf("Parent pid:%d\t++counter:%d\n",getpid(),++(p->counter));
						pthread_mutex_unlock(&(p->lock));
				}
				wait(NULL);
		}else if(pid == 0){
				int fd;
				fd = open("process_mutex",O_RDWR|O_CREAT,0664);
				p = mmap(NULL,sizeof(date),PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
				close(fd);
				for(int i=0;i<30;i++){
						pthread_mutex_lock(&(p->lock));
						printf("Child pid:%d\tcounter+=2:%d\n",getpid(),(p->counter)+=2);
						pthread_mutex_unlock(&(p->lock));
				}
				exit(-1);
		}else{
				perror("fork call failed");
				exit(-1);
		}
		pthread_mutex_destroy(&(p->lock));
		pthread_mutexattr_destroy(&attr);
		munmap(p,sizeof(date));
		return 0;
}
