#include<linux/init.h>
#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/spi/spi.h>


static struct spi_board_info spi_onboard_dev_info = {

	
		.modalias = "ws2812",
		.max_speed_hz = 800000,
		.mode = SPI_MODE_3,
		.bus_num = 1,
		.chip_select = 1,

};
struct spi_master *master;
static struct spi_device *spi_p_device;


int WS_device_init(void)
{ 	int ret;
	master = spi_busnum_to_master( spi_onboard_dev_info.bus_num );
	if( !master )
		return -ENODEV;

  	spi_p_device = spi_new_device( master, &spi_onboard_dev_info );
  	if( !spi_p_device )
		return -ENODEV;

	spi_p_device->bits_per_word = 16;

	ret = spi_setup( spi_p_device );
	if( ret )
		spi_unregister_device( spi_p_device );
	else
		printk( KERN_INFO "SPI device registered to SPI bus");
	return 0;

}

void WS_device_exit(void)
{
  spi_unregister_device(spi_p_device);
}

module_init(WS_device_init);
module_exit(WS_device_exit);
MODULE_LICENSE("GPL");
