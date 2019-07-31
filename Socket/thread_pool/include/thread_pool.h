#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <ctype.h>
#include <errno.h>
#include <pthread.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>


#define	TRUE	1
#define FALSE 0
#define	BUFSIZE	1500
#define	LISTEN	128
#define	IPSIZE	16
#define	IP	"192.168.11.128"
#define	PORT	8000
#define	EPOLL	1
#define	TIME_OUT	3
#define	DEFCOUNT 10

pthread_mutex_t alock = PTHREAD_MUTEX_INITIALIZER;

typedef struct
{
	void * (*task)(void*);
	void * arg;
}task_t;
typedef struct 
{
	int thread_max;
	int thread_min;
	int thread_alive;
	int thread_busy;
	int thread_shutdown;
	int thread_wait;
	int queue_max;
	int queue_cur;
	int queue_front;
	int queue_rear;
	pthread_cond_t not_full;
	pthread_cond_t not_empty;
	pthread_mutex_t lock;
	task_t * queue_task;
	pthread_t * tids;
	pthread_t manager_tid;	
	
}pool_t;
void err_str(const char * ,int );
pool_t * Pool_create(int ,int ,int);
int Init_socket(void);
int Pool_destroy(pool_t *); 
int Producer_add(pool_t *,void *(*)(void*),void *);
void * Custom(void *);
void * Manager(void *);
void * Jobs(void * );
