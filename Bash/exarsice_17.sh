#!/bin/bash

if [ $# -ne 1 ]; then
	echo "Invalid number of arguments: $0 <userName>"
	exit 1
fi

if [ $(id -u) -ne 0 ]; then 
	echo "Only the root-user can run this program"
	exit 2
fi

userName=$1

if ! id -u $userName &>/dev/null ; then
	echo "No such user in this system!"
	exit 3
fi

newFile=$(mktemp)
ps -e -o user=,pid=,etimes= |sort | uniq > $newFile
newFile2=$(mktemp)
while read line; do
	avgTime=$(ps -e -o etimes= |wc -l)
	user=$(echo $line| cut -d" " -f 1)
	if id $user &>/dev/null ; then
		numCmd=$(ps -u $user | wc -l )
		numCmdFoo=$(ps -u $userName | wc -l )
		if [ $numCmd -gt $numCmdFoo ]; then
			echo "User=$user" >> $newFile2
		fi
	fi	
	Time=$(expr $Time + $(echo $line| cut -d" " -f 3))
done < $newFile
echo
cat $newFile2 | sort | uniq
echo 
rm -- $newFile2
AVG_TIME=$(expr $Time / $avgTime )
while read line; do
	if [ $( echo $line | cut -d" " -f3 ) > $(echo "${AVG_TIME} * 2"|bc) ]; then
		#kill -s SIGTERM $(echo $line | cut -d" " -f2 )
		#sleep 1
		#kill -s SIGKILL $(echo $line | cut -d" " -f2 )
		echo "Terminated $(echo $line | cut -d' ' -f2) "
	fi	
done < <(cat $newFile | grep "^${userName}" )	
rm -- $newFile
exit 
