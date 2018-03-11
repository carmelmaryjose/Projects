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
#include<linux/pinctrl/pinmux.h>

struct inst_device
{
  int devnum;
  int trigger_pin;
  int echo_pin;
  int samples;
  int period;
  int pw;
  int avg;
  long long stime;
  long long etime;
unsigned  int dist;
  unsigned long sampling_delay;
  unsigned int irqnum;
  struct miscdevice *gpiod;
  struct hrtimer sampling_timer;
  struct pinparaargs *pinconfigargs;
};

int dnum;

struct inst_device *devins[10];
int pw;
int avg;



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
  devins[dnum]->etime= rdtscll(devins[dnum]->etime);
  pw = (int)(devins[dnum]->etime - devins[dnum]->stime);
  devins[dnum]->dist =  do_div(pw,23529);
  avg = avg + devins[dnum]->dist;
  printk("\n distance %d", devins[dnum]->dist);
  return (irq_handler_t) IRQ_HANDLED;

}

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
  int devnum = (int) file->private_data;
  int minornum =iminor(inode);
  printk("entered");
  for(j=1;j<=n;j++)
  {
    if(devins[j]->gpiod->minor == minornum)
    {
      file->private_data =j;
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
  int c=0;
  int devnum = (int) file->private_data;
  dnum = devnum;
  printk("enteredioctl");
  int k;
  int intres=0;
  struct pinparaargs *pinconfigargs = kzalloc(sizeof(struct pinparaargs), GFP_KERNEL);
  if (copy_from_user(pinconfigargs,( struct pinparaargs *)arg, sizeof(struct pinparaargs))) {
         return -1;
  }

  devins[devnum]->pinconfigargs = (struct pinparaargs*)arg;
  if(devins[devnum]->pinconfigargs->pinparaargs1>0 && devins[devnum]-> pinconfigargs->pinparaargs2>0)
  {
    printk("cmd%d",cmd);
    switch(cmd)
    {
      case CONFIG_PINS:
              printk("pin conifg");
              devins[devnum]->trigger_pin = devins[devnum]->pinconfigargs->pinparaargs1;
              printk("%d",devins[devnum]->trigger_pin);
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
                printk("%d",devins[devnum]->echo_pin);
              }
              else
              {
                printk("valid echo pin input required");
              }
              break;
      case SET_PARAMETERS:
              printk("setting parameters");
              devins[devnum]->samples = devins[devnum]->pinconfigargs->pinparaargs1;
              printk("%d",devins[devnum]->samples);
              devins[devnum]->period = devins[devnum]->pinconfigargs->pinparaargs2;
              devins[devnum]->sampling_delay= 1000000*(devins[devnum]->period);
              printk("%d",devins[devnum]->period);
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
  devins[dnum]->stime = rdtscll (devins[dnum]->stime);
  printk("\npin should glow");
  gpio_set_value_cansleep(pinnum[devins[dnum]->trigger_pin].linux_pin,0);
  donesampling++;
  return HRTIMER_RESTART;
}

static int __init timer_init(void)
{
  ktime_t ktime= ktime_set(0,devins[dnum]->sampling_delay);
  hrtimer_init (&devins[dnum]->sampling_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
  devins[dnum]->sampling_timer.function = &timer_callback;
  hrtimer_start (&devins[dnum]-> sampling_timer, ktime, HRTIMER_MODE_REL);
  return 0;
}



static void __exit timer_exit(void)
{
  int tret;
  tret = hrtimer_cancel(&devins[dnum]->sampling_timer);
}



ssize_t gpiod_write (struct file *file, const char *buf, size_t count, loff_t *ppos)

{

  timer_init();
  return count;
}

ssize_t gpiod_read(struct file *file, const char *buf, size_t count, loff_t *ppos)
{
  avg=avg/devins[dnum]->samples;
  copy_to_user(buf, &devins[dnum]->dist, sizeof(devins));
  return count;
}


static const struct file_operations gpiod_fops = {
    .owner 		= THIS_MODULE,
    .write			= gpiod_write,
    .open			= gpiod_open,
    .read     = gpiod_read,
    .release		= gpiod_close,
    .llseek 		= no_llseek,
    .unlocked_ioctl = gpiod_ioctl,
};



static int __init misc_init(void)
{
    int error;
    char str[10];
    for(j=1;j<=n;j++)
    {
      devins[j]=kzalloc(sizeof(struct inst_device),GFP_KERNEL);
      devins[j]->gpiod=kzalloc(sizeof(struct miscdevice),GFP_KERNEL);
   sprintf(str,"HCSR_%d",j);
   devins[j]->gpiod->name = str;
   devins[j]->gpiod->minor = MISC_DYNAMIC_MINOR;
   devins[j]->gpiod->fops = &gpiod_fops;
   error = misc_register(devins[j]->gpiod);
    }
        return 0;
}


static void __exit misc_exit(void)
{
  timer_exit();
  free_irq(devins[dnum]->irqnum, NULL);
  for(j=1;j<=n;j++)
  {
    gpio_direction_output(pinnum[devins[j]->echo_pin].linux_pin,0);
    misc_deregister(devins[j]->gpiod);
    pr_info("exit\n");
    kfree(devins[j]->gpiod);
    kfree(devins[j]);

gpio_free(pinnum[devins[j]->trigger_pin].linux_pin);
gpio_free(pinnum[devins[j]->echo_pin].linux_pin);
}
}
module_init(misc_init);
module_exit(misc_exit);
MODULE_LICENSE("GPL");
