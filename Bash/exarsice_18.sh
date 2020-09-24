#!/bin/bash

NEW_FILE=$(mktemp)
cat /etc/passwd | awk -F: '{print $1, $6}' > $NEW_FILE
NEW_FILE2=$(mktemp)
while read arg1 arg2; do
	
	if ! id -u $arg1 &>/dev/null ; then
		echo "There is no such user ( $arv1 )  here" 
		continue
	fi	

	if [ ! -d $arg2 -o ! -r $arg2 ]; then
		echo "Not a directory or can't be red ( $arg2 )"
		continue
	fi
	
	sudo find $arg2 -type f -printf "%T+ %u %p\n" &>/dev/null >> $NEW_FILE2

done < $NEW_FILE

cat $NEW_FILE2 | sort -k1 -rn | head -n 1 | cut -d" " -f 2-

rm -- $NEW_FILE
rm -- $NEW_FILE2

exit
