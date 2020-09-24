#!/bin/bash

if [ $# -ne 1 ]; then 
	echo "Invalid number of arguments"
	exit 1
fi

if [[ ! -d $1 || ! -r $1 || ! -w $1 ]]; then 
	echo "The directory doesn't exist, can't be read or can't be written"
	exit 2
fi

dirName=$1
newFile=$(mktemp)
find $dirName -type f -printf "%f %p\n" | egrep "^[A-Za-z]+_report-[0-9]+.txt" | cut -d" " -f 2 >> $newFile
if [ $(cat $newFile | wc -l ) -eq 0 ]; then 
	echo "In $dirName directory there are no such files"
	rm -- $dirName
	exit 3
fi	
newFile2=$(mktemp)
shellAcc=$(stat --printf "%X" $0 )
while read line; do
	timeShell=$(echo $line | rev| cut -d"/" -f 1|rev | cut -d"_" -f 2- | cut -d"-" -f 2- | cut -d"." -f 1 )
	timeMod=$(echo $(stat --printf "%Y" $line))
	if [ $(($timeShell)) -lt $(($timeMod)) ]; then
		text=$(echo $line)
		lineText=$(echo $text |sed "s/${timeShell}/${shellAcc}/g" )
		echo "Renaming $line -> $lineText "
	        mv $line $lineText 
		echo $lineText >> $newFile2
	fi	
done < $newFile	
rm -- $newFile
if [ $(cat $newFile2 | wc -l ) -eq 0 ]; then
	echo "In the $dirName directory, there are no files to modify"
	rm -- $newFile2
	exit 5
fi	
while read line; do
	newFile3=$(mktemp)
	cat $line | cut -d" " -f 3 >> $newFile3
	if [ $(cat $newFile3 | wc -l ) -eq 0 ]; then 
		echo "File (${line}) is empty"
		rm -- $newFile3
		continue
	fi	
	while read file; do
		if [[ ! -f $file || ! -r $file ]]; then
			echo " File (${file}) does not exist or can't be read"
			continue
		fi	
		fileName=$(echo $file | rev |cut -d"/" -f1 | rev )
		name=$(echo $fileName | cut -d"." -f1)
		reName=$(echo "${name}_${shellAcc}.txt")
		echo "Moving ${file} -> ${dirName}/extracted/${reName}"
		mv $file "${dirName}/extracted/${reName}"
	done < $newFile3	
	rm -- $newFile3
done < $newFile2
rm -- $newFile2
exit 0
