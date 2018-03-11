#include <linux/platform_device.h>
#ifndef __HPDE_H__
#define __HPDE_H__

struct P_chip {
		char 			*name;
		int			dev_no;
		struct platform_device 	plf_dev;
};
#endif
