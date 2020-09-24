#!/bin/bash

if [ $# -ne 3 ]; then
	echo "Invalid number of arguments"
	exit 1
fi

if [[ ! -d $1 || ! -r $1 || ! -w $1 ]]; then
        echo "Can not be red or directory does not exist"
	exit 2
fi

if [[ ! -d $2 || ! -r $2 || ! -w $2 ]]; then
        echo "Can not be red or directory does not exist"
	exit 3
fi

#if [ $(id -u) -ne 0 ]; then 
#	echo "Only the root user can run this program"
#	exit 5
#fi

if [ $(ls -l ${2} | wc -l ) -ne 1 ] ; then
	echo "The directory must be empty"
	echo -n "Do you want to make it empty (y/n): "; read asw
	if [ $asw == "y" ]; then
		rm -r $2
		mkdir $2
		echo "$2 is now empty!"
	else 
		echo "OK, but the program will exit!"
		exit 4
	fi
fi

while read -r line; do
	destination=$(echo "${line}/" | cut -d"/" -f 2-| rev | cut -d"/" -f 3- | rev)
	mkdir -p "${2}/${destination}"
        mv $line "${2}/${destination}"
	echo "Moved $(echo $line| cut -d"/" -f 2- ) to ${2}"
done < <(find $1 -type f -name "*${3}*" 2>/dev/null)	
exit
