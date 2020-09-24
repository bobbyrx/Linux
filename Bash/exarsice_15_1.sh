#!/bin/bash

if [ $(id -u) -eq 0 ]; then
	echo "This program should be used only by the root user"
	exit 1
fi

while read lineName lineHome; do
	if [ ! -d $lineHome ] || sudo -u $lineName [ ! -w $lineHome ];then
		echo $lineName
	fi	
done < <(cat /etc/passwd | awk -F":" '{print $1 , $6}' )	
exit
