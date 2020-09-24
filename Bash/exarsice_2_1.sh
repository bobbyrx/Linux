#!/bin/bash
Counter=1
while read line;
do
	echo "$Counter. $line"
	((Counter++))
done < <(cut -d" " -f 4 $1) 
exit
