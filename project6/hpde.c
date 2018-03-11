#include<linux/init.h>
#include<linux/kernel.h>
#include<linux/module.h>
 #include<linux/platform_device.h>

 #include "hpde.h"

 static int HCSR_device_release(void)
 {
   printk("\n device exiting \n");
 }


 static struct P_chip P1_chip =
 {
   .name="hc01",
   .dev_no = 20,
   .plf_dev =
   {
     .name = "h01" ,
     .id = -1,
     .dev.release = HCSR_device_release,
   }
 };

static int HCSR_device_init(void)
{
  platform_device_register(&P1_chip.plf_dev);
}

static int HCSR_device_exit(void)
{
  platform_device_unregister(&P1_chip.plf_dev);
}
module_init(HCSR_device_init);
module_exit(HCSR_device_exit);
MODULE_LICENSE("GPL");
