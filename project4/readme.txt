----------------------------------------------------
CSE 522: Assignment 2 Schedulability Analysis
----------------------------------------------------

Files in Submission:

	User Level File: main.c
	Text file: data.txt
	Makefile: Makefile
	Task synthesis file: synth_task.c
	Excel sheet: Final_Stats3.xlsx
	Report: Report.pdf


Instructions for compiling the program:

	1. data.txt already contains tasksets. It can be changed according to user requirement.
	2. The files are compiled by using the following command `make`. 
	3. Run the file by using command `./main.o` 
	4. EDF, RM and DM analysis are done on all tasksets and reported if they are schedulable or not
	5. The file synth_task.c contains the program that was used to synthesize tasks, apply schedulability analysis
	   and the number of schedulable tasks are extracted to excel sheet. The extracted values and the XY plots are 
	   present in Final_Stats3.xlsx.
	6. This can be run by command `./task.o`
	7. The report contains an explanation of the methods followed for steps 3 and 4.
	
