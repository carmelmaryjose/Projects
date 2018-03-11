#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include <linux/sched.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
#include<sys/ioctl.h>
#include <linux/ioctl.h>
#include <linux/rtc.h>
#include <time.h>
#include <semaphore.h>

#define IOCTL_Constant 'k'
#define CONFIG_PINS _IOW(IOCTL_Constant, 2, int)
#define SET_PARAMETERS _IOW(IOCTL_Constant,1,int)

long result;



struct distance
{
  int dist;
};

struct distance sensor_distance[100];

int fd[100];

struct pinparaargs
{
  int pinparaargs1;
  int pinparaargs2;
};



int main(int argc, char **argv)

{
  int i,instances;
  char mod_name[40];
  printf("ins ");
  scanf("%d",&instances);
  struct pinparaargs p[100];
  struct pinparaargs q[100];
  for(i=1;i<=instances;i++)
  {
    sprintf(path,"/dev/HCSR_%d",i);
      fd[i] = open(mod_name, O_RDWR);
  if (fd[i] < 0 ){
		printf("%d \n", errno);
		return 0;
		}

    scanf("%d",&(p[i].pinparaargs1));
      scanf("%d",&(p[i].pinparaargs2));
    result = ioctl(fd[i], CONFIG_PINS,&p[i]);
    scanf("%d",&(q[i].pinparaargs1));
      scanf("%d",&(q[i].pinparaargs2));
    result = ioctl(fd[i], SET_PARAMETERS,&q);
    //write(fd[i],&p[i],sizeof(p[i]));
    //read(fd[i],&sensor_distance, sizeof(sensor_distance));
    //printf(" average distance %d",  sensor_distance.dist);
    printf("write in user");

    }
    return 0;
}
