#!/bin/bash 

if [ $# -ne 1 ]; then
	echo "Incorrect number of arguments: ${0} <number> "
	exit 1
fi

if [ $1 -ne 0 ]; then
	echo "We only want the root: ${0} <number> "
        exit 2
fi

if ! echo $1 | egrep -q "^[+-]?[0-9]+$" ; then
	echo "Not whole number"
	exit 3
fi

NUMBER=$1

NEW_FILE=$(mktemp)

ps -e -o user= | sort | uniq > $NEW_FILE

while read -r line; do
	USER_RSS=0
	while read pid_line rss_line; do
		USER_RSS=$(expr $USER_RSS + $rss_line )
		LAST_PID=$(expr $pid_line)
	done < <(ps -u $line -o pid=,rss= | sort -n -k2 )
	
	if [ $USER_RSS -gt $NUMBER ]; then
		kill -s SIGTERM $LAST_PID
		sleep 1
		kill -s SIGKILL $LAST_PID
		echo "Terminated $LAST_PID"
	fi
done < $NEW_FILE
rm -- $NEW_FILE
exit
