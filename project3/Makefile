APP = main

CC = i586-poky-linux-gcc
ARCH = x86
CROSS_COMPILE = i586-poky-linux-
IOT_HOME = /opt/iot-devkit/1.7.2/sysroots

DEST := $(shell uname -m)


PATH := $(PATH):$(IOT_HOME)/x86_64-pokysdk-linux/usr/bin/i586-poky-linux

SROOT=$(IOT_HOME)/i586-poky-linux
ifeq ($(DEST), gal)	
all:
	$(CC) -o main.o main.c --sysroot=$(SROOT) -lpthread -lrt -Wall
else
all:
	gcc -o main.o main.c -lpthread -lrt -Wall
endif

clean:
	rm -f main
	rm -f *.ko
	rm -f *.o
	rm -f Module.symvers
	rm -f modules.order
	rm -f *.mod.c
	rm -rf .tmp_versions
	rm -f *.mod.c
	rm -f *.mod.o
	rm -f \.*.cmd
	rm -f Module.markers
	rm -f $(APP) 
	rm -f *.log

cleanlog:
	rm -f *.log
