#include  <stdio.h>
#include  <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include  <sys/types.h>
#include <stdlib.h>
#include<sys/ioctl.h>
#include <sys/wait.h>

#define sys_barrier_init 359 //syscall number for barrier init 
#define sys_barrier_wait 360 //syscall number for barrier wait

#define   MAX_COUNT1  5
#define   MAX_COUNT2  20
#define   BUF_SIZE   100


void  ChildProcess(char [], char []);    /* child process prototype  */
void *thread_function(void *);
void *thread_function1(void *);
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex3 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex4 = PTHREAD_MUTEX_INITIALIZER;
int  counter1 = 0,counter2 = 0,p=20,q=20;//p and q are for sychronization loops,which is modified to 20 instead of 100
int id1,id2;
long int s_time;
pthread_t tid[MAX_COUNT1];
pthread_t tid1[MAX_COUNT2];

struct barrier
{
	unsigned int count;
	unsigned int barrier_id;
	int timeout;
	int curr;
	int t_flag;
};

void  main()
{
     pid_t   pid1, pid2, pid;
     
     int     status;
 
     char    buf[BUF_SIZE];
	struct barrier *t1 = (struct barrier*)malloc(sizeof(struct barrier));
	struct barrier *t2 = (struct barrier*)malloc(sizeof(struct barrier));
	printf("enter the average sleep time for the threads in micro seconds");
	scanf("%ld",&s_time);
	s_time=s_time*1000000; //converting to micro seconds
	t1->count=5;
	t1->barrier_id=0;
	t1->timeout=50000;
	t1->curr=0;
	t1->t_flag=0;
	printf("calling first write\n");
	id1=syscall(sys_barrier_init,t1->count,t1->barrier_id,t1->timeout);
	if (id1 == -1) {
        	fprintf(stderr, "Error calling syscall: %s\n", strerror(errno));
        	exit(0);
    	}
	t1->barrier_id=id1;
	printf("return first write\n");
	printf("the id is %d\n",id1);

	t2->count=10;
	t2->barrier_id=0;
	t2->timeout=60000;
	t2->curr=0;
	t2->t_flag=0;
	printf("calling first write\n");
	id2=syscall(sys_barrier_init,t1->count,t1->barrier_id,t1->timeout);
	if (id2 == -1) {
        	fprintf(stderr, "Error calling syscall: %s\n", strerror(errno));
        	exit(0);
    	}
	t2->barrier_id=id2;
	printf("return second write\n");
	printf("the id is %d\n",id2);
	
	printf("%d\t%d\t%d\t%d\n%d\t%d\t%d\t%d\n",t1->count,t1->barrier_id,t1->timeout,t1->curr,t2->count,t2->barrier_id,t2->timeout,t2->curr);
	/*child process 1 */     
	printf("*** Parent is about to fork process 1 ***\n");
     if ((pid1 = fork()) < 0) {
          printf("Failed to fork process 1\n");
          exit(1);
     }
     else if (pid1 == 0) 
          ChildProcess("First", "   ");
	/*child process 2 */
     printf("*** Parent is about to fork process 2 ***\n");
     if ((pid2 = fork()) < 0) {
          printf("Failed to fork process 2\n");
          exit(1);
     }
     else if (pid2 == 0) 
          ChildProcess("Second", "      ");
	
     sprintf(buf, "*** Parent enters waiting status .....\n");
     write(1, buf, strlen(buf));
     pid = wait(&status);
     sprintf(buf, "*** Parent detects process %d was done ***\n", pid);
     write(1, buf, strlen(buf));
     pid = wait(&status);
     printf("*** Parent detects process %d is done ***\n", pid);
     printf("*** Parent exits ***\n");
    
     exit(0);
}

void  ChildProcess(char *number, char *space)
{
     pid_t  pid;
     int    i,j;
     char   buf[BUF_SIZE];

     pid = getpid();
     sprintf(buf, "%s%s child process starts (pid = %d)\n", 
             space, number, pid);
     write(1, buf, strlen(buf));
     for (i = 0; i < MAX_COUNT1; i++) {
	  pthread_create( &tid[i], NULL, thread_function, NULL );
         
     }
 	
	for (j = 0; j < MAX_COUNT2; j++) {
	 pthread_create( &tid1[j], NULL, thread_function1, NULL );
         
     }
   
     write(1, buf, strlen(buf));   
     for(i=0; i < MAX_COUNT1; i++)
	   {
	      pthread_join( tid[i], NULL);
	   } 
	for(j=0; j < MAX_COUNT2; j++)
	   {
	      pthread_join( tid1[j], NULL);
	   }  
      printf("Final counter value: %d\n", counter1+counter2);
	sprintf(buf, "%s%s child (pid = %d) is about to exit\n", space, number, pid);
     exit(0);
}

void *thread_function(void *dummyPtr)
	{
	  int ret,n;
	while(p>0)
	{
	  pthread_mutex_lock(&mutex1);
	 printf("Thread number %ld\n", pthread_self());
	   counter1++;
	//sleep for random time
	n = rand() % s_time + 1;
	usleep(n); 
	printf(" counter value1: %d\n", counter1);
	pthread_mutex_unlock(&mutex1);
	//syscall for barrier wait on barrier 1  
	ret=syscall(sys_barrier_wait,id1); 
	pthread_mutex_lock(&mutex3);
	p--;
	pthread_mutex_unlock(&mutex3);
	}
	   
	  
	}
void *thread_function1(void *dummyPtr)
	{
	  int ret,n;
	
	while(q>0)
	
	{
	  pthread_mutex_lock( &mutex2 );
	 printf("Thread number %ld\n", pthread_self());
	   counter2++;
	//sleep for random time
	n = rand() % s_time + 1;
	usleep(n); 
	printf(" counter value2: %d\n", counter2);
	pthread_mutex_unlock( &mutex2 );
	//syscall for barrier wait on barrier2	
	ret=syscall(sys_barrier_wait,id2); 
	pthread_mutex_lock( &mutex4);
	q--;
	pthread_mutex_unlock( &mutex4 );
	}   
	  
	}

