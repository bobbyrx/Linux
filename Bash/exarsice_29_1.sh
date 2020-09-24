#!/bin/bash

if [ $# -lt 1 ]; then
	echo "Invalid number of arguments"
	exit 1
fi
numLines=0
if [[ $1 == "-n" && $# -ge 2 ]]; then 
	if [ $(echo $2 | egrep "^[0-9]+$" | wc -l ) -eq 1 ]; then
		echo "Number of lines is: ${2}"
	       	numLines=$2
	else
		echo "If the first argument is the string (-n), then the second argument must be a whole positive number" 	
		exit 2
	fi
elif [[ $1 == "-n" && $# -lt 2 ]]; then
	echo "Invalid number of arguments"
	exit 3	
else 
	echo "Number of lines is: 10"
	numLines=10
fi

newFile=$(mktemp)

for file in $@; do
	if [[ $file != "-n" && $(echo $file | egrep "^[0-9]+$" |wc -l) -ne 1 && -f $file && -r $file ]]; then
		echo $file >> $newFile
	fi			
done	

if [ $(cat $newFile | wc -l ) -eq 0 ]; then
	echo "All the files you have imputed are non-existed or can't be read"
	rm -- $newFile
	exit 3
fi

newFile2=$(mktemp)

while read file; do
	nameFile=$(echo $file | cut -d"." -f 1)
	while read line; do
		cutLine1=$(echo $line | cut -d" " -f 1-2)
		cutLine2=$(echo $line | cut -d" " -f 3-)
		echo "${cutLine1} ${nameFile} ${cutLine2}" >> $newFile2
	done < $file
done < $newFile	
rm -- $newFile
cat $newFile2 | sort -n | head -n $numLines
rm -- $newFile2
exit 0

