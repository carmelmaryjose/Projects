----------------------------------------------------
Real-Time Task Models in Linux
----------------------------------------------------


Files in Submission:

	User Level File: main.c
	Text file: data.txt
	Makefile: Makefile
	Report: Report.pdf

Program:
	1. The input to the program is a specification of a taskset present in data.txt
		eg: 2 3000                          // there are two tasks and execution 
						       terminates after 3000ms
		    P 20 500 200 L3 300 U3 400      // task 1 is a periodic task with priority 20 
						       and a period of 500ms.
						    // in its task body, it runs the busy loop 
						       for 200 iterations,locks mutex 3, runs 
						       busy loop for 300 iterations, unlocks 
						       mutex 3 and finally runs 400 iterations of 
						       busy loop
		    A 10 1 500                      // task 2 is an aperiodic task of priority 
						       10. It is triggered by event 1 and then 
						       runs 500 iterations of busy loop
	2. Threads are created for each task specified in data.txt. All threads are activated at the same time by using barrier. 
   	3. Event 0 and 1 arrive when we release left or right mouse clicks. The events trigger aperiodic tasks present in the taskset
	4. At most 10 mutex locks are used for the shared resources for periodic tasks
	5. CPU affinity is used to ensure that all threads are bound to one core
	6. Conditional directive is used to invoke normal and PI enabled pthread mutexes for locking resources	
	7. The priority numbers given in input file are real-time priority levels and tasks are scheduled under the real time policy SCHED_FIFO
	8. When the current iteration of a periodic task is not done before the end of the period, its next iteration is started immediately as soon as the task finishes its current task body
	9. When the execution terminates, any waiting tasks should exit immediately. Any running or ready task should exit after completing the current iteration of its task body
	10.Verification of the scheduling events of tasks in the host system is done by using linux internal tracer ftrace to collect schedule events such as context switch, wakeup etc

Instructions for compiling the program:

	1. The MOUSE DEVICE path needs to be changed from "/dev/input/event2" to correct value as per the settings of the linux system.This can be found through `cat /proc/bus/input/devices`  
	2. Compile the Makefile for an Ubuntu system using command `make all`
	   Compile the Makefile for a galileo board using command `make DEST='gal' all`
	3. Scp the following files to the galileo board using command `scp filename root@ip:`
	4. Run the program by command `./main.o number` where number is 0 for normal mutex and 1 for PI enabled mutex
		eg: `./main.o 1`
	
 



