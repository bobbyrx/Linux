#!/bin/bash 

if [ $# -eq 1 ]; then
	if [[ ! -d $1 || ! -r $1 ]]; then 
	      echo "The directory <$1> can't be red or does not exist"
	      exit 1
	fi
  	FILE=$1
	find $FILE -printf "%Y %p\n" | egrep "^[NL]" | cut -d" " -f 2	
elif [ $# -eq 2 ]; then	
	if [[ ! -d $1 || ! -r $1 ]]; then 
	      echo "The directory <$1> can't be red or does not exist"
	      exit 1
	fi
	FILE=$1
	NUMBER=$2
	NEW_FILE=$(mktemp)
	find $FILE -printf "%n %p\n" > $NEW_FILE
	while read line1 line2; do
		if [ $line1 -ge $NUMBER ]; then
			echo $line2
		fi	
	done < $NEW_FILE
	rm -- $NEW_FILE	
else 
	echo "Invalid number of arguments: $0 <dirname> (<number>)"
	exit 2
fi	
exit 
