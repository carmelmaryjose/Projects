#!/bin/bash

echo 'Enter the number of devices'
read count
f='HCSR_'
i=1
echo 'Enter the bash script '
while [ $i -le $count ]
do
	path=$f$i
	echo 'enter the trigger pin for Device'$i
	read trigger
	echo $trigger > /sys/class/HCSR_CLASS/$path/trig_pin


	echo 'enter the echo pin for Device'$i
	read e_pin
	echo $e_pin > /sys/class/HCSR_CLASS/$path/ech_pin


	echo 'enter the number of samples for Device'$i
	read num
	echo $num > /sys/class/HCSR_CLASS/$path/sample_num


	echo 'enter the sampling period for Device'$i
	read samp
	echo $samp > /sys/class/HCSR_CLASS/$path/sample_perd

	echo 'enter the value for enable (1: measurement)'
	read en_value
	echo $en_value > /sys/class/HCSR_CLASS/$path/enable


	echo 'the most recent distance measure is'
	cat /sys/class/HCSR_CLASS/$path/dis


	((i++))
done
