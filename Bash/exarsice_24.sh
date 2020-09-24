#!/bin/bash 

if [ $(id -u) -eq 0 ]; then
	echo "Only the root-user can run this program"
	exit 1
fi

newFile=$(mktemp)
ps -e -o user=| egrep -v "^root" | sort | uniq > $newFile
newFile2=$(mktemp)
ps -e -user root -o rss= |sort| uniq > $newFile2
rootRss=0
while read line; do
	rootRss=$(( $rootRss + $line ))	
done < $newFile2

while read userName; do
	directory=$(cat /etc/passwd | cut -d":" -f 1,6 | egrep "^${userName}" | cut -d":" -f 2)
	owner=$(stat --printf "%u" $directory )
	if [[ ! -d $directory || $owner != $userName || $(stat --printf "%A" $directory | cut -c 3) != "w" ]]; then 
		countRss=0
		while read userRss; do
			countRss=$(( $countRss + $userRss ))
		done < <(ps -e -u $userName -o rss= | sort | uniq)
		if [ $countRss -gt $rootRss ]; then 
			echo "Killing process of this user: $userName"
			while read userPid; do
				#kill -s SIGTERM $userPid
				#sleep 1
				#kill -s SIGKILL $userPid
				echo "Terminated $userPid"
			done < <(ps -e -u $userName -o pid= | sort| uniq)	
		fi		
	fi	
done < $newFile	

rm -- $newFile
rm -- $newFile2

exit
