#!/bin/bash

if [ $# -ne 2 ]; then 
	echo "Invalid number of arguments: $0 <fileName.csv> <fileName.csv>"
	exit 1
fi

a=$1
b=$2

if [[ ! -f $a || ! -r $a ]]; then
       echo "The file can't be read or does not exist"
	exit 2
fi

if [[ ! -f $b || ! -w $b ]]; then 
	echo "The file can't be read or does not exist"
	exit 3
fi

if [ $(echo $a | egrep ".csv"| wc -l ) -eq 0 ]; then
	echo "The file ( ${a} ) is not a CSV file"
	exit 4
fi

if [ $(echo $b | egrep ".csv"|wc -l ) -eq 0 ]; then
	echo "The file ( ${b} ) is not a CSV file"
	exit 5
fi

if [ $(cat $a| wc -l) -eq 0 ]; then 
	echo "The file ( ${a} ) is empty"
	exit 6
fi

if [ $(cat $b| wc -l) -ne 0 ]; then
	echo "The file ( ${b} ) is not empty"
	echo -n "Do you want to clear it?(y/n): "; read asw
	if [ $asw == "y" ]; then 
		rm $b
		touch $b 
		echo "The file is now empty"
	else 
		echo "Ok, but the program is going to close"
		exit 7
	fi
fi	

while read line; do
	cutLine=$(echo $line | cut -d"," -f 2-)
	if [ $(egrep ",${cutLine}$" $b | wc -l) -eq 0 ] ; then
		echo $line >> $b
		echo "Line: ${line} -> ${b}"
	fi	
done < <(cat $a | sort -t"," -k1 -n )	
exit
