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
#include <linux/types.h>

struct led_info
{
int num_led;
int string[16];
}*pin_arg;

void IOSetup(void)
{
	int FdExport, Fd24, Fd44, Fd72,status;

	FdExport = open("/sys/class/gpio/export", O_WRONLY);
		if (FdExport < 0)
		{
			status=0;//printf("\n gpio export open failed");
		}

		if(0< write(FdExport,"24",3))
			status=0;//printf("error open fail FdExport 24");
		if(0< write(FdExport,"44",2))
			status=0;//printf("error open fail FdExport 44");
		if(0< write(FdExport,"72",2))
			status=0;//printf("error open fail FdExport 72");

		close(FdExport);

	    /* Initialize all GPIOs */
		Fd24 = open("/sys/class/gpio/gpio24/direction", O_WRONLY);
		if (Fd24 < 0)
		{
			status=0;//printf("\n gpio24 direction open failed");
		}
		//IO00
		Fd44 = open("/sys/class/gpio/gpio44/direction", O_WRONLY);
		if (Fd44 < 0)
		{
			status=0;//printf("\n gpio44 direction open failed");
		}


		if(0< write(Fd24,"out",3))
			status=0;//printf("error direction write fail Fd24");

		if(0< write(Fd44,"out",3))
			status=0;
			//printf("error direction write fail Fd44");


		Fd44 = open("/sys/class/gpio/gpio44/value", O_WRONLY);
		if (Fd44 < 0)
				{
					status=0;//printf("\n gpio44 value open failed");
				}
		Fd72 = open("/sys/class/gpio/gpio72/value", O_WRONLY);
		if (Fd72 < 0)
				{
					status=0;//printf("\n gpio72 value open failed");
				}

		if(0< write(Fd44,"1",1))
			status=0;		//printf("error valud write Fd44 value");

		if(0< write(Fd72,"0",1))
			status=0;
					//printf("error value write Fd72 value");
}

int main(int argc, char **argv)
{
int fd,i;
int res=0;
IOSetup();
sleep(1);
pin_arg = (struct led_info *) malloc(sizeof(struct led_info));
printf("enter the n number of leds that need to be lit up\n");
scanf("%d",&pin_arg->num_led);
printf("enter 3 bit pixel info for all led (rgb)\n");
for(i=0;i<pin_arg->num_led;i++)
	scanf("%d",&pin_arg->string[i]);


fd=open("/dev/ws2812", O_RDWR);
if (fd < 0 ){
		printf("%d error \n", errno);
		return 0;
		}
res = ioctl(fd,0,0);
	if(res == 0)
		printf("\nioctl success\n");
res = write(fd, pin_arg, sizeof(struct led_info));
	if(res > 0)
		printf("\nwrite success\n");
close(fd);
return 0;
}
