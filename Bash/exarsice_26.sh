#!/bin/bash

if [ $# -ne 2 ]; then 
	echo "Invalid number of arguments: $0 <fileName> <dirName>"
	exit 1
fi

file=$1
dirName=$2

if [[ ! -d $dirName || ! -r $dirName ]]; then 
        echo "The directory can't be read or it does not exist"
	exit 2
fi

if [[ ! -f $file || ! -r $file ]]; then 
        echo "The file can't be read or it does not exist"
	exit 3
fi

if [ $(find $dirName -type f |wc -l ) -ne 0 ]; then
	echo "The directory is not empty"
	echo -n "Do you want to clear the directory (y/n): " ; read asw
	if [ $asw == "y" ]; then 
	       rm -r $dirName
       	       mkdir $dirName
	       echo "$dirName is now clear"
	else 
 	       echo "Ok, but the program is now closing"
	       exit 4
	fi
fi

newFile=$(mktemp)

cat $file | cut -d":" -f 1 | sed "s/ *( *[A-Za-z ]* *) *//g" | sed "s/  */ /g"|sed "s/^ *//g" |sed "s/ *$//g" |sort|uniq  >> $newFile

count=1000

touch ${dirName}/"dict.txt"

echo "Created file ${dirName}/dict.txt"

while read firstName secondName; do

	touch ${dirName}/${count}".txt"

	cat $file | egrep "${firstName}" | egrep "${secondName}[: ]" >> ${dirName}/${count}".txt"

	echo "Created file ${dirName}/${count}.txt"

	echo "${firstName} ${secondName};${count}" >> ${dirName}/"dict.txt"

	count=$(( $count + 1 ))

done < $newFile	

rm -- $newFile

exit
