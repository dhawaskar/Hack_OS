#!/bin/bash

echo "Utility script which would Check if anu of system call intercepted"

check_sys_call(){
	rm address.txt
	touch address.txt
	sudo dmesg -C
	clear
	sudo insmod part4.ko
	sudo rmmod part4.ko
	sudo insmod part3.ko
	sudo dmesg -C
	sudo insmod part4.ko
	dmesg | grep "****************System call hack address is intercepted"
	dmesg | grep "###################   No system call is intercepted ##########################"	
	echo "From the utility function"
}


check_sys_call
