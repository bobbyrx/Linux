#!/bin/bash

if [ $# -ne 1 ]; then 
	echo "Invalid number of arguments: $0 <dirname>"
	exit 1
fi

if [[ ! -d $1 || ! -r $1 ]]; then
        echo "Directory is not readable or does not exist"
	exit 2
fi

newFile=$(mktemp)
find $1 -mindepth 3 -maxdepth 3 -type d -printf "%f\n" | sort | uniq 2>/dev/null >> $newFile
newFile2=$(mktemp)
while read -r line; do
	num=0
	while read file; do
		count=$(wc -l $file| cut -d" " -f 1)
		num=$(( $num + $count ))
	done < <(find $1 -mindepth 3 -maxdepth 3 -type d -name $line -exec find {} -type f -name "[0-9]*.txt" \;)	
	echo "$line $num" >> $newFile2
done < $newFile	
rm -- $newFile

cat $newFile2 | sort -t" " -k2 -nr | head -n 3

rm -- $newFile2

exit 

