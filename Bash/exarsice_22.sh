#!/bin/bash

if [ $# -eq 1 ]; then
	fileName=$1		
	if [[ ! -d $fileName || ! -r $fileName ]]; then
		echo "Can not read the directory or it does not exist!"
		exit 2
	fi
	newFile=$(mktemp)
	find $fileName -printf "%Y %f %l\n" | grep "^[Ll]" | cut -d" " -f 2- > $newFile
	counter=0
	while read symlink object; do
		if [ $(find $fileName -type f,l -name $object |wc -l) -eq 0 ]; then
			counter=$(( $counter+1))
		else	
			echo "l${symlink} -> $object"
		fi
	done < $newFile	
	echo "Broken symliks: $counter" 
	rm -- $newFile
elif [ $# -eq 2 ]; then
	fileName=$1
	file=$2	
	if [[ ! -d $fileName || ! -r $fileName ]]; then
		echo "Can not read the directory or it does not exist!"
		exit 3
	fi

	if [[ ! -f $file || ! -w $file ]]; then
		echo "Can not write the file or it does not exist!"
		exit 4
	fi
	newFile=$(mktemp)
	find $fileName -printf "%Y %f %l\n" | grep "^[Ll]" | cut -d" " -f 2- > $newFile
	counter=0
	while read symlink object; do
		if [ $(find $fileName -type f,l -name $object |wc -l) -eq 0 ]; then
			counter=$(( $counter+1))
		else	
			echo "l${symlink} -> $object" >> $file
		fi
	done < $newFile	
	echo "Broken symlinks: $counter" >>$file
	echo "Inputed the result in $file !"
	rm -- $newFile
else 
	echo "Invalid number of arguments: $0 <dirName> (<fileName>)"
	exit 1
fi
exit 
