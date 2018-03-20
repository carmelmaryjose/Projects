#define _GNU_SOURCE 
#include<stdio.h>
#include <sched.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <signal.h>
#include <sys/time.h>
#include <linux/input.h>

/*Global variables*/
pthread_mutex_t lock[9];
pthread_barrier_t barrier;
pthread_mutexattr_t attr;
timer_t timer1;
static int return_value;
struct input_event ie;
static int num,flag1=0;
static int p_count, ap_count;
static pthread_mutex_t left_click_mutex = PTHREAD_MUTEX_INITIALIZER, right_click_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t left_click_cond = PTHREAD_COND_INITIALIZER, right_click_cond = PTHREAD_COND_INITIALIZER;



/*Timer handler for all tasks*/
void timer_handler (int signum)
{
	printf ("Timer has expired");
	flag1=1;
}

/* Structure to access and store line 1 of input file */
typedef struct line_one
{
	int line,t_period;
}line1;

/* Structure to access and store remaining lines of input file */
typedef struct line
{
	char type;
	int priority;
	int period;
	int loop;
	struct l_lock
	{
	char ll[1];
	int busy;
	struct l_lock *next;
	}*head,*first,*temp;
}task;

/*Funtion to perform busy loop*/
void loop(int x)
{
	int k=0, j=0;
	printf("Starting to iterate with final value %d\n",x); 
	for (k = 0; k < x; k++)
	{
		j = j + k;
	}
	printf("Result is %d after %d iterations\n",j,k);
}

/*Function to add two timespecs*/
struct  timespec  tsAdd (struct  timespec  t1,struct  timespec  t2)
{    
	
	struct  timespec  result ;
    	result.tv_sec = t1.tv_sec + t2.tv_sec ;
    	result.tv_nsec = t1.tv_nsec + t2.tv_nsec ;
   	if (result.tv_nsec >= 1000000000L) 
		{		
        	result.tv_sec++ ;  
		result.tv_nsec = result.tv_nsec - 1000000000L ;
    		}
    return (result) ;
}

/*Function to compare two timespecs*/
int  tsCompare (struct  timespec  t1,struct  timespec  t2)
{	/* Less than */
	if (t1.tv_sec < t2.tv_sec)
	        return (-1) ;	
	/* Greater than */			
    	else if (t1.tv_sec > t2.tv_sec)
        	return (1) ;
	/* Less than */				
    	else if (t1.tv_nsec < t2.tv_nsec)
        	return (-1) ;
	/* Greater than */				
    	else if (t1.tv_nsec > t2.tv_nsec)
        	return (1) ;	
	/* Equal to */			
    	else
        	return (0) ;				
}

/*Function to subtract two timespecs*/
struct  timespec  tsSubtract (struct  timespec  t1,struct  timespec  t2)
{   
	struct  timespec  result ;
    	if ((t1.tv_sec < t2.tv_sec) || ((t1.tv_sec == t2.tv_sec) && (t1.tv_nsec <= t2.tv_nsec))) 
		{		
		/* t1 <= t2 */
        	result.tv_sec = result.tv_nsec = 0 ;
    		} 
	else 
		{						
		/* t1 > t2 */
	        result.tv_sec = t1.tv_sec - t2.tv_sec ;
	        if (t1.tv_nsec < t2.tv_nsec) 
			{
		        result.tv_nsec = t1.tv_nsec + 1000000000L - t2.tv_nsec ;
			/* Borrow a second. */
		        result.tv_sec-- ;				
        		} 
		else 
			{
	        	result.tv_nsec = t1.tv_nsec - t2.tv_nsec ;
	        	}
		}

	return (result) ;
}

/*Thread function for periodic function*/
void* periodic_function(void *ptr)
{	
	/* Barrier */
	printf("\nPeriodic thread waiting for barrier");
	pthread_barrier_wait(&barrier);
	
	int flag=0;
	int ret; 
	struct timespec now,diff,next,period;

	/*Enable pthread cancel*/
	ret = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
        if (ret != 0)
        	printf("\npthread_setcancelstate failure");

	/*Local Structure*/
	task *arg; 
	arg = (task*) ptr; 
	period.tv_sec=0;
	period.tv_nsec=arg->period * 1000000;

	printf("\n------------------------------------------------------------------");
	printf("\nStarting periodic function with priority %d period %d sequence %d ",arg->priority,arg->period,arg->loop);
	
	/*change pointer to the first location of linked list */
	arg->temp = arg->first;	
	
	/*loop to print out values of linked list*/
	while (arg->temp != 0)	
		{
   		printf("%s %d ", arg->temp->ll,arg->temp->busy);
	        arg->temp = arg->temp->next;
		}

	while(1)
	{

	flag=1;
	do
	{
	printf("\nStarting period of thread with priority %d period %d and loop %d\n", arg->priority, arg->period, arg->loop);
	
	/*Get current time and calculate time at end of period*/
	clock_gettime(CLOCK_MONOTONIC, &now);
	next=tsAdd(now,period);

	/*loop */
	loop(arg->loop);

	/*change pointer to the first location of linked list */
	arg->temp = arg->first;	

	/*loop through values of linked list*/
	while (arg->temp != 0)	
		{
		/*if else structure for locking, unlocking and iterations*/
   		printf("%s %d ", arg->temp->ll,arg->temp->busy);
		if(!strcmp(arg->temp->ll,"L0"))
			{printf("Locking Mutex L0\n");
			pthread_mutex_lock(&lock[0]);
			}
		else if(!strcmp(arg->temp->ll,"L1"))
			{printf("Locking Mutex L1\n");
			pthread_mutex_lock(&lock[1]);
			}
		else if(!strcmp(arg->temp->ll,"L2"))
			{printf("Locking Mutex L2\n");
			pthread_mutex_lock(&lock[2]);
			}
		else if(!strcmp(arg->temp->ll,"L3"))
			{printf("Locking Mutex L3\n");
			pthread_mutex_lock(&lock[3]);
			}
		else if(!strcmp(arg->temp->ll,"L4"))
			{printf("Locking Mutex L4\n");
			pthread_mutex_lock(&lock[4]);
			}
		else if(!strcmp(arg->temp->ll,"L5"))
			{printf("Locking Mutex L5\n");
			pthread_mutex_lock(&lock[5]);
			}
		else if(!strcmp(arg->temp->ll,"L6"))
			{printf("Locking Mutex L6\n");
			pthread_mutex_lock(&lock[6]);
			}
		else if(!strcmp(arg->temp->ll,"L7"))
			{printf("Locking Mutex L7\n");
			pthread_mutex_lock(&lock[1]);
			}
		else if(!strcmp(arg->temp->ll,"L8"))
			{printf("Locking Mutex\n");
			pthread_mutex_lock(&lock[8]);
			}
		else if(!strcmp(arg->temp->ll,"L9"))
			{printf("Locking Mutex\n");
			pthread_mutex_lock(&lock[9]);
			}
		else if(!strcmp(arg->temp->ll,"U0"))
			{printf("Unlocking Mutex\n");
			pthread_mutex_unlock(&lock[0]);
			}
		else if(!strcmp(arg->temp->ll,"U1"))
			{printf("Unlocking Mutex U1\n");
			pthread_mutex_unlock(&lock[1]);
			}
		else if(!strcmp(arg->temp->ll,"U2"))
			{printf("Unlocking Mutex U2\n");
			pthread_mutex_unlock(&lock[2]);
			}
		else if(!strcmp(arg->temp->ll,"U3"))
			{printf("Unlocking Mutex U3\n");
			pthread_mutex_unlock(&lock[3]);
			}
		else if(!strcmp(arg->temp->ll,"U4"))
			{printf("Unlocking Mutex U4\n");
			pthread_mutex_unlock(&lock[4]);
			}
		else if(!strcmp(arg->temp->ll,"U5"))
			{printf("Unlocking Mutex U5\n");
			pthread_mutex_unlock(&lock[5]);
			}
		else if(!strcmp(arg->temp->ll,"U6"))
			{printf("Unlocking Mutex U6\n");
			pthread_mutex_unlock(&lock[6]);
			}
		else if(!strcmp(arg->temp->ll,"U7"))
			{printf("Unlocking Mutex U7\n");
			pthread_mutex_unlock(&lock[7]);
			}
		else if(!strcmp(arg->temp->ll,"U8"))
			{printf("Unlocking Mutex U8\n");
			pthread_mutex_unlock(&lock[8]);
			}
		else if(!strcmp(arg->temp->ll,"U9"))
			{printf("Unlocking Mutex U9\n");
			pthread_mutex_unlock(&lock[9]);
			}
		/*Loop */
		loop(arg->temp->busy);
	
		/*Point to next linked list*/
	        arg->temp = arg->temp->next;
		}
	/*Get current time*/
	clock_gettime(CLOCK_MONOTONIC, &now);

	
	if (tsCompare(next,now)>0)
		{
		/*Sleep till end of period */	
		flag=1;
		diff=tsSubtract(next,now);
		printf("Ending period of thread with priority %d and period %d. Thread put to sleep for", arg->priority, arg->period);
		printf("\n%lld.%.9lds", (long long)diff.tv_sec, diff.tv_nsec);
		printf("\n------------------------------------------------------------------\n\n");
		nanosleep(&diff,NULL);
		}
	else 	
		/*Start next iteration*/
		flag=0;
	}while(flag==1);
	}

	/*free memory*/
	free(arg);
}

/*Thread function for mouse click*/
void* mouse_click_function(void *ptr)
{
	int fd, bytes,ret;
	ret = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
        if (ret != 0)
        	printf("\npthread_setcancelstate failure");
    
	const char *pDevice = "/dev/input/event2";

	/*Open mouse file*/
	fd = open(pDevice, O_RDWR);
	if(fd == -1)
		printf("ERROR Opening %s\n", pDevice);
   	while(1)
   	{
        /*Read the input obtained from the mouse device*/
        bytes = read(fd, &ie, sizeof(struct input_event));
        if(bytes > 0)
        {
        	/*Compare the value read from the device for manipulation*/
            if((ie.code == BTN_LEFT) && (ie.value == 0))
            {
            	printf("\n------------------------------------------------------------------");
            	printf("\nLeft click detecteed\n");
            	/*Send out the signal for the condition variables to all threads of event0 waiting for it*/
            	pthread_cond_broadcast(&left_click_cond);
            }
            if((ie.code == BTN_RIGHT) && (ie.value == 0))
            {	
            	printf("\n------------------------------------------------------------------");
            	printf("\nRight click detected\n");
            	/*Send out the signal for the condition variables to all threads of event1 waiting for it*/            	
            	pthread_cond_broadcast(&right_click_cond);
            }
        }   
    }
	return 0;
}

/*Thread function for aperiodic thread*/
void* aperiodic_function(void *ptr)
{	
	/*Barrier programmed to wait for all threads to initlaise*/ 
	printf("\nAperiodic thread waiting for barrier");
	pthread_barrier_wait(&barrier);
	return_value = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	if (return_value != 0)
    	printf("\npthread_setcancelstate error");
	task *arg; 
	/*Store the argument passed to the thread in a local variable*/
	arg = (task*) ptr; 
	{
		if(arg->period == 0)
		{	
			while(1)
			{
				pthread_mutex_lock(&left_click_mutex);
				/*Condition variable waiting for signal from mouse click function for event0*/
				pthread_cond_wait(&left_click_cond, &left_click_mutex);
				printf("Starting execution of aperiodic thread with priority %d\n", arg->priority);
				/*Call loop to perform iterations*/
				loop(arg->loop);
				printf("Ending execution of aperiodic thread with priority %d\n", arg->priority);
				printf("------------------------------------------------------------------\n");
				pthread_mutex_unlock(&left_click_mutex);
			}
		}
		if(arg->period == 1)
		{
			while(1)
			{
				pthread_mutex_lock(&right_click_mutex);			
				pthread_cond_wait(&right_click_cond, &right_click_mutex);
				/*Condition variable waiting for signal from mouse click for event1*/
				printf("Starting execution of aperiodic thread with priority %d\n", arg->priority);
				/*Call loop to perform iterations*/
				loop(arg->loop);
				printf("Ending execution of aperiodic thread with priority %d\n", arg->priority);
				printf("------------------------------------------------------------------\n");
				pthread_mutex_unlock(&right_click_mutex);
			}
		}
		
	}
	return 0;
}

/*Main function*/
int main(int argc,char* argv[])
{	
	/*setting cpu affinity to single CPU*/
	cpu_set_t  mask;
	CPU_ZERO(&mask);
	CPU_SET(0, &mask);
	if (sched_setaffinity(0, sizeof(mask), &mask) < 0) 
	{
   	perror("sched_setaffinity");
	}
	/*local variable initialization*/	
	char c;
	int i,sec,msec;

	struct sched_param para;
	struct sigaction sa;
 	struct itimerval timer;
	
	FILE *ptr_file;														
	line1 *l1 = NULL;
	printf("%c\n", *argv[1]);
	if(*argv[1] == '0')
	{
		/*Initialize the mutexes*/
		printf("You've chosen to run with normal mutexes\n");
		for(i=0;i<10;i++)
		{
			if (pthread_mutex_init(&lock[i], NULL) != 0)
    		{
        		printf("\n mutex init has failed\n");
    		}
		}
	}
	else if(*argv[1] == '1')
	{
		printf("You've chosen to run with PI-enabled mutexes\n");
		/*Setting priority inversion to pthreads*/
		return_value=pthread_mutexattr_init(&attr);
		if(return_value!=0)
			printf("\npthread mutex attr init failure"); 				
		return_value=pthread_mutexattr_setprotocol(&attr,PTHREAD_PRIO_INHERIT);
		if(return_value!=0)
			printf("\npthread protocol failure");

		/*Initialize the mutexes*/
		for(i=0;i<10;i++)
			{
		 		if (pthread_mutex_init(&lock[i], &attr) != 0)
    			{
        			printf("\n mutex init has failed\n");
    			}
			}
		}
	else 
	{
		printf("Invalid argument\n");
		exit(0);
	}

	/*Open file*/
	ptr_file =fopen("data.txt", "r");	 
	if (ptr_file != NULL)			
		{
		/*allocate memory for line1*/
		l1 = malloc(sizeof(line1));

		/*read from file to structure*/	
		fscanf(ptr_file,"%d %d",&l1->line,&l1->t_period);			
		printf("******************************************************************\n");
		printf("L1: %d %d",l1->line,l1->t_period);
		num=l1->line;

		/*Initialize barrier with count read from file*/
		return_value = pthread_barrier_init(&barrier, NULL, num);

		/*Pthreads and Structure for tasksets*/
		task *l2[num];			
		pthread_t periodic_thread[num];
		pthread_t aperiodic_thread[num], mouse_click_thread;

		/*Next line of file*/
		fseek(ptr_file, +1L, SEEK_CUR);	

		/*Assign priority for mouseclick thread*/	
		para.sched_priority = 99;

		/*Create mouseclick thread*/
		return_value = pthread_create(&mouse_click_thread, NULL, &mouse_click_function, NULL);
		if(return_value != 0)
		{
			printf("mouse_click_thread failed\n");
		}

		/*Set scheduling parameter*/
		return_value = pthread_setschedparam(mouse_click_thread, SCHED_FIFO, &para);
		if(return_value != 0)
		{
			printf("mouse_click_thread priority set failed\n");
		}

		/*Reading taskset parameters from file to structure*/		
		for(i=0;i<num;i++)
			{
			/*allocating memory*/
			l2[i] = malloc(sizeof(task));	

			/*read from file to structure*/ 
			fscanf(ptr_file,"%c %d %d %d",&l2[i]->type,&l2[i]->priority,&l2[i]->period,&l2[i]->loop);	 
			if(l2[i]->type =='A')
				{	
				printf("\nL%d: %c %d %d %d ",i+2,l2[i]->type,l2[i]->priority,l2[i]->period,l2[i]->loop);
				fseek(ptr_file, +1L, SEEK_CUR);
				continue;
				}

			/*Check if next character is end of line*/
			fscanf(ptr_file,"%c",&c);	
			LOOP:	if(c=='\n'||feof(ptr_file))	
				{
				/*end of line*/			
				fseek(ptr_file,-1L, SEEK_CUR);	
				goto NEXT;	
				}
				else
				{
				/*not end of line*/
				fseek(ptr_file,-1L, SEEK_CUR);	

				/*allocate memory for linked list*/
				l2[i]->head=malloc(sizeof(l2[i]->head));

				/*read from file to linked list pointer head*/	
				fscanf(ptr_file,"%s %d",l2[i]->head->ll,&l2[i]->head->busy);	
					if (l2[i]->first != 0)	
					{
					/*pointer to first location of linked list is not null*/
            				l2[i]->temp->next = l2[i]->head;	
            				l2[i]->temp = l2[i]->head;		
        				}
        				else
        				{
					/*First location of linked list is empty*/
            				l2[i]->first = l2[i]->temp = l2[i]->head;
        				}
				fscanf(ptr_file,"%c",&c);	

				/*loop to read next values to linked list*/
				goto LOOP;	
				}	
			NEXT:	
				/*assign next values of linked list as null*/
				l2[i]->temp->next = 0;	

				/*change pointer to the first location of linked list*/
				l2[i]->temp = l2[i]->first;	 
				printf("\nL%d: %c %d %d %d ",i+2,l2[i]->type,l2[i]->priority,l2[i]->period,l2[i]->loop);

				/*loop to print out values of linked list*/				
				while (l2[i]->temp != 0)	
					{
	        			printf("%s %d ", l2[i]->temp->ll,l2[i]->temp->busy);
	        			l2[i]->temp = l2[i]->temp->next;
					}

				/*next line of file*/
				fseek(ptr_file, +1L, SEEK_CUR);	
			}
			printf("\n******************************************************************");

		/*Creating threads*/
		for(i=0;i<num;i++)
			{
			if(l2[i]->type=='P')
				{
				/*Set priority*/
				para.sched_priority = l2[i]->priority;

				/*Create periodic thread*/
				return_value = pthread_create(&periodic_thread[p_count], NULL, periodic_function, (void *)l2[i]);
				if (return_value != 0)
					{
					printf("\nperiodic_thread failed");
					}

				/*Set schedule parameter*/
				return_value = pthread_setschedparam(periodic_thread[p_count], SCHED_FIFO, &para);
				if (return_value != 0)
					{
					printf("periodic_thread priority set failed\n");
					}
				p_count++;
				}
			else if(l2[i]->type=='A')
				{
				/*Set priority*/
				para.sched_priority = l2[i]->priority;

				/*Create aperiodic thread*/
				return_value = pthread_create(&aperiodic_thread[ap_count], NULL, &aperiodic_function, (void*)l2[i]);
				if (return_value != 0)
					{
					printf("aperiodic_thread failed\n");
					}

				/*Set schedule parameter*/
				return_value = pthread_setschedparam(aperiodic_thread[ap_count], SCHED_FIFO, &para);
				if (return_value != 0)
					{
					printf("aperiodic_thread priority set failed\n");
					}
				ap_count++;
				}
			}
		printf("Finished creating threads in main");

		/*Assigning value for timer values*/
		sec=l1->t_period/1000;
		msec=l1->t_period % 1000;
	
 		/* Install timer_handler as the signal handler for SIGVTALRM. */
 		memset (&sa, 0, sizeof (sa));
 		sa.sa_handler = &timer_handler;
		sigaction (SIGPROF, &sa, NULL);
	
 		/* Configure the timer to expire after timer period */
 		timer.it_value.tv_sec =sec;
 		timer.it_value.tv_usec = msec * 1000;

 		/* Timer expiration only once */
 		timer.it_interval.tv_sec = 0;
 		timer.it_interval.tv_usec = 0;

 		/* Start a timer */
		setitimer (ITIMER_PROF, &timer, NULL);

		/*Execute main until timer expires*/
		while(1)
			{
			if(flag1==1)
				break;
			}

		/*Cancel Threads*/
		return_value = pthread_cancel(mouse_click_thread);
           		if (return_value != 0)
               			printf("\npthread_cancel fail");
			else
				printf("\npthread_cancelled mouse click thread");
		for(i=0;i<p_count;i++)
		{
		return_value = pthread_cancel(periodic_thread[i]);
           		if (return_value != 0)
               			printf("\npthread_cancel fail");
			else
				printf("\npthread_cancelled periodic_thread[%d]",i);
		}
		for(i=0;i<ap_count;i++)
		{
		return_value = pthread_cancel(aperiodic_thread[i]);
           		if (return_value != 0)
               			printf("\npthread_cancel fail");
			else
				printf("\npthread_cancelled aperiodic_thread[%d]",i);
		}
		
		/*Join threads to main*/
		return_value=pthread_tryjoin_np(mouse_click_thread,NULL);
		for(i=0;i<p_count;i++)
			{
			return_value=pthread_tryjoin_np(periodic_thread[i], NULL);
			}
		for(i=0;i<ap_count;i++)
			{
			return_value=pthread_tryjoin_np(aperiodic_thread[i], NULL);
			}
		}	
		
	else
		printf("File open unsuccessful");

	/* Close file */
	fclose(ptr_file);
	printf("\nProgram exiting\n");

	/* Exit */
	return  0;
}
