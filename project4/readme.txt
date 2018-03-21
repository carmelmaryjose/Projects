----------------------------------------------------
Schedulability Analysis
----------------------------------------------------

Files in Submission:

	User Level File: main.c
	Text file: data.txt
	Makefile: Makefile
	Task synthesis file: synth_task.c
	Excel sheet: Final_Stats3.xlsx
	Report: Report.pdf

Objective:
	1. An analysis program written in c, running in Linux without any IDE environment, reads in task parameters from the input text file and reports that each taskset is schedulale or not
	2. For each taskset, the analysis program considers Earliest deadline first, Rate monotonic and Deadline monotonic algorithms
	3. The program chooses utilizatin based analysis first. If the attempt is inconclusive, the program then applies the test approaches based on response time and loading facotr.
	4. When response time based analysis method is used, the computed worst case response time is reported. If the loading factor approach is used, the first missing deadline is reported
	5. For comparative analysis, XY plots that illustrate the percentage of random task sets of various utilizations that are schedulable under EDF, RM and DM is shown. The analysis needs to generate synthetic tasksets to test the schedulability. 

Instructions for compiling the program:

	1. data.txt already contains tasksets. It can be changed according to user requirement.
	2. The files are compiled by using the following command `make`. 
	3. Run the file by using command `./main.o` 
	4. EDF, RM and DM analysis are done on all tasksets and reported if they are schedulable or not
	5. The file synth_task.c contains the program that was used to synthesize tasks, apply schedulability analysis and the number of schedulable tasks are extracted to excel sheet. The extracted values and the XY plots are present in Final_Stats3.xlsx.
	6. This can be run by command `./task.o`
	7. The report contains an explanation of the methods followed for steps 3 and 4.
	
