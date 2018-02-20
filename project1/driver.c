
#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/device.h>
#include<linux/spi/spi.h>
#include<linux/fs.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include<linux/cdev.h>
#include<asm/uaccess.h>
#include<linux/types.h>
#include<linux/gpio.h>
#include<linux/ioctl.h>
#include<linux/delay.h>
#include<linux/timer.h>

#define DEVICE_NAME "ws2812"
#define SPIDEV_MAJOR	153	/* assigned */

#define SPI_CLASS_NAME "ws_class"

static int buf_size = 1024;


static u8 r[]={0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0xE,0xE,0xE,0xE,0xE,0xE,0xE,0xE,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,
	       };
static u8 g[]={0xE,0xE,0xE,0xE,0xE,0xE,0xE,0xE,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,
	};
static u8 b[]={0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0xE,0xE,0xE,0xE,0xE,0xE,0xE,0xE};
u8 rgb[386]={};
struct led_info
{
int num_led;
int string[16];
};
static struct class *ws2812_class;
struct per_device{
	struct cdev cdev;
	dev_t ws_dev;
	struct spi_device *ws_spi;
	u32 speed_hz;
	u8 *buffer;
}*ws;

/* devide id table */
#ifdef CONFIG_OF
static const struct of_device_id ws_dt_ids[] = {
	{ .compatible = "rohm,dh2228fv" },
	{ .compatible = "lineartechnology,ltc2488" },
	{ .compatible = "ge,achc" },
	{ .compatible = "semtech,sx1301" },
	{},
};
MODULE_DEVICE_TABLE(of, ws_dt_ids);
#endif


int ws_open (struct inode *inode, struct file *file)
{
	printk("\nws_open called\n");

	/* Get the per-device structure that contains this cdev */
	ws = container_of(inode->i_cdev, struct per_device, cdev);
	/*allocate memory for buffer*/
	if (!ws->buffer) 
			ws->buffer = kmalloc(buf_size, GFP_KERNEL);
			
	
	file->private_data = ws;
	nonseekable_open(inode, file);
	printk("device is opening \n");
	return 0;
	
}

int ws_release (struct inode *inode, struct file *file )
{	printk("\nws_release called\n");

	int status = 0;

	ws = file->private_data;
	file->private_data = NULL;
 	int dofree;
	kfree(ws->buffer);
	ws->buffer = NULL;
	dofree = (ws->ws_spi == NULL);
	if (dofree)
		kfree(ws);
	printk("\ndevice closed\n");
	return status;
}

int ws_sync(struct per_device *ws, struct spi_message *message)
{
	
	int status;
	
	

	if (ws->ws_spi == NULL)
		status = -ESHUTDOWN;
	else
		status = spi_sync(ws->ws_spi, message);

	
		if (status == 0)
			status = message->actual_length;
	
	return status;
}

int ws_sync_write(struct per_device *ws, size_t len)
{	
		
	
	struct spi_transfer	t = {
			.tx_buf		= rgb,
			.len		= sizeof(rgb),
			.speed_hz	= 3200000,
			
		};
	struct spi_message	m;

	spi_message_init(&m);
	spi_message_add_tail(&t, &m);
	ws_sync(ws, &m);
	
	return 0;
	
}

ssize_t ws_write (struct file *file, const char *buf, size_t count, loff_t *ppos)
{	int i;
	int status = 0;
	unsigned long	missing=0;
	printk("\nwrite called\n");
	ws = file->private_data;
	/* allocating memory for structure */
	struct led_info *p = kmalloc(sizeof(struct led_info), GFP_KERNEL);
	/*structure values copied from user space to kernel space */
  if (copy_from_user(p,( struct led_info *)buf, sizeof(struct led_info))) {
        printk(KERN_ERR "Copy from user failed\n");
	missing=1;
        return -1;
  	}	

	int k=0,j=0,q=0;
	int temp;
	
	while(q<20)
	{k=0;
	for(i=0;i<= p->num_led ;i++)
		{
		/*for red colour */
		if(p->string[i]==100)
			{
			for(j=0;j<24;j++)
				{
				rgb[k]=r[j];			
				k++;
				}
			}
		/*for blue colour*/
		else if(p->string[i]==10)
			{
			
			for(j=0;j<24;j++)
				{
				rgb[k]=g[j];
				k++;
				}
			}
		/*for green colour*/
		else if(p->string[i]==1)
			{
			
			for(j=0;j<24;j++)
				{
				rgb[k]=b[j];
				k++;
				}
			}
	
		
		}
		

	

	 
	if (missing == 0) {
		msleep(1000);
		status = ws_sync_write(ws, count);
		
	} else
		status = -EFAULT;
	temp=p->string[0];
	/*circulating the led colours*/
	for(i=0;i <= p->num_led; i++)
	{
	p->string[i]=p->string[i+1];
	}
	p->string[i]=temp;
	q++;
	}
	return status;
}
int reset(void)
{
/*setting mode of spi_device as 3*/
ws->ws_spi->mode=SPI_MODE_3;
return 0;
}
static long ws_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
printk("\nioctl called\n");
reset();
return 0;
}
/*file operation structure*/
static const struct file_operations ws2812_fops = {
    	.owner =    THIS_MODULE,
    	.write =    ws_write,
    	
    	.open =        ws_open,
	.unlocked_ioctl=    ws_ioctl,
    	.release =    ws_release
};
static int ws2812_probe(struct spi_device *found_spi)
{
	
	int status;
	
	int ret;
	/*checking if device found in device id table*/
	if (found_spi->dev.of_node && !of_match_device(ws_dt_ids, &found_spi->dev)) {
		dev_err(&found_spi->dev, "buggy DT: spidev listed directly in DT\n");
		WARN_ON(found_spi->dev.of_node &&
			!of_match_device(ws_dt_ids, &found_spi->dev));
	}

	/* Allocate driver data */
	ws = kzalloc(sizeof(*ws), GFP_KERNEL);
	if (!ws)
		return -ENOMEM;



	/* Initialize the driver data */
	ws->ws_spi = found_spi;
	
	/* Request dynamic allocation of a device major number */
	if (alloc_chrdev_region(&ws->ws_dev, 0, 1, DEVICE_NAME) < 0) {
			printk(KERN_DEBUG "Can't register device\n"); return -1;
	}

	/* create class */
	ws2812_class = class_create(THIS_MODULE, DEVICE_NAME);


	/* Connect the file operations with the cdev */
	cdev_init(&ws->cdev, &ws2812_fops);
	ws->cdev.owner = THIS_MODULE;

	/* Connect the major/minor number to the cdev */
	ret = cdev_add(&ws->cdev, (ws->ws_dev), 1);

	if (ret) {
		printk("Bad cdev\n");
		return ret;
	}

	/* create device */
	struct device *dev;
	dev = device_create(ws2812_class, &found_spi->dev,ws->ws_dev, ws, DEVICE_NAME);		
	
		status = IS_ERR(dev) ? PTR_ERR(dev) : 0;
	ws->speed_hz = found_spi->max_speed_hz;
	if (status == 0)
		{
		printk("device created");
		spi_set_drvdata(found_spi, ws);
		}
	else
		{
		printk("device not created");
		kfree(ws);
		}
	return status;
}

static int ws2812_remove(struct spi_device *found_spi)
{	
	
	
	ws=spi_get_drvdata(found_spi);
	/* Release the major number */
	unregister_chrdev_region((ws->ws_dev), 1);

	/* Destroy device */
	device_destroy (ws2812_class, MKDEV(MAJOR(ws->ws_dev), 0));
	cdev_del(&ws->cdev);
	ws->ws_spi = NULL;
	spi_set_drvdata(found_spi, NULL);
	kfree(ws);
	
	/* Destroy driver_class */
	class_destroy(ws2812_class);


	return 0;
}



static struct spi_driver ws2812_driver = {
	.driver = {
		.name	= "ws2812",
		.owner	= THIS_MODULE,
		.of_match_table = of_match_ptr(ws_dt_ids),
	},
	.probe		= ws2812_probe,
	.remove		= ws2812_remove,
	
};
module_spi_driver(ws2812_driver);

int __init ws2812_init(void)
{
int status;
/*registering device*/
status = register_chrdev(SPIDEV_MAJOR, "spi", &ws2812_fops);
	if (status < 0)
		return status;
/*creating class*/
	ws2812_class = class_create(THIS_MODULE, SPI_CLASS_NAME);
	if (IS_ERR(ws2812_class)) {
		unregister_chrdev(SPIDEV_MAJOR, ws2812_driver.driver.name);
		return PTR_ERR(ws2812_class);
	}
/*registering driver*/
	status = spi_register_driver(&ws2812_driver);
	if (status < 0) {
		class_destroy(ws2812_class);
		unregister_chrdev(SPIDEV_MAJOR, ws2812_driver.driver.name);
	}
	return 0;
}

void __exit ws2812_exit(void)
{
	spi_unregister_driver(&ws2812_driver);
	class_destroy(ws2812_class);
	unregister_chrdev(SPIDEV_MAJOR, ws2812_driver.driver.name);
	
}

MODULE_LICENSE("GPL");
