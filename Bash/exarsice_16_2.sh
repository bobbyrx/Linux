#!/bin/bash 

if [ $# -ne 3 ]; then
	echo "Ivalid number of arguments: $0 <fileName> <string1> <string2> "
	exit 1
fi

if [ ! -f $1 -a ! -r $1 ]; then
	echo "Not a file or it is not readable"
	exit 2
fi

if [ ! -n $2 -a ! -n $3 ]; then
	echo "The string has a 0 lenght"
	exit 3
fi

FILE=$1
STRING1=$2
STRING2=$3
LINE1=$(cat $FILE | grep "${STRING1}=" )
LINE2=$(cat $FILE | grep "${STRING2}=" )

VALUE1=$( echo $LINE1 | cut -d"=" -f 2 )
VALUE2=$( echo $LINE2 | cut -d"=" -f 2 )

for Arg2 in $VALUE2; do
	boolean=false
	for Arg1 in $VALUE1; do
		if [ $Arg2 == $Arg1 ]; then
			boolean=true			
			break			
		fi	
	done	
	if [ $boolean == "false" ]; then
		REPLACE+="${Arg2} "
	fi	
done	
REAL_VALUE="$(echo $LINE2 | cut -d= -f1)=$REPLACE"
sed -i "s/${LINE2}/${REAL_VALUE}/g" $FILE
echo "The changes have been done successfully"
exit 
