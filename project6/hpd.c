#include<linux/kernel.h>
#include<linux/fs.h>
#include<linux/gpio.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <linux/string.h>
#include <linux/device.h>
#include <linux/jiffies.h>
#include <linux/init.h>
#include<linux/ioctl.h>
#include<linux/interrupt.h>
#include<asm/delay.h>
#include<linux/delay.h>
#include<linux/hrtimer.h>
#include<linux/ktime.h>
#include<linux/sysfs.h>
#include<linux/slab.h>
#include<linux/pinctrl/pinmux.h>
#include<linux/platform_device.h>
#include "hpde.h"

#define DEVICE_NAME "HCSR_SENSOR"
#define CLASS_NAME "HCSR_CLASS"
#define DRIVER_NAME "hpd"


static struct class *HCSR_class;
dev_t HCSR_dev=1;

static const struct platform_device_id HCSR_id_table[]=
{
  {"h01",0},
  {"h02",0},
};

struct inst_device
{
  int devnum;
  int trigger_pin;
  int echo_pin;
  int samples;
  int period;
  int pw;
  int avg;
  int en;
  long long stime;
  long long etime;
  unsigned  int dist;
  unsigned long sampling_delay;
  unsigned int irqnum;
  struct device *dev;
  struct miscdevice *gpiod;
  struct hrtimer sampling_timer;
  struct pinparaargs *pinconfigargs;
};

int dnum;

int k,c=0;


struct inst_device *devins[10];
int pw;
unsigned int avg=0;


#define IOCTL_Constant 'k'
#define CONFIG_PINS _IOW(IOCTL_Constant, 2, int)
#define SET_PARAMETERS _IOW(IOCTL_Constant,1,int)
#define a 14
int n;
int num=10;
module_param(n,int,0);

int donesampling=0;


struct pinparaargs
{
  int pinparaargs1;
  int pinparaargs2;
};

static irq_handler_t echpin_irq_handler( unsigned int irq, void *dev_id, struct pt_regs *regs)
{

  	printk("\n interrupt triggered");
 	rdtscll(devins[dnum]->etime);
  	pw = (int)(devins[dnum]->etime - devins[dnum]->stime);
 	devins[dnum]->dist =  do_div(pw,23529);
  	avg = avg + devins[dnum]->dist;
  	printk("\n distance %d", devins[dnum]->dist);
  	return (irq_handler_t) IRQ_HANDLED;

}

void calltimer(void);
int interruptable_pins[]={ 0,1,2,3,4,5,6,9,10,11,12,13};

struct pinnumber
{
  int shield_pin;
  int linux_pin;
  int level_shifter_gpio;
  int pin_mux_1;
};

int j;


static struct pinnumber pinnum [14]=
{
           {0, 11, 32, 0},
           {1, 12, 28, 45},
           {2, 13, 34, 77},
           {3, 14, 16, 76},
           {4,  6,  0,  0},
           {5,  0, 18, 66},
           {6,  1, 20, 68},
           {7,  38,  0, 0},
           {8,  40,  0, 70},
           {9,  4, 22,  74},
           {10, 10, 26, 44},
           {11,  5, 24, 0},
           {12, 15, 42, 0},
           {13,  7, 30, 46}
};



int gpiod_open (struct inode *inode, struct file *file)
{

  int minornum =iminor(inode);
  printk("\nentered \n");
  for(j=1;j<=n;j++)
  {
    if(devins[j]->gpiod->minor == minornum)
    {
      file->private_data = (void*)j;
      devins[j]->devnum=j;
      break;
    }
  }
  return 0;
}

int gpiod_close (struct inode *inode, struct file *file )
{

  return 0;
}

static long gpiod_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
  	int devnum = (int) file->private_data;
  	int intres=0;

    	struct pinparaargs *pinconfigargs = kzalloc(sizeof(struct pinparaargs), GFP_KERNEL);
    	dnum = devnum;
    	printk("\nenteredioctl");
  	if (copy_from_user(pinconfigargs,( struct pinparaargs *)arg, sizeof(struct pinparaargs))) {
         	return -1;
  	}

  	devins[devnum]->pinconfigargs = (struct pinparaargs*)arg;
  	if(devins[devnum]->pinconfigargs->pinparaargs1>0 && devins[devnum]-> pinconfigargs->pinparaargs2>0)
  	{
    	printk("\ncmd%d",cmd);
   	 switch(cmd)
    		{
     		case CONFIG_PINS:
              	printk("\npin conifg");
              	devins[devnum]->trigger_pin = devins[devnum]->pinconfigargs->pinparaargs1;
              	printk("\n%d",devins[devnum]->trigger_pin);
              	for(k=0;k<12;k++)
              	{
                if(devins[devnum]->pinconfigargs->pinparaargs2 == interruptable_pins[k]);
                {
                  c=1;

                }
              	}
              	if(c==1)
              	{
                devins[devnum]->echo_pin = devins[devnum]->pinconfigargs->pinparaargs2;
                printk("\n%d",devins[devnum]->echo_pin);
              	}
              	else
              	{
                printk("\nvalid echo pin input required");
              	}
              	break;
      	case SET_PARAMETERS:
              printk("\nsetting parameters");
              devins[devnum]->samples = devins[devnum]->pinconfigargs->pinparaargs1;
              printk("\n%d",devins[devnum]->samples);
              devins[devnum]->period = devins[devnum]->pinconfigargs->pinparaargs2;
              devins[devnum]->sampling_delay= 1000000*(devins[devnum]->period);
              printk("\n%d",devins[devnum]->period);
              break;
    }
  }
    else
    {
      return -EINVAL;
    }
    gpio_direction_output(pinnum[devins[devnum]->trigger_pin].level_shifter_gpio,1);
    gpio_set_value_cansleep(pinnum[devins[devnum]->trigger_pin].level_shifter_gpio,0);
    gpio_request(pinnum[devins[devnum]->trigger_pin].linux_pin,"sysfs" );
    gpio_direction_output(pinnum[devins[devnum]->trigger_pin].linux_pin,1);
    gpio_request(pinnum[devins[devnum]->echo_pin].linux_pin,"sysfs" );
    gpio_direction_output(pinnum[devins[devnum]->echo_pin].level_shifter_gpio,1);
    gpio_set_value_cansleep(pinnum[devins[devnum]->echo_pin].level_shifter_gpio,1);
    gpio_direction_input(pinnum[devins[devnum]->echo_pin].linux_pin);
    printk("\n %d",pinnum[devins[devnum]->echo_pin].linux_pin);
    devins[devnum]->irqnum= gpio_to_irq(pinnum[devins[devnum]->echo_pin].linux_pin);
    printk("\n %d",devins[devnum]->irqnum);
    intres = request_irq(devins[dnum]->irqnum, (irq_handler_t) echpin_irq_handler, IRQF_TRIGGER_FALLING, "echpin_irq_handler",NULL);
    printk("\n%d",intres);
  return 0;
}

enum hrtimer_restart timer_callback( struct hrtimer *timer_for_restart)
{
  ktime_t currtime, interval;
  currtime = ktime_get();
  interval = ktime_set (0,devins[dnum]->sampling_delay);
  hrtimer_forward( &devins[dnum]->sampling_timer,currtime,interval);
  if(donesampling>devins[dnum]->samples)
  {
    return HRTIMER_NORESTART;
  }

  gpio_set_value_cansleep(pinnum[devins[dnum]->trigger_pin].linux_pin,1);
  printk("\nLinux pin in timer %d\n",gpio_get_value(pinnum[devins[dnum]->trigger_pin].linux_pin));
  printk("\npin should glow");
  udelay(1000);
  gpio_set_value_cansleep(pinnum[devins[dnum]->trigger_pin].linux_pin,0);
  printk("\nLinux pin in timer %d\n",gpio_get_value(pinnum[devins[dnum]->echo_pin].linux_pin));
  donesampling++;
  return HRTIMER_RESTART;
}

static int __init timer_init(void)
{
  ktime_t ktime= ktime_set(0,devins[dnum]->sampling_delay);
  hrtimer_init (&devins[dnum]->sampling_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
  devins[dnum]->sampling_timer.function = &timer_callback;
  hrtimer_start (&devins[dnum]-> sampling_timer, ktime, HRTIMER_MODE_REL);
  printk("\ntimerinit dnum: %d\n",dnum);
  return 0;
}

void calltimer()
{
  printk("\ncalltimer dnum: %d\n",dnum);
  timer_init();
}

static void __exit timer_exit(void)
{
  int tret;
  if(dnum)
  tret = hrtimer_cancel(&devins[dnum]->sampling_timer);
}



ssize_t gpiod_write (struct file *file, const char *buf, size_t count, loff_t *ppos)

{
   calltimer();
  return count;
}

ssize_t gpiod_read(struct file *file, const char *buf, size_t count, loff_t *ppos)
{

  avg=avg/(devins[dnum]->samples);
  copy_to_user((struct devins*) buf, &avg, sizeof(devins));
  return count;
}


static ssize_t trig_pin_store(struct device *dev,
                               struct device_attribute *attr,
                               char *buf, size_t count)
{
  struct inst_device *devins_cur =  dev_get_drvdata(dev);
  printk("\ntrig_pin_store entered\n");
  printk ("\n :%d\n", devins_cur->devnum);
  sscanf(buf,"%d",&(devins[devins_cur->devnum]->trigger_pin));
  printk("\n trig_pin:%d \n",(devins[devins_cur->devnum]->trigger_pin));
  gpio_direction_output(pinnum[devins[devins_cur->devnum]->trigger_pin].level_shifter_gpio,1);
  gpio_set_value_cansleep(pinnum[devins[devins_cur->devnum]->trigger_pin].level_shifter_gpio,0);
  gpio_request(pinnum[devins[devins_cur->devnum]->trigger_pin].linux_pin,"sysfs" );
  gpio_direction_output(pinnum[devins[devins_cur->devnum]->trigger_pin].linux_pin,1);
  printk("Linux pin in trigger_store %d",gpio_get_value(pinnum[devins[devins_cur->devnum]->trigger_pin].linux_pin));
  return count;
}

static ssize_t ech_pin_store(struct device *dev,struct device_attribute *attr,char *buf, size_t count)
{
  int res_irq;
  unsigned int irq_no;
  struct inst_device *devins_cur =  dev_get_drvdata(dev);
  printk("\ntrig_pin_store entered\n");
  printk ("\n echpin :%d \n", devins_cur->devnum);
  sscanf(buf,"%d",&(devins[devins_cur->devnum]->echo_pin));
		for(k=0;k<12;k++)
              {
                if(devins[devins_cur->devnum]->echo_pin == interruptable_pins[k]);
                {
                  c=1;

                }
              }
              if(c==1)
              {
                printk("\n %d \n",devins[devins_cur->devnum]->echo_pin);
              }
              else
              {
                printk("\n valid echo pin input required \n");
              }
		gpio_request(pinnum[devins[devins_cur->devnum]->echo_pin].linux_pin,"sysfs" );
		gpio_direction_output(pinnum[devins[devins_cur->devnum]->echo_pin].level_shifter_gpio,1);
		gpio_set_value_cansleep(pinnum[devins[devins_cur->devnum]->echo_pin].level_shifter_gpio,1);
		gpio_direction_input(pinnum[devins[devins_cur->devnum]->echo_pin].linux_pin);
    		printk("\nThe gpio pin for echo is %d\n",pinnum[devins[devins_cur->devnum]->echo_pin].linux_pin);
   		irq_no = gpio_to_irq(pinnum[devins[devins_cur->devnum]->echo_pin].linux_pin);
   		devins[devins_cur->devnum]->irqnum = irq_no;
   		printk("\n irqnum%d \n", devins[devins_cur->devnum]->irqnum);
   		res_irq = request_irq(irq_no, (irq_handler_t) echpin_irq_handler, IRQF_TRIGGER_FALLING, "echpin_irq_handler",NULL);
   		printk("\n%d",res_irq);
        	return count;
}

static ssize_t sample_num_store(struct device *dev,struct device_attribute *attr,char *buf, size_t count)
{

    struct inst_device *devins_cur =  dev_get_drvdata(dev);
    printk("\nsample_num_store entered\n");
    printk ("\n dev_number :%d \n", devins_cur->devnum);
		sscanf(buf,"%d",&(devins[devins_cur->devnum]->samples));
    printk("\n samples:%d \n",(devins[devins_cur->devnum]->samples));
    return count;
}

static ssize_t sample_perd_store(struct device *dev,
                               struct device_attribute *attr,
                               char *buf, size_t count)
{
  struct inst_device *devins_cur =  dev_get_drvdata(dev);
  sscanf(buf,"%d",&(devins[devins_cur->devnum]->period));
  return count;
}

static ssize_t enable_store(struct device *dev,struct device_attribute *attr,char *buf, size_t count)
{
  int enable;
  struct inst_device *devins_cur =  dev_get_drvdata(dev);
  sscanf(buf,"%d",&(devins[devins_cur->devnum]->en));
  printk(" \n i will enter the loop \n");
  printk("\n%d\n",(devins[devins_cur->devnum]->en));
  enable = devins[devins_cur->devnum]->en;
		if(enable == 1)
    		{
    		printk("\n loop entered \n ");
    		dnum = devins_cur->devnum;
    		printk("\ndnum: %d\n",dnum);
    		calltimer();
    		}
  return count;
}

static ssize_t dis_show(struct device *dev,struct device_attribute *attr,char *buf)
{

struct inst_device *devins_cur =  dev_get_drvdata(dev);
avg=avg/devins[devins_cur->devnum]->samples;
printk("\n %d\n",avg);
return sprintf(buf,"%d\n",avg);
}

static const struct file_operations gpiod_fops = {
    .owner 		= THIS_MODULE,
    .write			= gpiod_write,
    .open			= gpiod_open,
    .read =  gpiod_read,
    .release		= gpiod_close,
    .llseek 		= no_llseek,
    .unlocked_ioctl = gpiod_ioctl,
};



static DEVICE_ATTR(trig_pin , S_IWUSR, NULL, trig_pin_store);
static DEVICE_ATTR(ech_pin ,  S_IWUSR, NULL, ech_pin_store);
static DEVICE_ATTR(sample_num ,  S_IWUSR, NULL, sample_num_store);
static DEVICE_ATTR(sample_perd , S_IWUSR, NULL, sample_perd_store);
static DEVICE_ATTR(enable , S_IWUSR, NULL, enable_store);
static DEVICE_ATTR(dis, S_IRUSR, dis_show, NULL);



static int HCSR_driver_probe (struct platform_device *dev_found)
{
  int error;
  char str[10];
  HCSR_class = class_create(THIS_MODULE, CLASS_NAME);

  for(j=1;j<=n;j++)
  {
	 HCSR_dev = (dev_t)j;
	 devins[j]=kzalloc(sizeof(struct inst_device),GFP_KERNEL);
	 devins[j]->gpiod=kzalloc(sizeof(struct miscdevice),GFP_KERNEL);
	 sprintf(str,"HCSR_%d",j);
	 devins[j]->gpiod->name = str;
	 devins[j]->gpiod->minor = MISC_DYNAMIC_MINOR;
	 devins[j]->gpiod->fops = &gpiod_fops;
	 printk("\n%s\n",str);
	 error = misc_register(devins[j]->gpiod);
	 devins[j]->dev = device_create(HCSR_class, NULL, devins[j]->gpiod->minor ,NULL, str);
	 error = device_create_file (devins[j]->dev, &dev_attr_trig_pin);
	 error = device_create_file (devins[j]->dev, &dev_attr_ech_pin);
	 error = device_create_file (devins[j]->dev, &dev_attr_sample_num);
	 error = device_create_file (devins[j]->dev, &dev_attr_sample_perd);
	 error = device_create_file (devins[j]->dev, &dev_attr_enable);
	 error = device_create_file (devins[j]->dev, &dev_attr_dis);
	 devins[j]->devnum = j;
	 dev_set_drvdata(devins[j]->dev,devins[j]);
  }
  j--;
      return 0;
};
static int HCSR_driver_remove(struct platform_device *pdev)
{
  printk ("\n H1 \n");
  timer_exit();
  printk ("\nH2\n");

  for(j=1;j<=n;j++)
  {
  HCSR_dev = (dev_t)j;
  if(devins[j]->irqnum)
  {
  free_irq(devins[j]->irqnum, NULL);
  }
  printk ("\n H3 \n");
  device_destroy(HCSR_class, devins[j]->gpiod->minor);
  printk (" \n H4 \n");
  gpio_direction_output(pinnum[devins[j]->echo_pin].linux_pin,0);
  misc_deregister(devins[j]->gpiod);
  printk ("\n H5 \n");
  pr_info("exit\n");
  kfree(devins[j]->gpiod);
  printk ("\n H6 \n");
  kfree(devins[j]);
  printk (" \n H7 \n");
  gpio_free(pinnum[devins[j]->trigger_pin].linux_pin);
  printk ("\n H8 \n");
  gpio_free(pinnum[devins[j]->echo_pin].linux_pin);
  printk ("\n H9 \n");

}
      class_unregister(HCSR_class);
      printk (" \n H10 \n ");
      class_destroy(HCSR_class);
      printk ("\n H11 \n");
  return 0;
};

static struct platform_driver HCSR_driver = {
	.driver		= {
		.name	= DRIVER_NAME,
		.owner	= THIS_MODULE,
	},
	.probe		= HCSR_driver_probe,
	.remove		= HCSR_driver_remove,
	.id_table	= HCSR_id_table,
};
module_platform_driver(HCSR_driver);
MODULE_LICENSE("GPL");
