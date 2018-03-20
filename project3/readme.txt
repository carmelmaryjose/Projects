----------------------------------------------------
Real-Time Task Models in Linux
----------------------------------------------------


Files in Submission:

	User Level File: main.c
	Text file: data.txt
	Makefile: Makefile
	Report: Report.pdf


Instructions for compiling the program:

	1. The MOUSE DEVICE path needs to be changed from "/dev/input/event2" to correct value as per the settings of the linux system.This 
	   can be found through `cat /proc/bus/input/devices`  
	2. Compile the Makefile for an Ubuntu system using command `make all`
	   Compile the Makefile for a galileo board using command `make DEST='gal' all`
	3. Scp the following files to the galileo board using command `scp filename root@ip:`
	4. Run the program by command `./main.o number` where number is 0 for normal mutex and 1 for PI enabled mutex
		eg: `./main.o 1`
	
 



