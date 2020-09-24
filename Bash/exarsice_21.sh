#!/bin/bash

if [ $(id -u) -ne 0 ]; then
	echo "The program can only be run by the root-user!"
	exit 1
fi

NEW_FILE=$(mktemp)

ps -e -o user= | sort | uniq > $NEW_FILE

while read cmdUser; do
	maxRss=0
	maxPid=0
	rssTotal=0
	while read cmdPid cmdRss; do
		if [ $maxRss -lt $cmdRss ]; then
			maxRss=$cmdRss
			maxPid=$cmdPid	
		fi
		rssTotal=$(( $rrTotal + $cmdRss ))
	done < <(ps -u $cmdUser -o pid=,rss= | sort -n -k2)	
	rssNumber=$(ps -u $cmdUser | wc -l)
	rssAvg=$(( $rssTotal / $rssNumber ))
	if [ $maxRss -ge $(echo $(( $rssAvg *2 ))) ]; then
		kill -s SIGTERM $maxPid
		sleep 1
		kill -s SIGKILL $maxPid
		echo "Terminated $maxPid !"
	fi	
done < $NEW_FILE

rm -- $NEW_FILE

exit
