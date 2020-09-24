#!/bin/bash 

if [ $# -ne 2 ]; then 
	echo "Invalid number of arguments: $0 <dirname> <string>"
	exit 1
fi

if [[ ! -d $1 || ! -r $1 ]]; then 
        echo "Can not be red or does not exist"
	exit 2
fi

if [ -z $2 ]; then
        echo "The string has 0 length"
	exit 3
fi

find $1 -maxdepth 1 -type f -printf "%f\n" | egrep "vmlinuz-[0-9]+.[0-9]+.[0-9]+-${2}" | sort -V -t"-" -k 2 -r | head -n 1
