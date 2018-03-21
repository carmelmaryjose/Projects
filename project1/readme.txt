Platform device driver and sysfs interface for HC-SR04

Files:

	-user.c
	-board.c
	-driver.c
	-Makefile

Objective: 
	- Implement a SPI device driver which can accept pixel information from users and light up 16 RGB LEDs of a LED ring. 
	The LEDs of the ring are connected serially and uses a 1-wire communication protocol to receive pixel data for full color display
	
Instructions for compiling the program:

	1. Compile the Makefile and cross compile the device,driver and main program by using the command `make`
	2. Copy the following files to the galileo board using the following command `sudo scp filename root@'ip_of_device':/home`
		-board.ko
		-driver.ko
		-user
	2. Insert the module board.ko into the kernel by using command `insmod board.ko` 
	3. Insert the module driver.ko using command `insmod driver.ko` 
	4. Run the program by command `./user`
	5. Follow the program instructions to enter the number of leds to be lit and the values for each led as pixels
		eg: number of leds to be lit: 16
 		    pixel info for each led: 100(for red)
					     010(for green)
					     001(for blue)	


	note: The program has been coded such that the leds will circulate for 20 seconds.
